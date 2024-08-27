from unittest.mock import patch

from turbodbc.cursor import _has_numpy_support


def test_has_numpy_support_fails():
    with patch("builtins.__import__", side_effect=ImportError):
        assert not _has_numpy_support()


def test_has_numpy_support_succeeds():
    assert _has_numpy_support()
