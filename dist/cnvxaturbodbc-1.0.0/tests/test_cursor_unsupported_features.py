import pytest
from helpers import for_one_database, get_credentials

from turbodbc import connect

"""
Test optional features mentioned in PEP-249 "behave" as specified
"""


@for_one_database
def test_callproc_unsupported(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()

    with pytest.raises(AttributeError):
        cursor.callproc()
