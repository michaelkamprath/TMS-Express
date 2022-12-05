cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)

include(cmake/FindJSON.cmake)

include(FetchContent)
FetchContent_Declare(googletest
        GIT_REPOSITORY "https://github.com/google/googletest.git"
        GIT_TAG "release-1.12.1"
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(TMS-Test
        src/LPC_Analysis/Autocorrelator.cpp
        test/AutocorrelatorTests.cpp

        src/Frame_Encoding/Frame.cpp
        test/FrameTests.cpp

        src/Frame_Encoding/FrameEncoder.cpp
        test/FrameEncoderTests.cpp
        )

target_link_libraries(
        TMS-Test
        nlohmann_json::nlohmann_json
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(TMS-Test)

