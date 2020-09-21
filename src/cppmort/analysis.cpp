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
#include <map>
#include <iostream>
#include <libgen.h>
#include "cppmort.h"

void performAnalysis()
{
    SrcDB db;
    if (!db.loadDatabase("cppmort.db"))
        return;

    //libraryCallAnalysis(db);
    deadCodeAnalysis(db);
}

void libraryCallAnalysis(SrcDB &db)
{
    functionRefList libList = db.getLibraryEntries();
    CallMap libCallMap;
    for (functionRef r: libList)
    {
        CallDef *cdp = findMatchingCallDef(libCallMap,r);
        if (!cdp)
        {
            CallDef def;
            def.import(r);
            libCallMap[r.signature].push_back(def);
            continue;
        }
        cdp->import(r);
    }
    libraryCallOutput(libCallMap);
}

CallDefList deadCodeAnalysis(SrcDB &db)
{
    functionRefList dlist = db.getEntries();
    CallMap dCallMap;
    for (functionRef r: dlist)
    {
        CallDef *cdp = findMatchingCallDef(dCallMap,r);
        if (!cdp)
        {
            CallDef def;
            def.import(r);
            dCallMap[r.signature].push_back(def);
            continue;
        }
        cdp->import(r);
    }
    CallDefList rv;
    for (auto it: dCallMap)
    {
        for (auto cdit: it.second)
        {
            if (cdit.callers.empty())
                rv.push_back(cdit);
        }
    }
    return rv;
}

void libraryCallOutput(CallMap &m)
{
    // Extract all the keys
    CallDefList l;
    for (auto it: m)
    {
        for (auto cdit: it.second)
            l.push_back(cdit);
    }
    struct {
        bool operator()(CallDef a,CallDef b) const
        {
            return a.declaration < b.declaration;
        }
    } fileNameSortCB;
    std::sort(l.begin(),l.end(),fileNameSortCB);
    std::string currFilename = "";
    for (CallDef cd: l)
    {
        if (currFilename != cd.declaration.filename)
        {
            std::cout << cd.declaration.filename << std::endl;
            currFilename = cd.declaration.filename;
        }
        std::cout << "  Line " << cd.declaration.lineno << ": -> " << cd.signature << std::endl;
        if (cd.callers.size())
        {
            std::cout << "  Called by:" << std::endl;
            for (SourceLocation call: cd.callers)
            {
                std::cout << "    " << call.filename << ":" << call.lineno << std::endl;
            }
        }
    }
}

CallDef *findMatchingCallDef(CallMap &m,functionRef &r)
{
    auto it = m.find(r.signature);
    if (it == m.end())
        return nullptr;
    if (it->second.size() == 1)
        return &it->second.front();
    for (size_t i = 0; i < it->second.size(); i++)
    {
        CallDef *rv = &it->second[i];
        std::string declDir = dirname(const_cast<char *>(rv->declaration.filename.c_str()));
        std::string bodyDir = dirname(const_cast<char *>(rv->body.filename.c_str()));
        std::string funcDir = dirname(const_cast<char *>(r.filename.c_str()));
        if (declDir == funcDir || bodyDir == funcDir)
        {
            return rv;
        }
    }
    return nullptr;
}
