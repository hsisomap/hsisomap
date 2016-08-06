//
// Created by Can on 4/4/16.
//


#include <hsisomap/graph/knngraph/KNNGraph_AdaptiveK_HIDENN.h>
#include <hsisomap/graph/AdjacencyList.h>
#include <hsisomap/graph/BoostAdjacencyList.h>
#include <hsisomap/Logger.h>
#include <hsisomap/util/VpTree.h>
#include <hsisomap/util/UnionFind.h>

#include <hsisomap/subsetter/Subsetter.h>
#include <hsisomap/gsl_util/matrix_util.h>
#include <random>

#include <gsl/gsl_fit.h>
#include <gsl/gsl_matrix.h>

HSISOMAP_NAMESPACE_BEGIN

struct UndirectedEdge {
  Index index_b;
  Index index_a;
  Scalar weight;
  UndirectedEdge(Index index_a, Index index_b, Scalar weight) : index_a(index_a), index_b(index_b), weight(weight) { }
};


KNNGraph_AdaptiveK_HIDENN::KNNGraph_AdaptiveK_HIDENN(std::shared_ptr<gsl::Matrix> data, PropertyList property_list)
    : data_(data), property_list_(property_list) {

  LOGI("Subsetting.")
//  std::shared_ptr<Subsetter> subsetter = SubsetterWithImplementation(SUBSETTER_IMPLEMENTATION_RANDOMSKEL,
//                                                                     data_,
//                                                                     {
//                                                                      {SUBSETTER_SUBSETS, property_list_[KNNGRAPH_ADAPTIVE_K_HIDENN_SUBSET_NUMBER]}});

  std::shared_ptr<Subsetter> subsetter = SubsetterWithImplementation(SUBSETTER_IMPLEMENTATION_EMBEDDING,
                                                                     data_,
                                                                     {{SUBSETTER_DEFAULT_EMBEDDING,
                                                                       SUBSETTER_DEFAULT_EMBEDDING_PCA},
                                                                      {SUBSETTER_EMBEDDING_SLICING_MODE,
                                                                       SUBSETTER_EMBEDDING_SLICING_MODE_FIRST_MEAN},
                                                                      {SUBSETTER_SUBSETS, property_list_[KNNGRAPH_ADAPTIVE_K_HIDENN_SUBSET_NUMBER]}});


  auto subset_indexes = subsetter->subsets();

  Index K1, K2;
  if (subset_indexes.size() > 199) {
    K1 = 2;
    K2 = 1;
    LOGI("K1 = 2, K2 = 1");
  } else if (subset_indexes.size() > 74) {
    K1 = 3;
    K2 = 1;
    LOGI("K1 = 3, K2 = 1");
  } else {
    K1 = 4;
    K2 = 2;
    LOGI("K1 = 4, K2 = 2");
  }


  gsl::Matrix mean_intrinsic_dimensionality_log(subset_indexes.size(), 1);

  std::vector<Index> optim_ks(subset_indexes.size(), 0);

  Index s = 0;
  for (auto indexes_in_subset : subset_indexes) {
    auto subset_data = gsl::GetRows(data_, indexes_in_subset);
    VpTree<PixelView, SquaredDistance> vptree;
    auto pixel_views = CreatePixelViewsFromMatrix(*subset_data);
    vptree.create(pixel_views);

    Scalar mean_intrinsic_dimensionality = 0;
    Scalar log_k_kp = log(static_cast<Scalar>(K1) / static_cast<Scalar>(K2));
    for (Index i = 0; i < indexes_in_subset.size(); ++i) {
      std::vector<PixelView> results;
      std::vector<Scalar> distance_squares;
      vptree.search(pixel_views[i], K1 + 1, &results, &distance_squares);
      mean_intrinsic_dimensionality += 2 * log_k_kp / log(distance_squares[K1] / distance_squares[K2]);
    }
    mean_intrinsic_dimensionality /= indexes_in_subset.size();

    std::vector<Scalar> slope_of_k(23, 0);
    for (Index k = 3; k <= 22; ++k) {
//      LOGI("K = " << k);
//      LOGI(indexes_in_subset.size());
      std::vector<Index> randomed_indexes_in_subset(indexes_in_subset);

      gsl::Matrix log_N_log_L(100 - 10 + 1, 2);
      Index Ni = 0;
      for (Index N = 10; N <= 100; ++N) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(randomed_indexes_in_subset.begin(), randomed_indexes_in_subset.end(), g);

        std::vector<Index> indexes_in_subset_of_subset(N, 0);
        std::copy_n(randomed_indexes_in_subset.begin(), N, indexes_in_subset_of_subset.begin());

        auto subset_of_subset_data = gsl::GetRows(data_, indexes_in_subset_of_subset);
        VpTree<PixelView, SquaredDistance> vptree;
        auto pixel_views = CreatePixelViewsFromMatrix(*subset_of_subset_data);
        vptree.create(pixel_views);

        gsl::Matrix knngraph_subset_of_subset(N, N);
        for (Index n = 0; n < N; ++n) {
          std::vector<PixelView> results;
          std::vector<Scalar> distance_squares;
          vptree.search(pixel_views[n], k + 1, &results, &distance_squares);
          for (Index ki = 1; ki < results.size(); ++ki) {
            knngraph_subset_of_subset(n, results[ki].index) = sqrt(distance_squares[ki]);
            knngraph_subset_of_subset(results[ki].index, n) = sqrt(distance_squares[ki]);
          }


        }

        Scalar L = 0;
        for (Index r = 0; r < knngraph_subset_of_subset.rows(); ++r) {
          for (Index c = r + 1; c < knngraph_subset_of_subset.cols(); ++c) {
            L += knngraph_subset_of_subset(r, c);
          }
        }

        log_N_log_L(Ni, 0) = log(static_cast<Scalar>(N));
        log_N_log_L(Ni, 1) = log(L * 2);
        ++Ni;
      }

      double c0, c1, cov00, cov01, cov11, sumsq;
      gsl_fit_linear(&(log_N_log_L.m_->data[0]), 2, &(log_N_log_L.m_->data[1]), 2, log_N_log_L.rows(), &c0, &c1, &cov00, &cov01, &cov11, &sumsq);
      slope_of_k[k] = c1;
//      LOGI("c0 " << c0 << " c1 " << c1);
    }



    LOGI(mean_intrinsic_dimensionality);
    mean_intrinsic_dimensionality_log(s, 0) = mean_intrinsic_dimensionality;

    Scalar min_diff = 10000;
    Index optim_k = 0;
    Scalar optim_slope = 1.0 - 1.0 / mean_intrinsic_dimensionality;
    for (Index k = 3; k <= 22; ++k) {
      if (std::abs(optim_slope - slope_of_k[k]) < min_diff) {
        min_diff = std::abs(optim_slope - slope_of_k[k]);
        optim_k = k;
      }
    }

    LOGI("Optim k: " << optim_k);
    optim_ks[s] = optim_k;

    ++s;
  }

