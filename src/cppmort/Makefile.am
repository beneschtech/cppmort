bin_PROGRAMS = cppmort
cppmort_CXXFLAGS = $(CXXFLAGS) -I$(top_srcdir) -I$(top_srcdir)/src/lib
cppmort_CFLAGS = $(CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/src/lib
cppmort_SOURCES = main.cpp CPPMConfig.cpp OutputDir.cpp SrcDb.cpp analysis.cpp
cppmort_LDADD = \
    -L$(top_srcdir)/src/lib -lcppmort \
    $(BDB_LIBS)
