#ifndef COFFEE_CORE_MATRICES_H
#define COFFEE_CORE_MATRICES_H

#include "vectors.h"

namespace Coffee{
namespace CVectors{

template<typename T,size_t Size> struct _cbasic_tmatrix
{
    _cbasic_tvector<T,Size> d[Size];

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

    /*
     * Standard operators
     *
     */

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
    _cbasic_tmatrix<T,Size>& operator=(const _cbasic_tmatrix<T,Size>& matrix)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] = matrix[x][y];
        return *this;
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
    void operator*=(const _cbasic_tmatrix<T,Size>& matrix)
    {
        *this = (*this) * matrix;
    }
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
};

/*
 * Non-member operators for matrices
 *
 */

template<typename T,size_t Size>
_cbasic_tmatrix<T,Size> operator+(
        _cbasic_tmatrix<T,Size> const& src,
        _cbasic_tmatrix<T,Size> const& matrix)
{
    _cbasic_tmatrix<T,4> newmat(src);

    newmat += matrix;

    return newmat;
}

template<typename T,size_t Size>
_cbasic_tmatrix<T,Size> operator-(
        _cbasic_tmatrix<T,Size> const& src,
        _cbasic_tmatrix<T,Size> const& matrix)
{
    _cbasic_tmatrix<T,4> newmat(src);

    newmat -= matrix;

    return newmat;
}

template<typename T,size_t Size>
_cbasic_tmatrix<T,Size> operator*(
        _cbasic_tmatrix<T,Size> const& m1,
        _cbasic_tmatrix<T,Size> const& m2)
{
    _cbasic_tmatrix<T,Size> res(0);

    for(size_t i=0;i<Size;i++)
        for(size_t j=0;j<Size;j++)
            res[i] += m1[j]*m2[i][j];

    return res;
}

template<typename T,size_t Size>
_cbasic_tmatrix<T,Size> operator/(
        _cbasic_tmatrix<T,Size> const& src,
        const T& val)
{
    _cbasic_tmatrix<T,Size> newmat(src);

    newmat /= val;

    return newmat;
}

template<typename T,size_t Size>
_cbasic_tmatrix<T,Size> operator*(
        _cbasic_tmatrix<T,Size> const& src,
        const T& val)
{
    _cbasic_tmatrix<T,Size> newmat(src);

    newmat *= val;

    return newmat;
}

/*
 * Below here goes utility functions
 *
 */

template<typename T> _cbasic_tmatrix<T,4> translation(
        const _cbasic_tmatrix<T,4>& mat,
        const _cbasic_tvector<T,3>& vector)
{
    _cbasic_tmatrix<T,4> newmat(mat);

    newmat[3] = _cbasic_tvector<T,4>(T(0));

    for(size_t i=0;i<3;i++)
        newmat[3] += mat[i]*vector[i];
    newmat[3] += mat[3];

    return newmat;
}

template<typename T> _cbasic_tmatrix<T,4> scale(
        const _cbasic_tmatrix<T,4>& matrix,
        const _cbasic_tvector<T,3>& vector)
{
    _cbasic_tmatrix<T,4> newmat(matrix);
    for(size_t i=0;i<3;i++)
        newmat[i] *= vector[i];
    newmat[3] = matrix[3];
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
