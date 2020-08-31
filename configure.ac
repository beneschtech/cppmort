AC_INIT([codemort],[0.0.1],[chris@beneschtech.com])
AM_INIT_AUTOMAKE
AC_CONFIG_FILES([Makefile src/Makefile src/fakecpp/Makefile])
AC_PROG_CXX
AC_CHECK_LIB([clang],[clang_createIndex],
[
    LDFLAGS="$LDFLAGS -lclang"
],
[
    echo "libclang not found"
    exit 1
])
AC_CHECK_LIB([sqlite3],[sqlite3_free],
[
    LDFLAGS="$LDFLAGS -lsqlite3"
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

AC_OUTPUT
