set(CLI_SOURCE_FILES hsisomap_cli.cpp)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIR})
find_package(GSL REQUIRED)
include_directories(${GSL_INCLUDE_DIR})
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIR})

add_executable(hsisomap_cli ${CLI_SOURCE_FILES})
target_link_libraries(hsisomap_cli hsisomap ${Boost_LIBRARIES})
