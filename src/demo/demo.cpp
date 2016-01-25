//
// Created by Can on 1/20/16.
//

#include <hsisomap/hsisomap>
#include <hsisomap/util/io_util.h>

using namespace std;
using namespace hsisomap;
using namespace gsl;

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

//  HsiData hsi_data_output(bb_data, 1, bb_data->rows(), bb_data->cols());
//  std::ofstream ofs("/Users/can/Results/PaviaU/optimcomp/cpptests/mcorrect/cache_fromcpp.txt");
//  ofs << *backbone.nn_cache();


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
  HsiData hsi_data("/Users/can/Results/candidate_scenes/ENVI Image Files/PaviaU");

}

int main(int argc, char *argv[]) {

//  BackboneDemo();
  LandmarkListDemo();

  return 0;
}