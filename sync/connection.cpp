#include "connection.h"
#include "../SQLArchiverResources.hxx"
#include <ErrHdl.hxx>
#include <stdlib.h>
#include <sstream>

void sqlError(SQLSMALLINT handleType, SQLHANDLE handle) {
	SQLWCHAR sqlState[6];
	SQLINTEGER nativeError;
	SQLWCHAR messageTextW[8000];
	SQLSMALLINT textLength;
	if (SQLGetDiagRec(handleType, handle, 1, sqlState, &nativeError, messageTextW, 8000, &textLength) == SQL_SUCCESS) {
		char messageText[8000];
		wcstombs(messageText, messageTextW, 8000);
		CharString errorMessage(messageText, textLength);
		ErrHdl::error(ErrClass::PRIO_SEVERE,
			ErrClass::ERR_SYSTEM,
			ErrClass::UNEXPECTEDSTATE,
			"connection.cpp",              // our file name
			"sqlError",                      // our function name
			errorMessage);
	}
	else {
		ErrHdl::error(ErrClass::PRIO_SEVERE,
			ErrClass::ERR_SYSTEM,
			ErrClass::UNEXPECTEDSTATE,
			"connection.cpp",              // our file name
			"sqlError",                      // our function name
			"Could not get diagnostic message from SQL Server");
	}
}

bool createConnection(SQLHANDLE& env, SQLHANDLE& conn) {
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env) != SQL_SUCCESS) {
		ErrHdl::error(ErrClass::PRIO_SEVERE,
			ErrClass::ERR_SYSTEM,
			ErrClass::UNEXPECTEDSTATE,
			"connection.cpp",              // our file name
			"createConnection",                      // our function name
			"Could not create environment handle");
		return false;
	}
	if (SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0) != SQL_SUCCESS) {
		ErrHdl::error(ErrClass::PRIO_SEVERE,
			ErrClass::ERR_SYSTEM,
			ErrClass::UNEXPECTEDSTATE,
			"connection.cpp",              // our file name
			"createConnection",                      // our function name
			"Could not set environment attributes");
		return false;
	}

	if (SQLAllocHandle(SQL_HANDLE_DBC, env, &conn) != SQL_SUCCESS) {
		sqlError(SQL_HANDLE_ENV, env);
		return false;
	}

	std::wstringstream connString;
	connString << L"DRIVER={SQL Server};Server=" << SQLArchiverResources::getServer() << L", " << SQLArchiverResources::getDatabasePort()
		<< L";DATABASE=" << SQLArchiverResources::getDatabase() << L";TrustServerCertificate=true;";
	if (SQLArchiverResources::getUsername().isEmpty()) {
		connString << L"Integrated Security=SSPI;";
	}
	else {
		connString << L"UID=" << SQLArchiverResources::getUsername() << L";Password=" << SQLArchiverResources::getPassword() << L";";
	}

	size_t connStringLen = connString.str().length();
	SQLWCHAR* inConnString = new SQLWCHAR[connStringLen + 1];
	wcsncpy(inConnString, connString.str().c_str(), connStringLen);
	SQLWCHAR outConnString[2048];
	SQLSMALLINT outConnStringSize;

	SQLRETURN ret = SQLDriverConnect(conn, NULL, inConnString, connStringLen, outConnString, 2048, &outConnStringSize, SQL_DRIVER_NOPROMPT);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		sqlError(SQL_HANDLE_DBC, conn);
		return false;
	}
	delete[] inConnString;
	
	return true;
}

bool closeConnection(SQLHANDLE& env, SQLHANDLE& conn) {
	SQLRETURN ret = SQLDisconnect(conn);	
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		sqlError(SQL_HANDLE_DBC, conn);
		return false;
	}
	ret = SQLFreeHandle(SQL_HANDLE_DBC, conn);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		sqlError(SQL_HANDLE_DBC, conn);
		return false;
	}
	ret = SQLFreeHandle(SQL_HANDLE_ENV, env);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		sqlError(SQL_HANDLE_ENV, env);
		return false;
	}
	return true;
}