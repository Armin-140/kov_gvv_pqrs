cmake_minimum_required(VERSION 3.8)
project(kov_gvv_pqrs)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)

set(ament_dependencies
  rclcpp
  std_msgs
  geometry_msgs
  nav_msgs
)

add_executable(lissajous_generator src/lissajous_generator.cpp)
ament_target_dependencies(lissajous_generator ${ament_dependencies})

install(TARGETS
  lissajous_generator
  DESTINATION lib/${PROJECT_NAME})

ament_package()
