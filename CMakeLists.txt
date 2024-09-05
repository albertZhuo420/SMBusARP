cmake_minimum_required(VERSION 3.10)
project(smbusarp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#添加头文件路径
include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable(${PROJECT_NAME} 
	${PROJECT_SOURCE_DIR}/src/SMBusARP.cpp
	${PROJECT_SOURCE_DIR}/src/SMBus.cpp
	${PROJECT_SOURCE_DIR}/src/main.cpp
)
