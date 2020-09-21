#include <iostream>
#include <db.h>

#include "SrcDb.h"

SrcDB::SrcDB()
{
    myEntries.clear();
}

bool SrcDB::loadDatabase(std::string fileName)
{
    DB *dbp = nullptr;
    int ret = db_create(&dbp,nullptr,0);
    if (ret != 0)
    {
        std::cerr << "db_create: " << db_strerror(ret) << std::endl;
        return false;
    }
    dbp->set_errfile(dbp,stderr);
    ret = dbp->open(dbp,nullptr,fileName.c_str(),nullptr,DB_UNKNOWN,DB_RDONLY,0);
    if (ret != 0)
    {
        dbp->err(dbp,ret,"DB->open");
        return false;
    }
    DBC *dbc = nullptr;
    ret = dbp->cursor(dbp,nullptr,&dbc,0);
    if (ret != 0)
    {
        dbp->close(dbp,0);
        dbp->err(dbp,ret,"DB->cursor");
        return false;
    }
    DBT key,data;
    while ((ret = dbc->c_get(dbc,&key,&data,DB_NEXT)) == 0)
    {
        functionRef r;
        if (r.fromDbEntry(key.size,key.data,data.size,data.data))
        {
            if (r.type == functionRef::LIBCALL)
            {
                myLibEntries.push_back(r);
            } else {
                myEntries.push_back(r);
            }
        }
    }
    dbc->c_close(dbc);
    dbp->close(dbp,0);
    if (ret != DB_NOTFOUND)
    {
        dbp->err(dbp,ret,"DB->cursor");
        return false;
    }
    return true;
}

functionRefList SrcDB::getLibraryEntries()
{
    return myLibEntries;
}

functionRefList SrcDB::getEntries()
{
    return myEntries;
}
