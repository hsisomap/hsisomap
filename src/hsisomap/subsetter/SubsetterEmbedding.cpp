//
// Created by Can on 1/18/16.
//

#include <hsisomap/subsetter/SubsetterEmbedding.h>
#include <hsisomap/Logger.h>
#include <hsisomap/gsl_util/embedding.h>
#include <hsisomap/gsl_util/matrix_util.h>
#include <numeric>

HSISOMAP_NAMESPACE_BEGIN

SubsetterEmbedding::SubsetterEmbedding(std::shared_ptr<HsiData> hsi_data, PropertyList property_list, std::shared_ptr<gsl::Matrix> optional_embedding) : hsi_data_(hsi_data), property_list_(property_list), embedding_(optional_embedding) {

  std::function <gsl::Embedding(const gsl::Matrix&)> EmbeddingFunction;
  if (property_list[SUBSETTER_DEFAULT_EMBEDDING] == SUBSETTER_DEFAULT_EMBEDDING_PCA) {
    EmbeddingFunction = std::bind(gsl::PCA, std::placeholders::_1, 1);
  } else {
    throw std::invalid_argument("Embedding function not supported.");
  }

  if (embedding_.get() == nullptr) {
    embedding_ = EmbeddingFunction(*(hsi_data_->data())).space;
  }

  Index num_subsets = static_cast<Index>(property_list_[SUBSETTER_SUBSETS]);
  Index recursion_depth = static_cast<Index>(ceil(log2(num_subsets)));

  // Define different division criteria based on embeddings as functions.
  std::function<std::pair<std::vector<Index>, std::vector<Index>>(std::vector<Index>, gsl::Matrix)> Divider;
  // Note that the ``score'' is destructible in the current algorithm, which is easier to implement.
  if (property_list_[SUBSETTER_EMBEDDING_SLICING_MODE] == SUBSETTER_EMBEDDING_SLICING_MODE_FIRST_MEAN) {
    Divider = [] (std::vector<Index> group, gsl::Matrix score) {
      // score is a column vector, i.e., a matrix with width of 1.
      std::pair<std::vector<Index>, std::vector<Index>> result;
      for (Index i = 0; i < score.rows(); ++i) {
        if (score(i, 0) >= 0.0) {
          result.first.push_back(group[i]);
        } else {
          result.second.push_back(group[i]);
        }
      }
      return result;
    };
  } else if (property_list_[SUBSETTER_EMBEDDING_SLICING_MODE] == SUBSETTER_EMBEDDING_SLICING_MODE_FIRST_MEDIAN) {
    Divider = [] (std::vector<Index> group, gsl::Matrix score) {
      std::pair<std::vector<Index>, std::vector<Index>> result;
      gsl::Matrix group_matrix(std::vector<std::vector<Index>>(1, group));
      gsl::SortMatrixRows(score, group_matrix);
      Index i;
      for (i = 0; i < group_matrix.cols() / 2; ++i) {
        result.first.push_back(group[i]);
      }
      for (; i < group_matrix.cols(); ++i) {
        result.second.push_back(group[i]);
      }
      return result;
    };
  } else {
    throw std::invalid_argument("Invalid SUBSETTER_EMBEDDING_SLICING_MODE.");
  }

  std::vector<std::vector<Index>> current_groups;
  std::vector<std::vector<Index>> next_groups;
  std::vector<Index> initial_indices(hsi_data_->data()->rows(), 0);
  std::iota(initial_indices.begin(), initial_indices.end(), 0);
  current_groups.push_back(initial_indices);

  Index group_counter = 1; // initially there is one subset before starting division.
  for (Index level = 0; level < recursion_depth; ++level) {
    for (Index group = 0; group < current_groups.size(); ++group) {
      auto current_indices = current_groups[group];
      auto current_image_data = hsi_data_->data()->GetRows(current_indices);

      // Calculate embeddings using the designated embedding function, except
      // for the initial embedding, which might be specified from outside.
      std::shared_ptr<gsl::Matrix> embedding(nullptr);
      if (level == 0) {
        embedding = embedding_;
      } else {
        embedding = (EmbeddingFunction(current_image_data)).space;
      }

      // Divide the current group into two groups using the Divider function.
      auto divided = Divider(current_indices, *embedding);

      next_groups.push_back(divided.first);
      next_groups.push_back(divided.second);
      group_counter++;

      // Terminate dividing before this level's recursion ends if the required number is reached.
      if (group_counter == num_subsets) {
        // Combine the groups divided at this recursion level and before this recursion level.
        next_groups.insert(next_groups.end(), current_groups.begin() + group + 1, current_groups.end());
        subsets_ = next_groups;
        break;
      }
    }
    if (group_counter == num_subsets) break;

    // Reorder the remaining groups such that the largest group gets divided first.
    std::sort(next_groups.begin(), next_groups.end(), [] (const std::vector<Index> a, const std::vector<Index> b) {
      return a.size() > b.size();
    });
    current_groups = next_groups;
    next_groups.clear();
  }

}


HSISOMAP_NAMESPACE_END

