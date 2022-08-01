#pragma once

#include "mnmatrix.h"
#include "vectors.h"

namespace typing {
namespace vectors {

template<typename T, size_t Size>
struct tmatrix
{
    using value_type = T;
    using col_type = tvector<T, Size>;

    static constexpr size_t col_size = Size;
    static constexpr size_t row_size = Size;
    static constexpr size_t  size = Size * Size;

    tvector<T, Size> d[Size];

    /*!
     * \brief Fill matrix with specific value
     * \param c Value to fill with
     */
    FORCEDINLINE
    tmatrix(const T& c)
    {
        for(size_t y = 0; y < Size; y++)
            for(size_t x = 0; x < Size; x++)
                (*this)[x][y] = c;
    }
    /*!
     * \brief Generate an identity matrix
     */
    FORCEDINLINE
    tmatrix()
    {
        for(size_t y = 0; y < Size; y++)
            for(size_t x = 0; x < Size; x++)
                if(x == y)
                    (*this)[x][y] = T(1);
                else
                    (*this)[x][y] = T(0);
    }
    FORCEDINLINE
    tmatrix(const tmatrix<T, Size + 1>& matrix)
    {
        for(size_t y = 0; y < Size; y++)
            for(size_t x = 0; x < Size; x++)
                (*this)[x][y] = matrix[x][y];
    }
    FORCEDINLINE
    tmatrix(const tmatrix<T, Size>& matrix)
    {
        for(size_t y = 0; y < Size; y++)
            for(size_t x = 0; x < Size; x++)
                (*this)[x][y] = matrix[x][y];
    }
    FORCEDINLINE
    tmatrix(const tmatrix<T, Size - 1>& matrix) : tmatrix()
    {
        for(size_t y = 0; y < Size - 1; y++)
            for(size_t x = 0; x < Size - 1; x++)
                (*this)[x][y] = matrix[x][y];
    }

    /*
     * Standard operators
     *
     */

    FORCEDINLINE tvector<T, Size>& operator[](size_t i)
    {
        return d[i];
    }
    FORCEDINLINE const tvector<T, Size>& operator[](size_t i) const
    {
        return d[i];
    }

    FORCEDINLINE bool operator==(const tmatrix<T, Size>& matrix) const
    {
        for(size_t y = 0; y < Size; y++)
            for(size_t x = 0; x < Size; x++)
                if(matrix[x][y] != (*this)[x][y])
                    return false;
        return true;
    }
    FORCEDINLINE tmatrix<T, Size>& operator=(const tmatrix<T, Size>& matrix)
    {
        for(size_t y = 0; y < Size; y++)
            for(size_t x = 0; x < Size; x++)
                (*this)[x][y] = matrix[x][y];
        return *this;
    }
    FORCEDINLINE void operator+=(const tmatrix<T, Size>& matrix)
    {
        for(size_t y = 0; y < Size; y++)
            for(size_t x = 0; x < Size; x++)
                (*this)[x][y] += matrix[x][y];
    }
    FORCEDINLINE void operator-=(const tmatrix<T, Size>& matrix)
    {
        for(size_t y = 0; y < Size; y++)
            for(size_t x = 0; x < Size; x++)
                (*this)[x][y] -= matrix[x][y];
    }
    FORCEDINLINE void operator*=(const tmatrix<T, Size>& matrix)
    {
        *this = (*this) * matrix;
    }
    FORCEDINLINE void operator*=(const T& val)
    {
        for(size_t y = 0; y < Size; y++)
            for(size_t x = 0; x < Size; x++)
                (*this)[x][y] *= val;
    }
    FORCEDINLINE void operator/=(const T& val)
    {
        for(size_t y = 0; y < Size; y++)
            for(size_t x = 0; x < Size; x++)
                (*this)[x][y] /= val;
    }

    FORCEDINLINE tvector<T, Size> operator*(tvector<T, Size> const& vec) const
    {
        tvector<T, Size> out(vec);

        for(size_t i = 0; i < Size; i++)
            out[i] = dot((*this)[i], vec);

        return out;
    }

