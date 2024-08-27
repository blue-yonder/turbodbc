from unittest.mock import patch

import pytest

from turbodbc.cursor import _has_arrow_support

# Skip all parquet tests if we can't import pyarrow.parquet
pytest.importorskip("pyarrow")

# Ignore these with pytest ... -m 'not parquet'
pyarrow = pytest.mark.pyarrow


@pyarrow
def test_has_arrow_support_fails():
    with patch("builtins.__import__", side_effect=ImportError):
        assert not _has_arrow_support()


@pyarrow
def test_has_arrow_support_succeeds():
    assert _has_arrow_support()
