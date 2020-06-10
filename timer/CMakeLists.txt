project(timer)

add_executable(posix-timer posix-timer.c)

install(TARGETS posix-timer RUNTIME DESTINATION .)
