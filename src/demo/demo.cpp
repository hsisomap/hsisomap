//
// Created by Can on 1/20/16.
//

#include <hsisomap/hsisomap>

using namespace std;
using namespace hsisomap;
using namespace gsl;
using namespace Dijkstra;

void BackboneDemo() {

//  HsiData hsi_data((string(argv[1])));
//  vector<Index> backbone_indices = Read1DVectorFromTextFile<Index>(string(argv[2]));


//  HsiData hsi_data("/Users/can/Results/PaviaU/optimcomp/PaviaU_spectral_serial");
  HsiData hsi_data("/Users/can/Results/candidate_scenes/ENVI Image Files/PaviaU");

  vector<Index> backbone_indices =
      Read1DVectorFromTextFile<Index>("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/backbone_sampled_index.txt");

//  for_each(backbone_indices.begin(), backbone_indices.end(), [] (Index &i) { i--; }); // If loading from Octave representation (1-based)

  Backbone backbone(hsi_data.data(), backbone_indices);
  auto bb_data = backbone.sampled_data();

  backbone.PrepareNNCache(10,
                          PropertyList({{BACKBONE_NNCACHE_PRIMARY_SEARCH_RANGE, 99},
                                        {BACKBONE_NNCACHE_SECONDARY_SEARCH_RANGE, 100}}));

//  {
//    std::ofstream ofs("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/cache_fromcpp.txt");
//    ofs << *backbone.nn_cache();
//  }

//  HsiData hsi_data_output(bb_data, 1, bb_data->rows(), bb_data->cols());


//  auto nncache_loaded = std::make_shared<gsl::Matrix>(hsi_data.data()->rows() - backbone.sampling_indices().size(), 11);
//  std::ifstream ifs("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/cache_nnlist.txt");
//  ifs >> *nncache_loaded;


//  std::ofstream ofs("/Users/can/Results/PaviaU/optimcomp/cpptests/temp2.txt");
//  ofs << nncache_loaded;

  HsiData isomapdata("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/liresult.img",
                     "/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/liresult.hdr");
  auto reconstructed = backbone.Reconstruct(*isomapdata.data(),
                                            PropertyList({{BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_STRATEGY,
                                                           BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED},
                                                          {BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED_NUMBER, 5}})/*,
                                            nncache_loaded*/);
  HsiData reconstructed_image(reconstructed, hsi_data.lines(), hsi_data.samples(), reconstructed->cols());
  reconstructed_image.WriteImageFile("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/recon_nn5_newnncache");

}

void LandmarkListDemo() {
  LOGTIMESTAMP("Landmark List Demo Begin.")

  LOGI("Load image.")

  HsiData hsi_data("/Users/can/Results/candidate_scenes/ENVI Image Files/PaviaU");
  vector<Index> backbone_indices =
      Read1DVectorFromTextFile<Index>("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/backbone_sampled_index.txt");

  LOGI("Backbone sampling.")

  Backbone backbone(hsi_data.data(), backbone_indices);
  auto bb_data = backbone.sampled_data();


  LOGI("Load Landmarks from file.")
  auto landmark = LandmarkWithImplementation(LANDMARK_IMPLEMENTATION_LIST,
                                             bb_data,
                                             PropertyList(),
                                             std::make_shared<std::vector<Index>>(
                                                 Read1DVectorFromTextFile<Index>(
                                                     "/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/selected_landmarks.txt")));

  auto landmark_data = landmark->landmark_data();
  LOGI("Create kNN Graph.")
  auto knngraph = KNNGraphWithImplementation(KNNGRAPH_IMPLEMENTATION_FIXED_K_WITH_MST,
                                             bb_data,
                                             PropertyList({{KNNGRAPH_GRAPH_BACKEND,
                                                            KNNGRAPH_GRAPH_BACKEND_ADJACENCYLIST},
                                                           {KNNGRAPH_FIXED_K_NUMBER, 30}}));
  auto graph = knngraph->knngraph();

  LOGI("Perform DijkstraCL from " << landmark->landmarks().size() << " landmarks to all the " << bb_data->rows()
           << " pixels.")
  auto dijkstra = DijkstraWithImplementation(DIJKSTRA_IMPLEMENTATION_CL, graph);
  dijkstra->SetSourceVertices(landmark->landmarks());

  dijkstra->Run();
  auto distance_matrix_landmark_to_all = dijkstra->GetDistanceMatrix();
  auto distance_matrix_landmarks = GetCols(distance_matrix_landmark_to_all, landmark->landmarks());
//  LOGI("DijkstraCL finished.")


//  {
//    ofstream ofs("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/debug_distances.txt");
//    ofs << *distance_matrix_landmark_to_all;
//  }
  auto landmark_cmds_embedding = CMDS(*distance_matrix_landmarks, bb_data->cols(), EMBEDDING_CMDS_SOLVE_EIGEN_ONLY);
  auto manifold = ConstructManifold(*distance_matrix_landmark_to_all,
                                    *distance_matrix_landmarks,
                                    landmark_cmds_embedding,
                                    bb_data->cols());
  {
    ofstream ofs("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/debug_evec.txt");
    ofs << *landmark_cmds_embedding.vectors;
  }

  {
    ofstream ofs("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/debug_eval.txt");
    ofs << *landmark_cmds_embedding.values;
  }

  {
    ofstream ofs("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/debug_manifold.txt");
    ofs << *manifold;
  }
  auto nncache_loaded = std::make_shared<gsl::Matrix>(hsi_data.data()->rows() - backbone.sampling_indices().size(), 11);
  std::ifstream ifs("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/cache_nnlist.txt");
  ifs >> *nncache_loaded;

  auto reconstructed = backbone.Reconstruct(*manifold,
                                            PropertyList({{BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_STRATEGY,
                                                           BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED},
                                                          {BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED_NUMBER, 5}}),
                                            nncache_loaded);
  HsiData reconstructed_image(reconstructed, hsi_data.lines(), hsi_data.samples(), reconstructed->cols());
  reconstructed_image.WriteImageFile("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/manifold_recon5");


  LOGTIMESTAMP("Landmark List Demo Finished.")

}

int main(int argc, char *argv[]) {

//  BackboneDemo();
  LandmarkListDemo();

  return 0;
}