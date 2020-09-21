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
#include <string.h>
#include <stdio.h>
#include "functionref.h"

functionRef::functionRef()
{
    type = NONE;
    filename = "<none>";
    lineNo = 0;
    calledFromFilename = "<none>";
    calledFromLineno = 0;
    signature = "";
}

bool functionRef::fromDbEntry(size_t keySize,void *keyData,size_t entrySize,void *entryData)
{
    // First the key, thats easy (filename, lineno)
    char *kp = reinterpret_cast<char *>(keyData);
    kp[keySize] = 0;
    size_t i = 0;
    while (i < keySize && kp[i] != ':') i++;
    kp[i] = 0;
    filename = kp;
    lineNo = strtoul(&kp[i+1],nullptr,10);

    // Now the type
    kp = reinterpret_cast<char *>(entryData);
    kp[entrySize] = 0;
    switch (*kp)
    {
    case 'L': type = LIBCALL; break;
    case 'D': type = FUNCDECL; break;
    case 'C': type = FUNCCALL; break;
    case 'B': type = FUNCBODY; break;
    case 'N': type = NONE; break;
    }

    // Function signature next
    kp += 2;
    i = 0;
    while (i < entrySize && kp[i] != ';') i++;
    kp[i] = 0;
    signature = kp;
    kp += i+1;
    i=0;
    while (i < entrySize && kp[i] != ';') i++;
    kp[i] = 0;
    calledFromFilename = kp;
    kp += i+1;
    calledFromLineno = strtoul(kp,nullptr,10);

    return true;
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

bool operator==(const functionRef &l,const functionRef &r)
{
    if (l.type != r.type)
        return false;
    if (l.lineNo != r.lineNo)
        return false;
    if (l.calledFromLineno != r.calledFromLineno)
        return false;
    if (l.signature != r.signature)
        return false;
    if (l.calledFromFilename != r.calledFromFilename)
        return false;
    if (l.filename != r.filename)
        return false;
    return true;
}

bool operator<(const functionRef &l,const functionRef &r)
{
    if (l.signature != r.signature)
        return (l.signature < r.signature);
    if (l.filename != r.filename)
        return (l.filename < r.filename);
    if (l.calledFromFilename != r.calledFromFilename)
        return (l.calledFromFilename < r.calledFromFilename);
    if (l.lineNo < r.lineNo)
        return (l.lineNo < r.lineNo);
    return l.calledFromLineno < r.calledFromLineno;
}

bool operator==(const SourceLocation &l,const SourceLocation &r)
{
    return (l.filename == r.filename && l.lineno == r.lineno);
}

bool operator<(const SourceLocation &l,const SourceLocation &r)
{
    if (l.filename != r.filename)
        return (l.filename < r.filename);
    return (l.lineno < r.lineno);
}

bool CallDef::import(functionRef &f)
{
    if (signature.length() && f.signature != signature)
        return false;
    if (!signature.length())
        signature = f.signature;
    if (f.type == functionRef::FUNCBODY)
    {
        if (!declaration.filename.size())
        {
            declaration.filename = f.filename;
            declaration.lineno = f.lineNo;
        }
        body.filename = f.filename;
        body.lineno = f.lineNo;
    }
    if (f.type == functionRef::FUNCDECL)
    {
        if (declaration.filename.size() && declaration.filename != f.filename)
            return false;
        declaration.filename = f.filename;
        declaration.lineno = f.lineNo;
    }
    if (f.type == functionRef::LIBCALL)
    {
        if (declaration.filename.size() && declaration.filename != f.filename)
            return false;
        declaration.filename = f.filename;
        declaration.lineno = f.lineNo;
        body = declaration;
        SourceLocation s;
        s.filename = f.calledFromFilename;
        s.lineno = f.calledFromLineno;
        callers.push_back(s);
    }
    if (f.type == functionRef::FUNCCALL)
    {
        if (declaration.filename.size() && declaration.filename != f.filename)
            return false;
        declaration.filename = f.filename;
        declaration.lineno = f.lineNo;
        SourceLocation s;
        s.filename = f.calledFromFilename;
        s.lineno = f.calledFromLineno;
        callers.push_back(s);
    }
    return true;
}
