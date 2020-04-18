Name: libjsonrpccppskale-server
Description: A C++ server implementation of json-rpc.
Version: ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
Libs: -L${FULL_PATH_LIBDIR} -ljsoncpp -ljsonrpccppskale-common -ljsonrpccppskale-server ${SERVER_LIBS}
Cflags: -I${FULL_PATH_INCLUDEDIR}
