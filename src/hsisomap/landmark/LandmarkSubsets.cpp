//
// Created by Can on 7/22/16.
//

#include <hsisomap/landmark/LandmarkSubsets.h>
#include <hsisomap/subsetter/Subsetter.h>
#include <hsisomap/gsl_util/embedding.h>
#include <gsl/gsl_blas.h>
#include <numeric>
#include <hsisomap/Logger.h>
#include <unordered_set>
#include <gsl/gsl_sort_vector.h>
#include <random>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector_double.h>

HSISOMAP_NAMESPACE_BEGIN

LandmarkSubsets::LandmarkSubsets(std::shared_ptr<gsl::Matrix> data, PropertyList property_list)
: data_(data), property_list_(property_list) {


// TODO: Set default property_list

  // Note that the implementation mainly considers 5 major local intrinsic dimensionality, so that the total dimension
  // of the data must be larger than 5. Otherwise it would be considered as a toy-example for test and visualization
  // purposes, which should have a dimension larger than 2, and the intrinsic dimensionality is considered as 2.

  kScalar SUBSET_COUNT_DIVIDER = (data_->cols() > 5) ? 10.0 : 4.0;
  kIndex COVERED_MAJOR_DIMENSIONS = (data_->cols() > 5) ? 5 : 2;

  // Note that if the landmark count cannot be divided by 10, additional (<10) landmarks need to be added at the end.
  kIndex subset_count = static_cast<Index>(property_list_[LANDMARK_COUNT] / SUBSET_COUNT_DIVIDER);
  std::shared_ptr<Subsetter> subsetter = SubsetterWithImplementation(SUBSETTER_IMPLEMENTATION_EMBEDDING,
                                                                     data_,
                                                                     {{SUBSETTER_DEFAULT_EMBEDDING,
                                                                       SUBSETTER_DEFAULT_EMBEDDING_PCA},
                                                                      {SUBSETTER_EMBEDDING_SLICING_MODE,
                                                                       SUBSETTER_EMBEDDING_SLICING_MODE_FIRST_MEAN},
                                                                      {SUBSETTER_SUBSETS, subset_count}});

  subset_indexes_ = subsetter->subsets();


  for (Index subset_number = 0; subset_number < subset_indexes_.size(); ++subset_number) {

    auto indexes_in_subset = subset_indexes_[subset_number];
//    LOGDEBUG("Subset " << subset_number);
    LOGI("Subset " << subset_number << " / " << subset_indexes_.size());

    auto subset_data = gsl::GetRows(data_, indexes_in_subset);
    auto subset_embedding = gsl::MNFWithNearestNeighborNoiseEstimation(*subset_data);


    // Preselection by noise exclusion
    // TODO: Try intrinsic dimensionality estimation

    std::vector<Index> indexes(indexes_in_subset.size(), 0); // indexes for preselection
    std::iota(std::begin(indexes), std::end(indexes), 0);

    if (property_list_[LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_PERCENTAGE] > 0) {

      std::vector<Scalar> noise_dnrm2(indexes_in_subset.size(), 0);

      kIndex noise_dimensions = static_cast<Index>(property_list_[LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_NOISE_DIMENSIONS]);

      for (Index r = 0; r < indexes_in_subset.size(); ++r) {
        gsl_vector_const_view row = gsl_matrix_const_subrow(subset_embedding.space->m_, r, data_->cols() - noise_dimensions, noise_dimensions);
        noise_dnrm2[r] = gsl_blas_dnrm2(&row.vector);
      }

      std::sort(std::begin(indexes), std::end(indexes), [&noise_dnrm2] (int a, int b) {
        return noise_dnrm2[a] < noise_dnrm2[b];
      });

      Index excluded_samples = static_cast<Index>(property_list_[LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_PERCENTAGE] * subset_data->rows());
      indexes.resize(indexes.size() - excluded_samples);

    }

    // Geometrical Coverage
    // 5-major-dimension covered by 10 landmarks implementation
    // Note that the calculation is done in subset embedding space
    auto preselected_data_embedding = gsl::GetRows(subset_embedding.space, indexes);
    std::unordered_set<Index> subset_landmarks;


    for (Index c = 0; c < COVERED_MAJOR_DIMENSIONS; ++c) {
      gsl_vector_const_view col = gsl_matrix_const_column(preselected_data_embedding->m_, c);
      // Note: vector view should be accessed by vector_get, not by 'data' pointers.

//      for (Index ii = 0; ii < preselected_data_embedding->m_->size1; ++ii) {
//        for (Index jj = 0; jj < preselected_data_embedding->m_->size2; ++jj) {
//          std::cout << gsl_matrix_get(preselected_data_embedding->m_, ii, jj) << " ";
//        }
//        std::cout << std::endl;
//      }
//      std::cout << std::endl;
//
//      for (Index ii = 0; ii < col.vector.size; ++ii) {
//        std::cout << gsl_vector_get(&col.vector, ii) << "+";
//      }

      // Try simple find (only max) first
      Index max_row = gsl_vector_max_index(&col.vector);
      Index min_row = gsl_vector_min_index(&col.vector);
      Index selected_global_index_max = indexes_in_subset[indexes[max_row]];
      Index selected_global_index_min = indexes_in_subset[indexes[min_row]];

      if (selected_global_index_max == selected_global_index_min) {
        throw std::invalid_argument("The subset has not enough major variation. Try adjusting parameters.");
      }

      // If either of the two values have overlapping, start full sorting procedure
      if (subset_landmarks.find(selected_global_index_max) != subset_landmarks.end() ||
          subset_landmarks.find(selected_global_index_min) != subset_landmarks.end()) {
        // Overlapping, start full sorting procedure

        LOGDEBUG("Overlapping landmarks, start full sorting procedure.");

        gsl_permutation *p = gsl_permutation_alloc(col.vector.size);
        gsl_sort_vector_index(p, &col.vector);

        // ascending order sorting, so find (non-overlapped) min value first
        Index pi = 0;
        do {
          if (pi == p->size) {
            throw std::invalid_argument("The subset has not enough major variation. Try adjusting parameters.");
          }
          Index min_row_new = p->data[pi];
          selected_global_index_min = indexes_in_subset[indexes[min_row_new]];
          ++pi;
        } while (subset_landmarks.find(selected_global_index_min) != subset_landmarks.end());

        subset_landmarks.insert(selected_global_index_min);

        // find non-overlapped max value
        pi = p->size - 1;
        do {
          if (pi == -1) {
            throw std::invalid_argument("The subset has not enough major variation. Try adjusting parameters.");
          }
          Index max_row_new = p->data[pi];
          selected_global_index_max = indexes_in_subset[indexes[max_row_new]];
          --pi;
        } while (subset_landmarks.find(selected_global_index_max) != subset_landmarks.end());

        subset_landmarks.insert(selected_global_index_max);


      } else {
        subset_landmarks.insert(selected_global_index_max);
        subset_landmarks.insert(selected_global_index_min);
      }


    }

//    std::copy(subset_landmarks.begin(), subset_landmarks.end(), std::ostream_iterator<int>(std::cout, "\n"));

    // Add to global landmark set
    for (const auto s : subset_landmarks) {
      landmarks_.push_back(s);
    }


  }

  LOGI(landmarks_.size() << " landmarks selected from subsets.");
  if (landmarks_.size() < property_list_[LANDMARK_COUNT]) {
    LOGI("Need " << (property_list_[LANDMARK_COUNT] - landmarks_.size()) << " more random landmarks.");


//  std::vector<Index> left_indexes(data_->rows(), 0);
//  std::iota(left_indexes.begin(), left_indexes.end(), 0);

    std::unordered_set<Index> left_indexes;
    for (Index i = 0; i < data_->rows(); ++i) {
      left_indexes.insert(i);
    }

    for (const auto li : landmarks_) {
      left_indexes.erase(li);
    }

    std::vector<Index> left_indexes_vector(left_indexes.begin(), left_indexes.end());

    // TODO: Add controlled random seed
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(left_indexes_vector.begin(), left_indexes_vector.end(), g);

    for (Index i = 0; i < property_list_[LANDMARK_COUNT] - landmarks_.size(); ++i) {
      landmarks_.push_back(left_indexes_vector[i]);
    }
    LOGI("Added random landmarks to meet the requirement.");

  }

//  LOGDEBUG("Optionally sort the landmark list for output purposes.");
//  std::sort(landmarks_.begin(), landmarks_.end());


}



HSISOMAP_NAMESPACE_END