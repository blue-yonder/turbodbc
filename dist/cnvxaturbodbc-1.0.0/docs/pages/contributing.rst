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

Earthly Setup
^^^^^^^^^^^^^

You can build turbodbc and run the full test suite with `earthly <https://earthly.dev>`_.

See the install instructions on how to get earthly: `https://earthly.dev/get-earthly <https://earthly.dev/get-earthly>`_.

Build and test turbodbc with the default arguments:

::

    earthly --allow-privileged +test

You can use the interactive mode to get a shell when tests fail:

::

    earthly -i --allow-privileged +test

Run a specific test setup with an export of the build package:

::

    earthly -P +test-python3.9-arrow7.x.x

The coverage results as well as the build tar.gz for turbodbc can be found in the created `result` directory.
To run other specific test targets and arguments, like different python or pyarrow versions, please see the Earthfile.

Run the full tests for different Python and package versions:

::

    earthly --allow-privileged +test-all

Build and save a Docker image for development usage:

::

    earthly +docker

It will create and tag a docker image `turbodbc:latest` which can be used for IDEs or to run builds
with less overhead and with a mounted source code and build directory:

::

    docker run -v $PWD:/src -it turbodbc:latest

This will give you a shell in a running container, there you can execute the different build steps as you wish:

* build setup: `cmake -DBOOST_ROOT=$CONDA_PREFIX -DBUILD_COVERAGE=ON -DCMAKE_INSTALL_PREFIX=./dist -DPYTHON_EXECUTABLE=/miniconda/envs/turbodbc-dev/bin/python -GNinja ..`
* compile: `ninja`
* install: `cmake --build . --target install`
* unit tests: `ctest -E turbodbc_integration_test --verbose`

To run the integration tests in this setup, it is required that the docker-compose setup is running:

::

    docker-compose -f earthly/docker-compose.yml up

    # to stop it
    docker-compose -f earthly/docker-compose.yml down


Start the container within the host network stack:

::

    docker run -v $PWD:/src --network=host -it turbodbc:latest

There you can run the integration tests:

::

    # the mssql database has to be created
    /opt/mssql-tools/bin/sqlcmd -S localhost -U SA -P 'StrongPassword1' -Q 'CREATE DATABASE test_db'

    ctest --verbose

Manual Host Native Setup
^^^^^^^^^^^^^^^^^^^^^^^^

For developing new features or just sampling the latest version of turbodbc directly on your host
do the following:

#.  Make sure your development environment meets the prerequisites mentioned
    in the :ref:`getting started guide <getting_started>`.

#.  Create development environment depending on your Python package manager.

    - For a pip-based workflow, create a virtual environment, activate it, and install
      the necessary packages numpy, pyarrow, pytest, and mock:

      ::

            pip install numpy pytest pytest-cov mock pyarrow

      Make sure you have a recent version of ``cmake`` installed. For some operating
      systems, binary wheels are available in addition to the package your operating
      system offers:

      ::

            pip install cmake

    - If you're using ``conda`` to manage your python packages, you can install the
      dependencies from conda-forge:

      ::

        conda create -y -q -n turbodbc-dev pyarrow numpy pybind11 boost-cpp \
            pytest pytest-cov mock cmake unixodbc gtest gmock -c conda-forge
        source activate turbodbc-dev

#.  Clone turbodbc into the virtual environment somewhere:

    ::

        git clone https://github.com/blue-yonder/turbodbc.git

#.  ``cd`` into the git repo and get ``pybind11``

    ::

        wget -q https://github.com/pybind/pybind11/archive/v2.9.1.tar.gz
        tar xvf v2.9.1.tar.gz

#.  Create a build directory somewhere and ``cd`` into it.

#.  Execute the following command:

    ::

        cmake -DCMAKE_INSTALL_PREFIX=./dist -DPYTHON_EXECUTABLE=`which python` /path/to/turbodbc

    where the final path parameter is the directory to the turbodbc git repo,
    specifically the directory containing ``setup.py``. This ``cmake`` command will
    prepare the build directory for the actual build step.

    .. note::
        The ``-DPYTHON_EXECUTABLE`` flag is not strictly necessary, but
        it helps ``pybind11`` to detect the correct Python version, in particular
        when using virtual environments.

#.  Run ``make``. This will build (compile) the source code.

    .. note::
        Some Linux distributions with very modern C++ compilers, e.g., Fedora 24+, may yield
        linker error messages such as

        ::

            arrow_result_set_test.cpp:168: undefined reference to `arrow::Status::ToString[abi:cxx11]() const'

        This error is caused because some Linux distributions use a C++11 compliant
        `ABI version <https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_dual_abi.html>`_
        of the standard library, while the ``pyarrow`` manylinux wheel does not. In this
        case, throw away your build directory and use

        ::

            cmake -DDISABLE_CXX11_ABI=ON -DCMAKE_INSTALL_PREFIX=./dist -DPYTHON_EXECUTABLE=`which python` /path/to/turbodbc

        in place of the CMake command in the previous step.

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


.. _GitHub: https://github.com/blue-yonder/turbodbc
