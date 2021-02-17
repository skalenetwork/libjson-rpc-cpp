/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    httpserver.cpp
 * @date    31.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/


#include <cstdlib>
#include <cstring>
#include <iostream>
#include <jsonrpccpp/common/specificationparser.h>
#include <sstream>

#include <unistd.h>
#include <stdio.h>
#include <limits.h>

//#include "common.h"


#include "httpserver.h"



using namespace jsonrpc;
using namespace std;

#define BUFFERSIZE 65536


std::mutex HttpServer::certs_mutex;
std::map<std::vector<uint8_t>, gnutls_x509_crt_t> HttpServer::verifiedCertificates;

/**
 * Get the client's certificate
 *
 * @param tls_session the TLS session
 * @return NULL if no valid client certificate could be found, a string
 *  	containing the certificate if found
 */


std::string cert_auth_get_dn(gnutls_x509_crt_t client_cert) {
  size_t lbuf = 0;

  gnutls_x509_crt_get_dn(client_cert, NULL, &lbuf);
  std::string buf;
  buf.reserve(lbuf);

  gnutls_x509_crt_get_dn(client_cert, &buf[0], &lbuf);
  return buf;
}


static gnutls_x509_crt_t get_client_certificate(gnutls_session_t tls_session) {
  unsigned int list_size;

  unsigned int client_cert_status;
  gnutls_x509_crt_t client_cert;

  if (tls_session == NULL)
    return NULL;

  const gnutls_datum_t* pcert = gnutls_certificate_get_peers(tls_session, &list_size);
  // pcert = const_cast<gnutls_datum_t*>(pcert);
  if (pcert == NULL || list_size == 0) {
    fprintf(stderr, "Failed to retrieve client certificate chain\n");
    return NULL;
  }

  const std::lock_guard< std::mutex > lock( HttpServer::certs_mutex );

  std::vector<uint8_t> pcert_data(pcert->size);
  for (size_t i = 0; i < pcert->size; ++i) {
    pcert_data[i] = pcert->data[i];
  }

  if ( HttpServer::verifiedCertificates.find(pcert_data) != HttpServer::verifiedCertificates.end() ) {
    gnutls_free( pcert->data );
    return HttpServer::verifiedCertificates[pcert_data];
  }

  if (gnutls_certificate_verify_peers2(tls_session, &client_cert_status)) {
    gnutls_free( pcert->data );
    std::cerr << "not verified" << std::endl;
    return NULL;
  }

  if (client_cert_status != 0 ) {
    gnutls_free( pcert->data );
    std::cerr << "client cert is not verified" << std::endl;
    return NULL;
  }

  if (gnutls_x509_crt_init(&client_cert)) {
    gnutls_free( pcert->data );
    fprintf(stderr, "Failed to initialize client certificate\n");
    return NULL;
  }
  /* Note that by passing values between 0 and listsize here, you
     can get access to the CA's certs */
  if (gnutls_x509_crt_import(client_cert, &pcert[0], GNUTLS_X509_FMT_DER)) {
    gnutls_free( pcert->data );
    fprintf(stderr, "Failed to import client certificate\n");
    gnutls_x509_crt_deinit(client_cert);
    return NULL;
  }

  HttpServer::verifiedCertificates[pcert_data] = client_cert;

  gnutls_free( pcert->data );

  return client_cert;
}

struct mhd_coninfo {
  struct MHD_PostProcessor *postprocessor;
  MHD_Connection *connection;
  stringstream request;
  HttpServer *server;
  int code;
};

HttpServer::HttpServer(int port, const std::string &sslcert,
                       const std::string &sslkey, const std::string & sslca, int threads)
    : AbstractServerConnector(), port(port), threads(threads), running(false),
      path_sslcert(sslcert), path_sslkey(sslkey), path_sslca(sslca), daemon(NULL), bindlocalhost(false) {

  if (path_sslca.length() > 0){
    check_client_cert = true;
  }
  else {
    check_client_cert = false;
  }
}

HttpServer::HttpServer(int port, const std::string &sslcert,  const std::string &sslkey,
           const std::string & sslca, bool check_client, int threads)
    : AbstractServerConnector(), port(port), threads(threads), running(false),
      path_sslcert(sslcert), path_sslkey(sslkey), path_sslca(sslca),
      check_client_cert(check_client), daemon(NULL), bindlocalhost(false) {}

