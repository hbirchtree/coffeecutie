#pragma once

#define C_FLAGS(type,utype) \
    \
    inline type operator~(type val){return (type)(~((utype)val));} \
    \
    inline type operator^(type lhs, type rhs){return (type)(((utype)lhs)^((utype)rhs));} \
    inline type operator&(type lhs, type rhs){return (type)(((utype)lhs)&((utype)rhs));} \
    inline type operator|(type lhs, type rhs){return (type)(((utype)lhs)|((utype)rhs));} \
    \
    inline type operator^=(type& lhs, type rhs){return lhs = (type)(((utype)lhs)^((utype)rhs));} \
    inline type operator&=(type& lhs, type rhs){return lhs = (type)(((utype)lhs)&((utype)rhs));} \
    inline type operator|=(type& lhs, type rhs){return lhs = (type)(((utype)lhs)|((utype)rhs));}

/*
 */

namespace Coffee{

template<typename T>
/*!
 * \brief Cast enum type to boolean
 * \param flagcombo
 * \return
 */
inline bool feval(T flagcombo)
{
    return (int)flagcombo;
}

}