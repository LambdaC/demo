#include <Windows.h>
#include <sql.h>
#include <sqlext.h>
#include <cstdio>

#define MAX_DATA 100

int main() {

    RETCODE rc;                 /* Return code for ODBC function */
    HENV henv;                  /* Environment handle */
    HDBC hdbc;                  /* Connection handle */
    HSTMT hstmt;                /* Statement handle */
    wchar_t szData[MAX_DATA];      /* Variable to hold data retrieved */
    SDWORD cbData;              /* Output length of data */

    SQLAllocEnv(&henv);
    SQLAllocConnect(henv, &hdbc);

    SQLConnect(hdbc, (SQLWCHAR*)L"Account", SQL_NTS, (SQLWCHAR*)L"sa", SQL_NTS, (SQLWCHAR*)L"password", SQL_NTS);

    SQLAllocStmt(hdbc, &hstmt);

    SQLExecDirect(hstmt, (SQLWCHAR*)L"select * from TBL_ACCOUNT", SQL_NTS);

    for (rc = SQLFetch(hstmt); rc == SQL_SUCCESS; rc = SQLFetch(hstmt)) {
        SQLGetData(hstmt, 1, SQL_C_TCHAR, szData, sizeof(szData), &cbData);
        wprintf(L"%s\n", szData);
    }

    SQLFreeStmt(hstmt, SQL_DROP);
    SQLDisconnect(hdbc);
    SQLFreeConnect(hdbc);
    SQLFreeEnv(henv);

    return 0;
}
