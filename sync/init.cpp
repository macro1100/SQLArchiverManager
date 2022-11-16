#include "init.h"
#include "connection.h"
#include <Types.hxx>
#include <Manager.hxx>
#include <thread>
#include <sstream>

#define SQLCHECK(statement, type, handle, freestmt) sqlRet = statement; \
	if (sqlRet != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) { \
		sqlError(type, handle); \
		if(freestmt) { \
			SQLFreeHandle(type, handle); \
		} \
		return; \
	}

namespace sync {

	void syncSystem(CharString, SystemNumType);
	void initSystem();

	void init() {
		initSystem();
	}

	void initSystem() {
		CharString sysName;
		SystemNumType sysNum;
		PVSSboolean ret = Manager::getSystemName(sysName);
		PVSSboolean retId = Manager::getSystemId(sysName, sysNum);
		if (!ret || !retId) {
			ErrHdl::error(ErrClass::PRIO_SEVERE,      // It is a severe error
				ErrClass::ERR_SYSTEM,
				ErrClass::UNEXPECTEDSTATE,  // fits all
				"init.cpp",              // our file name
				"init",                      // our function name
				"Could not get system name and number");
		}
		else {
			std::thread t(syncSystem, sysName, sysNum);
			t.detach();
		}
	}

	/// <summary>
	/// Sync the system name and number to the database. This is run on a separate thread,
	/// there can be no more calls to WinCC functions.
	/// </summary>
	/// <param name="sysName">The name of the system</param>
	/// <param name="sysNum">The number of the system</param>
	void syncSystem(CharString sysName, SystemNumType sysNum) {
		SQLHANDLE env;
		SQLHANDLE conn;
		bool ret = createConnection(env, conn);
		if (!ret)
			return;

		SQLHANDLE stmt;
		SQLRETURN sqlRet;
		SQLCHECK(SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt), SQL_HANDLE_DBC, conn, false)

			std::wstringstream query;
		query << L"SELECT COUNT(*) [COUNT] FROM [System] WHERE SYS_ID=" << sysNum.toPVSSulong();

		SQLCHECK(SQLExecDirect(stmt, (SQLWCHAR*)query.str().c_str(), SQL_NTS), SQL_HANDLE_STMT, stmt, true)

		SQLCHECK(SQLFetch(stmt), SQL_HANDLE_STMT, stmt, true)

		long rowCount = 0;
		SQLCHECK(SQLGetData(stmt, 1, SQL_C_LONG, &rowCount, 0, NULL), SQL_HANDLE_STMT, stmt, true)

		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		SQLCHECK(SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt), SQL_HANDLE_DBC, conn, false);

		query.str(std::wstring());
		SQLLEN cbValue = SQL_NTS;
		if (rowCount == 0) {
			query << L"INSERT INTO [System] (SYS_ID, SystemName) VALUES (" << sysNum.toPVSSulong() << L",?)";
		}
		else {
			query << L"UPDATE [System] SET SystemName=? WHERE SYS_ID=" << sysNum.toPVSSulong();
		}
		SQLCHECK(SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sysName.len(), 0, (SQLPOINTER)sysName.c_str(), sysName.len(), &cbValue), SQL_HANDLE_STMT, stmt, true)
		SQLCHECK(SQLExecDirect(stmt, (SQLWCHAR*)query.str().c_str(), SQL_NTS), SQL_HANDLE_STMT, stmt, true)
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		closeConnection(env, conn);
	}
}