set(CMAKE_CXX_COMPILER "/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/arm-unknown-linux-uclibcgnueabi-g++")
set(CMAKE_CXX_COMPILER_ARG1 "")
set(CMAKE_CXX_COMPILER_ID "GNU")
set(CMAKE_CXX_COMPILER_VERSION "4.4.0")
set(CMAKE_CXX_COMPILER_VERSION_INTERNAL "")
set(CMAKE_CXX_COMPILER_WRAPPER "")
set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT "98")
set(CMAKE_CXX_COMPILE_FEATURES "cxx_std_98;cxx_template_template_parameters;cxx_std_11;cxx_auto_type;cxx_decltype;cxx_default_function_template_args;cxx_defaulted_functions;cxx_deleted_functions;cxx_extern_templates;cxx_func_identifier;cxx_generalized_initializers;cxx_inline_namespaces;cxx_long_long_type;cxx_right_angle_brackets;cxx_rvalue_references;cxx_sizeof_member;cxx_static_assert;cxx_strong_enums;cxx_trailing_return_types;cxx_unicode_literals;cxx_uniform_initialization;cxx_variadic_macros;cxx_variadic_templates")
set(CMAKE_CXX98_COMPILE_FEATURES "cxx_std_98;cxx_template_template_parameters")
set(CMAKE_CXX11_COMPILE_FEATURES "cxx_std_11;cxx_auto_type;cxx_decltype;cxx_default_function_template_args;cxx_defaulted_functions;cxx_deleted_functions;cxx_extern_templates;cxx_func_identifier;cxx_generalized_initializers;cxx_inline_namespaces;cxx_long_long_type;cxx_right_angle_brackets;cxx_rvalue_references;cxx_sizeof_member;cxx_static_assert;cxx_strong_enums;cxx_trailing_return_types;cxx_unicode_literals;cxx_uniform_initialization;cxx_variadic_macros;cxx_variadic_templates")
set(CMAKE_CXX14_COMPILE_FEATURES "")
set(CMAKE_CXX17_COMPILE_FEATURES "")
set(CMAKE_CXX20_COMPILE_FEATURES "")

set(CMAKE_CXX_PLATFORM_ID "Linux")
set(CMAKE_CXX_SIMULATE_ID "")
set(CMAKE_CXX_COMPILER_FRONTEND_VARIANT "")
set(CMAKE_CXX_SIMULATE_VERSION "")



set(CMAKE_AR "/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/arm-unknown-linux-uclibcgnueabi-ar")
set(CMAKE_CXX_COMPILER_AR "CMAKE_CXX_COMPILER_AR-NOTFOUND")
set(CMAKE_RANLIB "/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/arm-unknown-linux-uclibcgnueabi-ranlib")
set(CMAKE_CXX_COMPILER_RANLIB "CMAKE_CXX_COMPILER_RANLIB-NOTFOUND")
set(CMAKE_LINKER "/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/arm-unknown-linux-uclibcgnueabi-ld")
set(CMAKE_MT "")
set(CMAKE_COMPILER_IS_GNUCXX 1)
set(CMAKE_CXX_COMPILER_LOADED 1)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_CXX_ABI_COMPILED TRUE)
set(CMAKE_COMPILER_IS_MINGW )
set(CMAKE_COMPILER_IS_CYGWIN )
if(CMAKE_COMPILER_IS_CYGWIN)
  set(CYGWIN 1)
  set(UNIX 1)
endif()

set(CMAKE_CXX_COMPILER_ENV_VAR "CXX")

if(CMAKE_COMPILER_IS_MINGW)
  set(MINGW 1)
endif()
set(CMAKE_CXX_COMPILER_ID_RUN 1)
set(CMAKE_CXX_SOURCE_FILE_EXTENSIONS C;M;c++;cc;cpp;cxx;m;mm;CPP)
set(CMAKE_CXX_IGNORE_EXTENSIONS inl;h;hpp;HPP;H;o;O;obj;OBJ;def;DEF;rc;RC)

foreach (lang C OBJC OBJCXX)
  if (CMAKE_${lang}_COMPILER_ID_RUN)
    foreach(extension IN LISTS CMAKE_${lang}_SOURCE_FILE_EXTENSIONS)
      list(REMOVE_ITEM CMAKE_CXX_SOURCE_FILE_EXTENSIONS ${extension})
    endforeach()
  endif()
endforeach()

set(CMAKE_CXX_LINKER_PREFERENCE 30)
set(CMAKE_CXX_LINKER_PREFERENCE_PROPAGATES 1)

# Save compiler ABI information.
set(CMAKE_CXX_SIZEOF_DATA_PTR "4")
set(CMAKE_CXX_COMPILER_ABI "ELF")
set(CMAKE_CXX_LIBRARY_ARCHITECTURE "")

if(CMAKE_CXX_SIZEOF_DATA_PTR)
  set(CMAKE_SIZEOF_VOID_P "${CMAKE_CXX_SIZEOF_DATA_PTR}")
endif()

if(CMAKE_CXX_COMPILER_ABI)
  set(CMAKE_INTERNAL_PLATFORM_ABI "${CMAKE_CXX_COMPILER_ABI}")
endif()

if(CMAKE_CXX_LIBRARY_ARCHITECTURE)
  set(CMAKE_LIBRARY_ARCHITECTURE "")
endif()

set(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX "")
if(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX)
  set(CMAKE_CL_SHOWINCLUDES_PREFIX "${CMAKE_CXX_CL_SHOWINCLUDES_PREFIX}")
endif()





set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/arm-unknown-linux-uclibcgnueabi/include/c++/4.4.0;/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/arm-unknown-linux-uclibcgnueabi/include/c++/4.4.0/arm-unknown-linux-uclibcgnueabi;/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/arm-unknown-linux-uclibcgnueabi/include/c++/4.4.0/backward;/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/lib/gcc/arm-unknown-linux-uclibcgnueabi/4.4.0/include;/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/lib/gcc/arm-unknown-linux-uclibcgnueabi/4.4.0/include-fixed;/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/arm-unknown-linux-uclibcgnueabi/include;/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include")
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "stdc++;m;gcc_s;gcc;c;gcc_s;gcc")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/lib/gcc/arm-unknown-linux-uclibcgnueabi/4.4.0;/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/arm-unknown-linux-uclibcgnueabi/lib;/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/arm-unknown-linux-uclibcgnueabi/sysroot/lib;/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/lib")
set(CMAKE_CXX_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")