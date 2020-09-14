bin_PROGRAMS = fakecpp
fakecpp_CXXFLAGS = $(CXXFLAGS) $(LLVMCXXFLAGS) -I$(top_srcdir)
fakecpp_CFLAGS = $(CFLAGS) $(LLVMCFLAGS) -I$(top_srcdir)
fakecpp_SOURCES = main.cpp clang.cpp database.cpp functionRef.cpp clangdirs.cpp
fakecpp_LDADD = \
    $(LLVMLDFLAGS) \
     -lclangASTMatchers \
     -lclangTooling \
     -lclangDriver \
     -lclangFrontend \
     -lclangSerialization \
     -lclangParse \
     -lclangSema \
     -lclangEdit \
     -lclangLex \
     -lclangAnalysis \
     -lclangAST \
     -lclangDriver \
     -lclangBasic \
    $(LLVMLIBS) \
    $(LLVMSYSLIBS) \
    $(BDB_LIBS)

BUILT_SOURCES = clangdirs.cpp
CLEANFILES = clangdirs.cpp
clangdirs.cpp: Makefile
	./addClangDir.sh
