#include <sqlite3.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "fakecpp.h"

const char *createTablesSql =
"PRAGMA foreign_keys=OFF; "
"BEGIN TRANSACTION; "
"CREATE TABLE calls ( namespace TEXT, class TEXT, funcname TEXT, filename TEXT, lineno INTEGER ); "
"CREATE TABLE deffuncs ( namespace TEXT, class TEXT, funcname TEXT, filename TEXT, lineno INTEGER ); "
"CREATE UNIQUE INDEX call_idx ON calls (namespace,class,funcname,filename,lineno); "
"CREATE UNIQUE INDEX def_idx ON deffuncs (filename,lineno); "
"COMMIT;";

static int checkTablesCallback(void *,int,char **argv,char **)
{
    std::string tbName = *argv;
    if (tbName != "calls" && tbName != "call_idx" && tbName != "def_idx" && tbName != "deffuncs")
        return 1;
    return 0;
}
static sqlite3 *dbPtr = nullptr;

/**
 * @brief connectToDatabase - Connects to the sqlite database indicated by the environment variable CPPMORTDB
 *        checks for database validity and drops/recreates if an invalid format
 * @return handle to database if successfull, nullptr if not
 */
sqlite3 *connectToDatabase()
{
    sqlite3 *rv = nullptr;
    const char *dbFile = getenv("CPPMORTDB");
    if (!dbFile)
    {
        std::cerr << "CPPMORTDB not set!" << std::endl;
        return rv;
    }
    bool needToCreate = false;
    struct stat s;
    if (stat(dbFile,&s) != 0) // File doesnt exist
    {
        needToCreate = true;
    }
    int rc = sqlite3_open(dbFile,&rv);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQLITE Error: " << sqlite3_errmsg(rv) << std::endl;
        sqlite3_close(rv);
        return nullptr;
    }
    char *errmsg;
    rc = sqlite3_exec(rv,"SELECT name FROM sqlite_master",checkTablesCallback,nullptr,&errmsg);
    if (rc == SQLITE_ABORT)
    {
        sqlite3_close(rv);
        ::unlink(dbFile);
        rv = nullptr;
        rc = sqlite3_open(dbFile,&rv);
        needToCreate = true;
    } else if (rc != 0)
    {
        if (!errmsg)
        {
            errmsg = const_cast<char *>(sqlite3_errmsg(rv));
        }
        std::cerr << "SQLITE Error: " << errmsg << std::endl;
        sqlite3_free(errmsg);
        return nullptr;
    }

    if (needToCreate)
    {
        rc = sqlite3_exec(rv,createTablesSql,nullptr,nullptr,&errmsg);
        if (rc != SQLITE_OK)
        {
            std::cerr << "SQLITE Error: " << errmsg << std::endl;
            sqlite3_free(errmsg);
            sqlite3_close(rv);
            return nullptr;
        }
    }
    return rv;
}

bool initDatabase()
{
    dbPtr = connectToDatabase();
    return (dbPtr != nullptr);
}

void storeEntries (std::vector<struct functionRef> &refs)
{
    const char *callsPrepSql = "INSERT INTO calls (namespace,class,funcname,filename,lineno) VALUES (?,?,?,?,?)";
    const char *defsPrepSql = "INSERT INTO deffuncs (namespace,class,funcname,filename,lineno) VALUES (?,?,?,?,?)";
    sqlite3_stmt *callsIns = nullptr,*defsIns = nullptr;
    int rc = 0;
    rc = sqlite3_prepare_v2(dbPtr,callsPrepSql,-1,&callsIns,nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQLITE Error: " << sqlite3_errmsg(dbPtr) << std::endl;
        return;
    }
    rc = sqlite3_prepare_v2(dbPtr,defsPrepSql,-1,&defsIns,nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQLITE Error: " << sqlite3_errmsg(dbPtr) << std::endl;
        return;
    }
    for (functionRef r: refs)
    {
        if (r.filename.substr(0,12) == "/usr/include")
            continue;
        switch (r.type)
        {
        case refType::funcCall:
        {
            sqlite3_bind_text(callsIns,1,r.ns.c_str(),r.ns.length(),SQLITE_STATIC);
            sqlite3_bind_text(callsIns,2,r.className.c_str(),r.className.length(),SQLITE_STATIC);
            sqlite3_bind_text(callsIns,3,r.funcName.c_str(),r.funcName.length(),SQLITE_STATIC);
            sqlite3_bind_text(callsIns,4,r.filename.c_str(),r.filename.length(),SQLITE_STATIC);
            sqlite3_bind_int(callsIns,5,r.lineNo);
            sqlite3_step(callsIns);
            sqlite3_reset(callsIns);
            break;
        }
        case refType::funcImplementation:
        {
            sqlite3_bind_text(defsIns,1,r.ns.c_str(),r.ns.length(),SQLITE_STATIC);
            sqlite3_bind_text(defsIns,2,r.className.c_str(),r.className.length(),SQLITE_STATIC);
            sqlite3_bind_text(defsIns,3,r.funcName.c_str(),r.funcName.length(),SQLITE_STATIC);
            sqlite3_bind_text(defsIns,4,r.filename.c_str(),r.filename.length(),SQLITE_STATIC);
            sqlite3_bind_int(defsIns,5,r.lineNo);
            sqlite3_step(defsIns);
            sqlite3_reset(defsIns);
            break;
        }
        default:;
        }
    }
    sqlite3_finalize(callsIns);
    sqlite3_finalize(defsIns);
}
