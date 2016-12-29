//
// Created by Can on 1/11/16.
//

#include <hsisomap/hsisomap>
#include <3rdparty/picojson.h>
#include <cli/configuration_defs.h>
#include <boost/filesystem.hpp>

// Internal accesses for debug information output purposes
#include <hsisomap/landmark/LandmarkSubsets.h>

void tasking(const picojson::value &task_value) {

  using namespace hsisomap;

  // TODO: Exit error code const-ify

  if (!task_value.is<picojson::object>()) {
    std::cerr << "Unexpected configuration structure error." << std::endl;
    exit(2);
  }
  auto task = task_value.get<picojson::object>();


  LOGI("Task Index: " << task[CONFIG::TASK_INDEX]);
  LOGI("Task Description: " << task[CONFIG::TASK_DESCRIPTION].to_str());

  // TODO: cross-task image data reuse without reloading - loading may take a long time.

  LOGI("Loading image.")
  HsiData hsi_data(task[CONFIG::INPUT].to_str());

  // Output root path
  boost::filesystem::path output_root_path(task[CONFIG::OUTPUT_ROOT_PATH].to_str());
  if (task[CONFIG::OUTPUT_ROOT_PATH].to_str() == "") {
    output_root_path = boost::filesystem::current_path();
  }

  // Output file
  if (task[CONFIG::OUTPUT_FILE].to_str() == "") {
    std::cerr << "Output file required." << std::endl;
    exit(3);
  }


  // Backbone processing
  if (!task[CONFIG::BACKBONE_SAMPLING].is<picojson::object>()) {
    std::cerr << "Unexpected configuration structure error." << std::endl;
    exit(2);
  }
  auto backbone_config = task[CONFIG::BACKBONE_SAMPLING].get<picojson::object>();


  std::vector<Index> backbone_indices;

  // TODO: Add support for SAMPLING implementation and None or missing backbone section (all pixels).

  if (backbone_config[CONFIG::IMPLEMENTATION].to_str() == CONFIG::LOADING) {

    // TODO: Output path handling: the case when absolute path is specified to overwrite output_root_path setting. Same for below.
    auto backbone_index_path =
        output_root_path / boost::filesystem::path(backbone_config[CONFIG::INDEX_INPUT_FILE].to_str());
    backbone_indices = Read1DVectorFromTextFile<Index>(backbone_index_path.string());

  }

  Backbone backbone(hsi_data.data(), backbone_indices);
  auto bb_data = backbone.sampled_data();


  // Landmark processing
  if (!task[CONFIG::LANDMARK].is<picojson::object>()) {
    std::cerr << "Unexpected configuration structure error." << std::endl;
    exit(2);
  }
  auto landmark_config = task[CONFIG::LANDMARK].get<picojson::object>();

  std::shared_ptr<Landmark> landmark;

  if (landmark_config[CONFIG::IMPLEMENTATION].to_str() == CONFIG::SUBSETS) {

    // Landmark count
    Scalar landmark_count = 0;
    if (landmark_config[CONFIG::COUNT].is<double>()) {
      landmark_count = landmark_config[CONFIG::COUNT].get<double>();
    } else {
      std::cerr << "Unexpected data type at " << CONFIG::LANDMARK << " -> " << CONFIG::COUNT << "." << std::endl;
      exit(3);
    }
    if (landmark_count == 0) {
      std::cerr << "Wrong landmark count value or not specified." << std::endl;
      exit(3);
    }

    // Landmark subset parameters

    // TODO: support other noise models
    auto landmark_subsets_noise_model = LANDMARK_SUBSETS_NOISE_MODEL_MNF;
    if (landmark_config[CONFIG::NOISE_MODEL].to_str() == CONFIG::MNF) {
      // Default value, do nothing
    } else {
      std::cerr << "Unexpected landmark subsets noise model: " << landmark_config[CONFIG::NOISE_MODEL] << "."
                << std::endl;
      exit(3);
    }

    Scalar landmark_subsets_preselection_noise_exclusion_percentage = 0.0;
    Scalar landmark_subsets_preselection_noise_exclusion_noise_dimensions = 1.0;

    if (landmark_config[CONFIG::PRESELECTION_NOISE_EXCLUSION_PERCENTAGE].is<double>()) {
      landmark_subsets_preselection_noise_exclusion_percentage =
          landmark_config[CONFIG::PRESELECTION_NOISE_EXCLUSION_PERCENTAGE].get<double>();
    }

    if (landmark_config[CONFIG::PRESELECTION_NOISE_EXCLUSION_NOISE_DIMENSION].is<double>()) {
      landmark_subsets_preselection_noise_exclusion_noise_dimensions =
          landmark_config[CONFIG::PRESELECTION_NOISE_EXCLUSION_NOISE_DIMENSION].get<double>();
    }

    landmark = LandmarkWithImplementation(LANDMARK_IMPLEMENTATION_SUBSETS, bb_data,
                                          {{LANDMARK_COUNT,
                                            landmark_count},
                                           {LANDMARK_SUBSETS_NOISE_MODEL,
                                            landmark_subsets_noise_model},
                                           {LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_PERCENTAGE,
                                            landmark_subsets_preselection_noise_exclusion_percentage},
                                           {LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_NOISE_DIMENSIONS,
                                            landmark_subsets_preselection_noise_exclusion_noise_dimensions}
                                          });

    // Save subset info if needed
    if (landmark_config[CONFIG::LANDMARK_SUBSET_INDEXES_OUTPUT_FILE].to_str() != "") {
      auto landmark_subset_indexes_output_file_path =
          output_root_path / boost::filesystem::path(landmark_config[CONFIG::LANDMARK_SUBSET_INDEXES_OUTPUT_FILE].to_str());
      LOGI("Save landmark subset indexes to file " << landmark_subset_indexes_output_file_path.string() << ".");
      auto landmark_subset_ptr = std::dynamic_pointer_cast<LandmarkSubsets>(landmark);

      {
        // Note that the subset_indexes is an array of non-equal length arrays. Therefore do not display through a gsl::Matrix.
        std::ofstream ofs(landmark_subset_indexes_output_file_path.string());
        auto subset_indexes = landmark_subset_ptr->subset_indexes();
        for (auto group : subset_indexes) {
          for (auto index : group) {
            ofs << index << " ";
          }
          ofs << std::endl;
        }

      }
    }

  }

  // TODO: Support other landmark selection methods


  if (landmark == nullptr) {
    std::cerr << "Fatal error in creating landmark object." << std::endl;
    exit(1);
  }

  if (landmark_config[CONFIG::LANDMARK_INDEX_OUTPUT_FILE].to_str() != "") {
    auto landmark_index_file_path =
        output_root_path / boost::filesystem::path(landmark_config[CONFIG::LANDMARK_INDEX_OUTPUT_FILE].to_str());
    LOGI("Save landmark list to file " << landmark_index_file_path.string() << ".");
    {
      std::ofstream ofs(landmark_index_file_path.string());
      std::copy(landmark->landmarks().begin(),
                landmark->landmarks().end(),
                std::ostream_iterator<Index>(ofs, "\n"));
    }
  }

  // kNN Graph Processing
  if (!task[CONFIG::KNNGRAPH].is<picojson::object>()) {
    std::cerr << "Unexpected configuration structure error." << std::endl;
    exit(2);
  }
  auto knngraph_config = task[CONFIG::KNNGRAPH].get<picojson::object>();

  std::shared_ptr<KNNGraph> knngraph;

  if (knngraph_config[CONFIG::IMPLEMENTATION].to_str() == CONFIG::ADAPTIVE_K_HIDENN) {

    Scalar knngraph_adaptive_k_hidenn_subset_number = 0;
    if (knngraph_config[CONFIG::SUBSET_COUNT].is<double>()) {
      knngraph_adaptive_k_hidenn_subset_number = knngraph_config[CONFIG::SUBSET_COUNT].get<double>();
    } else {
      std::cerr << "Unexpected data type at " << CONFIG::KNNGRAPH << " -> " << CONFIG::SUBSET_COUNT << "." << std::endl;
      exit(3);
    }

    if (knngraph_adaptive_k_hidenn_subset_number == 0) {
      std::cerr << "Wrong adaptive k hidenn subset number value or not specified." << std::endl;
      exit(3);
    }

    auto knngraph_graph_backend = KNNGRAPH_GRAPH_BACKEND_ADJACENCYLIST;
    if (knngraph_config[CONFIG::BACKEND].to_str() == CONFIG::ADJACENCY_LIST) {
      // Default, do nothing
      // TODO: Support boost graph
    } else {
      std::cerr << "Unexpected knngraph backend: " << knngraph_config[CONFIG::BACKEND] << "."
                << std::endl;
      exit(3);
    }

    knngraph = KNNGraphWithImplementation(KNNGRAPH_IMPLEMENTATION_ADAPTIVE_K_HIDENN,
                                          bb_data,
                                          PropertyList({{KNNGRAPH_ADAPTIVE_K_HIDENN_SUBSET_NUMBER,
                                                         knngraph_adaptive_k_hidenn_subset_number},
                                                        {KNNGRAPH_GRAPH_BACKEND,
                                                         knngraph_graph_backend}}));

  }

  // TODO: Support other knngraph methods

  if (knngraph == nullptr) {
    std::cerr << "Fatal error in creating knngraph object." << std::endl;
    exit(1);
  }

  // TODO: Support serializing knngraph


  // Dijkstra Processing

  if (!task[CONFIG::DIJKSTRA].is<picojson::object>()) {
    std::cerr << "Unexpected configuration structure error." << std::endl;
    exit(2);
  }
  auto dijkstra_config = task[CONFIG::DIJKSTRA].get<picojson::object>();

  std::shared_ptr<Dijkstra::Dijkstra> dijkstra;

  if (dijkstra_config[CONFIG::IMPLEMENTATION].to_str() == CONFIG::OPENCL) {

    LOGI("Perform DijkstraCL from " << landmark->landmarks().size() << " landmarks to all the " << bb_data->rows()
                                    << " pixels.")
    dijkstra = Dijkstra::DijkstraWithImplementation(Dijkstra::DIJKSTRA_IMPLEMENTATION_CL, knngraph->knngraph());
    dijkstra->SetSourceVertices(landmark->landmarks());
    dijkstra->Run();
    LOGI("DijkstraCL finished.")

  }

  // TODO: Support boost graph implementation

  if (dijkstra == nullptr) {
    std::cerr << "Fatal error in creating dijkstra object." << std::endl;
    exit(1);
  }

  LOGI("Performing CMDS...");

  auto distance_matrix_landmark_to_all = dijkstra->GetDistanceMatrix();
  auto distance_matrix_landmarks = GetCols(distance_matrix_landmark_to_all, landmark->landmarks());

  auto
      landmark_cmds_embedding = CMDS(*distance_matrix_landmarks, bb_data->cols(), gsl::EMBEDDING_CMDS_SOLVE_EIGEN_ONLY);
  auto manifold = ConstructManifold(*distance_matrix_landmark_to_all,
                                    *distance_matrix_landmarks,
                                    landmark_cmds_embedding,
                                    bb_data->cols());

  // Backbone Reconstruction Processing

  LOGI("Backbone reconstruction...");

  if (!task[CONFIG::BACKBONE_RECONSTRUCTION].is<picojson::object>()) {
    std::cerr << "Unexpected configuration structure error." << std::endl;
    exit(2);
  }
  auto backbone_reconstruction_config = task[CONFIG::BACKBONE_RECONSTRUCTION].get<picojson::object>();

  if (backbone_reconstruction_config[CONFIG::NNCACHE_INPUT_FILE].to_str() == "") {
    std::cerr << "Backbone reconstruction nncache input file required." << std::endl;
    exit(3);
  }

  auto nncache_input_file_path =
      output_root_path / boost::filesystem::path(backbone_reconstruction_config[CONFIG::NNCACHE_INPUT_FILE].to_str());

  auto nncache_loaded = std::make_shared<gsl::Matrix>(hsi_data.data()->rows() - backbone.sampling_indices().size(), 11);
  {
    std::ifstream ifs(nncache_input_file_path.string());
    ifs >> *nncache_loaded;
  }

  auto backbone_reconstruction_neighborhood_strategy = BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED;
  if (backbone_reconstruction_config[CONFIG::NEIGHBORHOOD_STRATEGY].to_str() == CONFIG::FIXED) {
    // Default, do nothing
  } else {
    std::cerr << "Unexpected backbone reconstruction neighborhood strategy: "
              << backbone_reconstruction_config[CONFIG::NEIGHBORHOOD_STRATEGY] << "."
              << std::endl;
    exit(3);
  }

  Scalar backbone_reconstruction_neighborhood_fixed_number = 0;
  if (backbone_reconstruction_config[CONFIG::NEIGHBORHOOD_SIZE].is<double>()) {
    backbone_reconstruction_neighborhood_fixed_number =
        backbone_reconstruction_config[CONFIG::NEIGHBORHOOD_SIZE].get<double>();
  }

  auto reconstructed = backbone.Reconstruct(*manifold,
                                            PropertyList({{BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_STRATEGY,
                                                           backbone_reconstruction_neighborhood_strategy},
                                                          {BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED_NUMBER,
                                                           backbone_reconstruction_neighborhood_fixed_number}}),
                                            nncache_loaded);

  LOGI("Save image...");

  HsiData reconstructed_image(reconstructed, hsi_data.lines(), hsi_data.samples(), reconstructed->cols());
  auto output_file_path = output_root_path / boost::filesystem::path(task[CONFIG::OUTPUT_FILE].to_str());
  reconstructed_image.WriteImageFile(output_file_path.string());

}

