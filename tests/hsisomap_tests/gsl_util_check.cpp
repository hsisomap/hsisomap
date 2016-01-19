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
      {{-5.159080e+02, -1.782623e+01, -7.870849e+01},
       {-3.725772e+02, 1.517595e+02, 6.133063e+01},
       {-3.491547e+01, 5.009619e-01, -3.524261e+01},
       {1.870913e+02, -1.827582e+02, -3.206677e+01},
       {-1.123487e+02, 1.574062e+02, -1.697152e+01},
       {6.729390e+01, -1.610238e+02, -4.659977e+01},
       {5.820760e+01, -1.632310e+02, 4.012166e+00},
       {-2.597429e+02, -7.911151e+01, -5.902290e+01},
       {4.709905e+02, 8.271917e+01, -4.950248e+01},
       {4.843802e+02, 1.544957e+02, -1.568470e+02},
       {2.272943e+02, -1.528261e+02, 5.011467e+01},
       {-4.623382e+02, 1.278980e+02, -4.487479e+01},
       {-5.456332e+02, -6.630935e+01, -2.164886e+01},
       {2.492696e+02, 8.521900e+00, 2.513179e+00},
       {2.529362e+02, 1.561240e+02, 1.610350e+02},
       {1.651198e+02, -1.483672e+01, 3.404396e+01},
       {-1.170046e+02, 2.481526e+01, 1.068144e+02},
       {3.612405e+02, -7.894842e+01, 5.728958e+01},
       {1.653652e+02, 1.136268e+02, -2.274561e+01},
       {-2.687209e+02, -6.099616e+01, 8.707717e+01}}
  ));
  auto result_vectors = std::make_shared<gsl::Matrix>(std::initializer_list<std::initializer_list<Scalar>>(
      {{7.207580e-01, 6.272547e-01, 2.950584e-01},
       {6.302026e-01, -4.156600e-01, -6.557984e-01},
       {2.887087e-01, -6.586185e-01, 6.948878e-01}}
  ));
  auto result_values = std::make_shared<gsl::Matrix>(std::initializer_list<std::initializer_list<Scalar>>(
      {{1.024164e+05, 1.385935e+04, 5.269492e+03}}
  ));
  // This test case's eigenvectors happens to be having the same direction as
  // (1, 0, ...,0) so that the direction of eigenvectors does not need to be
  // corrected to be compared with the ground-truth. If any new test cases
  // failed because of the directions of eigenvectors, make sure the
  // ground-truth eigenvectors have the same direction as (1, 0, ...,0) and
  // compile the code with the definition TEST_ENABLE_EIGENVECTOR_DIRECTION_CORRECTION
  gsl::Embedding result = gsl::PCA(*data);

  // Set appropriate equality determination limit.
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