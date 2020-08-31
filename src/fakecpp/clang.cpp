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
#include <iostream>
#include <limits.h>
#include <cstdlib>
#include <cstring>
#include "fakecpp.h"

#define IGNORE_DIRECTORY "/usr/include"

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

CXChildVisitResult visitor( CXCursor cursor, CXCursor /* parent */, CXClientData clientData )
{
  struct functionRef *r = reinterpret_cast<struct functionRef *>(clientData);
  CXSourceLocation location = clang_getCursorLocation( cursor );
  CXFile f;
  unsigned lineNo = 0;
  std::string fileName = "(None)";
  clang_getSpellingLocation(location,&f,&lineNo,nullptr,nullptr);
  if (lineNo)
  {
      CXString clfilename = clang_File_tryGetRealPathName(f);
      if (clfilename.data)
      {
          char actpath[PATH_MAX+1];
          memset(actpath,0,PATH_MAX+1);
          fileName = realpath(reinterpret_cast<const char *>(clfilename.data),actpath);
      }
  }

  if (fileName.substr(0,strlen(IGNORE_DIRECTORY)) == IGNORE_DIRECTORY)
  {
      clang_visitChildren(cursor,visitor,r);
      return CXChildVisit_Continue;
  }
  CXCursorKind cursorKind = clang_getCursorKind( cursor );
  std::string cursorTypeStr = getCursorKindName(cursorKind);
  std::string cursorName = getCursorSpelling(cursor);

  std::cout << r->level << ":" << fileName << ":" << lineNo << " -> " <<
               cursorTypeStr << " (" <<
               cursorName << ")\n";

  switch (cursorKind)
  {
  case CXCursor_Namespace:
  {
      struct functionRef cr(*r);
      if (cr.ns.length())
      {
          cr.ns.append("::");
          cr.ns.append(cursorName);
      } else {
          cr.ns = cursorName;
      }
      cr.level++;
      clang_visitChildren(cursor,visitor,&cr);
      break;
  }
  case CXCursor_FunctionDecl:
  case CXCursor_FunctionTemplate:
  case CXCursor_CXXMethod:
  case CXCursor_Constructor:
  case CXCursor_Destructor:
  {
      struct functionRef cr(*r);
      cr.type = refType::funcTemplate;
      cr.lineNo = lineNo;
      cr.funcName = cursorName;
      cr.filename = fileName;
      cr.level++;
      clang_visitChildren(cursor,visitor,&cr);
      refs.push_back(cr);
      break;
  }
  case CXCursor_CallExpr:
  {
      struct functionRef cr(*r);
      cr.type = refType::funcCall;
      cr.lineNo = lineNo;
      cr.funcName = cursorName;
      cr.filename = fileName;
      cr.level++;
      clang_visitChildren(cursor,visitor,&cr);
      refs.push_back(cr);
      break;
  }
  case CXCursor_ClassDecl:
  case CXCursor_ClassTemplate:
  {
      struct functionRef cr(*r);
      cr.className = cursorName;
      cr.level++;
      clang_visitChildren(cursor,visitor,&cr);
      break;
  }
  case CXCursor_CompoundStmt:
  case CXCursor_ReturnStmt:
  {
      if (r->type == refType::funcTemplate)
          r->type = refType::funcImplementation;
      clang_visitChildren(cursor,visitor,r);
      break;
  }
  case CXCursor_DeclRefExpr:
  {
      if (r->type == refType::funcCall && !r->className.length())
          r->className = cursorName;
      clang_visitChildren(cursor,visitor,r);
      break;
  }
  case CXCursor_NamespaceRef:
  {
      if (r->type == refType::funcCall && !r->ns.length())
          r->ns = cursorName;
      clang_visitChildren(cursor,visitor,r);
      break;
  }
  default:
      clang_visitChildren(cursor,visitor,r);
  }

  return CXChildVisit_Continue;
}
