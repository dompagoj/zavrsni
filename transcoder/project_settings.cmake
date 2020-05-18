find_program(CCACHE ccache)

if (CCACHE)
    message("Using ccache")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else ()
    message("ccache not found cannot use")
endif ()