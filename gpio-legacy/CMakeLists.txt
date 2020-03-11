project(audio-alsa-example)

add_executable(gpio-key gpio_key.c)

install(TARGETS gpio-key RUNTIME DESTINATION .)
