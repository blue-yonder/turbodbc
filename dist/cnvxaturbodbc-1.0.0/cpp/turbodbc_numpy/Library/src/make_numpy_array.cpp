#include <turbodbc_numpy/make_numpy_array.h>

#include <turbodbc_numpy/ndarrayobject.h>

#ifdef __GNUC__
#define EXTENSION __extension__
#else
#define EXTENSION
#endif

namespace turbodbc_numpy {

using pybind11::reinterpret_steal;
using pybind11::object;

object make_empty_numpy_array(numpy_type const & type)
{
    npy_intp no_elements = 0;
    int const flags = 0;
    int const one_dimensional = 1;
    // __extension__ needed because of some C/C++ incompatibility.
    // see issue https://github.com/numpy/numpy/issues/2539
    return reinterpret_steal<object>(EXTENSION PyArray_New(&PyArray_Type,
                                                               one_dimensional,
                                                               &no_elements,
                                                               type.code,
                                                               nullptr,
                                                               nullptr,
                                                               type.size,
                                                               flags,
                                                               nullptr));
}


pybind11::object make_empty_numpy_array(std::string const & descriptor)
{
    npy_intp no_elements = 0;
    int const flags = 0;
    int const one_dimensional = 1;
    // __extension__ needed because of some C/C++ incompatibility.
    // see issue https://github.com/numpy/numpy/issues/2539
    PyArray_Descr * descriptor_ptr = nullptr;
    EXTENSION PyArray_DescrConverter(pybind11::cast(descriptor).ptr(), &descriptor_ptr);

    return reinterpret_steal<object>(EXTENSION PyArray_NewFromDescr(&PyArray_Type,
                                                                        descriptor_ptr,
                                                                        one_dimensional,
                                                                        &no_elements,
                                                                        nullptr,
                                                                        nullptr,
                                                                        flags,
                                                                        nullptr));
}

}
