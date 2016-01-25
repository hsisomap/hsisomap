//
// Created by Can on 1/22/16.
//

#include <hsisomap/graph/knngraph/KNNGraph_FixedK_MST.h>
#include <hsisomap/graph/AdjacencyList.h>
#include <hsisomap/graph/BoostAdjacencyList.h>
#include <hsisomap/Logger.h>
#include <hsisomap/util/VpTree.h>
#include <hsisomap/util/UnionFind.h>

HSISOMAP_NAMESPACE_BEGIN

struct UndirectedEdge {
  Index index_a;
  Index index_b;
  Scalar weight;
  UndirectedEdge(Index index_a, Index index_b, Scalar weight) : index_a(index_a), index_b(index_b), weight(weight) { }
};


KNNGraph_FixedK_MST::KNNGraph_FixedK_MST(std::shared_ptr<gsl::Matrix> data, PropertyList property_list)
    : data_(data), property_list_(property_list) {

  if (property_list_[KNNGRAPH_FIXED_K] == 0.0) property_list_[KNNGRAPH_FIXED_K] = 30.0;
  if (property_list_[KNNGRAPH_FIXED_K_WITH_MST_EDGE_POOL_DEPTH] == 0.0) property_list_[KNNGRAPH_FIXED_K_WITH_MST_EDGE_POOL_DEPTH] = property_list_[KNNGRAPH_FIXED_K] + 100;

  if (property_list_[KNNGRAPH_GRAPH_BACKEND] == KNNGRAPH_GRAPH_BACKEND_ADJACENCYLIST) {
    knngraph_ = std::make_shared<GraphUtils::AdjacencyList>(data_->cols());
  } else if (property_list_[KNNGRAPH_GRAPH_BACKEND] == KNNGRAPH_GRAPH_BACKEND_BOOST) {
    knngraph_ = std::make_shared<GraphUtils::BoostAdjacencyList>(data_->cols());
  } else {
    throw std::invalid_argument("Invalid KNNGRAPH_GRAPH_BACKEND value.");
  }
  kIndex FIXED_K = static_cast<Index>(property_list_[KNNGRAPH_FIXED_K]);
  kIndex MST_EDGE_POOL_DEPTH = static_cast<Index>(property_list_[KNNGRAPH_FIXED_K_WITH_MST_EDGE_POOL_DEPTH]);

  LOGI("Constructing kNN graph with FixedK_MST.")

  VpTree<PixelView, SquaredDistance> vptree;
  LOGI("Create PixelView array.");
  auto pixel_views = CreatePixelViewsFromMatrix(*data_);
  LOGI("Creating VpTree for the current data matrix.")
  vptree.create(pixel_views);
  LOGI("VpTree created.")

  UnionFind uf(data_->cols());
  std::vector<UndirectedEdge> unused_edges;

  for (Index n = 0; n < data_->cols(); ++n) {
    PixelView current_pixel = pixel_views[n];
    std::vector<PixelView> results;
    std::vector<Scalar> distance_squares;
    vptree.search(current_pixel, MST_EDGE_POOL_DEPTH, &results, &distance_squares);

    for (Index j = 0; j < MST_EDGE_POOL_DEPTH; ++j) {
      if (j < FIXED_K) {
        knngraph_->Connect(current_pixel.index, results[j].index, std::sqrt(distance_squares[j]));
        uf.Connect(current_pixel.index, results[j].index);
      } else {
        unused_edges.push_back(UndirectedEdge(current_pixel.index, results[j].index, std::sqrt(distance_squares[j])));
      }
    }

    LOGI("kNN graph without MST has " << uf.count() << " connected parts.")

    if (uf.count() != 1) {
      LOGI("Start augmenting MST.")

      std::sort(std::begin(unused_edges), std::end(unused_edges), [] (UndirectedEdge a, UndirectedEdge b) {
        return a.weight < b.weight;
      });

      Index augment_count = 0;
      for (auto edge : unused_edges) {
        Index old_count = uf.count();
        uf.Connect(edge.index_a, edge.index_b);
        if (uf.count() < old_count) {
          knngraph_->Connect(edge.index_a, edge.index_b, edge.weight);
          augment_count++;
        }
        if (uf.count() == 1) break;
      }

      LOGI(augment_count << " edges augmented.")

      if (uf.count() != 1) {
        throw std::invalid_argument("MST Augmentation failed -- need to increase KNNGRAPH_FIXED_K_WITH_MST_EDGE_POOL_DEPTH");
      }

      LOGI("kNN graph construction finished.")

    }
  }

}

HSISOMAP_NAMESPACE_END