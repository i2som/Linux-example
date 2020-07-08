project(network-example)

add_executable(tcp_client tcp_client.c)
add_executable(tcp_server tcp_server.c)

install(TARGETS tcp_client RUNTIME DESTINATION .)
install(TARGETS tcp_server RUNTIME DESTINATION .)
