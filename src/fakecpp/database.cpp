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
#include "fakecpp.h"

static bool dbFileExists = false;
static std::string dbFileName = "";

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
    struct stat s;
    int rv = stat(dbFile,&s);
    if (rv == 0)
    {
        dbFileExists = true;
    }
    return true;
}
void storeEntries (std::vector<functionRef> &refs)
{
    for (functionRef r: refs)
    {
        size_t l1,l2;
        char *t1,*t2;
        if (r.toDbEntry(&l1,(void **)&t1,&l2,(void **)&t2))
        {
            std::cout << t1 << " -> " << t2 << std::endl;
        }
    }
}
