from unittest.mock import patch

import pytest

from turbodbc.cursor import _has_numpy_support

# Skip all parquet tests if we can't import pyarrow.parquet
pytest.importorskip("numpy")

# Ignore these with pytest ... -m 'not parquet'
numpy = pytest.mark.numpy


# Skip all parquet tests if we can't import pyarrow.parquet
@pytest.mark.numpy
def test_has_numpy_support_fails():
    with patch("builtins.__import__", side_effect=ImportError):
        assert not _has_numpy_support()


@pytest.mark.numpy
def test_has_numpy_support_succeeds():
    assert _has_numpy_support()
