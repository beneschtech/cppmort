AC_INIT([codemort],[0.0.1],[chris@beneschtech.com])
AM_INIT_AUTOMAKE
AC_CONFIG_FILES([Makefile src/Makefile src/fakecpp/Makefile])
AC_PROG_CXX
AC_ARG_WITH([llvm],
  [AS_HELP_STRING([--with-llvm],[Location of llvm-config executable])],
  [LLVMCONFIG=$withval],
  [LLVMCONFIG=llvm-config])
AS_IF([test -x $LLVMCONFIG],[AS_ECHO_N('Using LLVM ')],[AS_EXIT([1])])
[
$LLVMCONFIG --version
]

AC_CHECK_LIB([sqlite3],[sqlite3_free],
[
],
[
    echo "sqlite3 not found"
    exit 1
])

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

AC_SUBST(LLVMCXXFLAGS,[`$LLVMCONFIG --cxxflags`])
AC_SUBST(LLVMCFLAGS,[`$LLVMCONFIG --cflags`])
AC_SUBST(LLVMLDFLAGS,[`$LLVMCONFIG --ldflags`])
AC_SUBST(LLVMSYSLIBS,[`$LLVMCONFIG --system-libs`])
AC_SUBST(LLVMLIBS,[`$LLVMCONFIG --libs`])
AC_OUTPUT
