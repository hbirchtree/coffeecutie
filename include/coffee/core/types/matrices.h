#ifndef COFFEE_CORE_MATRICES_H
#define COFFEE_CORE_MATRICES_H

#include "vectors.h"
#include "mnmatrix.h"

namespace Coffee{

template<typename T,size_t Size> struct _cbasic_tmatrix
{
    typedef _cbasic_tvector<T,Size> col_type;
    static constexpr size_t size = Size*Size;

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
	_cbasic_tmatrix(const _cbasic_tmatrix<T, Size+1>& matrix)
	{
		for (size_t y = 0; y<Size; y++)
			for (size_t x = 0; x<Size; x++)
				(*this)[x][y] = matrix[x][y];
	}
    _cbasic_tmatrix(const _cbasic_tmatrix<T,Size>& matrix)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] = matrix[x][y];
    }
	_cbasic_tmatrix(const _cbasic_tmatrix<T, Size-1>& matrix)
		: _cbasic_tmatrix()
	{
		for (size_t y = 0; y<Size-1; y++)
			for (size_t x = 0; x<Size-1; x++)
				(*this)[x][y] = matrix[x][y];
	}

    /*
     * Standard operators
     *
     */

    FORCEDINLINE _cbasic_tvector<T,Size>& operator[](size_t i)
    {
        return d[i];
    }
    FORCEDINLINE const _cbasic_tvector<T,Size>& operator[](size_t i) const
    {
        return d[i];
    }

    FORCEDINLINE bool operator==(const _cbasic_tmatrix<T,Size>& matrix) const
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                if(matrix[x][y]!=(*this)[x][y])
                    return false;
        return true;
    }
    FORCEDINLINE _cbasic_tmatrix<T,Size>& operator=(const _cbasic_tmatrix<T,Size>& matrix)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] = matrix[x][y];
        return *this;
    }
    FORCEDINLINE void operator+=(const _cbasic_tmatrix<T,Size>& matrix)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] += matrix[x][y];
    }
    FORCEDINLINE void operator-=(const _cbasic_tmatrix<T,Size>& matrix)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] -= matrix[x][y];
    }
    FORCEDINLINE void operator*=(const _cbasic_tmatrix<T,Size>& matrix)
    {
        *this = (*this) * matrix;
    }
    FORCEDINLINE void operator*=(const T& val)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] *= val;
    }
    FORCEDINLINE void operator/=(const T& val)
    {
        for(size_t y=0;y<Size;y++)
            for(size_t x=0;x<Size;x++)
                (*this)[x][y] /= val;
    }

    FORCEDINLINE _cbasic_tvector<T,Size> operator*(
            _cbasic_tvector<T,Size> const& vec)
    {
        _cbasic_tvector<T,Size> out(vec);

	for(size_t i=0;i<Size;i++)
            out[i] = dot((*this)[i],vec);

        return out;
    }

    FORCEDINLINE _cbasic_tmatrix<T,Size> operator*(
	    T const& scl)
    {
	_cbasic_tmatrix<T,Size> out(*this);

	for(size_t i=0;i<Size;i++)
	    for(size_t j=0;j<Size;j++)
		out[i][j] *= scl;

	return out;
    }
};

/*
 * Non-member operators for matrices
 *
 */

template<typename T,size_t Size>
FORCEDINLINE _cbasic_tmatrix<T,Size> operator+(
        _cbasic_tmatrix<T,Size> const& src,
        _cbasic_tmatrix<T,Size> const& matrix)
{
    _cbasic_tmatrix<T,4> newmat(src);

    newmat += matrix;

    return newmat;
}

template<typename T,size_t Size>
FORCEDINLINE _cbasic_tmatrix<T,Size> operator-(
        _cbasic_tmatrix<T,Size> const& src,
        _cbasic_tmatrix<T,Size> const& matrix)
{
    _cbasic_tmatrix<T,4> newmat(src);

    newmat -= matrix;

    return newmat;
}

