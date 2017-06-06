Contributing
============

Ways to contribute
------------------

Contributions to turbodbc are most welcome! There are many options how you can
contribute, and not all of them require you to be an expert programmer:

*   Ask questions and raise issues on `GitHub`_. This may influence turbodbc's roadmap.
*   If you like turbodbc, star/fork/watch the project on `GitHub`_. This will improve visibility,
    and potentially attracts more contributors.
*   Report performance comparisons between turbodbc and other means to access a
    database.
*   Tell others about turbodbc on your blog, Twitter account, or at the coffee
    machine at work.
*   Improve turbodbc's documentation by creating pull requests on `GitHub`_.
*   Improve existing features by creating pull requests on `GitHub`_.
*   Add new features by creating pull requests on `GitHub`_.
*   Implement dialects for SQLAlchemy that connect to databases using turbodbc.


Pull requests
-------------

Pull requests are appreciated in general, but not all pull requests will be
accepted into turbodbc. Before starting to work on a pull request, please make sure
your pull request is aligned with turbodbc's vision of creating fast ODBC
database access for data scientists. The safest way is to ask on `GitHub`_ whether a
certain feature would be appreciated.

After forking the project and making your modifications, you can create a new pull
request on turbodbc's `GitHub`_ page. This will trigger an automatic build and,
eventually, a code review. During code reviews, I try to make sure that the added
code complies with clean code principles such as single level of abstraction,
single responsibility principle, principle of least astonishment, etc.

If you do not know what all of this means, just try to keep functions small (up to
five lines) and find meaningful names. If you feel like writing a comment, think
about whether the comment would make a nice variable or function name, and refactor
your code accordingly.

I am well aware that the current code falls short of clean code standards in one
place or another. Please do not take criticism regarding your code personally. Any
comments are purely directed to improve the quality of turbodbc's code base over its
current state.


Development version
-------------------

For developing new features or just sampling the latest version of turbodbc,
do the following:

#.  Make sure your development environment meets the prerequisites mentioned
    in the :ref:`getting started guide <getting_started>`.

#.  Create a Python virtual environment, activate it, and install the necessary
    packages numpy, pytest, and mock:

    ::

        pip install numpy pytest pytest-cov mock

#.  Make sure you have a recent version of ``cmake`` installed. For some operating
    systems, binary wheels are available in addition to the package your operating
    system offers:

    ::

        pip install cmake

#.  Clone turbodbc into the virtual environment somewhere:

    ::

        git clone https://github.com/blue-yonder/turbodbc.git

#.  ``cd`` into the git repo and pull in the ``pybind11`` submodule by running:

    ::

        git submodule update --init --recursive

#.  Create a build directory somewhere and ``cd`` into it.

#.  Execute the following command:

    ::

        cmake -DCMAKE_INSTALL_PREFIX=./dist /path/to/turbodbc

    where the final path parameter is the directory to the turbodbc git repo,
    specifically the directory containing ``setup.py``. This ``cmake`` command will
    prepare the build directory for the actual build step.

#.  Run ``make``. This will build (compile) the source code.

#.  At this point you can run the test suite. First, make a copy of the
    relevant json documents from the turbodbc ``python/turbodbc_test`` directory,
    there's one for each database. Then edit your copies with the relevant
    credentials. Next, set the environment variable ``TURBODBC_TEST_CONFIGURATION_FILES``
    as a comma-separated list of the json files you've just copied and run
    the test suite, as follows:

    ::

        export TURBODBC_TEST_CONFIGURATION_FILES="<Postgres json file>,<MySql json file>, <MS SQL json file>"
        ctest --output-on-failure

#.  Finally, to create a Python source distribution for ``pip`` installation, run
    the following from the build directory:

    ::

        make install
        cd dist
        python setup.py sdist

    This will create a ``turbodbc-x.y.z.tar.gz`` file locally which can be used
    by others to install turbodbc with ``pip install turbodbc-x.y.z.tar.gz``.
