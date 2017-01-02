set(DEMO_SOURCE_FILES demo.cpp)

find_package(GSL REQUIRED)
include_directories(${GSL_INCLUDE_DIR})
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(demo ${DEMO_SOURCE_FILES})
target_link_libraries(demo hsisomap)

