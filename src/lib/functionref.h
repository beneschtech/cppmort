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
#ifndef FUNCTIONREF_H
#define FUNCTIONREF_H

#include <sys/types.h>
#include <string>
#include <vector>

class functionRef
{
public:    
    functionRef();
    enum ftype {
        NONE,
        FUNCDECL,
        FUNCCALL,
        FUNCBODY,
        LIBCALL
    };
    ftype type;
    std::string filename;
    unsigned lineNo;
    std::string signature;
    std::string calledFromFilename;
    unsigned calledFromLineno;
    bool toDbEntry(size_t *keySize,void **keyData,size_t *entrySize,void **entryData);
    bool fromDbEntry(size_t keySize,void *keyData,size_t entrySize,void *entryData);
};
bool operator==(const functionRef &l,const functionRef &r);
bool operator<(const functionRef &l,const functionRef &r);

typedef std::vector<functionRef> functionRefList;
struct SourceLocation {
    std::string filename;
    unsigned lineno;
    SourceLocation(): filename(""),lineno(0) {}
};
typedef std::vector<SourceLocation> LocationList;
bool operator==(const SourceLocation &l,const SourceLocation &r);
bool operator<(const SourceLocation &l,const SourceLocation &r);

struct CallDef {
    std::string signature;
    SourceLocation declaration;
    SourceLocation body;
    LocationList callers;
    CallDef():signature("") { callers.clear(); }
    bool import(functionRef &);
};
typedef std::vector<CallDef> CallDefList;


#endif // FUNCTIONREF_H
