cmake_minimum_required(VERSION 3.2)
project(linux-examples-c)

add_subdirectory(canbus)
add_subdirectory(gpio-legacy)
add_subdirectory(gpio-libgpiod)
add_subdirectory(rtc)
add_subdirectory(serialport)
add_subdirectory(watchdog)
add_subdirectory(audio)
add_subdirectory(spi)
add_subdirectory(rpmsg)
add_subdirectory(network)
add_subdirectory(framebuffer)
add_subdirectory(i2ceeprom)
