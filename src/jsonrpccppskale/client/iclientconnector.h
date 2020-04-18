/*************************************************************************
 * libjson-rpc-skale-cpp
 *************************************************************************
 * @file    abstractclientconnector.h
 * @date    02.01.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_CLIENTCONNECTOR_H_
#define JSONRPC_CPP_CLIENTCONNECTOR_H_

#include <string>
#include <jsonrpccppskale/common/exception.h>

namespace jsonrpcskale
{
    class IClientConnector
    {
        public:
            virtual ~IClientConnector(){}

            virtual void SendRPCMessage(const std::string& message, std::string& result)  = 0;
    };
} /* namespace jsonrpcskale */
#endif /* JSONRPC_CPP_CLIENTCONNECTOR_H_ */
