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
#ifndef FAKECPP_H
#define FAKECPP_H

#include <vector>
#include <string>
#include <sys/types.h>

class functionRef
{
public:
    enum ftype {
        NONE,
        FUNCDECL,
        FUNCCALL,
        FUNCBODY,
        LIBCALL
    };

    functionRef();
    ftype type;
    std::string filename;
    unsigned lineNo;
    std::string signature;
    std::string calledFromFilename;
    unsigned calledFromLineno;
    bool toDbEntry(size_t *keySize,void **keyData,size_t *entrySize,void **entryData);
};

std::vector<std::string> parseArgs(int,char **);
void parseAst(std::vector<std::string> &args);
bool initDatabase();
void storeEntries (std::vector<functionRef> &);

#endif // FAKECPP_H
