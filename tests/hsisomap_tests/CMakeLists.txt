set(HSISOMAP_TESTS_SOURCE_FILES basic_check.cpp HsiData_check.cpp gsl_util_check.cpp Subsetter_check.cpp)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(run_hsisomap_tests ${HSISOMAP_TESTS_SOURCE_FILES})

target_link_libraries(run_hsisomap_tests gtest gtest_main)
target_link_libraries(run_hsisomap_tests hsisomap)

set(HSISOMAP_TESTS_TEST_DATA_PATH ".")
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(HSISOMAP_TESTS_TEST_DATA_PATH ${CMAKE_BINARY_DIR}/Debug/bin)
endif(CMAKE_BUILD_TYPE MATCHES Debug)
if(CMAKE_BUILD_TYPE MATCHES Release)
    set(HSISOMAP_TESTS_TEST_DATA_PATH ${CMAKE_BINARY_DIR}/Release/bin)
endif(CMAKE_BUILD_TYPE MATCHES Release)

file(COPY "test_data" DESTINATION ${HSISOMAP_TESTS_TEST_DATA_PATH})

