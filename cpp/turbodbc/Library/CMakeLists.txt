add_library(turbodbc
  src/buffer_size.cpp
  src/column.cpp
  src/command.cpp
  src/configuration.cpp
  src/connect.cpp
  src/connection.cpp
  src/cursor.cpp
  src/description.cpp
  src/descriptions/boolean_description.cpp
  src/descriptions/date_description.cpp
  src/descriptions/floating_point_description.cpp
  src/descriptions/integer_description.cpp
  src/descriptions/string_description.cpp
  src/descriptions/timestamp_description.cpp
  src/descriptions/unicode_description.cpp
  src/errors.cpp
  src/field_translator.cpp
  src/field_translators/boolean_translator.cpp
  src/field_translators/date_translator.cpp
  src/field_translators/float64_translator.cpp
  src/field_translators/int64_translator.cpp
  src/field_translators/string_translator.cpp
  src/field_translators/timestamp_translator.cpp
  src/make_description.cpp
  src/make_field_translator.cpp
  src/parameter.cpp
  src/parameter_sets/bound_parameter_set.cpp
  src/parameter_sets/field_parameter_set.cpp
  src/parameter_sets/set_field.cpp
  src/result_sets/bound_result_set.cpp
  src/result_sets/double_buffered_result_set.cpp
  src/result_sets/field_result_set.cpp
  src/result_sets/result_set.cpp
  src/result_sets/row_based_result_set.cpp
  src/string_helpers.cpp
  src/time_helpers.cpp)

set_target_properties(turbodbc PROPERTIES 
    INSTALL_RPATH "../../.."
)

target_link_libraries(turbodbc
	${Boost_LIBRARIES}
	${Simdutf_LIBRARIES}
	${Odbc_LIBRARIES}
	cpp_odbc
)

add_custom_command(TARGET turbodbc POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   $<TARGET_FILE:turbodbc> ${CMAKE_BINARY_DIR})

install(
    TARGETS turbodbc
    DESTINATION lib
)

install(
	DIRECTORY turbodbc
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
	PATTERN ".git" EXCLUDE
)

install(
	DIRECTORY src/
	DESTINATION src/turbodbc
	FILES_MATCHING PATTERN "*.cpp"
	PATTERN ".git" EXCLUDE
)
