/*************************************************************************
 * libjson-rpc-cpp-skale
 *************************************************************************
 * @file    mockclientconnector.h
 * @date    10/9/2014
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_MOCKCLIENTCONNECTOR_H
#define JSONRPC_MOCKCLIENTCONNECTOR_H

#include <jsonrpccppskale/client/iclientconnector.h>

namespace jsonrpcskale {

    class MockClientConnector : public IClientConnector
    {
        public:
            MockClientConnector();

            void SetResponse(const std::string &response);

            std::string GetRequest();
            Json::Value GetJsonRequest();
            virtual void SendRPCMessage(const std::string& message, std::string& result) ;

        private:
            std::string response;
            std::string request;
    };

} // namespace jsonrpcskale

#endif // JSONRPC_MOCKCLIENTCONNECTOR_H
