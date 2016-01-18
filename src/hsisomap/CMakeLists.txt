set(HSISOMAP_HEADER_FILE_NAMES hsisomap HsiData.h Matrix.h typedefs.h Logger.h HsiDataMask.h)

foreach (FILE ${HSISOMAP_HEADER_FILE_NAMES})
    list(APPEND HSISOMAP_HEADER_FILES "${HSISOMAP_INCLUDE_DIR}/hsisomap/${FILE}")
endforeach ()

set(HSISOMAP_SOURCE_FILES ${HSISOMAP_HEADER_FILES} HsiData.cpp Matrix.cpp Logger.cpp HsiDataMask.cpp)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIR})
find_package(GSL REQUIRED)
include_directories(${GSL_INCLUDE_DIR})
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_library(hsisomap ${HSISOMAP_SOURCE_FILES})
target_link_libraries(hsisomap ${OpenCL_LIBRARY} ${GSL_LIBRARY})
target_compile_definitions(hsisomap PRIVATE $<$<CONFIG:Debug>:NLOGTERMCOLOR>) # no terminal color when log in debug mode

