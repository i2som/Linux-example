project(spi-example)

add_executable(spidev_test spidev_test.c)

install(TARGETS spidev_test RUNTIME DESTINATION .)
