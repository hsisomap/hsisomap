set(CLI_SOURCE_FILES hsisomap_cli.cpp)
add_executable(hsisomap_cli ${CLI_SOURCE_FILES})
target_link_libraries(hsisomap_cli hsisomap)
