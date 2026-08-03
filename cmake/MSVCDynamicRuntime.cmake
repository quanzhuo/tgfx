cmake_minimum_required(VERSION 3.15)

if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif ()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

if (MSVC)
    message("MSVC build")

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_tgfx_msvc_runtime_flag /MDd)
    else ()
        set(_tgfx_msvc_runtime_flag /MD)
    endif ()

    foreach (_tgfx_lang C CXX)
        foreach (_tgfx_config "" _DEBUG _RELEASE _RELWITHDEBINFO _MINSIZEREL)
            set(_tgfx_flags_var CMAKE_${_tgfx_lang}_FLAGS${_tgfx_config})
            if (DEFINED ${_tgfx_flags_var})
                string(REGEX REPLACE "([/-]M[TD]d?)" "" _tgfx_flags "${${_tgfx_flags_var}}")
                string(APPEND _tgfx_flags " ${_tgfx_msvc_runtime_flag}")
                set(${_tgfx_flags_var} "${_tgfx_flags}")
            endif ()
        endforeach ()
    endforeach ()

    set(_tgfx_msvc_build_flags /MP ${_tgfx_msvc_runtime_flag})
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND _tgfx_msvc_build_flags /Z7)
    endif ()

    add_compile_options("$<$<COMPILE_LANGUAGE:C>:${_tgfx_msvc_build_flags}>")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${_tgfx_msvc_build_flags}>")
endif ()
