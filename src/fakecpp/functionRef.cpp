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
#include <fakecpp.h>
#include <string.h>
#include <stdio.h>

functionRef::functionRef()
{
    type = NONE;
    filename = "<none>";
    lineNo = 0;
    calledFromFilename = "<none>";
    calledFromLineno = 0;
    signature = "";
}

bool functionRef::toDbEntry(size_t *keySize, void **keyData, size_t *entrySize, void **entryData)
{
    if (type == NONE)
        return false;

    char buf[4096];

    // First set the key, which is filenane:lineno
    memset(buf,0,sizeof(buf));
    snprintf(buf,sizeof(buf)-1,"%s:%u",filename.c_str(),lineNo);
    *keyData = strdup(buf);
    *keySize = strlen(buf);

    // Data is type;signature;calledfile;calledline
    char entType;
    switch (type)
    {
    case NONE: entType = 'N'; break;
    case FUNCBODY: entType = 'B'; break;
    case FUNCCALL: entType = 'C'; break;
    case FUNCDECL: entType = 'D'; break;
    case LIBCALL: entType = 'L'; break;
    }

    memset(buf,0,sizeof(buf));
    snprintf(buf,sizeof(buf)-1,"%c;%s;%s;%u",entType,signature.c_str(),calledFromFilename.c_str(),calledFromLineno);
    *entryData = strdup(buf);
    *entrySize = strlen(buf);
    return true;
}

