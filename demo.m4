PHP_CHECK_LIBRARY(example-extra, example_critical_extra_function,
[
  dnl 添加所需路径
  PHP_ADD_INCLUDE($PHP_EXAMPLE_EXTRA/include)
  PHP_ADD_LIBRARY_WITH_PATH(example-extra, $PHP_EXAMPLE_EXTRA/lib, EXAMPLE_SHARED_LIBADD)
  AC_DEFINE(HAVE_EXAMPLEEXTRALIB,1,[Whether example-extra support is present and requested])
  EXAMPLE_SOURCES="$EXAMPLE_SOURCES example_extra.c"
],[
  AC_MSG_ERROR([example-extra lib not found. See config.log for more information.])
],[-L$PHP_EXAMPLE_EXTRA/lib]
)
