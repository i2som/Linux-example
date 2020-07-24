project(i2ceeprom-example)

add_executable(i2ceeprom-test i2ceeprom_test.c)

install(TARGETS i2ceeprom-test RUNTIME DESTINATION bin)
