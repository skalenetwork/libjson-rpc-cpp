/*************************************************************************
 * libjson-rpc-skale-cpp
 *************************************************************************
 * @file testserver.h
 * @date    08.03.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_SERVER_H
#define JSONRPC_CPP_SERVER_H

#include <jsonrpccppskale/server.h>
#include <jsonrpccppskale/client.h>

//Methods
namespace jsonrpcskale
{
    class TestServer : public jsonrpcskale::AbstractServer<TestServer>
    {
        public:
            TestServer(jsonrpcskale::AbstractServerConnector &connector, jsonrpcskale::serverVersion_t type = jsonrpcskale::JSONRPC_SERVER_V2);

            void sayHello(const Json::Value& request, Json::Value& response);
            void getCounterValue(const Json::Value& request, Json::Value& response);
            void add(const Json::Value& request, Json::Value& response);
            void sub(const Json::Value& request, Json::Value& response);

            void exceptionMethod(const Json::Value& request, Json::Value& response);

            //Notifications
            void initCounter(const Json::Value& request);
            void incrementCounter(const Json::Value& request);
            void initZero(const Json::Value& request);

            int getCnt();

            virtual bool bindAndAddMethod(const Procedure& proc, methodPointer_t pointer);
            virtual bool bindAndAddNotification(const Procedure& proc, notificationPointer_t pointer);
        private:
            int cnt;
    };
}

#endif // JSONRPC_CPP_SERVER_H
