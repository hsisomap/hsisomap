//
// Created by Can on 1/17/16.
//

#include <gtest/gtest.h>
#include <hsisomap/HsiData.h>

class HsiDataFixture: public ::testing::Test {
 protected:
  virtual void TearDown() {
  }

  virtual void SetUp() {
  }

 public:
  HsiDataFixture() : Test() {
    hsi_data_[0] = new ::hsisomap::HsiData("./test_data/hsi_data_test_1_bil", "");
    hsi_data_[1] = new ::hsisomap::HsiData("./test_data/hsi_data_test_1_bsq", "");
    hsi_data_[2] = new ::hsisomap::HsiData("./test_data/hsi_data_test_1_bip", "");
  }

  virtual ~HsiDataFixture() {
    delete hsi_data_[0];
    delete hsi_data_[1];
    delete hsi_data_[2];
  }

  ::hsisomap::HsiData * hsi_data_[3];

};

TEST_F(HsiDataFixture, hsidata_data_property_check) {
  EXPECT_EQ(hsi_data_[0]->lines(), 4);
  EXPECT_EQ(hsi_data_[0]->samples(), 5);
  EXPECT_EQ(hsi_data_[0]->bands(), 3);
  EXPECT_EQ(hsi_data_[0]->samples(), 5);
  EXPECT_EQ(hsi_data_[0]->samples(), 5);
}

TEST_F(HsiDataFixture, hsidata_data_block_check_bil) {
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(0, 0), 439);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(0, 1), 431);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(0, 2), 431);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(1, 0), 690);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(1, 1), 359);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(1, 2), 458);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(2, 0), 810);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(2, 1), 698);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(2, 2), 588);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(3, 0), 856);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(3, 1), 912);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(3, 2), 775);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(4, 0), 858);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(4, 1), 572);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(4, 2), 475);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(5, 0), 779);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(5, 1), 837);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(5, 2), 716);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(6, 0), 786);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(6, 1), 799);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(6, 2), 750);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(7, 0), 591);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(7, 1), 605);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(7, 2), 559);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(8, 0), 1222);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(8, 1), 992);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(8, 2), 670);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(9, 0), 1245);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(9, 1), 1041);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(9, 2), 552);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(10, 0), 928);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(10, 1), 871);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(10, 2), 824);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(11, 0), 579);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(11, 1), 382);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(11, 2), 374);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(12, 0), 404);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(12, 1), 395);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(12, 2), 494);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(13, 0), 1031);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(13, 1), 849);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(13, 2), 691);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(14, 0), 1173);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(14, 1), 686);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(14, 2), 705);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(15, 0), 965);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(15, 1), 785);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(15, 2), 704);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(16, 0), 808);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(16, 1), 543);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(16, 2), 647);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(17, 0), 1073);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(17, 1), 920);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(17, 2), 819);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(18, 0), 1029);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(18, 1), 769);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(18, 2), 580);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(19, 0), 639);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(19, 1), 496);
  EXPECT_DOUBLE_EQ((*(hsi_data_[0]->data()))(19, 2), 646);
}

TEST_F(HsiDataFixture, hsidata_data_block_check_bsq) {
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(0, 0), 439);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(0, 1), 431);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(0, 2), 431);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(1, 0), 690);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(1, 1), 359);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(1, 2), 458);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(2, 0), 810);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(2, 1), 698);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(2, 2), 588);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(3, 0), 856);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(3, 1), 912);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(3, 2), 775);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(4, 0), 858);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(4, 1), 572);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(4, 2), 475);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(5, 0), 779);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(5, 1), 837);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(5, 2), 716);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(6, 0), 786);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(6, 1), 799);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(6, 2), 750);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(7, 0), 591);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(7, 1), 605);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(7, 2), 559);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(8, 0), 1222);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(8, 1), 992);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(8, 2), 670);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(9, 0), 1245);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(9, 1), 1041);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(9, 2), 552);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(10, 0), 928);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(10, 1), 871);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(10, 2), 824);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(11, 0), 579);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(11, 1), 382);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(11, 2), 374);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(12, 0), 404);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(12, 1), 395);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(12, 2), 494);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(13, 0), 1031);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(13, 1), 849);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(13, 2), 691);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(14, 0), 1173);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(14, 1), 686);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(14, 2), 705);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(15, 0), 965);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(15, 1), 785);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(15, 2), 704);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(16, 0), 808);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(16, 1), 543);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(16, 2), 647);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(17, 0), 1073);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(17, 1), 920);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(17, 2), 819);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(18, 0), 1029);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(18, 1), 769);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(18, 2), 580);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(19, 0), 639);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(19, 1), 496);
  EXPECT_DOUBLE_EQ((*(hsi_data_[1]->data()))(19, 2), 646);
}