HttpServer::~HttpServer() {}

IClientConnectionHandler *HttpServer::GetHandler(const std::string &url) {
  if (AbstractServerConnector::GetHandler() != NULL)
    return AbstractServerConnector::GetHandler();
  map<string, IClientConnectionHandler *>::iterator it =
      this->urlhandler.find(url);
  if (it != this->urlhandler.end())
    return it->second;
  return NULL;
}

HttpServer& HttpServer::BindLocalhost() {
  this->bindlocalhost = true;
  return *this;
}

bool HttpServer::StartListening() {
  if (!this->running) {
    const bool has_epoll = (MHD_is_feature_supported(MHD_FEATURE_EPOLL) == MHD_YES);
    const bool has_poll = (MHD_is_feature_supported(MHD_FEATURE_POLL) == MHD_YES);
    unsigned int mhd_flags;

    if (has_epoll)
// In MHD version 0.9.44 the flag is renamed to
// MHD_USE_EPOLL_INTERNALLY_LINUX_ONLY. In later versions both
// are deprecated.
#if defined(MHD_USE_EPOLL_INTERNALLY)
      mhd_flags = MHD_USE_EPOLL_INTERNALLY;
#else
      mhd_flags = MHD_USE_EPOLL_INTERNALLY_LINUX_ONLY;
#endif
    else if (has_poll)
      mhd_flags = MHD_USE_POLL_INTERNALLY;

    if (this->bindlocalhost) {
      memset(&this->loopback_addr, 0, sizeof(this->loopback_addr));
      loopback_addr.sin_family = AF_INET;
      loopback_addr.sin_port = htons(this->port);
      loopback_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

      this->daemon = MHD_start_daemon(
            mhd_flags, this->port, NULL, NULL, HttpServer::callback, this,
            MHD_OPTION_THREAD_POOL_SIZE, this->threads, MHD_OPTION_SOCK_ADDR, (struct sockaddr *)(&(this->loopback_addr)), MHD_OPTION_END);

    } else if (this->path_sslcert != "" && this->path_sslkey != "") {
      try {
        SpecificationParser::GetFileContent(this->path_sslcert, this->sslcert);
        SpecificationParser::GetFileContent(this->path_sslkey, this->sslkey);
        SpecificationParser::GetFileContent(this->path_sslca, this->sslca);

        if ( this->sslca.length() == 0 ) {
          std::cerr << "Root ca not found " << std::endl;
        }

        this->daemon = MHD_start_daemon(
            MHD_USE_SSL | mhd_flags, this->port, NULL, NULL,
            HttpServer::callback, this,
            MHD_OPTION_HTTPS_MEM_KEY, this->sslkey.c_str(),
            MHD_OPTION_HTTPS_MEM_CERT, this->sslcert.c_str(),
            MHD_OPTION_HTTPS_MEM_TRUST, this->sslca.c_str(),
            MHD_OPTION_THREAD_POOL_SIZE, this->threads,
            MHD_OPTION_END);

      } catch (JsonRpcException &ex) {
        return false;
      }
    } else {
      this->daemon = MHD_start_daemon(
          mhd_flags, this->port, NULL, NULL, HttpServer::callback, this,
          MHD_OPTION_THREAD_POOL_SIZE, this->threads, MHD_OPTION_END);
    }
    if (this->daemon != NULL)
      this->running = true;
    else std::cerr << "Daemon is null " << std::endl;
  }
  return this->running;
}

bool HttpServer::StopListening() {
  if (this->running) {
    MHD_stop_daemon(this->daemon);
    this->running = false;
  }
  return true;
}

bool HttpServer::SendResponse(const string &response, void *addInfo) {
  struct mhd_coninfo *client_connection = static_cast<struct mhd_coninfo *>(addInfo);
  struct MHD_Response *result = MHD_create_response_from_buffer(
      response.size(), (void *)response.c_str(), MHD_RESPMEM_MUST_COPY);

  MHD_add_response_header(result, "Content-Type", "application/json");
  MHD_add_response_header(result, "Access-Control-Allow-Origin", "*");

  int ret = MHD_queue_response(client_connection->connection, client_connection->code, result);
  MHD_destroy_response(result);
  return ret == MHD_YES;
}

