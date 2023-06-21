cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ZLDemo)

add_executable(${PROJECT_NAME} main.cpp media_server.cpp)

find_package(PkgConfig REQUIRED)

pkg_check_modules(mkapi mkapi REQUIRED IMPORTED_TARGET)

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::mkapi)

# target_include_directories(${PROJECT_NAME}
#     PRIVATE
#     /usr/local/include)

# target_link_directories(${PROJECT_NAME}
#     PRIVATE
#     /usr/local/lib)

# target_link_libraries(${PROJECT_NAME} mk_api)


