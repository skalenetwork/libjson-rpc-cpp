Name: libjsonrpccppskale-client
Description: A C++ client implementation of json-rpc.
Version: ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
Libs: -L${FULL_PATH_LIBDIR} -ljsoncpp -ljsonrpccppskale-common -ljsonrpccppskale-client ${CLIENT_LIBS}
Cflags: -I${FULL_PATH_INCLUDEDIR}
