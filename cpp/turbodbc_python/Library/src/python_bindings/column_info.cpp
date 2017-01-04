#include <turbodbc/column_info.h>

// Include ostream explicitly before to_python_converter.hpp to avoid compilation problems on macOS
#include <ostream>

#include <boost/python/to_python_converter.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/variant/static_visitor.hpp>

#include <vector>

using turbodbc::column_info;

namespace turbodbc { namespace bindings {


struct column_info_to_object : boost::static_visitor<PyObject *> {
	static result_type convert(std::vector<column_info> const & columns) {
		boost::python::list result;
		for (auto const & column : columns) {
			boost::python::dict info;
			info["name"] = column.name;
			info["type_code"] = static_cast<int>(column.type);
			info["supports_null_values"] = column.supports_null_values;
			result.append(info);
		}
		return boost::python::incref(result.ptr());
	}
};


void for_column_info()
{
	boost::python::to_python_converter<std::vector<column_info>, column_info_to_object>();
}

} }


