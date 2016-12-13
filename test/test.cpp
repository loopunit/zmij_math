/*
 * test.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: zmij
 */

#include "math/vector.hpp"
#include "math/matrix.hpp"

#include "math/vector_io.hpp"
#include "math/matrix_io.hpp"

#include <iostream>

#include <gtest/gtest.h>

namespace math {

template <typename T, ::std::size_t N>
void
PrintTo(vector<T, N> const& vec, ::std::ostream* os)
{
    *os << io::vector_pretty << vec;
}

template < typename T, ::std::size_t RC, ::std::size_t CC >
void
PrintTo(matrix<T, RC, CC> const& mtx, ::std::ostream* os)
{
    *os << "\n" << io::vector_pretty << mtx;
}

namespace test {

using vector3d = math::vector<double, 3>;
using vector3df = math::vector<float, 3>;
using matrix3x3 = math::matrix<double, 3, 3>;

TEST(Vector, Construction)
{
    //EXPECT_TRUE(::std::is_standard_layout<vector3d>::value);
    EXPECT_TRUE(::std::is_trivially_copyable<vector3d>::value);
    EXPECT_TRUE(::std::is_trivially_default_constructible<vector3d>::value);
    EXPECT_TRUE(::std::is_trivial<vector3d>::value);
    EXPECT_TRUE(::std::is_trivially_copy_constructible<vector3d>::value);
    //EXPECT_TRUE(::std::is_pod<vector3d>::value);
    {
        // Default
        vector3d v1 {};
        EXPECT_EQ(0, v1[0]);
        EXPECT_EQ(0, v1[1]);
        EXPECT_EQ(0, v1[2]);
        EXPECT_TRUE(v1.is_zero());

        EXPECT_EQ(0, v1.x());
        EXPECT_EQ(0, v1.y());
        EXPECT_EQ(0, v1.z());

        EXPECT_EQ(sizeof(double)*3, sizeof(v1))
            << "Size of vector is equal to sizes of it's components";
    }
    {
        EXPECT_EQ(sizeof(float)*3, sizeof(vector3df))
            << "Size of vector is equal to sizes of it's components";
    }

    {
        // Initializer list
        vector3d v1{1, 0, 0};
        EXPECT_EQ(1, v1[0]);
        EXPECT_EQ(0, v1[1]);
        EXPECT_EQ(0, v1[2]);
    }

    {
        // Initializer list
        vector3d v1{1, 2, 3};
        EXPECT_EQ(1, v1[0]);
        EXPECT_EQ(2, v1[1]);
        EXPECT_EQ(3, v1[2]);
    }
    {
        // Pointer to double
        double vals[]{1, 2, 3};
        {
            vector3d v1{vals};
            EXPECT_EQ(1, v1[0]);
            EXPECT_EQ(2, v1[1]);
            EXPECT_EQ(3, v1[2]);
        }

        double* p = vals;
        {
            vector3d v1{p};
            EXPECT_EQ(1, v1[0]);
            EXPECT_EQ(2, v1[1]);
            EXPECT_EQ(3, v1[2]);
        }
    }
}

TEST(Vector, Modify)
{
    vector3d v1{};
    EXPECT_TRUE(v1.is_zero());

    v1[0] = 1;
    EXPECT_FALSE(v1.is_zero());
    EXPECT_EQ((vector3d{1, 0, 0}), v1);
    v1[1] = 2;
    EXPECT_EQ((vector3d{1, 2, 0}), v1);
    v1[2] = 3;
    EXPECT_EQ((vector3d{1, 2, 3}), v1);

    v1.x() *= 2;
    EXPECT_EQ((vector3d{2, 2, 3}), v1);
    v1.y() *= 2;
    EXPECT_EQ((vector3d{2, 4, 3}), v1);
    v1.z() *= 2;
    EXPECT_EQ((vector3d{2, 4, 6}), v1);
}

TEST(Vector, Add)
{
    vector3d v1{2,5,8}, v2{4,6,9};
    {
        vector3d expected{6, 11, 17};
        auto res = v1 + v2;
        EXPECT_EQ(expected, res);
    }
    {
        auto res = v1 - v2;
        vector3d expected{-2, -1, -1};
        EXPECT_EQ(expected, res);
    }
}

TEST(Vector, Multiply)
{
    vector3d initial{1,2,3}, expected{5, 10, 15};
    auto v1 = initial * 5;
    ::std::cerr << sizeof(v1) << " " << io::vector_pretty << v1 << "\n";
    EXPECT_EQ(expected, v1);
    v1 = expected / 5;
    EXPECT_EQ(initial, v1);
}

TEST(Vector, Magnitude)
{
    vector3d v1 {1, 1, 1};
    EXPECT_EQ(3, v1.magnitude_square());
    EXPECT_FALSE(v1.is_unit());
    EXPECT_FALSE(v1.is_zero());
}

TEST(Vector, Normalize)
{
    vector3d v1 {10, 0, 0};
    EXPECT_FALSE(v1.is_zero());
    EXPECT_FALSE(v1.is_unit());
    v1.normalize();
    EXPECT_TRUE(v1.is_unit());
}

TEST(Vector, Unit)
{
    {
        vector3d v1{1, 0, 0};
        EXPECT_TRUE(v1.is_unit());
    }
    {
        vector3d v1{0, 1, 0};
        EXPECT_TRUE(v1.is_unit());
    }
    {
        vector3d v1{0, 0, 1};
        EXPECT_TRUE(v1.is_unit());
    }
}

TEST(Vector, Iteration)
{
    vector3d v1{1, 2, 3};
    int idx = 0;
    for (auto v : v1) {
        EXPECT_EQ(idx + 1, v);
        ++idx;
    }
}

TEST(Matrix, Construction)
{
    {  // default
        matrix3x3 m1;
        EXPECT_EQ(sizeof(double)*9, sizeof(m1));
    }
    {  // init list
       matrix3x3 m1 {
           { 11, 12, 13 },
           { 21, 22, 23 },
           { 31, 32, 33 }
       };

       ::std::cerr << m1 << "\n"
            << io::vector_pretty << m1 << "\n";

       EXPECT_EQ(11, m1[0][0]);
       EXPECT_EQ(12, m1[0][1]);
       EXPECT_EQ(13, m1[0][2]);

       EXPECT_EQ(21, m1[1][0]);
       EXPECT_EQ(22, m1[1][1]);
       EXPECT_EQ(23, m1[1][2]);

       EXPECT_EQ(31, m1[2][0]);
       EXPECT_EQ(32, m1[2][1]);
       EXPECT_EQ(33, m1[2][2]);
    }
    {
        double vals[]{
            11, 12, 13,
            21, 22, 23,
            31, 32, 33
        };

        matrix3x3 m1{vals};
        EXPECT_EQ(11, m1[0][0]);
        EXPECT_EQ(12, m1[0][1]);
        EXPECT_EQ(13, m1[0][2]);

        EXPECT_EQ(21, m1[1][0]);
        EXPECT_EQ(22, m1[1][1]);
        EXPECT_EQ(23, m1[1][2]);

        EXPECT_EQ(31, m1[2][0]);
        EXPECT_EQ(32, m1[2][1]);
        EXPECT_EQ(33, m1[2][2]);
    }
    {
        double vals[][3]{
            { 11, 12, 13 },
            { 21, 22, 23 },
            { 31, 32, 33 }
        };
        matrix3x3 m1{vals};
        EXPECT_EQ(11, m1[0][0]);
        EXPECT_EQ(12, m1[0][1]);
        EXPECT_EQ(13, m1[0][2]);

        EXPECT_EQ(21, m1[1][0]);
        EXPECT_EQ(22, m1[1][1]);
        EXPECT_EQ(23, m1[1][2]);

        EXPECT_EQ(31, m1[2][0]);
        EXPECT_EQ(32, m1[2][1]);
        EXPECT_EQ(33, m1[2][2]);
    }
}

TEST(Matrix, Iteration)
{
    matrix3x3 m1 {
        { 11, 12, 13 },
        { 21, 22, 23 },
        { 31, 32, 33 }
    };
    auto it = m1.begin();
    auto e = m1.end();
    for (int i = 1; i < 4 && it != e; ++i) {
        for (int j = 1; j < 4 && it != e; ++j) {
            EXPECT_EQ(i * 10 + j, *it);
            ++it;
        }
    }
}

TEST(Matrix, Add)
{
    matrix3x3 m1{
        { 11, 12, 13 },
        { 21, 22, 23 },
        { 31, 32, 33 }
    };
    matrix3x3 m2{
        { 11, 12, 13 },
        { 21, 22, 23 },
        { 31, 32, 33 }
    };
    EXPECT_EQ(
    (matrix3x3{
        { 22, 24, 26 },
        { 42, 44, 46 },
        { 62, 64, 66 }
    }), m1 + m2);
    EXPECT_EQ(matrix3x3{}, m1 - m2);
}

TEST(Matrix, ScalarMultiply)
{
    matrix3x3 initial{
        { 11, 12, 13 },
        { 21, 22, 23 },
        { 31, 32, 33 }
    };
    matrix3x3 expected{
        { 22, 24, 26 },
        { 42, 44, 46 },
        { 62, 64, 66 }
    };
    EXPECT_EQ(expected, initial * 2);
    EXPECT_EQ(initial, expected / 2);
}

TEST(Matrix, VectorMultiply)
{
    matrix3x3 initial{
        { 11, 12, 13 },
        { 21, 22, 23 },
        { 31, 32, 33 }
    };
    vector3d expected{0, 0, 0};
    vector3d vec{1, 2, 3};
    EXPECT_EQ(expected, initial * vec);
}

TEST(Matrix, MatrixMultiply)
{
    matrix3x3 initial{
        { 11, 12, 13 },
        { 21, 22, 23 },
        { 31, 32, 33 }
    };
    matrix3x3 expected{
        { 22, 24, 26 },
        { 42, 44, 46 },
        { 62, 64, 66 }
    };
    vector3d vec{1, 2, 3};
    EXPECT_EQ(expected, initial * initial);
}

}  /* namespace test */
}  /* namespace math */


