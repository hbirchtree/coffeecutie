#ifndef COFFEE_CORE_MATRICES_H
#define COFFEE_CORE_MATRICES_H

#include "vectors.h"

namespace Coffee{
namespace CVectors{

template<typename T,size_t Size> struct _cbasic_tmatrix
{
    /*!
     * \brief Fill matrix with specific value
     * \param c Value to fill with
     */
    _cbasic_tmatrix(const T& c)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] = c;
    }
    /*!
     * \brief Generate an identity matrix
     */
    _cbasic_tmatrix()
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                if(x==y)
                    (*this)[x][y] = T(1);
                else
                    (*this)[x][y] = T(0);
    }
    _cbasic_tmatrix(const _cbasic_tmatrix<T,Size>& matrix)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] = matrix[x][y];
    }

    _cbasic_tvector<T,Size>& operator[](size_t i)
    {
        return d[i];
    }
    const _cbasic_tvector<T,Size>& operator[](size_t i) const
    {
        return d[i];
    }

    bool operator==(const _cbasic_tmatrix<T,Size>& matrix) const
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                if(matrix[x][y]!=(*this)[x][y])
                    return false;
        return true;
    }
    void operator=(const _cbasic_tmatrix<T,Size>& matrix)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] = matrix[x][y];
    }
    void operator+=(const _cbasic_tmatrix<T,Size>& matrix)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] += matrix[x][y];
    }
    void operator-=(const _cbasic_tmatrix<T,Size>& matrix)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] -= matrix[x][y];
    }

    _cbasic_tmatrix<T,Size> operator+(const _cbasic_tmatrix<T,Size>& matrix)
    {
        _cbasic_tmatrix<T,4> newmat(*this);

        newmat += matrix;

        return newmat;
    }
    _cbasic_tmatrix<T,Size> operator-(const _cbasic_tmatrix<T,Size>& matrix)
    {
        _cbasic_tmatrix<T,4> newmat(*this);

        newmat -= matrix;

        return newmat;
    }

    void operator*=(const _cbasic_tmatrix<T,Size>& matrix)
    {
        *this = (*this) * matrix;
    }
//    void operator/=(const _cbasic_tmatrix<T,Size>& matrix)
//    {
//        for(size_t y=0;y<Size;y++)
//            for(size_t x=0;x<Size;x++)
//                (*this)[x][y] /= matrix[x][y];
//    }
//    _cbasic_tmatrix<T,Size> operator*(const _cbasic_tmatrix<T,Size>& matrix)
//    {
//        _cbasic_tmatrix<T,Size> newmat(*this);

//        newmat *= matrix;

//        return newmat;
//    }
//    _cbasic_tmatrix<T,Size> operator/(const _cbasic_tmatrix<T,Size>& matrix)
//    {
//        _cbasic_tmatrix<T,4> newmat(*this);

//        newmat /= matrix;

//        return newmat;
//    }

    void operator*=(const T& val)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] *= val;
    }
    void operator/=(const T& val)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] /= val;
    }
    _cbasic_tmatrix<T,Size> operator*(const T& val)
    {
        _cbasic_tmatrix<T,Size> newmat(*this);

        newmat *= val;

        return newmat;
    }
    _cbasic_tmatrix<T,Size> operator/(const T& val)
    {
        _cbasic_tmatrix<T,Size> newmat(*this);

        newmat /= val;

        return newmat;
    }

    _cbasic_tvector<T,Size> d[Size];
};

template<typename T>
_cbasic_tmatrix<T,4> operator*(_cbasic_tmatrix<T,4> const& m1, _cbasic_tmatrix<T,4> const& m2)
{
    _cbasic_tmatrix<T,4> res(0);

    res[0] = m1[0]*m2[0][0]
            + m1[1]*m2[0][1]
            + m1[2]*m2[0][2]
            + m1[3]*m2[0][3];

    res[1] = m1[0]*m2[1][0]
            + m1[1]*m2[1][1]
            + m1[2]*m2[1][2]
            + m1[3]*m2[1][3];

    res[2] = m1[0]*m2[2][0]
            + m1[1]*m2[2][1]
            + m1[2]*m2[2][2]
            + m1[3]*m2[2][3];

    res[3] = m1[0]*m2[3][0]
            + m1[1]*m2[3][1]
            + m1[2]*m2[3][2]
            + m1[3]*m2[3][3];

    return res;
}

template<typename T> _cbasic_tmatrix<T,4> translation(
        const _cbasic_tmatrix<T,4>& mat,
        const _cbasic_tvector<T,3>& vector)
{
    _cbasic_tmatrix<T,4> newmat(mat);

    for(size_t i=0;i<3;i++)
        newmat[3] += mat[i]*vector[i];

    return newmat;
}

template<typename T> _cbasic_tmatrix<T,4> scale(
        const _cbasic_tmatrix<T,4>& matrix,
        const _cbasic_tvector<T,3>& vector)
{
    _cbasic_tmatrix<T,4> newmat(matrix);
    for(size_t i=0;i<3;i++)
        newmat[i] *= vector[i];
    return newmat;
}

template<typename T> _cbasic_tmatrix<T,4> rotate(
        const _cbasic_tmatrix<T,4>& matrix,
        const T& angle,
        const _cbasic_tvector<T,3>& vector)
{
    T const a = angle;
    T const b = CMath::cos(a);
    T const c = CMath::sin(a);

    _cbasic_tvector<T,3> axis(normalize(vector));
    _cbasic_tvector<T,3> temp((T(1) - c)*axis);
}

}
}

#endif
