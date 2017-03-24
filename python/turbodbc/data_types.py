class DataType(object):
    def __init__(self, matched_type_codes):
        self._matched_type_codes = matched_type_codes

    def __eq__(self, other):
        return other in self._matched_type_codes

    def __ne__(self, other):
        return not (self == other)

# Type codes according to underlying C++ library:
_BOOLEAN_CODE = 0
_INTEGER_CODE = 10
_FLOATING_POINT_CODE = 20
_STRING_CODE = 30
_UNICODE_CODE = 31
_TIMESTAMP_CODE = 40
_DATE_CODE = 41

# data types according to https://www.python.org/dev/peps/pep-0249/#type-objects-and-constructors
STRING = DataType([_STRING_CODE, _UNICODE_CODE])
BINARY = DataType([])
NUMBER = DataType([_BOOLEAN_CODE, _INTEGER_CODE, _FLOATING_POINT_CODE])
DATETIME = DataType([_DATE_CODE, _TIMESTAMP_CODE])
ROWID = DataType([])