    FORCEDINLINE tmatrix<T, Size> operator*(T const& scl) const
    {
        tmatrix<T, Size> out(*this);

        for(size_t i = 0; i < Size; i++)
            for(size_t j = 0; j < Size; j++)
                out[i][j] *= scl;

        return out;
    }
};

/*
 * Non-member operators for matrices
 *
 */

template<typename T, size_t Size>
FORCEDINLINE tmatrix<T, Size> operator+(
    tmatrix<T, Size> const& src, tmatrix<T, Size> const& matrix)
{
    tmatrix<T, Size> newmat(src);

    newmat += matrix;

    return newmat;
}

template<typename T, size_t Size>
FORCEDINLINE tmatrix<T, Size> operator-(
    tmatrix<T, Size> const& src, tmatrix<T, Size> const& matrix)
{
    tmatrix<T, Size> newmat(src);

    newmat -= matrix;

    return newmat;
}

template<typename T, size_t Size>
FORCEDINLINE tmatrix<T, Size> operator*(
    tmatrix<T, Size> const& m1, tmatrix<T, Size> const& m2)
{
    tmatrix<T, Size> res(0);

    for(size_t i = 0; i < Size; i++)
        for(size_t j = 0; j < Size; j++)
            res[i] += m1[j] * m2[i][j];

    return res;
}

template<typename T, size_t Size>
FORCEDINLINE tvector<T, Size> operator*(
    tvector<T, Size> const& m1, tmatrix<T, Size> const& m2)
{
    tvector<T, Size> res(0);

    for(size_t i = 0; i < Size; i++)
        for(size_t j = 0; j < Size; j++)
            res[i] += m1[j] * m2[i][j];

    return res;
}

template<typename T, size_t Size>
FORCEDINLINE tmatrix<T, Size> operator/(
    tmatrix<T, Size> const& src, const T& val)
{
    tmatrix<T, Size> newmat(src);

    newmat /= val;

    return newmat;
}

template<typename T, size_t Size>
FORCEDINLINE tmatrix<T, Size> operator*(
    tmatrix<T, Size> const& src, const T& val)
{
    tmatrix<T, Size> newmat(src);

    newmat *= val;

    return newmat;
}

/*
 * Below here goes utility functions
 *
 */

template<typename T, size_t Size>
FORCEDINLINE tmatrix<T, Size> translation(
    const tmatrix<T, Size>& mat, const tvector<T, Size - 1>& vector)
{
    tmatrix<T, Size> newmat(mat);

    newmat[Size - 1] = tvector<T, Size>(T(0));

    for(size_t i = 0; i < (Size - 1); i++)
        newmat[Size - 1] += mat[i] * vector[i];
    newmat[Size - 1] += mat[Size - 1];

    return newmat;
}

template<typename T, size_t Size>
FORCEDINLINE tmatrix<T, Size> scale(
    const tmatrix<T, Size>& matrix, const tvector<T, Size - 1>& vector)
{
    tmatrix<T, Size> newmat(matrix);
    for(size_t i = 0; i < (Size - 1); i++)
        newmat[i] *= vector[i];
    newmat[Size - 1] = matrix[Size - 1];
    return newmat;
}

template<typename T>
/*!
 * \brief Rotate a matrix by specified euler angle
 * \param matrix
 * \param angle
 * \param vector
 * \return
 */
FORCEDINLINE tmatrix<T, 4> rotate(
    const tmatrix<T, 4>& matrix, const T& angle, const tvector<T, 3>& vector)
{
    (void)matrix;

    T const a = angle;
    T const b = cos(a);
    T const c = sin(a);

    tvector<T, 3> axis(normalize(vector));
    //    tvector<T,3> temp((T(1) - c)*axis);

    return {};
}

template<typename T, size_t Size>
/*!
 * \brief Transpose a matrix
 * \param m
 * \return
 */
FORCEDINLINE tmatrix<T, Size> transpose(const tmatrix<T, Size>& m)
{
    tmatrix<T, Size> n;

    for(size_t i = 0; i < Size; i++)
        for(size_t j = 0; j < Size; j++)
            n[i][j] = m[j][i];

    return n;
}

template<typename T, size_t Size>
/* TODO: Fix this function!*/
FORCEDINLINE tvector<T, Size> get_translation(const tmatrix<T, Size>& m)
{
    return m[Size - 1];
}

template<typename T>
tmatrix<T, 4> inverse(const tmatrix<T, 4>& mv)
{
    tmatrix<T, 4> nv(mv);

    const T* m   = (const T*)mv.d;
    T*       inv = (T*)nv.d;

    T   det;
    int i;

    inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] +
             m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

    inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] +
             m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] +
             m[12] * m[7] * m[10];

    inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] +
             m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

    inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] +
              m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] +
              m[12] * m[6] * m[9];

    inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] +
             m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] +
             m[13] * m[3] * m[10];

    inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] +
             m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

    inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] -
             m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

    inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] +
              m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

    inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] +
             m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

    inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] -
             m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

    inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] +
              m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

    inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] -
              m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] -
             m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] -
              m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if(det == 0)
        return tmatrix<T, 4>();

    det = 1.0 / det;

    for(i = 0; i < 16; i++)
        inv[i] = inv[i] * det;

    return nv;
}

template<typename T>
FORCEDINLINE tvector<T, 3> decompose_matrix(tmatrix<T, 3> const& m)
{
    tvector<T, 3> out;

    out[0] = atan2(m[3][2], m[3][3]);
    out[1] = atan2(-m[3][1], sqrt(pow(m[3][2], 2) + m[3][3]));
    out[2] = atan2(m[2][1], m[1][1]);

    return out;
}

} // namespace vectors
} // namespace typing
