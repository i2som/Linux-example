project(libgpiod-user)

add_executable(rs485-receive rs485_receive.c serial.c)
add_executable(rs485-send rs485_send.c serial.c)
add_executable(uart-test uart_test.c)
add_executable(uart-select uart_select.c)

install(TARGETS rs485-receive RUNTIME DESTINATION .)
install(TARGETS rs485-send RUNTIME DESTINATION .)
install(TARGETS uart-test RUNTIME DESTINATION .)
install(TARGETS uart-select RUNTIME DESTINATION .)
