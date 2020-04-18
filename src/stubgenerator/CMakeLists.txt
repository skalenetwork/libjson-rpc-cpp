file(GLOB_RECURSE jsonrpcskalestub_source *.cpp)
file(GLOB_RECURSE jsonrpcskalestub_header *.h)
list(REMOVE_ITEM jsonrpcskalestub_source "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

include_directories(..)
include_directories(${ARGTABLE_INCLUDE_DIRS})

set(ALL_LIBS)


if (BUILD_SHARED_LIBS)
        add_library(libjsonrpcskalestub SHARED ${jsonrpcskalestub_source} ${jsonrpcskalestub_header} ${jsonrpcskale_helper_source_common})
        target_link_libraries(libjsonrpcskalestub jsonrpcskalecommon ${ARGTABLE_LIBRARIES})
        set_target_properties(libjsonrpcskalestub PROPERTIES OUTPUT_NAME jsonrpccppskale-stub)
        list(APPEND ALL_LIBS libjsonrpcskalestub)
endif()

# setup static common library
if (BUILD_STATIC_LIBS OR MSVC)
        add_library(libjsonrpcskalestubStatic STATIC ${jsonrpcskalestub_source} ${jsonrpcskalestub_header} ${jsonrpcskale_helper_source_common})
        target_link_libraries(libjsonrpcskalestubStatic common ${ARGTABLE_LIBRARIES})
        set_target_properties(libjsonrpcskalestubStatic PROPERTIES OUTPUT_NAME jsonrpccppskale-stub)

    if (NOT BUILD_SHARED_LIBS)
        add_library(libjsonrpcskalestub ALIAS libjsonrpcskalestubStatic)
    endif()

    list(APPEND ALL_LIBS libjsonrpcskalestubStatic)
endif()

set(VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
set_target_properties(${ALL_LIBS} PROPERTIES VERSION "${VERSION_STRING}" SOVERSION "${SO_VERSION}")

add_executable(jsonrpcskalestub main.cpp)
if (BUILD_STATIC_LIBS AND NOT BUILD_SHARED_LIBS)
        target_link_libraries(jsonrpcskalestub common libjsonrpcskalestubStatic )
else()
        target_link_libraries(jsonrpcskalestub jsonrpcskalecommon libjsonrpcskalestub )
endif()
#Generate manpage
if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    configure_file("${CMAKE_SOURCE_DIR}/doc/manpage.in" "${CMAKE_BINARY_DIR}/jsonrpcskalestub.1" @ONLY)
    add_custom_target(manpage ALL DEPENDS ${CMAKE_BINARY_DIR}/jsonrpcskalestub.1)
    install(FILES ${CMAKE_BINARY_DIR}/jsonrpcskalestub.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1/)
    add_dependencies(jsonrpcskalestub manpage)
endif()

configure_file(${CMAKE_SOURCE_DIR}/cmake/libjsonrpccppskale-stub.pc.cmake ${CMAKE_BINARY_DIR}/libjsonrpccppskale-stub.pc)

install(FILES
    "${CMAKE_BINARY_DIR}/libjsonrpccppskale-stub.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/stubgenerator/
    DESTINATION include/jsonrpccppskale/stubgen
    FILES_MATCHING PATTERN "*.h")

install(TARGETS ${ALL_LIBS} jsonrpcskalestub
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION bin
)
