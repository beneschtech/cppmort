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

/**
  * Clang specific functions for fakecpp
  */
#include <config.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
#include <iostream>
#include <limits.h>
#include <cstdlib>
#include <cstring>
#include "fakecpp.h"

//#define VERBOSE 1
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace clang;
using namespace llvm;

class CallExprCB : public MatchFinder::MatchCallback
{
public:
    virtual void run(const MatchFinder::MatchResult &);
};

class FunctionDeclCB: public MatchFinder::MatchCallback
{
public:
    virtual void run(const MatchFinder::MatchResult &);
};

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("fakecpp");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static llvm::cl::extrahelp MoreHelp("\nMore help text...");

static std::vector<functionRef> dbEntries;

void parseAst(std::vector<std::string> &callArgs)
{            
    int argSz = callArgs.size();
    const char ** argvals = (const char **)malloc(sizeof(char *)*argSz);
    for (size_t idx = 0; idx < callArgs.size(); idx++)
    {
        argvals[idx] = callArgs[idx].c_str();
    }
    clang::tooling::CommonOptionsParser OptionsParser(argSz, argvals, MyToolCategory);
    clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),OptionsParser.getSourcePathList());

    CallExprCB CEcb;
    FunctionDeclCB FDcb;
    clang::ast_matchers::MatchFinder CallFinder;

    clang::ast_matchers::StatementMatcher callMatcher = clang::ast_matchers::callExpr().bind("functions");
    clang::ast_matchers::DeclarationMatcher functionMatcher = clang::ast_matchers::functionDecl().bind("functions");

    CallFinder.addMatcher(callMatcher, &CEcb);
    CallFinder.addMatcher(functionMatcher,&FDcb);

    dbEntries.clear();
    Tool.run(clang::tooling::newFrontendActionFactory(&CallFinder).get());
    storeEntries(dbEntries);
}

std::string getFunctionSignature(const FunctionDecl *f)
{
    std::string rv;
    rv = f->getReturnType().getAsString();
    rv += " ";
    rv += f->getQualifiedNameAsString();
    if (f->param_empty())
        return rv+"()";
    rv += "(";
    for (auto it = f->param_begin(); it != f->param_end(); it++)
    {
        if (it != f->param_begin())
            rv += ",";
        rv += (*it)->getOriginalType().getAsString();
    }
    rv += ")";
    return rv;
}

void CallExprCB::run(const MatchFinder::MatchResult &Result) {
    ASTContext *Context = Result.Context;
    if (const CallExpr *E =
            Result.Nodes.getNodeAs<clang::CallExpr>("functions")) {
        FullSourceLoc callerLocation = Context->getFullLoc(E->getBeginLoc());
        if (callerLocation.isInSystemHeader() || !callerLocation.isValid())
        {
            return;
        }
        const FunctionDecl *target = E->getDirectCallee();
        FullSourceLoc targetLocation = Context->getFullLoc(target->getBeginLoc());
#ifdef VERBOSE
        std::string libInd = "";
        llvm::outs() << "Found call at " << callerLocation.getFileEntry()->tryGetRealPathName() << ":" << callerLocation.getSpellingLineNumber() << "\n";
        if (targetLocation.isInSystemHeader())
            libInd = " (library) ";
        llvm::outs() << " \\- " << libInd << targetLocation.getFileEntry()->tryGetRealPathName() << ":"
                     << targetLocation.getSpellingLineNumber() << " -> "
                     << getFunctionSignature(target) << "\n";
#endif
        functionRef r;
        if (targetLocation.isInSystemHeader())
        {
            r.type = functionRef::LIBCALL;
        } else {
            r.type = functionRef::FUNCCALL;
        }
        r.lineNo = targetLocation.getSpellingLineNumber();
        r.filename = targetLocation.getFileEntry()->tryGetRealPathName().data();
        r.calledFromLineno = callerLocation.getSpellingLineNumber();
        r.calledFromFilename = callerLocation.getFileEntry()->tryGetRealPathName().data();
        r.signature = getFunctionSignature(target);
        dbEntries.push_back(r);
    }
}

void FunctionDeclCB::run(const MatchFinder::MatchResult &Result) {
    ASTContext *Context = Result.Context;
    if (const FunctionDecl *E =
            Result.Nodes.getNodeAs<clang::FunctionDecl>("functions")) {
        FullSourceLoc funcLocation = Context->getFullLoc(E->getBeginLoc());
        if (funcLocation.isInSystemHeader() || !funcLocation.isValid())
        {
            return;
        }
        std::string funcName = getFunctionSignature(E);
#ifdef VERBOSE
        llvm::outs() << "Found " << funcName << " at " << funcLocation.getFileEntry()->tryGetRealPathName() << ":" << funcLocation.getSpellingLineNumber();
        if (E->isThisDeclarationADefinition())
            llvm::outs() << " (with body)";
        llvm::outs() << "\n";
#endif
        functionRef r;
        if (E->isThisDeclarationADefinition())
        {
            r.type = functionRef::FUNCBODY;
        } else {
            r.type = functionRef::FUNCDECL;
        }
        r.signature = funcName;
        r.lineNo = funcLocation.getSpellingLineNumber();
        r.filename = funcLocation.getFileEntry()->tryGetRealPathName().data();
        r.calledFromFilename = "";
        dbEntries.push_back(r);
    }
}
