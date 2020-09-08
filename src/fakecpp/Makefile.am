
bin_PROGRAMS = fakecpp
fakecpp_CXXFLAGS = $(CXXFLAGS) $(LLVMCXXFLAGS)
fakecpp_CFLAGS = $(CFLAGS) $(LLVMCFLAGS)
fakecpp_SOURCES = main.cpp clang.cpp sqlite.cpp
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
    -lsqlite3
