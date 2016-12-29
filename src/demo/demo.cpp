#include <hsisomap/hsisomap>
#include <numeric>
#include <random>

using namespace std;
using namespace hsisomap;
using namespace gsl;
using namespace Dijkstra;

void example1() {

  LOGTIMESTAMP("Landmark List Demo Begin.")

  LOGI("Load image.")

  HsiData hsi_data("/Users/can/Results/Botswana/images/Botswana");

  LOGI(hsi_data.lines());
  LOGI(hsi_data.samples());
  LOGI(hsi_data.bands());

  Index pixel_count = hsi_data.lines() * hsi_data.samples();

  // Run once to generate backbones
  // A random list of samples
  std::vector<Index> backbone_indices(pixel_count);
  std::iota(backbone_indices.begin(), backbone_indices.end(), 1);

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(backbone_indices.begin(), backbone_indices.end(), g);

  backbone_indices.resize(100000);
  std::sort(backbone_indices.begin(), backbone_indices.end());
  {
    ofstream ofs("backbone_sampled_index.txt");
    std::copy(backbone_indices.begin(), backbone_indices.end(), std::ostream_iterator<int>(ofs, "\n"));
  }

//  vector<Index> backbone_indices =
//      Read1DVectorFromTextFile<Index>("backbone_sampled_index.txt");

  LOGI("Backbone sampling.")

  Backbone backbone(hsi_data.data(), backbone_indices);
  auto bb_data = backbone.sampled_data();

  // Save to backbone image for other use
  HsiData bb_image(bb_data, 1, bb_data->rows(), bb_data->cols());
  bb_image.WriteImageFile("Botswana_backbone");

// Run once to save nn-cache
  backbone.PrepareNNCache(10,
                          PropertyList({{BACKBONE_NNCACHE_PRIMARY_SEARCH_RANGE, 100},
                                        {BACKBONE_NNCACHE_SECONDARY_SEARCH_RANGE, 200}}));

  {
    std::ofstream ofs("backbone_nncache.txt");
    ofs << *backbone.nn_cache();
  }


  LOGI("Load Landmarks from file.")
  auto landmark = LandmarkWithImplementation(LANDMARK_IMPLEMENTATION_LIST,
                                             bb_data,
                                             PropertyList(),
                                             std::make_shared<std::vector<Index>>(
                                                 Read1DVectorFromTextFile<Index>(
                                                     "/Users/can/Results/Botswana/images/selected_landmarks_subsetmaxvar.txt")));

  auto landmark_data = landmark->landmark_data();
  LOGI("Create kNN Graph.")
  auto knngraph = KNNGraphWithImplementation(KNNGRAPH_IMPLEMENTATION_ADAPTIVE_K_HIDENN,
                                             bb_data,
                                             PropertyList({{KNNGRAPH_ADAPTIVE_K_HIDENN_SUBSET_NUMBER, 100}}));

  auto graph = knngraph->knngraph();

  LOGI("Perform DijkstraCL from " << landmark->landmarks().size() << " landmarks to all the " << bb_data->rows()
           << " pixels.")
  auto dijkstra = DijkstraWithImplementation(DIJKSTRA_IMPLEMENTATION_CL, graph);
  dijkstra->SetSourceVertices(landmark->landmarks());

  dijkstra->Run();
  auto distance_matrix_landmark_to_all = dijkstra->GetDistanceMatrix();
  auto distance_matrix_landmarks = GetCols(distance_matrix_landmark_to_all, landmark->landmarks());
  LOGI("DijkstraCL finished.")


//  {
////    ofstream ofs("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/debug_distances.txt");
////    ofs << *distance_matrix_landmark_to_all;
////  }

  auto landmark_cmds_embedding = CMDS(*distance_matrix_landmarks, bb_data->cols(), EMBEDDING_CMDS_SOLVE_EIGEN_ONLY);
  auto manifold = ConstructManifold(*distance_matrix_landmark_to_all,
                                    *distance_matrix_landmarks,
                                    landmark_cmds_embedding,
                                    bb_data->cols());

  auto nncache_loaded = std::make_shared<gsl::Matrix>(hsi_data.data()->rows() - backbone.sampling_indices().size(), 11);
  std::ifstream ifs("backbone_nncache.txt");
  ifs >> *nncache_loaded;

  auto reconstructed = backbone.Reconstruct(*manifold,
                                            PropertyList({{BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_STRATEGY,
                                                           BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED},
                                                          {BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED_NUMBER, 5}}),
                                            nncache_loaded);
  HsiData reconstructed_image(reconstructed, hsi_data.lines(), hsi_data.samples(), reconstructed->cols());
  reconstructed_image.WriteImageFile("/Users/can/Results/Botswana/images/manifold_recon5");


  LOGTIMESTAMP("Landmark List Demo Finished.")
}