bool HttpServer::SendOptionsResponse(void *addInfo) {
  struct mhd_coninfo *client_connection = static_cast<struct mhd_coninfo *>(addInfo);
  struct MHD_Response *result = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_MUST_COPY);

  MHD_add_response_header(result, "Allow", "POST, OPTIONS");
  MHD_add_response_header(result, "Access-Control-Allow-Origin", "*");
  MHD_add_response_header(result, "Access-Control-Allow-Headers", "origin, content-type, accept");
  MHD_add_response_header(result, "DAV", "1");

  int ret = MHD_queue_response(client_connection->connection, client_connection->code, result);
  MHD_destroy_response(result);
  return ret == MHD_YES;
}

void HttpServer::SetUrlHandler(const string &url, IClientConnectionHandler *handler) {
  this->urlhandler[url] = handler;
  this->SetHandler(NULL);
}

int HttpServer::callback(void *cls, MHD_Connection *connection, const char *url,
                         const char *method, const char *version,
                         const char *upload_data, size_t *upload_data_size,
                         void **con_cls) {
  (void)version;
  if (*con_cls == NULL) {
    struct mhd_coninfo *client_connection = new mhd_coninfo;
    client_connection->connection = connection;
    client_connection->server = static_cast<HttpServer *>(cls);
    *con_cls = client_connection;
    return MHD_YES;
  }
  struct mhd_coninfo *client_connection = static_cast<struct mhd_coninfo *>(*con_cls);


  if ( client_connection->server->is_client_cert_checked() ) {

    const union MHD_ConnectionInfo *ci1;

    ci1 = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_GNUTLS_SESSION);

    gnutls_session_t tls_session1 = (gnutls_session_t)ci1->tls_session;

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
      std::cerr << "could not get cwd";
      exit(-1);
    }

    gnutls_certificate_credentials_t ca_cred;
    gnutls_certificate_allocate_credentials(&ca_cred);
    std::string ca_dir = string(cwd) + "/sgx_data/cert_data";
    int res = gnutls_certificate_set_x509_trust_dir(ca_cred, ca_dir.c_str(), GNUTLS_X509_FMT_PEM);
    if (res == 0) {
      std::cerr << "no ca cert" << std::endl;
      gnutls_certificate_free_credentials(ca_cred);
      delete client_connection;
      *con_cls = NULL;
      return MHD_NO;
    }

    gnutls_x509_crt_t client_certificate = get_client_certificate(tls_session1);

    if (client_certificate == NULL) {
      std::cerr << "no cert" << std::endl;
      gnutls_certificate_free_credentials(ca_cred);
      *con_cls = NULL;
      delete client_connection;
      return MHD_NO;
    } else {
      std::string dn = cert_auth_get_dn(client_certificate);
      if ( dn.size() == 0 ) {
        std::cerr << "Error occured. DN size is 0" << std::endl;
        gnutls_x509_crt_deinit(client_certificate);
        gnutls_certificate_free_credentials(ca_cred);
        *con_cls = NULL;
        delete client_connection;
        return MHD_NO;
      }
    }
    gnutls_x509_crt_deinit(client_certificate);
    gnutls_certificate_free_credentials(ca_cred);
  }


  if (string("POST") == method) {
    if (*upload_data_size != 0) {
      client_connection->request.write(upload_data, *upload_data_size);
      *upload_data_size = 0;
      return MHD_YES;
    } else {
      string response;
      IClientConnectionHandler *handler = client_connection->server->GetHandler(string(url));
      if (handler == NULL) {
        client_connection->code = MHD_HTTP_INTERNAL_SERVER_ERROR;
        client_connection->server->SendResponse("No client connection handler found", client_connection);
      } else {
        client_connection->code = MHD_HTTP_OK;
        handler->HandleRequest(client_connection->request.str(), response);
        client_connection->server->SendResponse(response, client_connection);
      }
    }
  } else if (string("OPTIONS") == method) {
    client_connection->code = MHD_HTTP_OK;
    client_connection->server->SendOptionsResponse(client_connection);
  } else {
    client_connection->code = MHD_HTTP_METHOD_NOT_ALLOWED;
    client_connection->server->SendResponse("Not allowed HTTP Method", client_connection);
  }


  if (client_connection != nullptr)
  {
    delete client_connection;
  }
  *con_cls = NULL;

  return MHD_YES;
}

bool HttpServer::is_client_cert_checked(){
  return check_client_cert;
}