template<typename T,size_t Size>
FORCEDINLINE _cbasic_tmatrix<T,Size> operator*(
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
FORCEDINLINE _cbasic_tmatrix<T,Size> operator/(
        _cbasic_tmatrix<T,Size> const& src,
        const T& val)
{
    _cbasic_tmatrix<T,Size> newmat(src);

    newmat /= val;

    return newmat;
}

template<typename T,size_t Size>
FORCEDINLINE _cbasic_tmatrix<T,Size> operator*(
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

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,4> translation(
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

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,4> scale(
        const _cbasic_tmatrix<T,4>& matrix,
        const _cbasic_tvector<T,3>& vector)
{
    _cbasic_tmatrix<T,4> newmat(matrix);
    for(size_t i=0;i<3;i++)
        newmat[i] *= vector[i];
    newmat[3] = matrix[3];
    return newmat;
}

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,4> rotate(
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

template<typename T, size_t Size>
FORCEDINLINE _cbasic_tmatrix<T,Size> transpose(const _cbasic_tmatrix<T,Size>& m)
{
    _cbasic_tmatrix<T,Size> n;

    for(size_t i=0;i<Size;i++)
        for(size_t j=0;j<Size;j++)
            n[i][j] = m[j][i];

    return n;
}

template<typename T>
/* TODO: Fix this function!*/
FORCEDINLINE _cbasic_tvector<T,4> get_translation(const _cbasic_tmatrix<T,4>& m)
{
    return m[3];
}

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,4> inverse(const _cbasic_tmatrix<T,4>& mv)
{
    _cbasic_tmatrix<T,4> nv(mv);

    const T* m = (const T*)mv.d;
    T* inv = (T*)nv.d;

    T det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] -
            m[5]  * m[11] * m[14] -
            m[9]  * m[6]  * m[15] +
            m[9]  * m[7]  * m[14] +
            m[13] * m[6]  * m[11] -
            m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
            m[4]  * m[11] * m[14] +
            m[8]  * m[6]  * m[15] -
            m[8]  * m[7]  * m[14] -
            m[12] * m[6]  * m[11] +
            m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
            m[4]  * m[11] * m[13] -
            m[8]  * m[5] * m[15] +
            m[8]  * m[7] * m[13] +
            m[12] * m[5] * m[11] -
            m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
            m[4]  * m[10] * m[13] +
            m[8]  * m[5] * m[14] -
            m[8]  * m[6] * m[13] -
            m[12] * m[5] * m[10] +
            m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
            m[1]  * m[11] * m[14] +
            m[9]  * m[2] * m[15] -
            m[9]  * m[3] * m[14] -
            m[13] * m[2] * m[11] +
            m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
            m[0]  * m[11] * m[14] -
            m[8]  * m[2] * m[15] +
            m[8]  * m[3] * m[14] +
            m[12] * m[2] * m[11] -
            m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
            m[0]  * m[11] * m[13] +
            m[8]  * m[1] * m[15] -
            m[8]  * m[3] * m[13] -
            m[12] * m[1] * m[11] +
            m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
            m[0]  * m[10] * m[13] -
            m[8]  * m[1] * m[14] +
            m[8]  * m[2] * m[13] +
            m[12] * m[1] * m[10] -
            m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
            m[1]  * m[7] * m[14] -
            m[5]  * m[2] * m[15] +
            m[5]  * m[3] * m[14] +
            m[13] * m[2] * m[7] -
            m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
            m[0]  * m[7] * m[14] +
            m[4]  * m[2] * m[15] -
            m[4]  * m[3] * m[14] -
            m[12] * m[2] * m[7] +
            m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
            m[0]  * m[7] * m[13] -
            m[4]  * m[1] * m[15] +
            m[4]  * m[3] * m[13] +
            m[12] * m[1] * m[7] -
            m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
            m[0]  * m[6] * m[13] +
            m[4]  * m[1] * m[14] -
            m[4]  * m[2] * m[13] -
            m[12] * m[1] * m[6] +
            m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
            m[1] * m[7] * m[10] +
            m[5] * m[2] * m[11] -
            m[5] * m[3] * m[10] -
            m[9] * m[2] * m[7] +
            m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
            m[0] * m[7] * m[10] -
            m[4] * m[2] * m[11] +
            m[4] * m[3] * m[10] +
            m[8] * m[2] * m[7] -
            m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
            m[0] * m[7] * m[9] +
            m[4] * m[1] * m[11] -
            m[4] * m[3] * m[9] -
            m[8] * m[1] * m[7] +
            m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
            m[0] * m[6] * m[9] -
            m[4] * m[1] * m[10] +
            m[4] * m[2] * m[9] +
            m[8] * m[1] * m[6] -
            m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return _cbasic_tmatrix<T,4>();

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        inv[i] = inv[i] * det;

    return nv;
}

}

#endif