void example2() {

  LOGTIMESTAMP("Landmark List Demo Begin.")

  LOGI("Load image.")

  HsiData hsi_data("/Users/can/Results/PaviaU/optimcomp/pcasubsetting_for_mnf_denoising/PaviaU_spectral_full_pcasubsetmnf350_fixed80");

  LOGI(hsi_data.lines());
  LOGI(hsi_data.samples());
  LOGI(hsi_data.bands());

  Index pixel_count = hsi_data.lines() * hsi_data.samples();

  // Run once to generate backbones
  // A random list of samples
//  std::vector<Index> backbone_indices(pixel_count);
//  std::iota(backbone_indices.begin(), backbone_indices.end(), 1);

//  std::random_device rd;
//  std::mt19937 g(rd());

//  std::shuffle(backbone_indices.begin(), backbone_indices.end(), g);

//  backbone_indices.resize(100000);
//  std::sort(backbone_indices.begin(), backbone_indices.end());
//  {
//    ofstream ofs("backbone_sampled_index.txt");
//    std::copy(backbone_indices.begin(), backbone_indices.end(), std::ostream_iterator<int>(ofs, "\n"));
//  }

  vector<Index> backbone_indices =
      Read1DVectorFromTextFile<Index>("/Users/can/Results/PaviaU/optimcomp/pcasubsetting_for_mnf_denoising/backbone_sampled_index_cpp.txt");

  LOGI("Backbone sampling.")

  Backbone backbone(hsi_data.data(), backbone_indices);
  auto bb_data = backbone.sampled_data();

  // Save to backbone image for other use
//  HsiData bb_image(bb_data, 1, bb_data->rows(), bb_data->cols());
//  bb_image.WriteImageFile("Botswana_backbone");

// Run once to save nn-cache
//  backbone.PrepareNNCache(10,
//                          PropertyList({{BACKBONE_NNCACHE_PRIMARY_SEARCH_RANGE, 300},
//                                        {BACKBONE_NNCACHE_SECONDARY_SEARCH_RANGE, 500}}));
//
//  {
//    std::ofstream ofs("/Users/can/Results/PaviaU/optimcomp/pcasubsetting_for_mnf_denoising/backbone_sampled_index_cpp_backbone_nncache.txt");
//    ofs << *backbone.nn_cache();
//  }


  LOGI("Load Landmarks from file.")
  auto landmark = LandmarkWithImplementation(LANDMARK_IMPLEMENTATION_LIST,
                                             bb_data,
                                             PropertyList(),
                                             std::make_shared<std::vector<Index>>(
                                                 Read1DVectorFromTextFile<Index>(
                                                     "/Users/can/Results/PaviaU/optimcomp/pcasubsetting_for_mnf_denoising_subsetisomap/presel_80pct/selected_landmarks_cpp.txt")));

//  auto landmark_data = landmark->landmark_data();
//  LOGI("Create kNN Graph.")
//  auto knngraph = KNNGraphWithImplementation(KNNGRAPH_IMPLEMENTATION_ADAPTIVE_K_HIDENN,
//                                             bb_data,
//                                             PropertyList({{KNNGRAPH_ADAPTIVE_K_HIDENN_SUBSET_NUMBER, 100}}));

  auto landmark_data = landmark->landmark_data();
  LOGI("Create kNN Graph.")
  auto knngraph = KNNGraphWithImplementation(KNNGRAPH_IMPLEMENTATION_FIXED_K_WITH_MST,
                                             bb_data,
                                             PropertyList({{KNNGRAPH_FIXED_K_NUMBER, 30}}));
  auto graph = knngraph->knngraph();

  LOGI("Perform DijkstraCL from " << landmark->landmarks().size() << " landmarks to all the " << bb_data->rows()
           << " pixels.")
  auto dijkstra = DijkstraWithImplementation(DIJKSTRA_IMPLEMENTATION_CL, graph);
  dijkstra->SetSourceVertices(landmark->landmarks());

  dijkstra->Run();
  auto distance_matrix_landmark_to_all = dijkstra->GetDistanceMatrix();
  auto distance_matrix_landmarks = GetCols(distance_matrix_landmark_to_all, landmark->landmarks());
  LOGI("DijkstraCL finished.")


//  {
////    ofstream ofs("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/debug_distances.txt");
////    ofs << *distance_matrix_landmark_to_all;
////  }

  auto landmark_cmds_embedding = CMDS(*distance_matrix_landmarks, bb_data->cols(), EMBEDDING_CMDS_SOLVE_EIGEN_ONLY);
  auto manifold = ConstructManifold(*distance_matrix_landmark_to_all,
                                    *distance_matrix_landmarks,
                                    landmark_cmds_embedding,
                                    bb_data->cols());

  auto nncache_loaded = std::make_shared<gsl::Matrix>(hsi_data.data()->rows() - backbone.sampling_indices().size(), 11);
  std::ifstream ifs("/Users/can/Results/PaviaU/optimcomp/pcasubsetting_for_mnf_denoising/backbone_sampled_index_cpp_backbone_nncache.txt");
  ifs >> *nncache_loaded;

  auto reconstructed = backbone.Reconstruct(*manifold,
                                            PropertyList({{BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_STRATEGY,
                                                           BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED},
                                                          {BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED_NUMBER, 5}}),
                                            nncache_loaded);
  HsiData reconstructed_image(reconstructed, hsi_data.lines(), hsi_data.samples(), reconstructed->cols());
  reconstructed_image.WriteImageFile("/Users/can/Results/PaviaU/optimcomp/pcasubsetting_for_mnf_denoising_subsetisomap/presel_80pct/manifold_recon5_cpp_fixedk30");


  LOGTIMESTAMP("Landmark List Demo Finished.")
}

