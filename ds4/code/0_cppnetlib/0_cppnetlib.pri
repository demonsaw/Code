
# Headers
HEADERS += $$PWD/boost/mime.hpp
HEADERS += $$PWD/boost/network.hpp
HEADERS += $$PWD/boost/network/constants.hpp
HEADERS += $$PWD/boost/network/detail/debug.hpp
HEADERS += $$PWD/boost/network/detail/directive_base.hpp
HEADERS += $$PWD/boost/network/detail/wrapper_base.hpp
HEADERS += $$PWD/boost/network/include/http/client.hpp
HEADERS += $$PWD/boost/network/include/http/server.hpp
HEADERS += $$PWD/boost/network/include/message.hpp
HEADERS += $$PWD/boost/network/message.hpp
HEADERS += $$PWD/boost/network/message/directives.hpp
HEADERS += $$PWD/boost/network/message/directives/detail/string_directive.hpp
HEADERS += $$PWD/boost/network/message/directives/detail/string_value.hpp
HEADERS += $$PWD/boost/network/message/directives/header.hpp
HEADERS += $$PWD/boost/network/message/directives/remove_header.hpp
HEADERS += $$PWD/boost/network/message/modifiers/add_header.hpp
HEADERS += $$PWD/boost/network/message/modifiers/body.hpp
HEADERS += $$PWD/boost/network/message/modifiers/clear_headers.hpp
HEADERS += $$PWD/boost/network/message/modifiers/destination.hpp
HEADERS += $$PWD/boost/network/message/modifiers/remove_header.hpp
HEADERS += $$PWD/boost/network/message/modifiers/source.hpp
HEADERS += $$PWD/boost/network/message/traits/body.hpp
HEADERS += $$PWD/boost/network/message/traits/destination.hpp
HEADERS += $$PWD/boost/network/message/traits/headers.hpp
HEADERS += $$PWD/boost/network/message/traits/source.hpp
HEADERS += $$PWD/boost/network/message/transformers.hpp
HEADERS += $$PWD/boost/network/message/transformers/selectors.hpp
HEADERS += $$PWD/boost/network/message/transformers/to_lower.hpp
HEADERS += $$PWD/boost/network/message/transformers/to_upper.hpp
HEADERS += $$PWD/boost/network/message/wrappers.hpp
HEADERS += $$PWD/boost/network/message/wrappers/body.hpp
HEADERS += $$PWD/boost/network/message/wrappers/destination.hpp
HEADERS += $$PWD/boost/network/message/wrappers/headers.hpp
HEADERS += $$PWD/boost/network/message/wrappers/source.hpp
HEADERS += $$PWD/boost/network/message_fwd.hpp
HEADERS += $$PWD/boost/network/protocol.hpp
HEADERS += $$PWD/boost/network/protocol/http.hpp
HEADERS += $$PWD/boost/network/protocol/http/algorithms/linearize.hpp
HEADERS += $$PWD/boost/network/protocol/http/client.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/async_impl.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/async_base.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/async_normal.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/async_protocol_handler.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/connection_delegate.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/connection_delegate_factory.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/normal_delegate.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/normal_delegate.ipp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/ssl_delegate.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/ssl_delegate.ipp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/sync_base.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/sync_normal.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/connection/sync_ssl.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/facade.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/macros.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/options.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/pimpl.hpp
HEADERS += $$PWD/boost/network/protocol/http/client/sync_impl.hpp
HEADERS += $$PWD/boost/network/protocol/http/errors.hpp
HEADERS += $$PWD/boost/network/protocol/http/impl/message.ipp
HEADERS += $$PWD/boost/network/protocol/http/impl/request.hpp
HEADERS += $$PWD/boost/network/protocol/http/impl/request_parser.ipp
HEADERS += $$PWD/boost/network/protocol/http/impl/response.ipp
HEADERS += $$PWD/boost/network/protocol/http/message.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/async_message.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/directives/major_version.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/directives/method.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/directives/minor_version.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/directives/status.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/directives/status_message.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/directives/uri.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/directives/version.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/header.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/header/name.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/header/value.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/message_base.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/body.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/clear_headers.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/destination.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/headers.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/major_version.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/method.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/minor_version.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/source.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/status.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/status_message.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/uri.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/modifiers/version.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/traits/status.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/traits/status_message.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/traits/version.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/anchor.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/body.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/destination.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/headers.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/helper.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/host.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/major_version.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/method.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/minor_version.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/path.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/port.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/protocol.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/query.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/ready.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/source.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/status.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/status_message.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/uri.hpp
HEADERS += $$PWD/boost/network/protocol/http/message/wrappers/version.hpp
HEADERS += $$PWD/boost/network/protocol/http/parser.hpp
HEADERS += $$PWD/boost/network/protocol/http/parser/incremental.hpp
HEADERS += $$PWD/boost/network/protocol/http/policies/async_connection.hpp
HEADERS += $$PWD/boost/network/protocol/http/policies/async_resolver.hpp
HEADERS += $$PWD/boost/network/protocol/http/policies/pooled_connection.hpp
HEADERS += $$PWD/boost/network/protocol/http/policies/simple_connection.hpp
HEADERS += $$PWD/boost/network/protocol/http/policies/sync_resolver.hpp
HEADERS += $$PWD/boost/network/protocol/http/request.hpp
HEADERS += $$PWD/boost/network/protocol/http/request_parser.hpp
HEADERS += $$PWD/boost/network/protocol/http/response.hpp
HEADERS += $$PWD/boost/network/protocol/http/server.hpp
HEADERS += $$PWD/boost/network/protocol/http/server/async_connection.hpp
HEADERS += $$PWD/boost/network/protocol/http/server/async_server.hpp
HEADERS += $$PWD/boost/network/protocol/http/server/impl/parsers.ipp
HEADERS += $$PWD/boost/network/protocol/http/server/options.hpp
HEADERS += $$PWD/boost/network/protocol/http/server/request.hpp
HEADERS += $$PWD/boost/network/protocol/http/server/request_parser.hpp
HEADERS += $$PWD/boost/network/protocol/http/server/socket_options_base.hpp
HEADERS += $$PWD/boost/network/protocol/http/server/storage_base.hpp
HEADERS += $$PWD/boost/network/protocol/http/support/client_or_server.hpp
HEADERS += $$PWD/boost/network/protocol/http/support/is_client.hpp
HEADERS += $$PWD/boost/network/protocol/http/support/is_http.hpp
HEADERS += $$PWD/boost/network/protocol/http/support/is_keepalive.hpp
HEADERS += $$PWD/boost/network/protocol/http/support/is_server.hpp
HEADERS += $$PWD/boost/network/protocol/http/support/is_simple.hpp
HEADERS += $$PWD/boost/network/protocol/http/tags.hpp
HEADERS += $$PWD/boost/network/protocol/http/traits.hpp
HEADERS += $$PWD/boost/network/protocol/http/traits/connection_keepalive.hpp
HEADERS += $$PWD/boost/network/protocol/http/traits/connection_policy.hpp
HEADERS += $$PWD/boost/network/protocol/http/traits/delegate_factory.hpp
HEADERS += $$PWD/boost/network/protocol/http/traits/impl/chunk_cache.ipp
HEADERS += $$PWD/boost/network/protocol/http/traits/impl/content.ipp
HEADERS += $$PWD/boost/network/protocol/http/traits/impl/cookies_container.ipp
HEADERS += $$PWD/boost/network/protocol/http/traits/impl/delimiters.ipp
HEADERS += $$PWD/boost/network/protocol/http/traits/impl/headers.ipp
HEADERS += $$PWD/boost/network/protocol/http/traits/impl/headers_container.ipp
HEADERS += $$PWD/boost/network/protocol/http/traits/impl/query_container.ipp
HEADERS += $$PWD/boost/network/protocol/http/traits/impl/request_methods.ipp
HEADERS += $$PWD/boost/network/protocol/http/traits/impl/response_code.ipp
HEADERS += $$PWD/boost/network/protocol/http/traits/impl/response_message.ipp
HEADERS += $$PWD/boost/network/protocol/http/traits/message_traits.hpp
HEADERS += $$PWD/boost/network/protocol/http/traits/parser_traits.hpp
HEADERS += $$PWD/boost/network/protocol/http/traits/resolver.hpp
HEADERS += $$PWD/boost/network/protocol/http/traits/resolver_policy.hpp
HEADERS += $$PWD/boost/network/protocol/stream_handler.hpp
HEADERS += $$PWD/boost/network/support/is_async.hpp
HEADERS += $$PWD/boost/network/support/is_default_string.hpp
HEADERS += $$PWD/boost/network/support/is_default_wstring.hpp
HEADERS += $$PWD/boost/network/support/is_http.hpp
HEADERS += $$PWD/boost/network/support/is_keepalive.hpp
HEADERS += $$PWD/boost/network/support/is_pod.hpp
HEADERS += $$PWD/boost/network/support/is_simple.hpp
HEADERS += $$PWD/boost/network/support/is_sync.hpp
HEADERS += $$PWD/boost/network/support/is_tcp.hpp
HEADERS += $$PWD/boost/network/support/is_udp.hpp
HEADERS += $$PWD/boost/network/support/pod_or_normal.hpp
HEADERS += $$PWD/boost/network/tags.hpp
HEADERS += $$PWD/boost/network/traits/char.hpp
HEADERS += $$PWD/boost/network/traits/headers_container.hpp
HEADERS += $$PWD/boost/network/traits/istream.hpp
HEADERS += $$PWD/boost/network/traits/istringstream.hpp
HEADERS += $$PWD/boost/network/traits/ostream_iterator.hpp
HEADERS += $$PWD/boost/network/traits/ostringstream.hpp
HEADERS += $$PWD/boost/network/traits/string.hpp
HEADERS += $$PWD/boost/network/traits/vector.hpp
HEADERS += $$PWD/boost/network/uri.hpp
HEADERS += $$PWD/boost/network/uri/accessors.hpp
HEADERS += $$PWD/boost/network/uri/builder.hpp
HEADERS += $$PWD/boost/network/uri/config.hpp
HEADERS += $$PWD/boost/network/uri/decode.hpp
HEADERS += $$PWD/boost/network/uri/detail/uri_parts.hpp
HEADERS += $$PWD/boost/network/uri/directives.hpp
HEADERS += $$PWD/boost/network/uri/directives/authority.hpp
HEADERS += $$PWD/boost/network/uri/directives/fragment.hpp
HEADERS += $$PWD/boost/network/uri/directives/host.hpp
HEADERS += $$PWD/boost/network/uri/directives/path.hpp
HEADERS += $$PWD/boost/network/uri/directives/port.hpp
HEADERS += $$PWD/boost/network/uri/directives/query.hpp
HEADERS += $$PWD/boost/network/uri/directives/scheme.hpp
HEADERS += $$PWD/boost/network/uri/directives/user_info.hpp
HEADERS += $$PWD/boost/network/uri/encode.hpp
HEADERS += $$PWD/boost/network/uri/schemes.hpp
HEADERS += $$PWD/boost/network/uri/uri.hpp
HEADERS += $$PWD/boost/network/uri/uri.ipp
HEADERS += $$PWD/boost/network/uri/uri_io.hpp
HEADERS += $$PWD/boost/network/utils/base64/encode.hpp
HEADERS += $$PWD/boost/network/utils/base64/encode-io.hpp
HEADERS += $$PWD/boost/network/utils/thread_group.hpp
HEADERS += $$PWD/boost/network/utils/thread_pool.hpp
HEADERS += $$PWD/boost/network/version.hpp


# Sources
SOURCES += $$PWD/libs/network/src/client.cpp
SOURCES += $$PWD/libs/network/src/server_request_parsers_impl.cpp
SOURCES += $$PWD/libs/network/src/uri/schemes.cpp
SOURCES += $$PWD/libs/network/src/uri/uri.cpp
