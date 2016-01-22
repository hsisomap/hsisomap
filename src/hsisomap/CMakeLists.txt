set(HSISOMAP_HEADER_FILE_NAMES hsisomap HsiData.h Matrix.h typedefs.h Logger.h HsiDataMask.h)
set(HSISOMAP_HEADER_FILE_NAMES ${HSISOMAP_HEADER_FILE_NAMES} subsetter/Subsetter.h subsetter/SubsetterEmbedding.h)
set(HSISOMAP_HEADER_FILE_NAMES ${HSISOMAP_HEADER_FILE_NAMES} gsl_util/embedding.h gsl_util/gsl_util.h gsl_util/matrix_util.h)
set(HSISOMAP_HEADER_FILE_NAMES ${HSISOMAP_HEADER_FILE_NAMES} backbone/Backbone.h)
set(HSISOMAP_HEADER_FILE_NAMES ${HSISOMAP_HEADER_FILE_NAMES} util/VpTree.h util/io_util.h)
set(HSISOMAP_HEADER_FILE_NAMES ${HSISOMAP_HEADER_FILE_NAMES} manifold_constructor/ManifoldConstructor.h)
set(HSISOMAP_HEADER_FILE_NAMES ${HSISOMAP_HEADER_FILE_NAMES} graph/AdjacencyList.h graph/BoostAdjacencyList.h graph/UndirectedWeightedGraph.h)
set(HSISOMAP_HEADER_FILE_NAMES ${HSISOMAP_HEADER_FILE_NAMES} graph/dijkstra/BoostDijkstra.h graph/dijkstra/DijkstraCL.h graph/dijkstra/Dijkstra.h)

foreach (FILE ${HSISOMAP_HEADER_FILE_NAMES})
    list(APPEND HSISOMAP_HEADER_FILES "${HSISOMAP_INCLUDE_DIR}/hsisomap/${FILE}")
endforeach ()

set(HSISOMAP_SOURCE_FILES ${HSISOMAP_HEADER_FILES} HsiData.cpp Matrix.cpp Logger.cpp)
set(HSISOMAP_SOURCE_FILES ${HSISOMAP_SOURCE_FILES} subsetter/Subsetter.cpp subsetter/SubsetterEmbedding.cpp)
set(HSISOMAP_SOURCE_FILES ${HSISOMAP_SOURCE_FILES} gsl_util/embedding.cpp gsl_util/matrix_util.cpp)
set(HSISOMAP_SOURCE_FILES ${HSISOMAP_SOURCE_FILES} backbone/Backbone.cpp)
set(HSISOMAP_SOURCE_FILES ${HSISOMAP_SOURCE_FILES} manifold_constructor/ManifoldConstructor.cpp)
set(HSISOMAP_SOURCE_FILES ${HSISOMAP_SOURCE_FILES} graph/AdjacencyList.cpp graph/BoostAdjacencyList.cpp)
set(HSISOMAP_SOURCE_FILES ${HSISOMAP_SOURCE_FILES} graph/dijkstra/BoostDijkstra.cpp graph/dijkstra/DijkstraCL.cpp graph/dijkstra/Dijkstra.cpp)


find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIR})
find_package(GSL REQUIRED)
include_directories(${GSL_INCLUDE_DIR})
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_library(hsisomap ${HSISOMAP_SOURCE_FILES})
target_link_libraries(hsisomap ${OpenCL_LIBRARY} ${GSL_LIBRARY})
target_compile_definitions(hsisomap PRIVATE $<$<CONFIG:Debug>:NLOGTERMCOLOR>) # no terminal color when log in debug mode
target_compile_definitions(hsisomap PRIVATE TYPEDEFS_CL_INDEX=int TYPEDEFS_CL_SCALAR=float)

