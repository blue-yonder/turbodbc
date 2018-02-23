from __future__ import absolute_import

from weakref import WeakSet

from .exceptions import translate_exceptions, InterfaceError
from .cursor import Cursor


class Connection(object):
    def _assert_valid(self):
        if self.impl is None:
            raise InterfaceError("Connection already closed")

    def __init__(self, impl):
        self.impl = impl
        self.cursors = WeakSet([])

    @translate_exceptions
    def cursor(self):
        """
        Create a new ``Cursor`` instance associated with this ``Connection``

        :return: A new ``Cursor`` instance
        """
        self._assert_valid()
        c = Cursor(self.impl.cursor())
        self.cursors.add(c)
        return c

    @translate_exceptions
    def commit(self):
        """
        Commits the current transaction
        """
        self._assert_valid()
        self.impl.commit()

    @translate_exceptions
    def rollback(self):
        """
        Roll back all changes in the current transaction
        """
        self._assert_valid()
        self.impl.rollback()

    def close(self):
        """
        Close the connection and all associated cursors. This will implicitly
        roll back any uncommitted operations.
        """
        for c in self.cursors:
            c.close()
        self.cursors = []
        self.impl = None

    @property
    def autocommit(self):
        """
        This attribute controls whether changes are automatically committed after each
        execution or not.
        """
        return self.impl.autocommit_enabled()

    @autocommit.setter
    def autocommit(self, value):
        self.impl.set_autocommit(value)


    def __enter__(self):
        """
        Conformance to PEP-343
        """
        return self

    def __exit__(self, type, value, traceback):
        """
        Conformance to PEP-343
        """
        return self.close()

