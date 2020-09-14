AC_INIT([codemort],[0.0.1],[chris@beneschtech.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
AC_CONFIG_FILES([Makefile src/Makefile src/fakecpp/Makefile])

## C++ project, need at least c++11
AC_LANG([C++])
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX(11,[],[mandatory])

## Autoscan results
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([realpath])
AC_CHECK_FUNCS([strdup])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADER_STDBOOL
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_TYPE_PID_T

## External libraries

### BDB
AC_CHECK_HEADERS([db.h],[],
[AS_EXIT(1)])
AC_SEARCH_LIBS([db_strerror],[db],[],[
AS_EXIT(1)])
AC_SUBST(BDB_LIBS,[-ldb])

### LLVM
[
LLVMCONFIG=llvm-config
]
AC_ARG_WITH([llvm],
  [AS_HELP_STRING([--with-llvm],[Location of llvm-config executable])],
  [LLVMCONFIG=$withval])
AS_IF([test -x $LLVMCONFIG],[AS_ECHO_N('Using LLVM ')],[AS_EXIT([1])])
[
$LLVMCONFIG --version
]
AC_SUBST(LLVMCXXFLAGS,[`$LLVMCONFIG --cxxflags`])
AC_SUBST(LLVMCFLAGS,[`$LLVMCONFIG --cflags`])
AC_SUBST(LLVMLDFLAGS,[`$LLVMCONFIG --ldflags`])
AC_SUBST(LLVMSYSLIBS,[`$LLVMCONFIG --system-libs`])
AC_SUBST(LLVMLIBS,[`$LLVMCONFIG --libs`])
AC_OUTPUT
