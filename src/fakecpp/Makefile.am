bin_PROGRAMS = fakecpp
fakecpp_CXXFLAGS = $(CXXFLAGS) $(LLVMCXXFLAGS) -I$(top_srcdir) -I$(top_srcdir)/src/lib
fakecpp_CFLAGS = $(CFLAGS) $(LLVMCFLAGS) -I$(top_srcdir) -I$(top_srcdir)/src/lib
fakecpp_SOURCES = main.cpp clang.cpp database.cpp clangdirs.cpp
fakecpp_LDADD = \
    -L$(top_srcdir)/src/lib -lcppmort \
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
