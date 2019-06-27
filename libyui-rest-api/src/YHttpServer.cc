/*
  Copyright (C) 2017 SUSE LLC

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <microhttpd.h>

#define YUILogComponent "rest-api"
#include "YUILog.h"
#include "YDialog.h"
#include "YJsonSerializer.h"
#include "YUILog.h"

#include "YHttpServer.h"
#include "YHttpHandler.h"
#include "YHttpDialogHandler.h"
#include "YHttpRootHandler.h"
#include "YHttpAppHandler.h"
#include "YHttpWidgetsHandler.h"
#include "YHttpWidgetsActionHandler.h"

YHttpServer * YHttpServer::_yserver = 0;


int YHttpServer::port_num()
{
    const char* env_port = getenv( YUITest_HTTP_PORT );
    return env_port ? atoi(env_port) : 0;
}

// For security reasons accept the connections only from the localhost
// by default, allow listening on all interfaces only when explicitly allowed.
bool remote_access()
{
    // remote access enabled
    if ((getenv( YUITest_HTTP_REMOTE ) && strcmp(getenv( YUITest_HTTP_REMOTE ), "1") == 0))
    {
        yuiWarning() << "Warning: Enabling remote access!" << std::endl;
        return true;
    }

    // allow only loop back connection ("http://localhost" only)
    yuiMilestone() << "Only localhost connections allowed." << std::endl;
    return false;
}

// return the IPv4 listen address (localhost or all)
in_addr_t listen_address_v4(bool remote)
{
    return (remote) ? INADDR_ANY : htonl(INADDR_LOOPBACK);
}

// return the IPv6 listen address (localhost or all)
struct in6_addr listen_address_v6(bool remote)
{
    return (remote) ? in6addr_any : in6addr_loopback;
}

YHttpServer::YHttpServer() : server_v4(nullptr), server_v6(nullptr), redraw(false)
{
    _yserver = this;
}

YHttpServer::~YHttpServer()
{
    yuiMilestone() << "Finishing the REST API HTTP server..." << std::endl;

    if (server_v4) {
        yuiMilestone() << "Stopping IPv4 HTTP server" << std::endl;
        MHD_stop_daemon(server_v4);
    }

    if (server_v6) {
        yuiMilestone() << "Stopping IPv6 HTTP server" << std::endl;
        MHD_stop_daemon(server_v6);
    }
}

YHttpServerSockets YHttpServer::sockets()
{
    fd_set rs, ws, es;
    int max = 0;

    YHttpServerSockets ret;

    if (server_v4)
    {
        FD_ZERO (&rs);
        FD_ZERO (&ws);
        FD_ZERO (&es);

        if (MHD_YES != MHD_get_fdset(server_v4, &rs, &ws, &es, &max))
        {
            yuiError() << "Cannot read the IPv4 FD set!" << std::endl;
            return ret;
        }

        for(int i = 0; i <= max; ++i) {
            if (FD_ISSET(i, &rs)) ret.add_read(i);
            if (FD_ISSET(i, &ws)) ret.add_write(i);
            if (FD_ISSET(i, &es)) ret.add_exception(i);
        }
    }

    if (server_v6)
    {
        FD_ZERO (&rs);
        FD_ZERO (&ws);
        FD_ZERO (&es);

        if (MHD_YES != MHD_get_fdset(server_v6, &rs, &ws, &es, &max))
        {
            yuiError() << "Cannot read the IPv6 FD set!" << std::endl;
            return ret;
        }

        for(int i = 0; i <= max; ++i) {
            if (FD_ISSET(i, &rs)) ret.add_read(i);
            if (FD_ISSET(i, &ws)) ret.add_write(i);
            if (FD_ISSET(i, &es)) ret.add_exception(i);
        }
    }

    if (ret.empty())
        yuiWarning() << "Not watching any FD!" << std::endl;

    return ret;
}

int YHttpServer::handle(struct MHD_Connection* connection,
    const char* url, const char* method, const char* upload_data,
    size_t* upload_data_size)
{
    yuiMilestone() << "Processing " << method << " request: "<< url << ", input data size: " << *upload_data_size << std::endl;

    // find the handler
    for(YHttpMount m: _mounts)
    {
        if (m.handles(url, method))
            return m.handler()->handle(connection, url, method, upload_data, upload_data_size, &redraw);
    }

    // if not found create an empty 404 error response
    yuiMilestone() << "URL path/method not found, returning error code 404" << std::endl;
    struct MHD_Response* response = MHD_create_response_from_buffer(0, 0, MHD_RESPMEM_PERSISTENT);
    return MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
}

static int
requestHandler(void *srv,
          struct MHD_Connection *connection,
          const char *url,
          const char *method,
          const char *version,
          const char *upload_data, size_t *upload_data_size, void **ptr)
{
    // remember the callback status
    static int aptr;

    if (&aptr != *ptr)
    {
        // do not respond on first call, it's used for the initial check to close invalid requests early
        *ptr = &aptr;
        // continue processing the request
        return MHD_YES;
    }
    // reset
    *ptr = NULL;

    YHttpServer *server = (YHttpServer *)srv;
    return server->handle(connection, url, method, upload_data, upload_data_size);
}

static int onConnect(void *srv, const struct sockaddr *addr, socklen_t addrlen) {
    if (addr->sa_family == AF_INET) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *) addr;
        // macro INET_ADDRSTRLEN contains the maximum length of an IPv4 address
        char buffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr_in->sin_addr), buffer, INET_ADDRSTRLEN);
        yuiMilestone() << "Received an IPv4 connection from " << buffer << std::endl;
    }

    if (addr->sa_family == AF_INET6) {
        struct sockaddr_in6 *addr6_in = (struct sockaddr_in6 *) addr;
        // macro INET6_ADDRSTRLEN contains the maximum length of an IPv6 address
        char buffer[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(addr6_in->sin6_addr), buffer, INET6_ADDRSTRLEN);
        yuiMilestone() << "Received an IPv6 connection from " << buffer << std::endl;
    }

    // always continue processing the request
    return MHD_YES;
}

void YHttpServer::start()
{
    mount("/", "GET", new YHttpRootHandler());
    mount("/dialog", "GET", new YHttpDialogHandler());
    mount("/widgets", "GET", new YHttpWidgetsHandler());
    mount("/widgets", "POST", new YHttpWidgetsActionHandler());
    mount("/application", "GET", new YHttpAppHandler());

    bool remote = remote_access();

    // setup the IPv4 server
    sockaddr_in server_socket;
    server_socket.sin_family = AF_INET;
    server_socket.sin_port = htons(port_num());
    server_socket.sin_addr.s_addr = listen_address_v4(remote);
    server_v4 = MHD_start_daemon (
                        // enable debugging output (on STDERR)
                        MHD_USE_DEBUG,
                        // the port number to use
                        port_num(),
                        // handler for new connections
                        &onConnect, this,
                        // handler for processing requests
                        &requestHandler, this,
                        // disable reusing the socket for multiple processes,
                        // for security reasons allow only one process to use this port
                        MHD_OPTION_LISTENING_ADDRESS_REUSE, (unsigned int) 0,
                        // set the port and interface to listen to
                        MHD_OPTION_SOCK_ADDR, &server_socket,
                        // finish the argument list
                        MHD_OPTION_END);

    // setup the IPv6 server
    sockaddr_in6 server_socket_v6;
    server_socket_v6.sin6_family = AF_INET6;
    server_socket_v6.sin6_port = htons(port_num());
    server_socket_v6.sin6_addr = listen_address_v6(remote);
    server_v6 = MHD_start_daemon (
                        // enable debugging output (on STDERR)
                        MHD_USE_DEBUG |
                        // use IPv6
                        MHD_USE_IPv6,
                        // the port number to use
                        port_num(),
                        // handler for new connections
                        &onConnect, this,
                        // handler for processing requests
                        &requestHandler, this,
                        // disable reusing the socket for multiple processes,
                        // for security reasons allow only one process to use this port
                        MHD_OPTION_LISTENING_ADDRESS_REUSE, (unsigned int) 0,
                        // set the port and interface to listen to
                        MHD_OPTION_SOCK_ADDR, &server_socket_v6,
                        // finish the argument list
                        MHD_OPTION_END);

    if (server_v4 == nullptr) {
      std::cerr << "Cannot start the IPv4 HTTP server at port " << port_num() << std::endl;
      yuiError() << "Cannot start the IPv4 HTTP server at port " << port_num() << std::endl;
    }
    else {
        yuiWarning() << "Started REST API HTTP server (IPv4) at port " << port_num() << std::endl;
    }

    if (server_v6 == nullptr) {
      std::cerr << "Cannot start the IPv6 HTTP server at port " << port_num() << std::endl;
      yuiError() << "Cannot start the IPv6 HTTP server at port " << port_num() << std::endl;
    }
    else {
        yuiWarning() << "Started REST API HTTP server (IPv6) at port " << port_num() << std::endl;
    }
    // FIXME: exit when no server available?
}

bool YHttpServer::process_data()
{
    redraw = false;
    yuiMilestone() << "Processing HTTP server data..." << std::endl;
    if (server_v4) MHD_run(server_v4);
    if (server_v6) MHD_run(server_v6);
    return redraw;
}

void YHttpServer::mount(const std::string& path, const std::string &method, YHttpHandler *handler)
{
    _mounts.push_back(YHttpMount(path, method, handler));
}
