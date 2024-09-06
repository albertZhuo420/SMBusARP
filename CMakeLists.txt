cmake_minimum_required(VERSION 3.10)
project(smbusarp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

function(get_gxx_version version_var)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
                    OUTPUT_VARIABLE version_output
                    ERROR_QUIET
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" version ${version_output})
    set(${version_var} ${version} PARENT_SCOPE)
endfunction()

get_gxx_version(GXX_VERSION)
message(STATUS "Detected G++ version: ${GXX_VERSION}")

# 比较版本号
function(compare_versions version1 version2 result_var)
    if(${version1} VERSION_LESS_EQUAL ${version2})
        set(${result_var} TRUE PARENT_SCOPE)
    else()
        set(${result_var} FALSE PARENT_SCOPE)
    endif()
endfunction()

compare_versions(${GXX_VERSION} "8.5" GXX_VERSION_LESS_EQUAL)
message(STATUS "G++ version <= 8.5: ${GXX_VERSION_LESS_EQUAL}")

# 添加std::filesystem库
if(GXX_VERSION_LESS_EQUAL)
    message(STATUS "G++ version is 8.5 or lower, adding -lstdc++fs")
    set(ADDITIONAL_LIBRARIES "-lstdc++fs")
endif()

#添加头文件路径
include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable(${PROJECT_NAME} 
	${PROJECT_SOURCE_DIR}/src/SMBus_ARP.cpp
	${PROJECT_SOURCE_DIR}/src/I2C_SMBus.cpp
	${PROJECT_SOURCE_DIR}/src/main.cpp
)

if(GXX_VERSION_LESS_EQUAL)
	target_link_libraries(${PROJECT_NAME}  ${ADDITIONAL_LIBRARIES})
endif()


