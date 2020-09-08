#ifndef FAKECPP_H
#define FAKECPP_H

#include <vector>
#include <string>

std::vector<std::string> parseArgs(int,char **);
std::string findClangExecutable();
std::string outTmpFile();
void parseAst(std::vector<std::string> &args);
pid_t launchProcess(std::vector<std::string> &, std::string clangExec);

enum refType
{
    none,
    funcTemplate,
    funcCall,
    funcImplementation,
    templateDef
};

struct functionRef {
    refType type;
    std::string filename;
    unsigned lineNo;
    std::string ns;
    std::string className;
    std::string funcName;
    unsigned level;
    functionRef();
};

bool initDatabase();
void storeEntries (std::vector<struct functionRef> &);

#endif // FAKECPP_H
