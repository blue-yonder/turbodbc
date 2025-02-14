#pragma once

#include <cpp_odbc/statement.h>
#include <turbodbc/parameter.h>
#include <turbodbc/configuration.h>

#include <vector>


namespace turbodbc {


/**
 * @brief This class manages a set of parameters that is bound to a
 *        statement. The class tracks the number of transferred records
 *        and manages parameter buffers.
 */
class bound_parameter_set {
public:
    bound_parameter_set(cpp_odbc::statement const & statement,
                        turbodbc::configuration const & configuration);
    
    /**
     * @brief Retrieve the number of buffered sets, i.e, the size
     *        of the parameter buffers in rows
     */
    std::size_t buffered_sets() const;

    /**
     * @brief Retrieve the number of already transferred sets based on
     *        success indicators retrieved by the database.
     */
    std::size_t transferred_sets() const;

    /**
     * @brief Retrieve the number of parameters in this set
     */
    std::size_t number_of_parameters() const;

    /**
     * @brief Retrieve access to the managed parameters
     */
    std::vector<std::shared_ptr<parameter>> const & get_parameters();

    /**
     * @brief Execute the current prepared statement with a batch
     *        of parameters. The statement will not be executed
     *        if no parameter sets are available.
     * @param sets_in_batch The number of parameter sets in the batch.
     */
    void execute_batch(std::size_t sets_in_batch);

    /**
     * @brief Replace the current parameter bound for a given index with a
     *        new one based on the provided type information. The current
     *        parameter will be dropped.
     * @param zero_based_parameter_index Index of the column. First column has index 0.
     * @param parameter_description Describes the parameter type
     */
    void rebind(std::size_t zero_based_parameter_index, std::unique_ptr<description const> parameter_description);

    /**
     * @brief Retrieve type codes of the initially bound parameter types.
     *        Useful for reverting to the original suggestion after rebinding.
     * @return
     */
    std::vector<type_code> const & get_initial_parameter_types() const;
private:
    cpp_odbc::statement const & statement_;
    std::size_t buffered_sets_;
    std::size_t transferred_sets_;
    SQLULEN confirmed_last_batch_;
    std::vector<std::shared_ptr<parameter>> parameters_;
    std::vector<type_code> initial_parameter_types_;
};


}