//  {
//    std::ofstream ofs("/Users/can/Results/PaviaU/optimcomp/cpptests/adaptivek/mean_id.txt");
//    ofs << mean_intrinsic_dimensionality_log;
//  }

  {
    std::ofstream ofs("/Users/can/Results/PaviaU/optimcomp/cpptests/adaptivek/optim_ks.txt");
    gsl::Matrix optim_ks_mat(std::vector<std::vector<Index>>(1, optim_ks));
    ofs << optim_ks_mat;
  }

  if (property_list_[KNNGRAPH_GRAPH_BACKEND] == KNNGRAPH_GRAPH_BACKEND_ADJACENCYLIST) {
    knngraph_ = std::make_shared<GraphUtils::AdjacencyList>(data_->rows());
  } else if (property_list_[KNNGRAPH_GRAPH_BACKEND] == KNNGRAPH_GRAPH_BACKEND_BOOST) {
    knngraph_ = std::make_shared<GraphUtils::BoostAdjacencyList>(data_->rows());
  } else {
    throw std::invalid_argument("Invalid KNNGRAPH_GRAPH_BACKEND value.");
  }

  kIndex MST_EDGE_POOL_DEPTH = 120;


  LOGI("Constructing kNN graph with FixedK_MST.")

  VpTree<PixelView, SquaredDistance> vptree;
  LOGI("Create PixelView array.");
  auto pixel_views = CreatePixelViewsFromMatrix(*data_);
  LOGI("Creating VpTree for the current data matrix.")
  vptree.create(pixel_views);
  LOGI("VpTree created. Now creating kNN graph.")

  UnionFind uf(data_->rows());
  std::vector<UndirectedEdge> unused_edges;


  s = 0;
  for (auto indexes_in_subset : subset_indexes) {

    Index adaptive_k = optim_ks[s];

    for (Index ni = 0; ni < indexes_in_subset.size(); ++ni) {
      Index n = indexes_in_subset[ni];

      PixelView current_pixel = pixel_views[n];
      std::vector<PixelView> results;
      std::vector<Scalar> distance_squares;
      vptree.search(current_pixel, MST_EDGE_POOL_DEPTH, &results, &distance_squares);

      for (Index j = 0; j < MST_EDGE_POOL_DEPTH; ++j) {
        if (j < adaptive_k) {
          knngraph_->Connect(current_pixel.index, results[j].index, std::sqrt(distance_squares[j]));
          uf.Connect(current_pixel.index, results[j].index);
        } else {
          unused_edges.push_back(UndirectedEdge(current_pixel.index, results[j].index, std::sqrt(distance_squares[j])));
        }
      }
    }

    ++s;
  }
  LOGI("kNN graph without MST has " << uf.count() << " connected parts.")

  if (uf.count() != 1) {
    LOGI("Start augmenting MST.")

    std::sort(std::begin(unused_edges), std::end(unused_edges), [](UndirectedEdge a, UndirectedEdge b) {
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
      throw std::invalid_argument(
          "MST Augmentation failed -- need to increase KNNGRAPH_FIXED_K_WITH_MST_EDGE_POOL_DEPTH");
    }

  }

  LOGI("kNN graph construction finished.")

}

HSISOMAP_NAMESPACE_END
