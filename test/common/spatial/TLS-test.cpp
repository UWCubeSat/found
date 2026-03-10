#include <gtest/gtest.h>
#include <Eigen/Core>

#include "test/common/common.hpp"

#include "common/logging.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/decimal.hpp"
#include <iostream>
#include <fstream>
#include <random>
using namespace std;
#define MAX_COLS_TO_TEST 10
#define MONTE_CARLO_ITERATIONS 10
#define MAX_NOISE_TO_TEST 0.5
#define MAX_COEFFICIENT 1000
#define DECIMAL_PLACES 100 // 1/DECIMAL_PLACES is used to determine how many decimal places we go to i.e 100 would be x.xx, 1000 would be x.xxx
#define ERROR_STEPS 30
#define TLS_TOLERANCE DECIMAL(0.05)

#define VECTOR3_EQUALS(vec1, vec2, tolerance)     \
    EXPECT_LT(abs(vec1(0) - vec2(0)), tolerance); \
    EXPECT_LT(abs(vec1(1) - vec2(1)), tolerance); \
    EXPECT_LT(abs(vec1(2) - vec2(2)), tolerance);

#define VECTOR2_EQUALS(vec1, vec2, tolerance)     \
    EXPECT_LT(abs(vec1(0) - vec2(0)), tolerance); \
    EXPECT_LT(abs(vec1(1) - vec2(1)), tolerance);

TEST(TLSTest, vec3Test)
{
    Eigen::Matrix<decimal, 6, 4> data{
        {1.0, 1.0, 1.0, 3.0},
        {1.0, 2.0, 3.0, 6.0},
        {0.0, 0.0, 0.0, 0.0},
        {-1.0, -1.0, -1.0, -3.0},
        {-1.0, 3.0, 0.0, 2.0},
        {4329.0, -4211.0, 0.0, 118.0}};
    Eigen::Vector<decimal, 3> expected(1, 1, 1);
    Eigen::Vector<decimal, 3> actual = found::TLS(data);
    VECTOR3_EQUALS(actual, expected, DEFAULT_TOLERANCE);
}

void TestPermuations(int rows, int cols, std::default_random_engine rando, ofstream *MyFile)
{
    int modulo = MAX_COEFFICIENT * DECIMAL_PLACES;
    decimal divisor = DECIMAL_PLACES;
    // Technically this isn't monte carlo but we can use the same variable
    for (int itera = 0; itera < MONTE_CARLO_ITERATIONS; itera++)
    {
        for (decimal noise = 0; noise < MAX_NOISE_TO_TEST; noise += DECIMAL(MAX_NOISE_TO_TEST) / ERROR_STEPS)
        {
            std::stringstream ss;
            decimal smallPercent = (noise/MAX_NOISE_TO_TEST)*(DECIMAL(1.0)/MONTE_CARLO_ITERATIONS)*(DECIMAL(1.0)/(MAX_COLS_TO_TEST-2));
            decimal mediumPercent = (DECIMAL(itera)/MONTE_CARLO_ITERATIONS)*(DECIMAL(1.0)/(MAX_COLS_TO_TEST-2));
            decimal bigPercent = (DECIMAL(cols-3)/(MAX_COLS_TO_TEST-2));
            ss << "cols: " << cols << ", noise: " << noise << ";\t" << (smallPercent + mediumPercent + bigPercent) * 100 << "%\n";
            LOG_INFO(ss.str());
            Eigen::Vector<decimal, Eigen::Dynamic> expected;
            expected.resize(cols - 1);
            for (int j = 0; j < cols - 1; j++)
            {
                expected(j) = (rando() % modulo) / divisor;
            }

            Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> testMat;
            testMat.resize(rows, cols);
            for (int i = 0; i < rows; i++)
            {
                decimal finalValue = 0;
                for (int j = 0; j < cols - 1; j++)
                {
                    decimal coefficient = (rando() % modulo) / divisor;
                    finalValue += coefficient * expected(j);
                    testMat(i, j) = coefficient;
                }
                testMat(i, cols - 1) = finalValue;
            }
            decimal averageError = 0;
            for (int iter = 0; iter < MONTE_CARLO_ITERATIONS; iter++)
            {
                Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> noisyMatrix;
                noisyMatrix.resize(rows, cols);
                for (int i = 0; i < rows; i++)
                {
                    for (int j = 0; j < cols; j++)
                    {
                        noisyMatrix(i, j) = testMat(i, j)  + testMat(i, j) * (rando() % 1)*2-1 * noise;
                    }
                    //noisyMatrix(i, cols-1) = testMat(i, cols-1);
                }
                Eigen::Vector<decimal, Eigen::Dynamic> actual = found::TLS(noisyMatrix);
                if ((actual-expected).norm() < DEFAULT_TOLERANCE) continue;
                averageError += abs((actual-expected).norm())/expected.norm();
            }
            averageError /= MONTE_CARLO_ITERATIONS;
            if (averageError > 10000) continue;
            *MyFile << rows << "," << cols << "," << noise << "," << averageError << "," << expected.norm() << "\n";
        }
    }
}

// I'm putting the permuations for performance testing in here cause i'm too lazy to figure out how to do it properly.
// I'll remove it later
TEST(TLSTest, vec2andPermutationTest)
{
    Eigen::Matrix<decimal, 4, 3> data{
        {0.239794, 0.0, 0.719382},
        {1.0, 2.0, 5.0},
        {0.0, 0.0, 0.0},
        {-1.0, 5.0, 2.0}};
    Eigen::Vector2d expected(3, 1);
    Eigen::Vector2d actual = found::TLS(data);
    VECTOR2_EQUALS(actual, expected, DEFAULT_TOLERANCE);

    ofstream MyFile("./TLS-data.csv");
    MyFile << "rows,cols,noise,error,expected mag\n";
    std::default_random_engine rando;

    for (int i = 3; i <= MAX_COLS_TO_TEST; i++)
    {
        std::stringstream ss;
        ss << "crunchy time\n";
        LOG_INFO(ss.str());
        TestPermuations(i + (rando() % 5) + 2, i, rando, &MyFile);
        std::stringstream ss1;
        ss1 << "we did it";
        LOG_INFO(ss1.str());
    }

    MyFile.close();
}