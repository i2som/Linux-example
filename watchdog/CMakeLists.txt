project(audio-alsa-example)

add_executable(watchdog-test watchdog.c)

install(TARGETS watchdog-test RUNTIME DESTINATION .)
