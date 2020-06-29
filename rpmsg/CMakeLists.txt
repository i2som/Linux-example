project(rpmsg-example)

add_executable(rpmsg rpmsg.c)

install(TARGETS rpmsg RUNTIME DESTINATION .)
