include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

add_executable(
    libtest
    src/validate.cpp
    src/load.cpp
)

target_link_libraries(
    libtest
    gtest_main
    gmock_main
    uzuki
)

include(GoogleTest)
gtest_discover_tests(libtest)
