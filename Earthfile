os-base:
    FROM condaforge/mambaforge:latest

    ENV DEBIAN_FRONTEND=noninteractive
    # set locale to utf-8, which is required for some odbc drivers (mysql);
    ENV LC_ALL=C.UTF-8

    RUN apt-get update && apt-get upgrade -y && \
        apt-get install -y unixodbc odbc-postgresql postgresql-client gnupg apt-transport-https && \
        apt-get clean

    # we need an mysql odbc driver for the integration tests
    # currently the test fail with a newer driver
    RUN cd /opt && \
        wget -q https://downloads.mysql.com/archives/get/p/10/file/mysql-connector-odbc-5.1.13-linux-glibc2.5-x86-64bit.tar.gz && \
        tar xzf mysql-connector-odbc-5.1.13-linux-glibc2.5-x86-64bit.tar.gz && \
        mysql-connector-odbc-5.1.13-linux-glibc2.5-x86-64bit/bin/myodbc-installer -d -a -n MySQL -t DRIVER=`ls /opt/mysql-*/lib/libmyodbc*w.so` && \
        rm /opt/*.tar.gz

    # we need an mssql odbc driver for the integration tests
    RUN wget -q https://packages.microsoft.com/keys/microsoft.asc -O- | apt-key add - && \
        wget -q https://packages.microsoft.com/config/debian/9/prod.list -O- > /etc/apt/sources.list.d/mssql-release.list && \
        apt-get update && \
        ACCEPT_EULA=Y apt-get install msodbcsql17 mssql-tools && \
        ls -l  /opt/microsoft/msodbcsql17/lib64/ && \
        odbcinst -i -d -f /opt/microsoft/msodbcsql17/etc/odbcinst.ini

    # not used for the moment as Exasol is not tested here
    # RUN cd /opt && \
    #     wget -q https://www.exasol.com/support/secure/attachment/111075/EXASOL_ODBC-6.2.9.tar.gz && \
    #     tar xzf EXASOL_ODBC-6.2.9.tar.gz && \
    #     echo "\n[EXASOL]\nDriver=`ls /opt/EXA*/lib/linux/x86_64/libexaodbc-uo2214lv1.so`\nThreading=2\n" >> /etc/odbcinst.ini && \
    #     rm /opt/*.tar.gz

python:
    ARG PYTHON_VERSION=3.10.14
    ARG ARROW_VERSION_RULE="<8.0.0"
    ARG NUMPY_VERSION_RULE=""
    ARG CONDA_EXTRA=""
    FROM +os-base

    RUN mamba create -y -q -n turbodbc-dev \
        c-compiler \
        make \
        ninja \
        cmake \
        coveralls \
        gmock=1.10.0 \
        gtest \
        cxx-compiler \
        mock \
        pytest \
        pytest-cov \
        python=${PYTHON_VERSION} \
        unixodbc \
        boost-cpp \
        numpy$NUMPY_VERSION_RULE \
        pyarrow$ARROW_VERSION_RULE \
        pybind11 \
        simdutf \
        $CONDA_EXTRA

    RUN echo "conda activate turbodbc-dev" >> ~/.bashrc
    RUN echo 'export UNIXODBC_INCLUDE_DIR=$CONDA_PREFIX/include' >> ~/.bashrc

    RUN cd /opt && \
        wget -q https://github.com/pybind/pybind11/archive/v2.9.1.tar.gz && \
        tar xvf v2.9.1.tar.gz

build:
    ARG PYTHON_VERSION=3.10.14
    ARG ARROW_VERSION_RULE="<8.0.0"
    ARG NUMPY_VERSION_RULE=""
    ARG CONDA_EXTRA=""
    FROM --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE="$ARROW_VERSION_RULE" \
        --build-arg NUMPY_VERSION_RULE="$NUMPY_VERSION_RULE" \
        --build-arg CONDA_EXTRA="$CONDA_EXTRA" \
        +python

    COPY . /src
    # remove a potential available host build
    RUN rm -rf /src/build && mkdir /src/build
    WORKDIR /src/build

    ENV ODBCSYSINI=/src/earthly/odbc
    ENV TURBODBC_TEST_CONFIGURATION_FILES="query_fixtures_postgresql.json,query_fixtures_mssql.json,query_fixtures_mysql.json"
    RUN ln -s /opt/pybind11-2.10.4 /src/pybind11

    RUN bash -iec " \
        cmake -DBOOST_ROOT=\${CONDA_PREFIX} -DBUILD_COVERAGE=ON \
            -DCMAKE_INSTALL_PREFIX=./dist  \
            -DPYTHON_EXECUTABLE=\$(which python) \
            -GNinja .. && \
        ninja && \
        cmake --build . --target install && \
        cd dist && \
        python setup.py sdist \
        "
    SAVE ARTIFACT /src/build/dist/dist /dist

test:
    ARG PYTHON_VERSION=3.10.14
    ARG ARROW_VERSION_RULE=""
    ARG NUMPY_VERSION_RULE=""
    ARG CONDA_EXTRA=""
    FROM --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE="$ARROW_VERSION_RULE" \
        --build-arg NUMPY_VERSION_RULE="$NUMPY_VERSION_RULE" \
        --build-arg CONDA_EXTRA="$CONDA_EXTRA" \
        +build

    WITH DOCKER --compose ../earthly/docker-compose.yml
        RUN /bin/bash -ic "\
            (r=20;while ! pg_isready --host=localhost --port=5432 --username=postgres ; do ((--r)) || exit; sleep 1 ;done) && \
            (r=10;while ! /opt/mssql-tools/bin/sqlcmd -S localhost -U SA -P 'StrongPassword1' -Q 'SELECT @@VERSION' ; do ((--r)) || exit; sleep 3 ;done) && \
            sleep 5 && \
            /opt/mssql-tools/bin/sqlcmd -S localhost -U SA -P 'StrongPassword1' -Q 'CREATE DATABASE test_db' && \
            ctest --verbose \
            "
    END

    RUN /bin/bash -ic '\
        mkdir ../gcov && cd ../gcov && \
        gcov -l -p `find ../build -name "*.gcda"` > /dev/null && \
        echo "Removing coverage for boost and C++ standard library" && \
        (find . -name "*#boost#*" | xargs rm) || echo "error while removing boost files" && \
        (find . -name "*#c++#*" | xargs rm) || echo "error while removing stdlib files" \
        '

    SAVE ARTIFACT python_cov.xml /result/cov/python/python_cov.xml
    SAVE ARTIFACT ../gcov /result/cov/cpp
    SAVE ARTIFACT /src/build/dist/dist /result/dist

test-python3.10-arrow7.x.x:
    ARG PYTHON_VERSION="3.10.2"
    COPY --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE=">=7,<8" \
        --build-arg NUMPY_VERSION_RULE=">=1.21.2" \
        +test/result /result

    SAVE ARTIFACT /result AS LOCAL result

test-python3.10-arrow8.x.x:
    ARG PYTHON_VERSION="3.10.2"
    COPY --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE=">=8,<9" \
        --build-arg NUMPY_VERSION_RULE=">=1.21.2" \
        +test/result /result

    SAVE ARTIFACT /result AS LOCAL result

test-python3.10-arrow9.x.x:
    ARG PYTHON_VERSION="3.10.2"
    COPY --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE=">=9,<10" \
        --build-arg NUMPY_VERSION_RULE=">=1.21.2" \
        +test/result /result

    SAVE ARTIFACT /result AS LOCAL result

test-python3.10-arrow10.x.x:
    ARG PYTHON_VERSION="3.10.2"
    COPY --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE=">=10,<11" \
        --build-arg NUMPY_VERSION_RULE=">=1.21.2" \
        +test/result /result

    SAVE ARTIFACT /result AS LOCAL result

test-python3.11-arrow11.x.x:
    ARG PYTHON_VERSION="3.11.3"
    COPY --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE=">=11,<12" \
        --build-arg NUMPY_VERSION_RULE=">=1.23.3" \
        +test/result /result

    SAVE ARTIFACT /result AS LOCAL result

test-python3.11-arrow12.x.x:
    ARG PYTHON_VERSION="3.11.3"
    COPY --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE=">=12,<13" \
        --build-arg NUMPY_VERSION_RULE=">=1.23.3" \
        +test/result /result

    SAVE ARTIFACT /result AS LOCAL result

test-python3.11-arrow13.x.x:
    ARG PYTHON_VERSION="3.11.3"
    COPY --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE=">=13,<14" \
        --build-arg NUMPY_VERSION_RULE=">=1.23.3" \
        +test/result /result

    SAVE ARTIFACT /result AS LOCAL result

test-python3.11-arrow14.x.x:
    ARG PYTHON_VERSION="3.11.3"
    COPY --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE=">=14,<15" \
        --build-arg NUMPY_VERSION_RULE=">=1.23.3" \
        +test/result /result

    SAVE ARTIFACT /result AS LOCAL result

test-python3.11-arrow15.x.x:
    ARG PYTHON_VERSION="3.11.3"
    COPY --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE=">=15,<16" \
        --build-arg NUMPY_VERSION_RULE=">=1.23.3" \
        +test/result /result

    SAVE ARTIFACT /result AS LOCAL result

test-python3.11-arrow16.x.x:
    ARG PYTHON_VERSION="3.11.3"
    COPY --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE=">=16,<17" \
        --build-arg NUMPY_VERSION_RULE=">=1.23.3" \
        +test/result /result

    SAVE ARTIFACT /result AS LOCAL result

test-python3.11-arrow-nightly:
    ARG PYTHON_VERSION="3.11.3"
    COPY --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg NUMPY_VERSION_RULE=">=1.23.3" \
        --build-arg CONDA_EXTRA="-c arrow-nightlies" \
        +test/result /result

    SAVE ARTIFACT /result AS LOCAL result/$EARTHLY_TARGET_NAME

test-python3.10-all:
    BUILD test-python3.10-arrow7.x.x
    BUILD test-python3.10-arrow8.x.x
    BUILD test-python3.10-arrow9.x.x
    BUILD test-python3.10-arrow11.x.x
    BUILD test-python3.10-arrow12.x.x
    BUILD test-python3.10-arrow13.x.x
    BUILD test-python3.10-arrow14.x.x
    BUILD test-python3.10-arrow15.x.x
    BUILD test-python3.10-arrow16.x.x
    BUILD test-python3.10-arrow-nightly

test-python3.11-all:
    BUILD test-python3.11-arrow7.x.x
    BUILD test-python3.11-arrow8.x.x
    BUILD test-python3.11-arrow9.x.x
    BUILD test-python3.11-arrow10.x.x
    BUILD test-python3.11-arrow11.x.x
    BUILD test-python3.11-arrow12.x.x
    BUILD test-python3.11-arrow13.x.x
    BUILD test-python3.11-arrow14.x.x
    BUILD test-python3.11-arrow15.x.x
    BUILD test-python3.11-arrow16.x.x
    BUILD test-python3.11-arrow-nightly

test-all:
    BUILD +test-python3.10-all
    BUILD +test-python3.11-all

docker:
    ARG PYTHON_VERSION=3.10.14
    ARG ARROW_VERSION_RULE="<8.0.0"
    ARG NUMPY_VERSION_RULE="<1.21.0"

    FROM --build-arg PYTHON_VERSION="$PYTHON_VERSION" \
        --build-arg ARROW_VERSION_RULE="$ARROW_VERSION_RULE" \
        --build-arg NUMPY_VERSION_RULE="$NUMPY_VERSION_RULE" \
        +build

    RUN apt-get install -y vim

    CMD ["/bin/bash"]
    SAVE IMAGE turbodbc:latest
