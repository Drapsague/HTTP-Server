cmake_minimum_required(VERSION 3.14)
project(ChatApp)

set(CMAKE_CXX_STANDARD 17)             

include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG origin/main
)
FetchContent_MakeAvailable(ftxui)       

add_executable(main 
  src/main.cpp
  src/client.cpp
)

target_link_libraries(main PRIVATE ftxui::screen ftxui::dom ftxui::component)

