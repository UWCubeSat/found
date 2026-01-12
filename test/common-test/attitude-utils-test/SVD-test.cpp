#include <gtest/gtest.h>

#include <cmath>
#include <utility>

#include "common/style.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "common/logging.hpp"
#include "common/decimal.hpp"
#include "test/common/common.hpp"

using found::Matrix;
using found::SVDResult;
using found::ComputeSVD;

TEST(SVDTest, Test1Random3x3) {
    int modulo = 100000;
    decimal divisor = 100;

    Matrix mat(3,3);

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            mat(i,j) = (rand() % modulo)/divisor;
        }
    }

    SVDResult result = ComputeSVD(mat, 3);
    
    Matrix reconstruction = result.U * result.S * result.V.Transpose();

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            decimal expected = mat.Get(i,j);
            decimal actual = reconstruction.Get(i,j);
            EXPECT_LT(abs(expected - actual), DEFAULT_TOLERANCE);
        }
    }
}
TEST(SVDTest, Test2Random3x3) {
    int modulo = 100000;
    decimal divisor = 100;

    Matrix mat(3,3);

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            mat(i,j) = (rand() % modulo)/divisor;
        }
    }

    SVDResult result = ComputeSVD(mat, 3);
    
    Matrix reconstruction = result.U * result.S * result.V.Transpose();

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            decimal expected = mat.Get(i,j);
            decimal actual = reconstruction.Get(i,j);
            EXPECT_LT(abs(expected - actual), DEFAULT_TOLERANCE);
        }
    }
}
TEST(SVDTest, Test3Random10x5) {
    int modulo = 100000;
    decimal divisor = 100;

    Matrix mat(10,5);

    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 5; j++){
            mat(i,j) = (rand() % modulo)/divisor;
        }
    }

    SVDResult result = ComputeSVD(mat, 5);
    
    Matrix reconstruction = result.U * result.S * result.V.Transpose();

    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 5; j++){
            decimal expected = mat.Get(i,j);
            decimal actual = reconstruction.Get(i,j);
            EXPECT_LT(abs(expected - actual), DEFAULT_TOLERANCE);
        }
    }
}