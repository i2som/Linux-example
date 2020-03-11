project(audio-alsa-example)

add_executable(rtc-test rtc_test.c)

install(TARGETS rtc-test RUNTIME DESTINATION .)
