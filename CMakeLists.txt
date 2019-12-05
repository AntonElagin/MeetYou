GroupSources(include/boost/beast beast)
GroupSources(example/http/server/sync "/")

add_executable (http-server-sync
    ${BOOST_BEAST_FILES}
    Jamfile
    http_server_sync.cpp
)

target_link_libraries(http-server-sync
    lib-asio
    lib-asio-ssl
    lib-beast)

set_property(TARGET http-server-sync PROPERTY FOLDER "example-http-server")