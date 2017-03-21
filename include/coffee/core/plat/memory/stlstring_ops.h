#pragma once


#include "../plat_quirks_toggling.h"

#include <algorithm>
#include <cctype>
#include <inttypes.h>

#ifdef COFFEE_USE_IOSTREAMS
#include <iomanip>
#endif

#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"
#include "../../coffee_mem_macros.h"
#include "../../coffee_assert_macros.h"

namespace Coffee{
namespace Mem{

namespace Search
{
/* Higher-level string utilities */
template<typename StrType> FORCEDINLINE
StrType CStrReplace(const StrType &target, const StrType &query,
					const StrType &replacement)
{
    StrType out = target;
    for(size_t pos=0;;pos+=replacement.length())
    {
        pos = out.find(query,pos);
        if(pos==StrType::npos)
            break;
        out.erase(pos,query.length());
        out.insert(pos,replacement);
    }
    return out;
}

FORCEDINLINE
CString CStrReplace(CString const& target, CString const& query, CString const& replacement)
{
	return CStrReplace<CString>(target,query,replacement);
}
FORCEDINLINE
CWString CStrReplace(CWString const& target, CWString const& query, CWString const& replacement)
{
	return CStrReplace<CWString>(target, query, replacement);
}

}

namespace Convert{

#define FMT_TYPE const constexpr cstring
#if defined(PRIu8a)
#define FMT_STR(bits, fmt) \
    FMT_TYPE fmt ## bits ## _fmt = "%" PRI ## fmt ## bits
#define FMT_PAIR(bits) \
    FMT_STR(bits, u); \
    FMT_STR(bits, i); \
    FMT_STR(bits, x);

