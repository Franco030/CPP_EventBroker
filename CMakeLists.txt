cmake_minimum_required(VERSION 3.15)
project(cpp_event_broker VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add the cpp_orm project from GitHub
include(FetchContent)
FetchContent_Declare(
  cpp_orm
  GIT_REPOSITORY https://github.com/Franco030/CPP_ORM.git
  GIT_TAG main
)
FetchContent_MakeAvailable(cpp_orm)

# Add JSON library
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# Event broker executable
add_executable(event_broker 
    src/main.cpp
    src/server/TcpServer.cpp
)

target_include_directories(event_broker PRIVATE include)

# Link against the 'orm' target from the cpp_orm project
find_package(Threads REQUIRED)
target_link_libraries(event_broker PRIVATE orm Threads::Threads nlohmann_json::nlohmann_json)

# Test Client executable
add_executable(broker_client examples/client.cpp)
