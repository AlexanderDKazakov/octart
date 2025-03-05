cmake_minimum_required(VERSION 3.10)
project(octart)

enable_testing()

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(googletest)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(octart
    main.cpp
    ./src/receiver/server.cpp
    ./src/sender/send.cpp
)

add_executable(octotest
  ./src/receiver/server.cpp
  ./src/sender/send.cpp
  ./src/test_main.cpp
)

target_link_libraries(octotest
  gtest gtest_main
)

add_test(NAME MyTest COMMAND octotest)

add_custom_target(tests
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target octotest
  COMMAND ./octotest
  DEPENDS octotest
)
