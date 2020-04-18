/*************************************************************************
 * libjson-rpc-skale-cpp
 *************************************************************************
 * @file    mockserverconnector.h
 * @date    10/10/2014
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_MOCKSERVERCONNECTOR_H
#define JSONRPC_MOCKSERVERCONNECTOR_H

#include <jsonrpccppskale/common/jsonparser.h>
#include <jsonrpccppskale/server/abstractserverconnector.h>

namespace jsonrpcskale {

class MockServerConnector : public AbstractServerConnector {
public:
  MockServerConnector();

  virtual bool StartListening();
  virtual bool StopListening();

  bool SetRequest(const std::string &request);
  Json::Value GetJsonRequest();

  std::string GetResponse();
  Json::Value GetJsonResponse();

private:
  std::string request;
  std::string response;
};

} // namespace jsonrpcskale

#endif // JSONRPC_MOCKSERVERCONNECTOR_H