    FMT_PAIR(8)
    FMT_PAIR(16)
    FMT_PAIR(32)
    FMT_PAIR(64)
#else
#define FMT_STR(bits, fmt, prefix) \
    FMT_TYPE fmt ## bits ## _fmt = "%" prefix #fmt

#define FMT_PAIR(bits, prefix) \
    FMT_STR(bits, u, prefix); \
    FMT_STR(bits, i, prefix); \
    FMT_STR(bits, x, prefix);

#if defined(COFFEE_ARCH_LLP64)
    FMT_PAIR(8,  "hh")
    FMT_PAIR(16, "h")
    FMT_PAIR(32, "")
    FMT_PAIR(64, "ll")
#else
    FMT_PAIR(8,  "hh")
    FMT_PAIR(16, "h")
    FMT_PAIR(32, "")
    FMT_PAIR(64, "l")
#endif
#endif

#undef FMT_TYPE
#undef FMT_STR
#undef FMT_PAIR

/*
 * In the below functions we remove the null-terminator from the strings.
 * It causes a literal NULL to appear in the strings, which is bad, m'kay?
 *
 */

#define SCALAR_CONVERT(name, type, fmt) \
    FORCEDINLINE CString name(type s) \
    { \
    CString str; \
    str.resize(C_CAST<size_t>(snprintf(nullptr,0,fmt,s)) + 1); \
    snprintf(&str[0],str.size() + 1,fmt,s); \
    str.resize(str.size() - 1); \
    return str; \
    }

/* Floating-point conversion */
SCALAR_CONVERT(scalarltostring, lscalar, "%Lf")
SCALAR_CONVERT(scalartostring, bigscalar, "%f")
SCALAR_CONVERT(scalarftostring, scalar, "%f")

#undef SCALAR_CONVERT

/* Unsigned integer conversion */
#define INTEGER_CONVERT(name, type, fmt) \
    FORCEDINLINE CString name(type s) \
    { \
        if(s==0) \
            return "0"; \
        type ss = s; \
        CString str; \
        str.resize(C_CAST<size_t>(snprintf(nullptr,0,fmt,ss)) + 1); \
        snprintf(&str[0],str.size() + 1,fmt,ss); \
        str.resize(str.size() - 1); \
        return str; \
    }

INTEGER_CONVERT(uintltostring, uint64, u64_fmt)
INTEGER_CONVERT(uinttostring, uint32, u32_fmt)
INTEGER_CONVERT(uintstostring, uint16, u16_fmt)
INTEGER_CONVERT(uintctostring, uint8, u8_fmt)

INTEGER_CONVERT(intltostring, int64, i64_fmt)
INTEGER_CONVERT(inttostring, int32, i32_fmt)
INTEGER_CONVERT(intstostring, int16, i16_fmt)
INTEGER_CONVERT(intctostring, int8, i8_fmt)

#undef INTEGER_CONVERT

FORCEDINLINE cstring booltostring(bool i)
{
    return (i) ? "true" : "false";
}

}

namespace Base64
{

const constexpr cstring b64_char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Reference: https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64 */
FORCEDINLINE CString encode(c_cptr ptr, szptr len)
{
    CString out;
    out.reserve(len/3 + len % 3 > 0);

    byte_t const* data = C_CAST<byte_t const*>(ptr);

    uint32 temp;
    szptr j = 0;
    for(szptr i=0;i<len/3; i++)
    {
        temp = C_CAST<uint32>((data[j++]) << 16);
        temp += C_CAST<uint32>((data[j++]) << 8);
        temp += (data[j++]);
        out.append(1,b64_char[(temp & 0x00FC0000) >> 18]);
        out.append(1,b64_char[(temp & 0x0003F000) >> 12]);
        out.append(1,b64_char[(temp & 0x00000FC0) >> 6]);
        out.append(1,b64_char[(temp & 0x0000003F)]);
    }

    switch(len % 3)
    {
    case 1:
        temp = (data[len-(len%3)+1]);
        out.append(1,b64_char[(temp & 0x00FC0000) >> 18]);
        out.append(1,b64_char[(temp & 0x0003F000) >> 12]);
        out.append(1,'=');
        out.append(1,'=');
        break;
    case 2:
        temp = (data[len-(len%3)+1]);
        out.append(1,b64_char[(temp & 0x00FC0000) >> 18]);
        out.append(1,b64_char[(temp & 0x0003F000) >> 12]);
        out.append(1,b64_char[(temp & 0x00000FC0) >> 6]);
        out.append(1,'=');
        break;
    }

    return out;
}

FORCEDINLINE bool decode(byte_t const* i_ptr, szptr i_len,
                         Vector<byte_t>* out)
{
    /*TODO: Implement Base64 decoding*/
    C_UNUSED(i_ptr);
    C_UNUSED(i_len);
    C_UNUSED(out);
    return false;
}

}

namespace StrUtil{

FORCEDINLINE CString encapsulate(cstring src, uint32 len)
{
    CString target;
    target.insert(0, src, len);
    return target;
}

FORCEDINLINE CString reverse(CString const& src_)
{
    CString src = src_;
    std::reverse(src.begin(), src.end());
    return src;
}

FORCEDINLINE CString printclean(CString const& src_)
{
    CString src = src_;
    src.erase(src.begin(),
              std::find_if(
                  src.begin(), src.end(),
                  [](CString::value_type c){return std::isprint(c) || std::iscntrl(c);}
                  )
              );
    return src;
}

FORCEDINLINE CString hexdump(c_cptr ptr, szptr len, bool spacing = true, szptr newline_freq = 0)
{
    byte_t const* data = (byte_t const*)ptr;
    CString out;
    out.reserve(len*2 /* Hexadec */ + len*spacing /* Space */);

    for(szptr i=0;i<len;i++)
    {
        out.append("  ");
        sprintf(&out[i*2+i],"%02x",data[i]);
        if(newline_freq == 0 || (i+1)%newline_freq != 0)
        {
            if(spacing)
                out.append(" ");
        }else
            out.append("\n");
    }

    return out;
}

/* Reference: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring */
FORCEDINLINE CString& ltrim(CString& s)
{
    s.erase(s.begin(),
            std::find_if(
                s.begin(),
                s.end(),
                std::not1(
                    std::ptr_fun<int,int>(std::isspace)
                    )
                )
            );
    return s;
}
FORCEDINLINE CString& rtrim(CString& s)
{
    s.erase(std::find_if(
                s.rbegin(),
                s.rend(),
                std::not1(
                    std::ptr_fun<int,int>(std::isspace)
                    )
                ).base(),
            s.end()
            );
    return s;
}
FORCEDINLINE CString& trim(CString& s)
{
    return ltrim(rtrim(s));
}
FORCEDINLINE CString& zeroltrim(CString& s)
{
    s.erase(0,s.find_first_not_of('0'));
    return s;
}
FORCEDINLINE CString& zerortrim(CString& s)
{
    s.erase(s.find_last_not_of('0')+1,s.size());
    return s;
}
FORCEDINLINE CString& zerotrim(CString& s)
{
    return zeroltrim(zerortrim(s));
}

template<typename T>
FORCEDINLINE std::basic_string<T>& lpad(std::basic_string<T>& s, T ch, uint32 len)
{
	if(len > s.size())
		s.insert(s.begin(), len - s.size(), ch);
    return s;
}

template<typename T>
FORCEDINLINE std::basic_string<T>& rpad(std::basic_string<T>& s, T ch, uint32 len)
{
	if(len > s.size())
		s.insert(s.end(), len - s.size(), ch);
    return s;
}

#if defined(COFFEE_USE_WONKY_HEXIFY)
FORCEDINLINE CString hexify(uint64 inp, bool trim_zero = false)
{
    CString out;
    out.resize(16);

    uint8 a;
    uint64 b;
    for(uint8 i=0;i<64;i+=4)
    {
        a = i / 4;
        b = inp;
        b = b << (64 - ( a + 1) * 4);
        b = b >> 60;
        if(b<10)
            out[15-a] = '0'+b;
        else
            out[15-a] = 'a'+b-10;
    }
    zeroltrim(out);
    if(trim_zero)
        zerortrim(out);
    return out;

}
#else
#define HEX_CONVERT(name, type, fmt) \
    FORCEDINLINE CString name(type s, bool trim_zero = false) \
    { \
        if(s==0) \
            return "0"; \
        type ss = s; \
        CString str; \
        str.resize(C_CAST<size_t>(snprintf(nullptr,0,fmt,ss)) + 1); \
        snprintf(&str[0],str.size() + 1,fmt,ss); \
        str.resize(str.size() - 1); \
        zeroltrim(str); \
        if(trim_zero) \
            zerortrim(str); \
        return str; \
    }

//HEX_CONVERT(hexify, uint8,  Convert::x8_fmt)
//HEX_CONVERT(hexify, uint16, Convert::x16_fmt)
//HEX_CONVERT(hexify, uint32, Convert::x32_fmt)
HEX_CONVERT(hexify, uint64, Convert::x64_fmt)

#endif

FORCEDINLINE CString pointerify(uint64 const& ptr)
{
    CString str = "0x";
    str += hexify(ptr);
    return str;
}
FORCEDINLINE CString pointerify(const void* const& ptr)
{
    return pointerify((uint64 const&)ptr);
}

template<typename CharT >
FORCEDINLINE std::basic_string<CharT> lower(typename std::basic_string<CharT> const& st)
{
    typename std::basic_string<CharT> o;
    o.reserve(st.size());
    for(typename std::basic_string<CharT>::value_type c : st)
        o.push_back(std::tolower(c));
    return o;
}

template<typename CharT >
FORCEDINLINE std::basic_string<CharT> upper(std::basic_string<CharT> const& st)
{
    std::basic_string<CharT> o;
    o.reserve(st.size());
    for(typename std::basic_string<CharT>::value_type c : st)
        o.push_back(std::toupper(c));
    return o;
}

template<typename CharT >
FORCEDINLINE std::basic_string<CharT> propercase(std::basic_string<CharT> const& input)
{
    std::basic_string<CharT> out;
    out.reserve(input.size());
    szptr i = 0;
    for(typename std::basic_string<CharT>::value_type c : input)
    {
        if((i++) < 1)
            out.push_back(std::toupper(c));
        else
            out.push_back(c);
    }
    return out;
}

}


}
}
