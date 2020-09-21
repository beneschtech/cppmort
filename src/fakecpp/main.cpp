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
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fakecpp.h"

// From clangdirs.cpp
void addClangDirs(std::vector<std::string> &args);

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
        return 0;

    // If we made it here, we have an AST in outfile and can parse it
    parseAst(args);
    return 0;
}

std::string fileExtension(std::string filename)
{
    std::string rv;
    size_t len = filename.length();
    len--;
    while (len && filename[len] != '.')
        len--;
    if (!len) // executable name
        return "";
    rv = filename.substr(len+1);
    return rv;
}

std::vector<std::string> parseArgs(int argc,char **argv)
{
    std::vector<std::string> rv,args,files;
    std::string command = "clang";
    argc--; argv++;

    while (argc)
    {
        std::string arg = *argv;
        if (arg == "-o") { // skip output files
            argv+=2; argc-=2;
            continue;
        }
        if (arg[0] == '-')
        {
            args.push_back(arg);
        } else {
            if (fileExtension(arg) != "o")
                files.push_back(arg);
        }
        argv++; argc--;
    }
    rv.push_back(command);
    if (!files.size()) // Most likely a linker command
    {
        return rv;
    }
    for (std::string arg: files)
    {
        rv.push_back(arg);
    }
    rv.push_back("--");
    for (std::string arg: args)
    {
        rv.push_back(arg);
    }
    addClangDirs(rv);
    return rv;
}

