FROM debian:buster-slim

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y unixodbc unixodbc-dev libboost-all-dev python3.7-dev python3.7-venv cmake g++ wget \
                       odbc-postgresql ninja-build gnupg apt-transport-https && \
    apt-get clean

# install the mysql odbc driver in /opt
RUN cd /opt && \
    wget -q https://dev.mysql.com/get/Downloads/Connector-ODBC/8.0/mysql-connector-odbc-8.0.15-linux-debian9-x86-64bit.tar.gz && \
    tar xzf mysql-connector-odbc-8.0.15-linux-debian9-x86-64bit.tar.gz && \
    /opt/mysql-connector-odbc-8.0.15-linux-debian9-x86-64bit/bin/myodbc-installer -d -a -n MySQL -t DRIVER=`ls /opt/mysql-*/lib/libmyodbc*w.so` && \
    rm /opt/*.tar.gz

# install the mssql odbc driver:
RUN wget -q https://packages.microsoft.com/keys/microsoft.asc -O- | apt-key add - && \
    wget -q https://packages.microsoft.com/config/debian/9/prod.list -O- > /etc/apt/sources.list.d/mssql-release.list && \
    apt-get update && \
    ACCEPT_EULA=Y apt-get install msodbcsql17 && \
    odbcinst -i -d -f /opt/microsoft/msodbcsql17/etc/odbcinst.ini

# install the exasol odbc driver:
RUN cd /opt && \
    wget -q https://www.exasol.com/support/secure/attachment/74441/EXASOL_ODBC-6.0.14.tar.gz && \
    tar xzf EXASOL_ODBC-6.0.14.tar.gz && \
    echo "\n[EXASOL]\nDriver=`ls /opt/EXA*/lib/linux/x86_64/libexaodbc-uo2214lv1.so`\nThreading=2\n" >> /etc/odbcinst.ini && \
    rm /opt/*.tar.gz

# create a python virtualenv at /venv that contains the necessary build and test packages:
RUN python3.7 -m venv /venv && \
    /venv/bin/pip install numpy==1.19.4 pandas==1.1.4 'pyarrow<2' pytest mock pytest-cov


# set locale to utf-8, which is required for some odbc drivers (mysql);
# also, set environment as set after 'source /venv/bin/activate'
ENV LC_ALL=C.UTF-8 VIRTUAL_ENV=/venv PATH=/venv/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
