// Copyright 2009 (c) Tarro, Inc.
// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

//[ hello_world_server_main
/*`
  This is a part of the 'Hello World' example. It's used to
  demonstrate how easy it is to set up an HTTP server.  All we do in
  this example is create a request handler and run the server.
 */
#include <boost/network/protocol/http/server.hpp>
#include <iostream>

namespace http = boost::network::http;

/*<< Defines the server. >>*/
struct hello_world;
typedef http::server<hello_world> server;

/*<< Defines the request handler.  It's a class that defines two
     functions, `operator()` and `log()` >>*/
struct hello_world {
  /*<< This is the function that handles the incoming request. >>*/
  void operator()(server::request const &request, server::connection_ptr connection) {
    server::string_type ip = source(request);
    unsigned int port = request.source_port;
    std::ostringstream data;
    data << "Hello, " << ip << ':' << port << '!';

    std::map<std::string, std::string> headers = {
      {"Content-Type", "text/plain"},
    };

    connection->set_status(server::connection::ok);
    connection->set_headers(headers);
    connection->write(data.str());
  }
};

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " address port" << std::endl;
    return 1;
  }

  try {
    /*<< Creates the request handler. >>*/
    hello_world handler;
    /*<< Creates the server. >>*/
    server::options options(handler);
    server server_(options.address(argv[1]).port(argv[2]));
    /*<< Runs the server. >>*/
    server_.run();
  }
  catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
//]
