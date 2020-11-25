#!/bin/bash

# This script should be run on the guest system to start 3 database services
# defined in docker-compose.yml and

set -ex

docker-compose up -d postgres mssql mysql
# TODO: wait a bit until mssql accepts connections
docker-compose exec mssql /opt/mssql-tools/bin/sqlcmd -S localhost -U SA -P 'StrongPassword1' -Q 'CREATE DATABASE test_db'

export TURBODBC_TEST_CONFIGURATION_FILES=query_fixtures_mysql.json,query_fixtures_postgresql.json,query_fixtures_mssql.json
docker-compose run turbodbc /turbodbc/contrib/docker/build_and_test.sh