TEST_F(HsiDataFixture, hsidata_data_block_check_bip) {
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(0, 0), 439);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(0, 1), 431);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(0, 2), 431);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(1, 0), 690);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(1, 1), 359);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(1, 2), 458);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(2, 0), 810);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(2, 1), 698);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(2, 2), 588);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(3, 0), 856);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(3, 1), 912);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(3, 2), 775);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(4, 0), 858);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(4, 1), 572);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(4, 2), 475);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(5, 0), 779);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(5, 1), 837);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(5, 2), 716);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(6, 0), 786);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(6, 1), 799);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(6, 2), 750);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(7, 0), 591);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(7, 1), 605);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(7, 2), 559);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(8, 0), 1222);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(8, 1), 992);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(8, 2), 670);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(9, 0), 1245);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(9, 1), 1041);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(9, 2), 552);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(10, 0), 928);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(10, 1), 871);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(10, 2), 824);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(11, 0), 579);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(11, 1), 382);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(11, 2), 374);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(12, 0), 404);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(12, 1), 395);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(12, 2), 494);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(13, 0), 1031);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(13, 1), 849);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(13, 2), 691);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(14, 0), 1173);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(14, 1), 686);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(14, 2), 705);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(15, 0), 965);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(15, 1), 785);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(15, 2), 704);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(16, 0), 808);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(16, 1), 543);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(16, 2), 647);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(17, 0), 1073);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(17, 1), 920);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(17, 2), 819);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(18, 0), 1029);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(18, 1), 769);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(18, 2), 580);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(19, 0), 639);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(19, 1), 496);
  EXPECT_DOUBLE_EQ((*(hsi_data_[2]->data()))(19, 2), 646);
}



/*
 * The hsidata_data_block_check_* test cases was partially generated by GNU Octave script below
 *
 * The envi file reader / writer package http://www.mathworks.com/matlabcentral/fileexchange/27172-envi-file-reader-writer was used.

rootpath = '{Source Root Directory}/tests/hsisomap_tests/test_data/';
diffstr = {'bil', 'bsq', 'bip'};
for d = 1:3
    test_data_files{d} = ['hsi_data_test_1_', diffstr{d}];
end

data = enviread([rootpath, test_data_files{1}], [rootpath, test_data_files{1}, '.hdr']);
info = enviinfo(data);
info.data_type = 4;
info.interleave = diffstr{2};
enviwrite(data, info, [rootpath, test_data_files{2}], [rootpath, test_data_files{2}, '.hdr']);
info.interleave = diffstr{3};
enviwrite(data, info, [rootpath, test_data_files{3}], [rootpath, test_data_files{3}, '.hdr']);

%%
clc

for d = 1:3
    fprintf('TEST_F(HsiDataFixture, hsidata_data_block_check_%s) {\n', diffstr{d});
    for i = 1 : size(data_serial, 1)
        for j = 1 : size(data_serial, 2)
            fprintf('  EXPECT_DOUBLE_EQ((*(hsi_data_[%u]->data()))(%u, %u), %.0f);\n', d - 1, i - 1, j - 1, data_serial(i, j));
        end
    end
    fprintf('}\n\n');
end

 */