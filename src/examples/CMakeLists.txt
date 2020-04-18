file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/gen)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/gen/abstractstubserver.h
    COMMAND jsonrpcskalestub ARGS ${CMAKE_CURRENT_SOURCE_DIR}/spec.json --cpp-server=AbstractStubServer --cpp-server-file=${CMAKE_BINARY_DIR}/gen/abstractstubserver.h
    MAIN_DEPENDENCY spec.json
    DEPENDS jsonrpcskalestub
    COMMENT "Generating Server Stubfiles"
    VERBATIM
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/gen/stubclient.h
    COMMAND jsonrpcskalestub ARGS ${CMAKE_CURRENT_SOURCE_DIR}/spec.json --cpp-client=StubClient --cpp-client-file=${CMAKE_BINARY_DIR}/gen/stubclient.h
    MAIN_DEPENDENCY spec.json
    DEPENDS jsonrpcskalestub
    COMMENT "Generating Client Stubfile"
    VERBATIM
)

add_custom_target(common_stubs
    DEPENDS ${CMAKE_BINARY_DIR}/gen/abstractstubserver.h ${CMAKE_BINARY_DIR}/gen/stubclient.h
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/gen/xbmcremote.h
    COMMAND jsonrpcskalestub ARGS ${CMAKE_CURRENT_SOURCE_DIR}/xbmc_remote.json --cpp-client=XbmcRemoteClient --cpp-client-file=${CMAKE_BINARY_DIR}/gen/xbmcremote.h
    MAIN_DEPENDENCY xbmc_remote.json
    DEPENDS jsonrpcskalestub
    COMMENT "Generating Client XBMC Stubfile"
    VERBATIM
)

include_directories(..)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${MHD_INCLUDE_DIRS})

if(UNIX)
    if(UNIX_DOMAIN_SOCKET_SERVER AND UNIX_DOMAIN_SOCKET_CLIENT)
        add_executable(unixdomainsocketserversample unixdomainsocketserver.cpp)
        target_link_libraries(unixdomainsocketserversample jsonrpcskaleserver)
        add_executable(unixdomainsocketclientsample unixdomainsocketclient.cpp)
        target_link_libraries(unixdomainsocketclientsample jsonrpcskaleclient)
    endif()

    if(FILE_DESCRIPTOR_SERVER AND FILE_DESCRIPTOR_CLIENT)
      add_executable(filedescriptorserversample filedescriptorserver.cpp)
      target_link_libraries(filedescriptorserversample jsonrpcskaleserver)
      add_executable(filedescriptorclientsample filedescriptorclient.cpp)
      target_link_libraries(filedescriptorclientsample jsonrpcskaleclient)
    endif()
endif(UNIX)

if (TCP_SOCKET_SERVER AND TCP_SOCKET_CLIENT)
    add_executable(tcpsocketclient tcpsocketclient.cpp)
    target_link_libraries(tcpsocketclient jsonrpcskaleclient)
    add_executable(tcpsocketserver tcpsocketserver.cpp)
    target_link_libraries(tcpsocketserver jsonrpcskaleserver)
endif()

if(HTTP_SERVER)
    add_executable(simpleserversample simpleserver.cpp)
    target_link_libraries(simpleserversample jsonrpcskaleserver)
endif()

if(HTTP_CLIENT)
    add_executable(simpleclientsample simpleclient.cpp)
    target_link_libraries(simpleclientsample jsonrpcskaleclient)
endif()

if(REDIS_SERVER)
    add_executable(redisserversample redisserver.cpp)
    target_link_libraries(redisserversample jsonrpcskaleserver)
endif()

if(REDIS_CLIENT)
    add_executable(redisclientsample redisclient.cpp)
    target_link_libraries(redisclientsample jsonrpcskaleclient)
endif()

if (COMPILE_STUBGEN)
    if(HTTP_CLIENT)
        add_executable(stubclientsample stubclient.cpp ${CMAKE_BINARY_DIR}/gen/stubclient.h)
        target_link_libraries(stubclientsample jsonrpcskaleclient)
        add_executable(xbmcremote xbmcremote.cpp ${CMAKE_BINARY_DIR}/gen/xbmcremote.h)
        target_link_libraries(xbmcremote jsonrpcskaleclient)
    endif()

    if(HTTP_SERVER)
        add_executable(stubserversample stubserver.cpp ${CMAKE_BINARY_DIR}/gen/abstractstubserver.h)
        target_link_libraries(stubserversample jsonrpcskaleserver)
    endif()
endif()

