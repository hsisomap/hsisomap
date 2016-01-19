//
// Created by Can on 1/18/16.
//

#include <gtest/gtest.h>
#include <hsisomap/gsl_util/embedding.h>
#include <hsisomap/gsl_util/matrix_util.h>

TEST(gsl_util_check, pca) {
  // Note that make_shared does not take implicit initializer list. Use explicit conversion or shared_ptr and new.
  auto data = std::make_shared<gsl::Matrix>(std::initializer_list<std::initializer_list<Scalar>>(
      {{439, 431, 431},
       {690, 359, 458},
       {810, 698, 588},
       {856, 912, 775},
       {858, 572, 475},
       {779, 837, 716},
       {786, 799, 750},
       {591, 605, 559},
       {1222, 992, 670},
       {1245, 1041, 552},
       {928, 871, 824},
       {579, 382, 374},
       {404, 395, 494},
       {1031, 849, 691},
       {1173, 686, 705},
       {965, 785, 704},
       {808, 543, 647},
       {1073, 920, 819},
       {1029, 769, 580},
       {639, 496, 646}}
  ));
  auto result_space = std::make_shared<gsl::Matrix>(std::initializer_list<std::initializer_list<Scalar>>(
      {{7.124635e+02, -1.876492e+02, 1.463782e+02},
       {8.557943e+02, -1.806343e+01, 2.864173e+02},
       {1.193456e+03, -1.693220e+02, 1.898440e+02},
       {1.415463e+03, -3.525812e+02, 1.930199e+02},
       {1.116023e+03, -1.241672e+01, 2.081151e+02},
       {1.295665e+03, -3.308468e+02, 1.784869e+02},
       {1.286579e+03, -3.330540e+02, 2.290988e+02},
       {9.686287e+02, -2.489345e+02, 1.660637e+02},
       {1.699362e+03, -8.710378e+01, 1.755842e+02},
       {1.712752e+03, -1.532728e+01, 6.823963e+01},
       {1.455666e+03, -3.226491e+02, 2.752013e+02},
       {7.660333e+02, -4.192493e+01, 1.802119e+02},
       {6.827383e+02, -2.361323e+02, 2.034378e+02},
       {1.477641e+03, -1.613010e+02, 2.275998e+02},
       {1.481308e+03, -1.369896e+01, 3.861217e+02},
       {1.393491e+03, -1.846597e+02, 2.591306e+02},
       {1.111367e+03, -1.450077e+02, 3.319010e+02},
       {1.589612e+03, -2.487714e+02, 2.823762e+02},
       {1.393737e+03, -5.619611e+01, 2.023410e+02},
       {9.596506e+02, -2.308191e+02, 3.121638e+02}}
  ));
  auto result_vectors = std::make_shared<gsl::Matrix>(std::initializer_list<std::initializer_list<Scalar>>(
      {{7.207580e-01, 6.272547e-01, 2.950584e-01},
       {6.302026e-01, -4.156600e-01, -6.557984e-01},
       {2.887087e-01, -6.586185e-01, 6.948878e-01}}
  ));
  auto result_values = std::make_shared<gsl::Matrix>(std::initializer_list<std::initializer_list<Scalar>>(
      {{1.024164e+05, 1.385935e+04, 5.269492e+03}}
  ));
  gsl::Embedding result = gsl::PCA(data);
  result.space->equality_limit_ = 1e-3;
  result.vectors->equality_limit_ = 1e-3;
  result.values->equality_limit_ = 1e-1;

  EXPECT_EQ(*result.space, *result_space);
  EXPECT_EQ(*result.vectors, *result_vectors);
  EXPECT_EQ(*result.values, *result_values);
}



/* The test data is partially generated with the help of the following GNU Octave code:
 *

rootpath = '{Source Root Directory}/tests/hsisomap_tests/test_data/';
data = enviread([rootpath, 'hsi_data_test_1_bil'], [rootpath, 'hsi_data_test_1_bil.hdr']);

data_serial = squeeze(reshape(permute(data, [2, 1, 3]), [size(data, 1) * size(data, 2), 1, size(data, 3)]));

%%
hsisomap_test_scripts_gsl_matrix_initialzier_list(data_serial, '%u');

%%

centered = data_serial - repmat(mean(data_serial, 1), [size(data_serial, 1), 1]);
covariance = cov(centered);
[V, D] = eig(covariance);
[D, D_o] = sort(diag(D), 'descend');
V = V(:, D_o);
D = diag(D);

%% make eigenvectors have the same direction as possible
datum = zeros(size(V, 1), 1);
datum(1) = 1;
for col = 1:size(V, 2)
    if dot(datum, V(:, col)) < 0
        V(:, col) = - V(:, col);
    end
end

%%
space = centered * V;

%%
hsisomap_test_scripts_gsl_matrix_initialzier_list(space, '%u');
%%
hsisomap_test_scripts_gsl_matrix_initialzier_list(V, '%u');
%%
hsisomap_test_scripts_gsl_matrix_initialzier_list(diag(D)', '%u');

 * hsisomap_test_scripts_gsl_matrix_initialzier_list.m

function hsisomap_test_scripts_gsl_matrix_initialzier_list(data, format_string)

clc
fprintf('{{');
for i = 1 : size(data, 1)
    for j = 1 : size(data, 2)
        fprintf(format_string, data(i, j));
        if j ~= size(data, 2)
            fprintf(', ');
        else
            fprintf('}');
        end
    end
    if i ~= size(data, 1)
        fprintf(', \n{');
    else
        fprintf('}\n');
    end
end

end

*/