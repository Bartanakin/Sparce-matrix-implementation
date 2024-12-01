set(PROJECT_NAME ComparisonMultMatrix)
project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} main.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread" )
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/config.txt ${PROJECT_BINARY_DIR}/config.txt
        COMMENT "Copied ${PROJECT_SOURCE_DIR}/config.txt to ${PROJECT_BINARY_DIR}/config.txt"
)