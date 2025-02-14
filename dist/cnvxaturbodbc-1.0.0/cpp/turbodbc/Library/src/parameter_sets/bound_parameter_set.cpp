#include <turbodbc/parameter_sets/bound_parameter_set.h>

#include <turbodbc/make_description.h>

#include <cpp_odbc/error.h>

#include <stdexcept>
#include <sqlext.h>

#include <functional>
#include <ciso646>

namespace turbodbc {

namespace {
    std::size_t const max_initial_string_length = 16;

    std::shared_ptr<parameter> make_suggested_parameter(cpp_odbc::statement const & statement, std::size_t one_based_index, turbodbc::configuration const & configuration)
    {
        auto description = make_description(statement.describe_parameter(one_based_index), configuration.options);
        if (((description->get_type_code() == type_code::string) or (description->get_type_code() == type_code::unicode))
            and (description->element_size() > (max_initial_string_length + 1)))
        {
            auto modified_description = statement.describe_parameter(one_based_index);
            modified_description.size = max_initial_string_length;
            description = make_description(modified_description, configuration.options);
        }
        return std::make_shared<parameter>(statement,
                                           one_based_index,
                                           configuration.options.parameter_sets_to_buffer,
                                           std::move(description));
    }

    std::shared_ptr<parameter> make_default_parameter(cpp_odbc::statement const & statement, std::size_t one_based_index, turbodbc::configuration const & configuration)
    {
        auto const prefer_unicode = configuration.options.prefer_unicode;
        type_code const code = prefer_unicode ? type_code::unicode : type_code::string;
        auto description = make_description(code, 1);
        return std::make_shared<parameter>(statement,
                                           one_based_index,
                                           configuration.options.parameter_sets_to_buffer,
                                           std::move(description));
    }

    using parameter_factory = std::function<std::shared_ptr<parameter>(cpp_odbc::statement const &, std::size_t, turbodbc::configuration const &)>;
}


bound_parameter_set::bound_parameter_set(cpp_odbc::statement const & statement,
                                         turbodbc::configuration const & configuration) :
        statement_(statement),
        buffered_sets_(configuration.options.parameter_sets_to_buffer),
        transferred_sets_(0),
        confirmed_last_batch_(0)
{
    std::size_t const n_parameters = statement_.number_of_parameters();
    auto const query_db_for_initial_types = configuration.capabilities.supports_describe_parameter;
    parameter_factory make_parameter(query_db_for_initial_types ? make_suggested_parameter : make_default_parameter);
    for (std::size_t one_based_index = 1; one_based_index <= n_parameters; ++one_based_index) {
        try {
            parameters_.push_back(make_parameter(statement_, one_based_index, configuration));
        } catch (cpp_odbc::error const &) {
            parameters_.push_back(make_default_parameter(statement_, one_based_index, configuration));
        }
        initial_parameter_types_.push_back(parameters_.back()->get_type_code());
    }
    statement_.set_attribute(SQL_ATTR_PARAMS_PROCESSED_PTR, &confirmed_last_batch_);
}

std::size_t bound_parameter_set::buffered_sets() const
{
    return buffered_sets_;
}

std::size_t bound_parameter_set::transferred_sets() const
{
    return transferred_sets_;
}


std::size_t bound_parameter_set::number_of_parameters() const
{
    return parameters_.size();
}


std::vector<std::shared_ptr<parameter>> const & bound_parameter_set::get_parameters()
{
    return parameters_;
}


void bound_parameter_set::execute_batch(std::size_t sets_in_batch)
{
    if ((sets_in_batch != 0) and not parameters_.empty()) {
        if (sets_in_batch <= buffered_sets_) {
            statement_.set_attribute(SQL_ATTR_PARAMSET_SIZE, sets_in_batch);
            statement_.execute_prepared();
            transferred_sets_ += confirmed_last_batch_;
        } else {
            throw std::logic_error("A batch cannot be larger than the number of buffered sets");
        }
    }
}


void bound_parameter_set::rebind(std::size_t parameter_index,
                                 std::unique_ptr<description const> parameter_description)
{
    parameters_[parameter_index] = std::make_shared<parameter>(statement_,
                                                               parameter_index + 1,
                                                               buffered_sets_,
                                                               std::move(parameter_description));
}

std::vector<type_code> const & bound_parameter_set::get_initial_parameter_types() const
{
    return initial_parameter_types_;
}


}