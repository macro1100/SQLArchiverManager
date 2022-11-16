#pragma once

#include <Windows.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

bool createConnection(SQLHANDLE& env, SQLHANDLE& conn);
bool closeConnection(SQLHANDLE& env, SQLHANDLE& conn);
void sqlError(SQLSMALLINT handleType, SQLHANDLE handle);