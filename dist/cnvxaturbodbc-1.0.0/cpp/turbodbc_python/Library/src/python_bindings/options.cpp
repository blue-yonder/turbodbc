#include <turbodbc/configuration.h>

#include <pybind11/pybind11.h>

#include <ciso646>

#include <boost/variant.hpp>

// The following conversion logic is adapted from https://github.com/pybind/pybind11/issues/576
// it is placed in this file because it is the only place where a function requiring
// conversion to/from turbodbc::buffer_size is used. There is no global converter registry,
// so the only other way to place the conversion logic is in a header file.
namespace pybind11 { namespace detail {

struct variant_caster_visitor : boost::static_visitor<handle> {
    variant_caster_visitor(return_value_policy policy, handle parent) :
        policy(policy),
        parent(parent)
    {}

    return_value_policy policy;
    handle parent;

    template<class T>
    handle operator()(T const& src) const {
        return make_caster<T>::cast(src, policy, parent);
    }
};

template <>
struct type_caster<turbodbc::buffer_size> {
    using Type = turbodbc::buffer_size;

    PYBIND11_TYPE_CASTER(Type, _("BufferSize"));

    template<class T>
    bool try_load(handle py_value, bool convert) {
        auto caster = make_caster<T>();
        if (caster.load(py_value, convert)) {
            value = cast_op<T>(caster);
            return true;
        }
        return false;
    }

    bool load(handle py_value, bool convert) {
        return (try_load<turbodbc::megabytes>(py_value, convert)) or
               (try_load<turbodbc::rows>(py_value, convert));
    }

    static handle cast(Type const & cpp_value, return_value_policy policy, handle parent) {
        return boost::apply_visitor(variant_caster_visitor(policy, parent), cpp_value);
    }
};

} }


namespace turbodbc { namespace bindings {

void for_options(pybind11::module & module)
{
    pybind11::class_<turbodbc::options>(module, "Options")
        .def(pybind11::init<>())
        .def_readwrite("read_buffer_size", &turbodbc::options::read_buffer_size)
        .def_readwrite("parameter_sets_to_buffer", &turbodbc::options::parameter_sets_to_buffer)
        .def_readwrite("varchar_max_character_limit", &turbodbc::options::varchar_max_character_limit)
        .def_readwrite("use_async_io", &turbodbc::options::use_async_io)
        .def_readwrite("prefer_unicode", &turbodbc::options::prefer_unicode)
        .def_readwrite("autocommit", &turbodbc::options::autocommit)
        .def_readwrite("large_decimals_as_64_bit_types", &turbodbc::options::large_decimals_as_64_bit_types)
        .def_readwrite("limit_varchar_results_to_max", &turbodbc::options::limit_varchar_results_to_max)
        .def_readwrite("force_extra_capacity_for_unicode", &turbodbc::options::force_extra_capacity_for_unicode)
        .def_readwrite("fetch_wchar_as_char", &turbodbc::options::fetch_wchar_as_char)
    ;

}

} }
