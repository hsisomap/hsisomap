//
// Created by Can on 1/19/16.
//

#include <gtest/gtest.h>
#include <hsisomap/HsiData.h>
#include <hsisomap/subsetter/Subsetter.h>

class SubsetterFixture: public ::testing::Test {
 protected:
  virtual void TearDown() {
  }

  virtual void SetUp() {
  }

 public:
  SubsetterFixture() : Test() {
    hsi_data_[0] = std::make_shared<::hsisomap::HsiData>("./test_data/hsi_data_test_1_bil", "");
  }

  virtual ~SubsetterFixture() {
  }

  std::shared_ptr<::hsisomap::HsiData> hsi_data_[1];

};

TEST_F(SubsetterFixture, subsetter_subsetterembedding_check) {
  using namespace ::hsisomap;
  auto
      subsetter = SubsetterWithImplementation(SUBSETTER_IMPLEMENTATION_EMBEDDING,
                                              hsi_data_[0],
                                              PropertyList({{SUBSETTER_DEFAULT_EMBEDDING,
                                                             SUBSETTER_DEFAULT_EMBEDDING_PCA},
                                                            {SUBSETTER_EMBEDDING_SLICING_MODE,
                                                             SUBSETTER_EMBEDDING_SLICING_MODE_FIRST_MEAN},
                                                            {SUBSETTER_SUBSETS, 6}}));

  std::vector<std::vector<Index>> ground_truth {
      {8, 9},
      {13, 14, 17, 18},
      {10, 15},
      {3, 5, 6},
      {2, 4, 7, 16, 19},
      {0, 1, 11, 12}
  };
  EXPECT_EQ(subsetter->subsets(), ground_truth);
}

/* The test data are partially generated with the help of the following GNU Octave code
 *

rootpath = '{Source Root Path}/tests/hsisomap_tests/test_data/';
data = enviread([rootpath, 'hsi_data_test_1_bil'], [rootpath, 'hsi_data_test_1_bil.hdr']);
data_serial = squeeze(reshape(permute(data, [2, 1, 3]), [size(data, 1) * size(data, 2), 1, size(data, 3)]));

%%
goal = 6;
leveldepth = ceil(log(goal)/log(2));
labels = false(size(data_serial, 1), leveldepth);
generatedlabels = [];

currentgroups = {1:size(data_serial, 1)};
nextgroups = {};
%%
groupcounter = 1;

for d = 1:leveldepth

    for g = 1:size(currentgroups, 1)

        currentidx = currentgroups{g};
        current_data_serial = data_serial(currentidx, :);
        [coeff, score] = pca(current_data_serial);
        princscore = score(:, 1);

        nextgroups = [nextgroups; {currentidx(princscore>=0)}; {currentidx(princscore<0)}];
        groupcounter = groupcounter + 1
        if groupcounter == goal
            finalgroups = [nextgroups; {currentgroups{g+1:end}}'];
            break;
        end

    end
    if groupcounter == goal
        break;
    end

    sizeofgroups = zeros(size(nextgroups,1), 1);
    for j = 1:size(nextgroups,1);
        sizeofgroups(j) = size(nextgroups{j}, 2);
    end
    [~, sizeofgroups_order] = sort(-sizeofgroups);

    currentgroups = {nextgroups{sizeofgroups_order}}';
    nextgroups = {};

end

%%
clc
for i = 1 : size(finalgroups, 1)
    fprintf('{');
    for j = 1 : size(finalgroups{i}, 2)
        fprintf('%u', finalgroups{i}(j) - 1);
        if j ~= size(finalgroups{i}, 2)
            fprintf(', ');
        end
    end
    if i ~= size(finalgroups, 1)
        fprintf('}, \n');
    else
        fprintf('}\n');
    end
end

 */