import turbodbc.data_types
from turbodbc import BINARY, DATETIME, NUMBER, ROWID, STRING

ALL_TYPE_CODES = [
    turbodbc.data_types._BOOLEAN_CODE,
    turbodbc.data_types._INTEGER_CODE,
    turbodbc.data_types._FLOATING_POINT_CODE,
    turbodbc.data_types._STRING_CODE,
    turbodbc.data_types._UNICODE_CODE,
    turbodbc.data_types._TIMESTAMP_CODE,
    turbodbc.data_types._DATE_CODE,
]

ALL_DATA_TYPES = [STRING, BINARY, NUMBER, DATETIME, ROWID]


def test_each_type_code_matches_one_data_type():
    for type_code in ALL_TYPE_CODES:
        matches = [type for type in ALL_DATA_TYPES if type_code == type]
        assert 1 == len(matches)


def test_each_type_code_mismatches_all_but_one_data_type():
    for type_code in ALL_TYPE_CODES:
        mismatches = [type for type in ALL_DATA_TYPES if type_code != type]
        expected = len(ALL_DATA_TYPES) - 1
        assert expected == len(mismatches)
