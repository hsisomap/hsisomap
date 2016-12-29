//
// Created by Can Jin on 12/28/16.
//

#ifndef HSISOMAP_HSISOMAP_CLI_DEFS_H
#define HSISOMAP_HSISOMAP_CLI_DEFS_H


namespace hsisomap {
namespace CONFIG {

const std::string TYPE = "type";
const std::string HSISOMAP_TASK_CONFIGURATION = "hsisomap_task_configuration";
const std::string VERSION = "version";
const std::string TASKS = "tasks";
const std::string TASK_INDEX = "task index";
const std::string TASK_DESCRIPTION = "task description";

const std::string INPUT = "input";
const std::string OUTPUT_ROOT_PATH = "output root path";
const std::string BACKBONE_SAMPLING = "backbone sampling";
const std::string IMPLEMENTATION = "implementation";
const std::string LOADING = "loading";
const std::string INDEX_INPUT_FILE = "index input file";
const std::string LANDMARK = "landmark";
const std::string SUBSETS = "subsets";
const std::string COUNT = "count";
const std::string NOISE_MODEL = "noise model";
const std::string MNF = "mnf";
const std::string PRESELECTION_NOISE_EXCLUSION_PERCENTAGE = "preselection noise exclusion percentage";
const std::string PRESELECTION_NOISE_EXCLUSION_NOISE_DIMENSION = "preselection noise exclusion noise dimension";
const std::string LANDMARK_INDEX_OUTPUT_FILE = "landmark index output file";
const std::string KNNGRAPH = "knngraph";
const std::string ADAPTIVE_K_HIDENN = "adaptive k hidenn";
const std::string SUBSET_COUNT = "subset count";
const std::string BACKEND = "backend";
const std::string ADJACENCY_LIST = "adjacency list";
const std::string DIJKSTRA = "dijkstra";
const std::string OPENCL = "opencl";
const std::string RETAINED_BANDS = "retained bands";
const std::string BACKBONE_RECONSTRUCTION = "backbone reconstruction";
const std::string NNCACHE_INPUT_FILE = "nncache input file";
const std::string NEIGHBORHOOD_STRATEGY = "neighborhood strategy";
const std::string FIXED = "fixed";
const std::string NEIGHBORHOOD_SIZE = "neighborhood size";
const std::string OUTPUT_FILE = "output file";

}
}

#endif //HSISOMAP_HSISOMAP_CLI_DEFS_H