void mnf_example() {

  Matrix x({{ 82, 64, 96}, { 91, 10, 97}, { 13, 28, 16}, { 92, 55, 98} });
  auto noise_estimation = NearestNeighborNoiseEstimation(x);
  Embedding mnf_embedding = MNF(x, *noise_estimation);

  cout << *mnf_embedding.space;

}


void mnf_subsetlm_synthetic() {

  HsiData hsi_data("/Users/can/Results/tests/syn1");
  std::shared_ptr<Subsetter> subsetter = SubsetterWithImplementation(SUBSETTER_IMPLEMENTATION_EMBEDDING,
                                                                     hsi_data.data(),
                                                                     {{SUBSETTER_DEFAULT_EMBEDDING,
                                                                       SUBSETTER_DEFAULT_EMBEDDING_PCA},
                                                                      {SUBSETTER_EMBEDDING_SLICING_MODE,
                                                                       SUBSETTER_EMBEDDING_SLICING_MODE_FIRST_MEAN},
                                                                      {SUBSETTER_SUBSETS, 4}});

//  Matrix subset_idx(subsetter->subsets());
//  cout << subset_idx << endl;
  Index max_length = 0;
  for (auto s : subsetter->subsets()) {
    if (s.size() > max_length) max_length = s.size();
  }
  for (auto s : subsetter->subsets()) {
    for (Index i = 0; i < max_length; ++i) {
      if (i >= s.size()) {
        cout << -1 << " ";
      } else {
        cout << s[i] << " ";
      }
    }
    cout << endl;
  }
  cout << endl;


//  auto landmarker = LandmarkWithImplementation(LANDMARK_IMPLEMENTATION_SUBSETS, hsi_data.data(),
//                                               {{LANDMARK_COUNT,
//                                                 16},
//                                                {LANDMARK_SUBSETS_NOISE_MODEL,
//                                                LANDMARK_SUBSETS_NOISE_MODEL_MNF},
//                                                {LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_PERCENTAGE,
//                                                 0.0},
//                                               });

  auto landmarker = LandmarkWithImplementation(LANDMARK_IMPLEMENTATION_SUBSETS, hsi_data.data(),
                                               {{LANDMARK_COUNT,
                                                 16},
                                                {LANDMARK_SUBSETS_NOISE_MODEL,
                                                 LANDMARK_SUBSETS_NOISE_MODEL_MNF},
                                                {LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_PERCENTAGE,
                                                 0.5},
                                                {LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_NOISE_DIMENSIONS,
                                                 1}
                                               });
  auto lms = landmarker->landmarks();

  std::copy(lms.begin(), lms.end(), std::ostream_iterator<int>(cout, "\n"));


}




