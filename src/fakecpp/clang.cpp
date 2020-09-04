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
#include <clang-c/Index.h>
#include <clang/AST/PrettyPrinter.h>
#include <iostream>
#include <limits.h>
#include <cstdlib>
#include <cstring>
#include "fakecpp.h"

static std::vector<std::string> ignoredDirs = { "/usr/include","/usr/lib" };

CXChildVisitResult visitor( CXCursor cursor, CXCursor /* parent */, CXClientData clientData );
static std::vector<functionRef> refs;

void parseAst(std::string filename)
{
    CXIndex index = clang_createIndex(0,1);
    CXTranslationUnit tu = clang_createTranslationUnit(index,filename.c_str());
    if (!tu)
        return;
    CXCursor rootCursor = clang_getTranslationUnitCursor(tu);
    functionRef r;
    clang_visitChildren(rootCursor,visitor,&r);
    clang_disposeTranslationUnit(tu);
    clang_disposeIndex(index);
    storeEntries(refs);
}

std::string getStdString(const CXString &s)
{
    std::string rv = clang_getCString(s);
    clang_disposeString(s);
    return rv;
}

std::string getCursorKindName( CXCursorKind cursorKind )
{
  CXString kindName  = clang_getCursorKindSpelling( cursorKind );
  std::string result = clang_getCString( kindName );

  clang_disposeString( kindName );
  return result;
}

std::string getCursorSpelling( CXCursor cursor )
{
  CXString cursorSpelling = clang_getCursorSpelling( cursor );
  std::string result      = clang_getCString( cursorSpelling );

  clang_disposeString( cursorSpelling );
  return result;
}

bool isAllowedDirectory(std::string f)
{
    for (std::string p: ignoredDirs)
    {
        if (p == f.substr(0,p.length()))
            return false;
    }
    return true;
}

bool isFunctionImplementation(CXCursor &cursor,std::string &decl,std::string &filename,unsigned &lineno)
{
    std::string cs = getStdString(clang_getCursorPrettyPrinted(cursor,nullptr));
    if (cs.find('{') == std::string::npos) // Just a declaration, not the "meat" of the function, so we dont care
        return false;
    clang::LangOptions lo;
    struct clang::PrintingPolicy pol(lo);
    pol.adjustForCPlusPlus();
    pol.TerseOutput = true;
    pol.FullyQualifiedName = true;
    decl = getStdString(clang_getCursorPrettyPrinted(cursor,&pol));
    CXSourceLocation location = clang_getCursorLocation( cursor );
    CXFile f;
    lineno = 0;
    filename = "(None)";
    clang_getSpellingLocation(location,&f,&lineno,nullptr,nullptr);
    if (lineno)
    {
        filename = getStdString(clang_File_tryGetRealPathName(f));
    }
    return isAllowedDirectory(filename);
}

CXChildVisitResult visitor( CXCursor cursor, CXCursor /* parent */, CXClientData clientData )
{
  struct functionRef *r = reinterpret_cast<struct functionRef *>(clientData);

  CXCursorKind cursorKind = clang_getCursorKind( cursor );

  switch (cursorKind)
  {
  case CXCursor_FunctionDecl:
  case CXCursor_FunctionTemplate:
  case CXCursor_CXXMethod:
  case CXCursor_Constructor:
  case CXCursor_Destructor:
  {
      std::string decl,filename;
      unsigned lineno;
      if (isFunctionImplementation(cursor,decl,filename,lineno))
      {
          std::cout << "Found " << decl << " at " << filename << ":" << lineno << std::endl;
          struct functionRef cr;
          cr.type = refType::funcImplementation;
          cr.lineNo = lineno;
          cr.funcName = decl;
          cr.filename = filename;
          refs.push_back(cr);
      }
      break;
  }
  case CXCursor_CallExpr:
  {
      std::string decl,filename;
      unsigned lineno;
      if (isFunctionImplementation(cursor,decl,filename,lineno))
      {
          std::cout << "Found " << decl << " at " << filename << ":" << lineno << std::endl;
          struct functionRef cr;
          cr.type = refType::funcImplementation;
          cr.lineNo = lineno;
          cr.funcName = decl;
          cr.filename = filename;
          refs.push_back(cr);
      }
      break;
  }
  default:;
  }
  clang_visitChildren(cursor,visitor,r);
  return CXChildVisit_Continue;
}
