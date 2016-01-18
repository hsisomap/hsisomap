//
// Created by Can on 1/11/16.
//

#include <hsisomap/hsisomap>

int main() {
  LOGTIMESTAMP("hsisomap_cli")
  LOGFILE("text.txt");

  LOGR("haha")
  hsisomap::HsiData("/Users/can/Results/candidate_scenes/ENVI Image Files/PaviaU");

  return 0;
}
