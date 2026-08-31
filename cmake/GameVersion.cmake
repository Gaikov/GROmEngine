include_guard(GLOBAL)

include(CMakeParseArguments)

set_property(GLOBAL PROPERTY GROM_GAME_VERSION_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(grom_configure_game_version)
    cmake_parse_arguments(
            GAME_VERSION
            ""
            "TARGET;PROPERTIES_FILE;PRODUCT_NAME"
            ""
            ${ARGN}
    )

    foreach (REQUIRED_ARG TARGET PROPERTIES_FILE PRODUCT_NAME)
        if (NOT DEFINED GAME_VERSION_${REQUIRED_ARG} OR GAME_VERSION_${REQUIRED_ARG} STREQUAL "")
            message(FATAL_ERROR "grom_configure_game_version requires ${REQUIRED_ARG}")
        endif ()
    endforeach ()

    if (NOT TARGET "${GAME_VERSION_TARGET}")
        message(FATAL_ERROR "grom_configure_game_version target does not exist: ${GAME_VERSION_TARGET}")
    endif ()

    get_filename_component(
            VERSION_FILE
            "${GAME_VERSION_PROPERTIES_FILE}"
            ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    if (NOT EXISTS "${VERSION_FILE}")
        message(FATAL_ERROR "Game version file not found: ${VERSION_FILE}")
    endif ()

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${VERSION_FILE}")
    file(STRINGS "${VERSION_FILE}" VERSION_LINES ENCODING UTF-8)

    set(VERSION_NAME_FOUND FALSE)
    set(VERSION_CODE_FOUND FALSE)
    foreach (VERSION_LINE IN LISTS VERSION_LINES)
        if (VERSION_LINE MATCHES "^[ \t]*versionName[ \t]*=[ \t]*([^ \t#]+)[ \t]*$")
            if (VERSION_NAME_FOUND)
                message(FATAL_ERROR "Duplicate versionName in ${VERSION_FILE}")
            endif ()
            set(VERSION_NAME "${CMAKE_MATCH_1}")
            set(VERSION_NAME_FOUND TRUE)
        elseif (VERSION_LINE MATCHES "^[ \t]*versionCode[ \t]*=[ \t]*([^ \t#]+)[ \t]*$")
            if (VERSION_CODE_FOUND)
                message(FATAL_ERROR "Duplicate versionCode in ${VERSION_FILE}")
            endif ()
            set(VERSION_CODE "${CMAKE_MATCH_1}")
            set(VERSION_CODE_FOUND TRUE)
        endif ()
    endforeach ()

    if (NOT VERSION_NAME_FOUND OR
            NOT VERSION_NAME MATCHES "^[0-9A-Za-z][0-9A-Za-z._+-]*$")
        message(FATAL_ERROR "Invalid or missing versionName in ${VERSION_FILE}")
    endif ()

    if (NOT VERSION_CODE_FOUND OR NOT VERSION_CODE MATCHES "^[1-9][0-9]*$")
        message(FATAL_ERROR "Invalid or missing versionCode in ${VERSION_FILE}; expected a positive integer")
    endif ()

    if (VERSION_CODE GREATER 2100000000)
        message(FATAL_ERROR "versionCode in ${VERSION_FILE} exceeds the Android limit 2100000000")
    endif ()

    if (NOT GAME_VERSION_PRODUCT_NAME MATCHES "^[0-9A-Za-z][0-9A-Za-z ._+:-]*$")
        message(FATAL_ERROR "Invalid PRODUCT_NAME: ${GAME_VERSION_PRODUCT_NAME}")
    endif ()

    string(MAKE_C_IDENTIFIER "${GAME_VERSION_TARGET}" TARGET_ID)
    set(GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/${TARGET_ID}")
    file(MAKE_DIRECTORY "${GENERATED_INCLUDE_DIR}")

    get_property(GAME_VERSION_MODULE_DIR GLOBAL PROPERTY GROM_GAME_VERSION_MODULE_DIR)
    set(GAME_VERSION_HEADER "${GENERATED_INCLUDE_DIR}/GameVersion.h")
    configure_file(
            "${GAME_VERSION_MODULE_DIR}/GameVersion.h.in"
            "${GAME_VERSION_HEADER}"
            @ONLY
    )

    target_include_directories("${GAME_VERSION_TARGET}" PRIVATE "${GENERATED_INCLUDE_DIR}")
    target_sources("${GAME_VERSION_TARGET}" PRIVATE "${GAME_VERSION_HEADER}")

    message(STATUS "${GAME_VERSION_PRODUCT_NAME} version: ${VERSION_NAME} (${VERSION_CODE})")
endfunction()
