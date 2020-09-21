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
#ifndef CPPMORT_H
#define CPPMORT_H

#include <string>
#include <vector>
#include <functionref.h>
#include <map>
#include <algorithm>

#include "CPPMConfig.h"
#include "OutputDir.h"
#include "SrcDb.h"

#define PACKAGE_DESCRIPTION "Cppmort detects dead code in a C/C++ project.\n" \
    "It does so by invoking 'make' or optionally 'gmake' on your project in the current directory.\n" \
    "It creates a BDB database named cppmort.db (overwritten each run) which it then analyzes to create a report in a specified directory.\n" \
    "It uses the LLVM / Clang tools to examine the source code, so your code must compile under clang"
extern char *_progname;

bool parseArgs(Config &,int argc, char **argv);
void usage(int exitCode=0) __attribute__((noreturn));
void performAnalysis();

// Analysis functions
void libraryCallAnalysis(SrcDB &db);
typedef std::map<std::string,std::vector<CallDef> > CallMap;
void libraryCallOutput(CallMap &);
CallDef *findMatchingCallDef(CallMap &,functionRef &r);
CallDefList deadCodeAnalysis(SrcDB &);
void deadCodeOutput(CallDefList &);

#endif // CPPMORT_H
