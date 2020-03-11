project(libgpiod-user)

add_executable(libgpiod-led libgpiod-led.c)
add_executable(libgpiod-input libgpiod-input.c)
add_executable(libgpiod-event libgpiod-event.c)

target_link_libraries(libgpiod-led gpiod)
target_link_libraries(libgpiod-input gpiod)
target_link_libraries(libgpiod-event gpiod)

install(TARGETS libgpiod-led RUNTIME DESTINATION .)
install(TARGETS libgpiod-input RUNTIME DESTINATION .)
install(TARGETS libgpiod-event RUNTIME DESTINATION .)
