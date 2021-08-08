//
// Copyright 2017 (c) Arun Chandrasekaran <aruncxy@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

//
// Example for performing streaming file upload operations directly to
// filesystem using async server
//
// If you use wget, do the following at the client side:
//
// wget localhost:9190/upload?filename=Earth.mp4
//      --post-file=$HOME/Videos/Earth-From-Space.mp4
//
#include <boost/shared_ptr.hpp>
#include <boost/network/protocol/http/server.hpp>
#include <boost/network/utils/thread_pool.hpp>
#include <boost/asio.hpp>

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <map>

struct connection_handler;

typedef boost::network::http::server<connection_handler> server;

///
/// Custom exception type
///
struct file_uploader_exception : public std::runtime_error {
    file_uploader_exception(const std::string err) :
        std::runtime_error(err) {
    }
};

///
/// Encapsulates request & connection
///
struct file_uploader : std::enable_shared_from_this<file_uploader> {
    const server::request&  req;
    server::connection_ptr  conn;

    std::mutex              mtx;
    std::condition_variable condvar;

    FILE*                   fp = NULL;

public:
    file_uploader(const server::request& req, const server::connection_ptr& conn)
        : req(req)
        , conn(conn) {
        const std::string dest = destination(req);

        if (dest.find("/upload") != std::string::npos) {
            auto queries = get_queries(dest);
            auto fname = queries.find("filename");
            if (fname != queries.end()) {
                fp = ::fopen(fname->second.c_str(), "wb");
                if (!fp) {
                    throw file_uploader_exception("Failed to open file to write");
                }
            } else {
                throw file_uploader_exception("'filename' cannot be empty");
            }
        }
    }

    ~file_uploader() {
        if (fp) {
            ::fflush(fp);
            ::fclose(fp);
        }
    }

    ///
    /// Non blocking call to initiate the data transfer
    ///
    void async_recv() {
        std::size_t content_length = 0;
        auto const& headers = req.headers;
        for (auto item : headers) {
            if (boost::to_lower_copy(item.name) == "content-length") {
                content_length = std::stoll(item.value);
                break;
            }
        }

        read_chunk(conn, content_length);
    }

    ///
    /// The client shall wait by calling this until the transfer is done by
    /// the IO threadpool
    ///
    void wait_for_completion() {
        std::unique_lock<std::mutex> _(mtx);
        condvar.wait(_);
    }

private:
    ///
    /// Parses the string and gets the query as a key-value pair
    ///
    /// @param [in] dest String containing the path and the queries, without the fragment,
    ///                  of the form "/path?key1=value1&key2=value2"
    ///
    std::map<std::string, std::string> get_queries(const std::string dest) {

        std::size_t pos = dest.find_first_of("?");

        std::map<std::string, std::string> queries;
        if (pos != std::string::npos) {
            std::string query_string = dest.substr(pos + 1);

            // Replace '&' with space
            for (pos = 0; pos < query_string.size(); pos++) {
                if (query_string[pos] == '&') {
                    query_string[pos] = ' ';
                }
            }

            std::istringstream sin(query_string);
            while (sin >> query_string) {

                pos = query_string.find_first_of("=");

                if (pos != std::string::npos) {
                    const std::string key = query_string.substr(0, pos);
                    const std::string value = query_string.substr(pos + 1);
                    queries[key] = value;
                }
            }
        }

        return queries;
    }

    ///
    /// Reads a chunk of data
    ///
    /// @param [in] conn        Connection to read from
    /// @param [in] left2read   Size to read
    ///
    void read_chunk(server::connection_ptr conn, std::size_t left2read) {
        conn->read(boost::bind(&file_uploader::on_data_ready,
                               file_uploader::shared_from_this(),
                               _1, _2, _3, conn, left2read));
    }

    ///
    /// Callback that gets called when the data is ready to be consumed
    ///
    void on_data_ready(server::connection::input_range range,
                       boost::system::error_code error,
                       std::size_t size,
                       server::connection_ptr conn,
                       std::size_t left2read) {
        if (!error) {
            ::fwrite(boost::begin(range), size, 1, fp);
            std::size_t left = left2read - size;
            if (left > 0)
                read_chunk(conn, left);
            else
                wakeup();
        }
    }

    ///
    /// Wakesup the waiting thread
    ///
    void wakeup() {
        std::unique_lock<std::mutex> _(mtx);
        condvar.notify_one();
    }
};

///
/// Functor that gets executed whenever there is a packet on the HTTP port
///
struct connection_handler {
    ///
    /// Gets executed whenever there is a packet on the HTTP port.
    ///
    /// @param [in] req Request object that holds the protobuf data
    /// @param [in] conn Connection object
    ///
    void operator()(server::request const& req, const server::connection_ptr& conn) {
        static std::map<std::string, std::string> headers = {
            {"Connection","close"},
            {"Content-Type", "text/plain"}
        };

        const std::string dest = destination(req);

        if (req.method == "POST" && dest.find("/upload") != std::string::npos) {
            try {
                auto start = std::chrono::high_resolution_clock::now();
                // Create a file uploader
                std::shared_ptr<file_uploader> uploader(new file_uploader(req, conn));
                // On success to create, start receiving the data
                uploader->async_recv();
                // Wait until the data transfer is done by the IO threads
                uploader->wait_for_completion();

                // Respond to the client
                conn->set_status(server::connection::ok);
                conn->set_headers(headers);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> diff = end - start;
                std::ostringstream stm;
                stm << "Took " << diff.count() << " milliseconds for the transfer." << std::endl;
                conn->write(stm.str());
            } catch (const file_uploader_exception& e) {
                conn->set_status(server::connection::bad_request);
                conn->set_headers(headers);
                const std::string err = e.what();
                conn->write(err);
            }
        } else {
            conn->set_status(server::connection::bad_request);
            conn->set_headers(headers);
            conn->write("Only path allowed is /upload.");
        }
    }
};

int main(int ac, const char *av[])
{
    if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " <listener-port>" << std::endl;
        return EXIT_SUCCESS;
    }

    // Create a connection handler
    connection_handler handler;

    // Setup the async server
    server local_server(server::options(handler)
                        .address("0.0.0.0")
                        .port(av[1])
                        .reuse_address(true)
                        .thread_pool(std::make_shared<boost::network::utils::thread_pool>(2)));

    // Start the server eventloop
    local_server.run();

    return EXIT_SUCCESS;
}
