#pragma once

#define C_FC(t,v) static_cast<t>(v)

#define C_FLAGS(e,t) \
    \
    inline e operator~(e val){return C_FC(e,~(C_FC(t,val)));} \
    \
    inline e operator^(e lhs, e rhs){return C_FC(e,C_FC(t,lhs)^C_FC(t,rhs));} \
    inline e operator&(e lhs, e rhs){return C_FC(e,C_FC(t,lhs)&C_FC(t,rhs));} \
    inline e operator|(e lhs, e rhs){return C_FC(e,C_FC(t,lhs)|C_FC(t,rhs));} \
    \
    inline e operator^=(e& lhs, e rhs){return lhs = C_FC(e,C_FC(t,lhs)^C_FC(t,rhs));} \
    inline e operator&=(e& lhs, e rhs){return lhs = C_FC(e,C_FC(t,lhs)^C_FC(t,rhs));} \
    inline e operator|=(e& lhs, e rhs){return lhs = C_FC(e,C_FC(t,lhs)^C_FC(t,rhs));}

namespace Coffee{

template<typename T>
/*!
 * \brief Cast enum type to boolean
 * \param flagcombo
 * \return
 */
inline bool feval(T flagcombo)
{
    return static_cast<int>(flagcombo);
}

template<typename T>
inline bool feval(T flags, T target)
{
    return (flags&target)==target;
}

}
