//
// Created by Can on 4/28/16.
//

#include <hsisomap/subsetter/SubsetterRandomSkel.h>
#include <hsisomap/Logger.h>
#include <numeric>
#include <random>
#include <hsisomap/util/VpTree.h>
#include <hsisomap/hsisomap>


HSISOMAP_NAMESPACE_BEGIN

SubsetterRandomSkel::SubsetterRandomSkel(std::shared_ptr<gsl::Matrix> data, PropertyList property_list)
    : data_(data), property_list_(property_list) {


  std::vector<Index> initial_indices(data_->rows(), 0);
  std::iota(initial_indices.begin(), initial_indices.end(), 0);

  std::vector<std::vector<Index>> subsets;

  Index num_subsets = static_cast<Index>(property_list_[SUBSETTER_SUBSETS]);
  Index k_limit = static_cast<Index>(static_cast<Scalar >(initial_indices.size()) / static_cast<Scalar>(num_subsets));

  while (initial_indices.size() > 0) {


    auto subset_of_subset_data = gsl::GetRows(data_, initial_indices);
    VpTree<PixelView, SquaredDistance> vptree;
    auto pixel_views = CreatePixelViewsFromMatrix(*subset_of_subset_data);
    vptree.create(pixel_views);


    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<Index> rand_dist(0, initial_indices.size() - 1);
    Index random_center_in_initial_indices = rand_dist(g);

          std::vector<PixelView> results;
          std::vector<Scalar> distance_squares;
          vptree.search(pixel_views[random_center_in_initial_indices], k_limit, &results, &distance_squares);

    std::vector<Index> remove_list;
    remove_list.push_back(random_center_in_initial_indices);

    for (Index ki = 1; ki < results.size(); ++ki) {
      remove_list.push_back(results[ki].index);
          }

    std::vector<Index> selected_original_indexes;
    Index cumulator = 0;
    for (Index i : remove_list) {
//      LOGI("REMOVE " << initial_indices[i]);
      selected_original_indexes.push_back(initial_indices[i]);
      Index initial_indices_size = initial_indices.size();
      initial_indices.erase(initial_indices.begin() + (i - cumulator));
      ++cumulator;
    }

    LOGI("New Subset");

    subsets.push_back(selected_original_indexes);

  }

  subsets_ = subsets;

}

HSISOMAP_NAMESPACE_END