int main(int argc, char **argv) {

  if (argc == 1 || argc > 2) {
    std::cout << "Hyperspectral ISOMAP Command Line Interface" << std::endl << "Usage: " << std::endl
              << "hsisomap_cli <JSON Configuration File>" << std::endl;
    exit(0);
  }

  std::string config_file = std::string(argv[1]);

  picojson::value json;

  std::ifstream ifs(config_file);
  ifs >> json;
  std::string err = picojson::get_last_error();

  if (!err.empty()) {
    std::cerr << err << std::endl;
    exit(1);
  }

  if (!json.is<picojson::object>()) {
    std::cerr << "Unexpected configuration structure error." << std::endl;
    exit(2);
  }

  auto main_config = json.get<picojson::object>();
  if (main_config[hsisomap::CONFIG::TYPE].to_str() != hsisomap::CONFIG::HSISOMAP_TASK_CONFIGURATION) {
    std::cerr << "Wrong file format.";
  }

  if (main_config[hsisomap::CONFIG::VERSION].to_str() != "0") {
    std::cerr << "Wrong version. Supported version is 0.";
  }

  if (!main_config[hsisomap::CONFIG::TASKS].is<picojson::array>()) {
    std::cerr << "Unexpected configuration structure error." << std::endl;
    exit(2);
  }

  auto task_vector = main_config[hsisomap::CONFIG::TASKS].get<picojson::array>();

  for (int i = 0; i < task_vector.size(); ++i) {
    LOGTIMESTAMP("Processing task " << i + 1 << ".");
    tasking(task_vector[i]);
    LOGTIMESTAMP("Finished task " << i + 1 << ".");
  }

  return 0;
}
