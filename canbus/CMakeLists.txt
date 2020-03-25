project(can-example)

add_executable(can-send can_send.c)
add_executable(can-receive can_receive.c)

install(TARGETS can-send RUNTIME DESTINATION .)
install(TARGETS can-receive RUNTIME DESTINATION .)
