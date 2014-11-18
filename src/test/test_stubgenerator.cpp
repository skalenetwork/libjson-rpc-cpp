/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    test_connector_http.cpp
 * @date    28.09.2013
 * @author  Peter Spiess-Knafl <peter.knafl@gmail.com>
 * @license See attached LICENSE.txt
 ************************************************************************/

#include <boost/test/unit_test.hpp>

#ifdef STUBGEN_TESTING
#define BOOST_TEST_MODULE stubgenerator

#include <jsonrpccpp/common/specificationparser.h>
#include <stubgenerator/server/cppserverstubgenerator.h>
#include <stubgenerator/client/cppclientstubgenerator.h>
#include <stubgenerator/client/jsclientstubgenerator.h>
#include <stubgenerator/helper/cpphelper.h>

#include <sstream>

using namespace jsonrpc;
using namespace std;

BOOST_AUTO_TEST_SUITE(stubgenerator)

BOOST_AUTO_TEST_CASE(test_stubgen_cppclient)
{
    stringstream stream;
    vector<Procedure> procedures = SpecificationParser::GetProceduresFromFile("testspec6.json");
    CodeGenerator cg(stream);
    CPPClientStubGenerator stubgen("ns1::ns2::TestStubClient", procedures, cg);
    stubgen.generateStub();
    string result = stream.str();

    BOOST_CHECK_NE(result.find("#ifndef JSONRPC_CPP_STUB_NS1_NS2_TESTSTUBCLIENT_H_"), string::npos);
    BOOST_CHECK_NE(result.find("#define JSONRPC_CPP_STUB_NS1_NS2_TESTSTUBCLIENT_H_"), string::npos);
    BOOST_CHECK_NE(result.find("namespace ns1"), string::npos);
    BOOST_CHECK_NE(result.find("namespace ns2"), string::npos);
    BOOST_CHECK_NE(result.find("class TestStubClient : public jsonrpc::Client"), string::npos);
    BOOST_CHECK_NE(result.find("std::string test_method(const std::string& name) throw (jsonrpc::JsonRpcException)"), string::npos);
    BOOST_CHECK_NE(result.find("void test_notification(const std::string& name) throw (jsonrpc::JsonRpcException)"), string::npos);
    BOOST_CHECK_NE(result.find("double test_method2(const Json::Value& object, const Json::Value& values) throw (jsonrpc::JsonRpcException)"), string::npos);
    BOOST_CHECK_NE(result.find("void test_notification2(const Json::Value& object, const Json::Value& values) throw (jsonrpc::JsonRpcException)"), string::npos);
    BOOST_CHECK_NE(result.find("#endif //JSONRPC_CPP_STUB_NS1_NS2_TESTSTUBCLIENT_H_"), string::npos);

    BOOST_CHECK_EQUAL(CPPHelper::class2Filename("ns1::ns2::TestClass"), "testclass.h");
}

BOOST_AUTO_TEST_CASE(test_stubgen_cppserver)
{
    stringstream stream;
    vector<Procedure> procedures = SpecificationParser::GetProceduresFromFile("testspec6.json");
    CodeGenerator cg(stream);
    CPPServerStubGenerator stubgen("ns1::ns2::TestStubServer", procedures, cg);
    stubgen.generateStub();
    string result = stream.str();

    BOOST_CHECK_NE(result.find("#ifndef JSONRPC_CPP_STUB_NS1_NS2_TESTSTUBSERVER_H_"), string::npos);
    BOOST_CHECK_NE(result.find("#define JSONRPC_CPP_STUB_NS1_NS2_TESTSTUBSERVER_H_"), string::npos);
    BOOST_CHECK_NE(result.find("namespace ns1"), string::npos);
    BOOST_CHECK_NE(result.find("namespace ns2"), string::npos);
    BOOST_CHECK_NE(result.find("class TestStubServer : public jsonrpc::AbstractServer<TestStubServer>"), string::npos);
    BOOST_CHECK_NE(result.find("virtual std::string test.method(const std::string& name) = 0;"), string::npos);
    BOOST_CHECK_NE(result.find("virtual void test.notification(const std::string& name) = 0;"), string::npos);
    BOOST_CHECK_NE(result.find("virtual double test.method2(const Json::Value& object, const Json::Value& values) = 0;"), string::npos);
    BOOST_CHECK_NE(result.find("virtual void test.notification2(const Json::Value& object, const Json::Value& values) = 0;"), string::npos);
    BOOST_CHECK_NE(result.find("#endif //JSONRPC_CPP_STUB_NS1_NS2_TESTSTUBSERVER_H_"), string::npos);
}

BOOST_AUTO_TEST_CASE(test_stubgen_jsclient)
{
    stringstream stream;
    vector<Procedure> procedures = SpecificationParser::GetProceduresFromFile("testspec6.json");
    CodeGenerator cg(stream);
    JSClientStubGenerator stubgen("TestStubClient", procedures, cg);
    stubgen.generateStub();
    string result = stream.str();

    BOOST_CHECK_NE(result.find("function TestStubClient(url) {"), string::npos);
    BOOST_CHECK_NE(result.find("TestStubClient.prototype.test_method = function(name, callbackSuccess, callbackError)"), string::npos);
    BOOST_CHECK_NE(result.find("TestStubClient.prototype.test_notification = function(name, callbackSuccess, callbackError)"), string::npos);
    BOOST_CHECK_NE(result.find("TestStubClient.prototype.test_method2 = function(object, values, callbackSuccess, callbackError)"), string::npos);
    BOOST_CHECK_NE(result.find("TestStubClient.prototype.test_notification2 = function(object, values, callbackSuccess, callbackError)"), string::npos);

    BOOST_CHECK_EQUAL(JSClientStubGenerator::class2Filename("TestClass"), "testclass.js");
}

BOOST_AUTO_TEST_CASE(tst_stubgen_indentation)
{
    stringstream stream;
    CodeGenerator cg(stream);
    cg.setIndentSymbol("   ");
    cg.increaseIndentation();
    cg.write("abc");
    BOOST_CHECK_EQUAL(stream.str(), "   abc");

    stringstream stream2;
    CodeGenerator cg2(stream2);
    cg2.setIndentSymbol("\t");
    cg2.increaseIndentation();
    cg2.write("abc");
    BOOST_CHECK_EQUAL(stream2.str(), "\tabc");
}


BOOST_AUTO_TEST_SUITE_END()
#endif