cmake_minimum_required(VERSION 3.16)

project(tilkal LANGUAGES CXX)

option(TILKAL_ENABLE_EXPERIMENTAL_MODULES "Enable experimental single-partition C++20 modules on supported toolchains" OFF)

set(TILKAL_SOURCES
    ai.cpp
    bans.cpp
    client.cpp
    clients.cpp
    commands.cpp
    createur/createur.cpp
    definitions.cpp
    door.cpp
    doors.cpp
    events.cpp
    forbnicks.cpp
    httpclient.cpp
    objcategs.cpp
    object.cpp
    objects.cpp
    player.cpp
    players.cpp
    room.cpp
    server.cpp
    skill.cpp
    skillprots.cpp
    skills.cpp
    tilkal.cpp
    tools.cpp
    xml.cpp
    wwwfiles.cpp
    ai/standard.cpp
    ai/beggar.cpp
    ai/shopkeeper.cpp
)

set(TILKAL_SINGLE_PARTITION_MODULES
    modules/tilkal.bans.ixx
)

if(TILKAL_ENABLE_EXPERIMENTAL_MODULES)
    if(CMAKE_VERSION VERSION_LESS 3.28)
        message(FATAL_ERROR "Experimental modules require CMake 3.28 or newer.")
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
        message(FATAL_ERROR "Experimental modules require GCC 14 or newer.")
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17)
        message(FATAL_ERROR "Experimental modules require Clang 17 or newer.")
    endif()

    list(REMOVE_ITEM TILKAL_SOURCES bans.cpp)
endif()

add_executable(tilkal_server ${TILKAL_SOURCES})

target_include_directories(tilkal_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/createur
)

if(TILKAL_ENABLE_EXPERIMENTAL_MODULES)
    set_target_properties(tilkal_server PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS YES
    )
else()
    set_target_properties(tilkal_server PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS YES
    )
endif()

if(TILKAL_ENABLE_EXPERIMENTAL_MODULES)
    target_sources(tilkal_server PRIVATE
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
            ${TILKAL_SINGLE_PARTITION_MODULES}
    )
endif()

target_compile_definitions(tilkal_server PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
)

if(WIN32)
    target_link_libraries(tilkal_server PRIVATE ws2_32)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(tilkal_server PRIVATE Threads::Threads)
endif()
