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

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fakecpp.h"

/**
 * This program takes the args it was passed, as it masquerades as a c/c++ compiler and launches clang with
 * a set of options that makes it create an AST in /tmp
 * If successfull, will parse said AST and store the results into the sqlite database mentioned in environment
 * variable CPPMORTDB, passed from cppmort executable acting like make.
 */
int main(int argc,char *argv[])
{
    if (!initDatabase())
        return 5;

    // Not really parsing, more like collecting
    std::vector<std::string> args = parseArgs(argc,argv);
    if (args.size() == 1)
        return 1;

    // If we made it here, we have an AST in outfile and can parse it
    parseAst(args);
    return 0;
}

std::vector<std::string> parseArgs(int argc,char **argv)
{
    std::vector<std::string> rv;
    argc--; argv++;
    while (argc)
    {
        std::string arg = *argv;
        rv.push_back(arg);
        argv++; argc--;
    }
    rv.push_back("-idirafter");
    rv.push_back("/opt/llvm/lib/clang/12.0.0/include");
    return rv;
}

std::string findClangExecutable()
{
    char *pathEnv = strdup(getenv("PATH"));
    char *token = strtok(pathEnv,":");
    while (token)
    {
        std::string candidate = token;
        candidate.append("/clang");
        struct stat s;
        if (stat(candidate.c_str(),&s) == 0)
        {
            free(pathEnv);
            return candidate;
        }
        token = strtok(nullptr,":");
    }
    free(pathEnv);
    return "";
}

std::string outTmpFile()
{
    char *tmppath = strdup(P_tmpdir "/XXXXXX");
    int fd = mkstemp(tmppath);
    ::close(fd);
    std::string rv = tmppath;
    rv.append(".ast");
    ::rename(tmppath,rv.c_str());
    ::free(tmppath);
    return rv;
}

pid_t launchProcess(std::vector<std::string> &args,std::string clangExec)
{
    pid_t pid = fork();

    if (pid != 0)
        return pid; // Parent process

    const char **argList = (const char **)malloc((args.size() + 1) * sizeof(char *));
    memset(argList,0,(args.size() + 1) * sizeof(char *));
    int idx = 0;
    for (std::string s: args)
    {
        argList[idx] = args[idx].c_str();
        idx++;
    }
    execv(clangExec.c_str(),const_cast<char *const *>(argList));
    ::exit(0); // Should never reach here
    return 0;
}

functionRef::functionRef()
{
    type = refType::none;
    ns = "";
    funcName = "";
    lineNo = 0;
    className = "";
    filename = "<none>";
}
