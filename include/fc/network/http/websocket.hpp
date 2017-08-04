#pragma once

#include <functional>
#include <memory>
#include <string>
#include <fc/any.hpp>
#include <fc/network/ip.hpp>
#include <fc/signals.hpp>
#include <chrono>

namespace fc {
    namespace http {
        namespace detail {
            class websocket_server_impl;

            class websocket_tls_server_impl;

            class websocket_client_impl;

            class websocket_tls_client_impl;
        } // namespace detail;

        using clock=std::chrono::steady_clock;
        using millisecond = std::chrono::milliseconds;
        using second = std::chrono::milliseconds;

        class websocket_connection {
        public:
            websocket_connection(const uint64_t limit) : limit(limit), counter(0) {}

            virtual ~websocket_connection() {}

            virtual void send_message(const std::string &message) = 0;

            virtual void close(int64_t code, const std::string &reason) {};

            void on_message(const std::string &message) {
                auto current_time = clock::now();

                if (limit>=counter) {
                    _on_message(message);
                    ++counter;
                }

                auto delta = std::chrono::duration_cast<std::chrono::seconds>(current_time - time);

                if (delta > std::chrono::seconds(0)) {
                  time = current_time;
                } else {
                   counter = 0;
                   time = current_time;
               }

            }

            string on_http(const std::string &message) { return _on_http(message); }

            void on_message_handler(const std::function<void(const std::string &)> &h) { _on_message = h; }

            void on_http_handler(const std::function<std::string(const std::string &)> &h) { _on_http = h; }

            void set_session_data(fc::any d) { _session_data = std::move(d); }

            fc::any &get_session_data() { return _session_data; }

            fc::signal<void()> closed;
        private:
            fc::any _session_data;
            std::function<void(const std::string &)> _on_message;
            std::function<string(const std::string &)> _on_http;
            const uint64_t limit;
            uint64_t counter;
            clock::time_point time;
        };

        typedef std::shared_ptr<websocket_connection> websocket_connection_ptr;

        typedef std::function<void(const websocket_connection_ptr &)> on_connection_handler;

        class websocket_server {
        public:
            websocket_server(const uint64_t limit);

            ~websocket_server();

            void on_connection(const on_connection_handler &handler);

            void listen(uint16_t port);

            void listen(const fc::ip::endpoint &ep);

            void start_accept();

        private:
            friend class detail::websocket_server_impl;

            std::unique_ptr<detail::websocket_server_impl> my;
        };


        class websocket_tls_server {
        public:
            websocket_tls_server(
                    const std::string &server_pem = std::string(),
                    const std::string &ssl_password = std::string(),
                    const uint64_t limit = -1
            );

            ~websocket_tls_server();

            void on_connection(const on_connection_handler &handler);

            void listen(uint16_t port);

            void listen(const fc::ip::endpoint &ep);

            void start_accept();

        private:
            friend class detail::websocket_tls_server_impl;

            std::unique_ptr<detail::websocket_tls_server_impl> my;
        };

        class websocket_client {
        public:
            websocket_client(const std::string &ca_filename = "_default");

            ~websocket_client();

            websocket_connection_ptr connect(const std::string &uri);

            websocket_connection_ptr secure_connect(const std::string &uri);

        private:
            std::unique_ptr<detail::websocket_client_impl> my;
            std::unique_ptr<detail::websocket_tls_client_impl> smy;
        };

        class websocket_tls_client {
        public:
            websocket_tls_client(const std::string &ca_filename = "_default");

            ~websocket_tls_client();

            websocket_connection_ptr connect(const std::string &uri);

        private:
            std::unique_ptr<detail::websocket_tls_client_impl> my;
        };

    }
}
