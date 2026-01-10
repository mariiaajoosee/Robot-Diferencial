cmake_minimum_required(VERSION 3.8)
project(tarea1)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

# Includes
include_directories(
 include
)


# Install launch files and related config files.
install(DIRECTORY
  launch
 DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY
  config
 DESTINATION share/${PROJECT_NAME}
)

# Add Executables
add_executable(velocidad src/velocidad.cpp)
add_executable(posicion src/posicion.cpp)
add_executable(circulo src/circulo.cpp)
add_executable(posicion_control src/posicion_control.cpp)
add_executable(follow src/follow.cpp)
add_executable(sensor src/sensor.cpp)
add_executable(sensor_simple src/sensor_simple.cpp)
add_executable(mando src/mando.cpp)
# Dependencies
ament_target_dependencies(velocidad rclcpp geometry_msgs)
ament_target_dependencies(posicion rclcpp geometry_msgs)
ament_target_dependencies(circulo rclcpp geometry_msgs)
ament_target_dependencies(posicion_control rclcpp geometry_msgs)
ament_target_dependencies(follow rclcpp geometry_msgs)
ament_target_dependencies(sensor rclcpp geometry_msgs)
ament_target_dependencies(sensor_simple rclcpp geometry_msgs)
ament_target_dependencies(mando rclcpp geometry_msgs sensor_msgs)

# Declare and install executables
install(TARGETS
 velocidad
 posicion
 circulo
 posicion_control
 follow
 sensor
 sensor_simple
 mando
 DESTINATION lib/${PROJECT_NAME}
)

ament_package()
