/**
 *
 *    ######   #######  #     #  #######   #####    #####   #     #  #######  #######   #####   #     #
 *   #     #  #        ##    #  #        #     #  #     #  #     #     #     #        #     #  #     #
 *   #     #  #        # #   #  #        #        #        #     #     #     #        #        #     #
 *  ######   #####    #  #  #  #####     #####   #        #######     #     #####    #        #######
 *  #     #  #        #   # #  #              #  #        #     #     #     #        #        #     #
 * #     #  #        #    ##  #        #     #  #     #  #     #     #     #        #     #  #     #
 * ######   #######  #     #  #######   #####    #####   #     #     #     #######   #####   #     #
 *
 * LLC
 *
 **/
#include <config.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <db.h>

#include "fakecpp.h"

static std::string dbFileName = "";
static DB *dbHandle = nullptr;

bool initDatabase()
{
    const char *dbFile = getenv("CPPMORTDB");
    if (dbFile)
    {
        dbFileName = dbFile;
    } else {
        std::cerr << "CPPMORTDB not set!" << std::endl;
        return false;
    }

    int ret = db_create(&dbHandle,nullptr,0);
    if (ret != 0)
    {
        std::cerr << "db_create: " << db_strerror(ret) << std::endl;
        return false;
    }
    dbHandle->set_flags(dbHandle,DB_DUP);
    ret = dbHandle->open(dbHandle,nullptr,dbFileName.c_str(),nullptr,DB_BTREE,DB_CREATE,0644);
    if (ret != 0)
    {
        dbHandle->err(dbHandle,ret,"%s",dbFile);
        dbHandle->close(dbHandle,0);
        return false;
    }

    return true;
}
void storeEntries (std::vector<functionRef> &refs)
{
    std::cout << "Storing results to " << dbFileName << std::endl;
    for (functionRef r: refs)
    {
        size_t l1,l2;
        char *t1,*t2;
        DBT key,data;
        if (r.toDbEntry(&l1,(void **)&t1,&l2,(void **)&t2))
        {
            memset(&key,0,sizeof(key));
            memset(&data,0,sizeof(data));
            key.size = l1;
            key.data = t1;
            data.size = l2;
            data.data = t2;
            int ret = dbHandle->put(dbHandle,nullptr,&key,&data,0);
            functionRef tr;
            free(t1);
            free(t2);
            if (ret != 0 && ret != DB_KEYEXIST)
            {
                dbHandle->err(dbHandle,ret,"DB->put");
                std::cerr << "Bad entry: " << t1 << " -> " << t2 << std::endl;
                return;
            }

        }
    }
    dbHandle->close(dbHandle,0);
}
