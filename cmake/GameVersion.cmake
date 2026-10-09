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
    set(ASSET_KEY_FOUND FALSE)
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
        elseif (VERSION_LINE MATCHES "^[ \t]*assetEncryptionKey[ \t]*=[ \t]*([^ \t#]+)[ \t]*$")
            if (ASSET_KEY_FOUND)
                message(FATAL_ERROR "Duplicate assetEncryptionKey in ${VERSION_FILE}")
            endif ()
            set(ASSET_ENCRYPTION_KEY "${CMAKE_MATCH_1}")
            set(ASSET_KEY_FOUND TRUE)
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

    string(LENGTH "${ASSET_ENCRYPTION_KEY}" ASSET_KEY_LENGTH)
    if (NOT ASSET_KEY_FOUND OR
            NOT ASSET_ENCRYPTION_KEY MATCHES "^[0-9A-Fa-f]+$" OR
            NOT ASSET_KEY_LENGTH EQUAL 64)
        message(FATAL_ERROR
                "Invalid or missing assetEncryptionKey in ${VERSION_FILE}; expected exactly 64 hex characters")
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

    string(RANDOM LENGTH 64 ALPHABET 0123456789abcdef ASSET_KEY_MASK_HEX)
    set(ASSET_KEY_MASK_VALUES "")
    set(ASSET_KEY_ENCRYPTED_VALUES "")
    foreach (BYTE_INDEX RANGE 0 31)
        math(EXPR HEX_INDEX "${BYTE_INDEX} * 2")
        string(SUBSTRING "${ASSET_ENCRYPTION_KEY}" ${HEX_INDEX} 2 KEY_BYTE_HEX)
        string(SUBSTRING "${ASSET_KEY_MASK_HEX}" ${HEX_INDEX} 2 MASK_BYTE_HEX)
        math(EXPR KEY_BYTE "0x${KEY_BYTE_HEX}")
        math(EXPR MASK_BYTE "0x${MASK_BYTE_HEX}")
        math(EXPR ENCRYPTED_BYTE "${KEY_BYTE} ^ ${MASK_BYTE}")
        string(APPEND ASSET_KEY_MASK_VALUES "${MASK_BYTE},")
        string(APPEND ASSET_KEY_ENCRYPTED_VALUES "${ENCRYPTED_BYTE},")
    endforeach ()

    set(ASSET_KEY_HEADER "${GENERATED_INCLUDE_DIR}/AssetCryptoKey.h")
    configure_file(
            "${GAME_VERSION_MODULE_DIR}/AssetCryptoKey.h.in"
            "${ASSET_KEY_HEADER}"
            @ONLY
    )

    target_include_directories("${GAME_VERSION_TARGET}" PRIVATE "${GENERATED_INCLUDE_DIR}")
    target_sources("${GAME_VERSION_TARGET}" PRIVATE "${GAME_VERSION_HEADER}" "${ASSET_KEY_HEADER}")

    if (WEB_ASM)
        if (NOT CMAKE_PROJECT_NAME MATCHES "^[0-9A-Za-z][0-9A-Za-z._+-]*$")
            message(FATAL_ERROR
                    "Invalid CMake project name for WASM package: ${CMAKE_PROJECT_NAME}")
        endif ()

        get_target_property(WASM_OUTPUT_NAME "${GAME_VERSION_TARGET}" OUTPUT_NAME)
        if (NOT WASM_OUTPUT_NAME)
            set(WASM_OUTPUT_NAME "${GAME_VERSION_TARGET}")
        endif ()

        set(WASM_PACKAGE_NAME
                "${CMAKE_PROJECT_NAME}-${VERSION_NAME}-${VERSION_CODE}.zip")

        set(WASM_PACKAGE_TARGET "${TARGET_ID}_wasm_package")
        add_custom_target(
                "${WASM_PACKAGE_TARGET}"
                ALL
                COMMAND "${CMAKE_COMMAND}" -E chdir
                        "$<TARGET_FILE_DIR:${GAME_VERSION_TARGET}>"
                        "${CMAKE_COMMAND}" -E tar cf "${WASM_PACKAGE_NAME}"
                        --format=zip
                        "${WASM_OUTPUT_NAME}.html"
                        "${WASM_OUTPUT_NAME}.js"
                        "${WASM_OUTPUT_NAME}.wasm"
                        "${WASM_OUTPUT_NAME}.data"
                COMMENT "Creating WASM package ${WASM_PACKAGE_NAME}"
                VERBATIM
        )
        add_dependencies("${WASM_PACKAGE_TARGET}" "${GAME_VERSION_TARGET}")
    endif ()

    message(STATUS "${GAME_VERSION_PRODUCT_NAME} version: ${VERSION_NAME} (${VERSION_CODE})")
endfunction()
