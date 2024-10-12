project(BitsaversSearch C CXX)
cmake_minimum_required(VERSION 3.3.2)

include(cmake/App.cmake)

set(SOURCES "src/json/json-forwards.h"
            "src/json/json.h"
            "src/jsoncpp.cpp"
            "src/Updater.h"
            "src/Updater.cpp"
            "src/MyApp.h"
            "src/MyApp.cpp"
            "src/main.cpp")

add_app("${SOURCES}")
target_include_directories(BitsaversSearch PRIVATE "deps/asio/include")