void paviau_mnf_landmark_tests() {


  LOGTIMESTAMP("Landmark List Demo Begin.")

  LOGI("Load image.")

  HsiData hsi_data("/Volatile/hsi/PaviaU");

  LOGI(hsi_data.lines());
  LOGI(hsi_data.samples());
  LOGI(hsi_data.bands());

  Index pixel_count = hsi_data.lines() * hsi_data.samples();

  // Run once to generate backbones
  // A random list of samples
  std::vector<Index> backbone_indices(pixel_count);
  std::iota(backbone_indices.begin(), backbone_indices.end(), 0); // Backbone indices start from 0

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(backbone_indices.begin(), backbone_indices.end(), g);

  backbone_indices.resize(100000);
  std::sort(backbone_indices.begin(), backbone_indices.end());
  {
    ofstream ofs("/Volatile/playground/PaviaU/backbone_sampled_index.txt");
    std::copy(backbone_indices.begin(), backbone_indices.end(), std::ostream_iterator<int>(ofs, "\n"));
  }

//  vector<Index> backbone_indices =
//      Read1DVectorFromTextFile<Index>("/Volatile/playground/PaviaU/backbone_sampled_index.txt");

  LOGI("Backbone sampling.")

  Backbone backbone(hsi_data.data(), backbone_indices);
  auto bb_data = backbone.sampled_data();

//   Save to backbone image for other use
  HsiData bb_image(bb_data, 1, bb_data->rows(), bb_data->cols());
  bb_image.WriteImageFile("Botswana_backbone");

// Run once to save nn-cache
  backbone.PrepareNNCache(10,
                          PropertyList({{BACKBONE_NNCACHE_PRIMARY_SEARCH_RANGE, 100},
                                        {BACKBONE_NNCACHE_SECONDARY_SEARCH_RANGE, 200}}));

  {
    std::ofstream ofs("/Volatile/playground/PaviaU/backbone_nncache.txt");
    ofs << *backbone.nn_cache();
  }

//  LOGI("Load Landmarks from file.")
//  auto landmark = LandmarkWithImplementation(LANDMARK_IMPLEMENTATION_LIST,
//                                             bb_data,
//                                             PropertyList(),
//                                             std::make_shared<std::vector<Index>>(
//                                                 Read1DVectorFromTextFile<Index>(
//                                                     "/private/var/tmp/Volatile/Archive/Results/PaviaU/optimcomp/pcasubsetting_on_original/selected_landmarks.txt")));

  LOGI("Landmark Subset-MNF MAXVAR");
  auto landmark = LandmarkWithImplementation(LANDMARK_IMPLEMENTATION_SUBSETS, bb_data,
                                             {{LANDMARK_COUNT,
                                               2000},
                                              {LANDMARK_SUBSETS_NOISE_MODEL,
                                               LANDMARK_SUBSETS_NOISE_MODEL_MNF},
                                              {LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_PERCENTAGE,
                                               0.0},
                                              {LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_NOISE_DIMENSIONS,
                                               1}
                                             });
//
//
//  auto landmark_data = landmark->landmark_data();
//
  LOGI("Save landmark list to file.");

  {
    std::ofstream ofs("/private/var/tmp/Volatile/Archive/remote/local_subsetmnf_maxvar_landmarks_ak.txt");
    std::copy(landmark->landmarks().begin(), landmark->landmarks().end(), std::ostream_iterator<Index>(ofs, "\n"));
    std::cout << landmark->landmarks().size() << endl;
  }


  LOGI("Create kNN Graph.")
  auto knngraph = KNNGraphWithImplementation(KNNGRAPH_IMPLEMENTATION_ADAPTIVE_K_HIDENN,
                                             bb_data,
                                             PropertyList({{KNNGRAPH_ADAPTIVE_K_HIDENN_SUBSET_NUMBER, 100}, {KNNGRAPH_GRAPH_BACKEND, KNNGRAPH_GRAPH_BACKEND_ADJACENCYLIST}}));

//  LOGI("Create kNN Graph. -- fixed k")
//  auto knngraph = KNNGraphWithImplementation(KNNGRAPH_IMPLEMENTATION_FIXED_K_WITH_MST,
//                                             bb_data,
//                                             PropertyList({{KNNGRAPH_FIXED_K_NUMBER, 10},
//                                                           {KNNGRAPH_GRAPH_BACKEND, KNNGRAPH_GRAPH_BACKEND_BOOST}}));


  auto graph = knngraph->knngraph();

  LOGI("Perform DijkstraCL from " << landmark->landmarks().size() << " landmarks to all the " << bb_data->rows()
                                  << " pixels.")
//  auto dijkstra = DijkstraWithImplementation(DIJKSTRA_IMPLEMENTATION_BOOST, graph);
  auto dijkstra = DijkstraWithImplementation(DIJKSTRA_IMPLEMENTATION_CL, graph);
  dijkstra->SetSourceVertices(landmark->landmarks());

  dijkstra->Run();
  auto distance_matrix_landmark_to_all = dijkstra->GetDistanceMatrix();
  auto distance_matrix_landmarks = GetCols(distance_matrix_landmark_to_all, landmark->landmarks());
  LOGI("DijkstraCL finished.")


//  {
////    ofstream ofs("/private/var/tmp/Volatile/Archive/Results/PaviaU/optimcomp/cpptests/mcorrect/debug_distances.txt");
////    ofs << *distance_matrix_landmark_to_all;
////  }

  auto landmark_cmds_embedding = CMDS(*distance_matrix_landmarks, bb_data->cols(), EMBEDDING_CMDS_SOLVE_EIGEN_ONLY);
  auto manifold = ConstructManifold(*distance_matrix_landmark_to_all,
                                    *distance_matrix_landmarks,
                                    landmark_cmds_embedding,
                                    bb_data->cols());

  auto nncache_loaded = std::make_shared<gsl::Matrix>(hsi_data.data()->rows() - backbone.sampling_indices().size(), 11);
  std::ifstream ifs("/Volatile/playground/PaviaU/backbone_nncache.txt");
  ifs >> *nncache_loaded;

  auto reconstructed = backbone.Reconstruct(*manifold,
                                            PropertyList({{BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_STRATEGY,
                                                           BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED},
                                                          {BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED_NUMBER, 5}}),
                                            nncache_loaded);
  HsiData reconstructed_image(reconstructed, hsi_data.lines(), hsi_data.samples(), reconstructed->cols());
  reconstructed_image.WriteImageFile("/Volatile/playground/PaviaU/manifold_recon5_ak");


  LOGTIMESTAMP("Landmark List Demo Finished.");

}



int main() {

//  example2();

  paviau_mnf_landmark_tests();
  
}

