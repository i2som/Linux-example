project(framebuffer-example)

add_executable(framebuffer-test fb_test.c)

install(TARGETS framebuffer-test RUNTIME DESTINATION bin)
