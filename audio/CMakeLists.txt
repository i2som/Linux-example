project(audio-alsa-example)

add_executable(alsa_capture alsa_capture.c)
add_executable(alsa_play alsa_play.c)

target_link_libraries(alsa_capture asound)
target_link_libraries(alsa_play asound)

install(TARGETS alsa_capture RUNTIME DESTINATION .)
install(TARGETS alsa_play RUNTIME DESTINATION .)
