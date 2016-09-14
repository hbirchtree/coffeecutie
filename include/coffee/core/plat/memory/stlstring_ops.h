#pragma once


#include "../plat_quirks_toggling.h"

#include <algorithm>
#include <cctype>

#ifdef COFFEE_USE_IOSTREAMS
#include <iomanip>
#endif

#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"
#include "../../coffee_mem_macros.h"

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

#if defined(COFFEE_ARCH_LLP64)
	const constexpr cstring u8_fmt  = "%hhu";
	const constexpr cstring u16_fmt = "%hu";
	const constexpr cstring u32_fmt = "%u";
	const constexpr cstring u64_fmt = "%llu";

	const constexpr cstring i8_fmt = "%hhd";
	const constexpr cstring i16_fmt = "%hd";
	const constexpr cstring i32_fmt = "%d";
	const constexpr cstring i64_fmt = "%lld";
#else
	const constexpr cstring u8_fmt = "%hhu";
	const constexpr cstring u16_fmt = "%hu";
	const constexpr cstring u32_fmt = "%u";
	const constexpr cstring u64_fmt = "%lu";

	const constexpr cstring i8_fmt = "%hhd";
	const constexpr cstring i16_fmt = "%hd";
	const constexpr cstring i32_fmt = "%d";
	const constexpr cstring i64_fmt = "%ld";
#endif

/*
 * In the below functions we remove the null-terminator from the strings.
 * It causes a literal NULL to appear in the strings, which is bad, m'kay?
 *
 */

/* Floating-point conversion */
FORCEDINLINE CString scalarltostring(lscalar const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%Lf",s));
    snprintf(&str[0],str.size(),"%Lf",s);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString scalartostring(bigscalar const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%f",s));
    snprintf(&str[0],str.size(),"%f",s);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString scalarftostring(scalar const& s)
{
    CString str;
    str.resize(snprintf(nullptr,0,"%f",s));
    snprintf(&str[0],str.size(),"%f",s);
    str.resize(str.size()-1);
    return str;
}
/* Unsigned integer conversion */
FORCEDINLINE CString uintltostring(uint64 const& s)
{
    if(s==0)
        return "0";
    // TODO: Find out what the fuck is going on here
    uint64 ss = s*10;
    CString str;
#if defined(COFFEE_RASPBERRYPI)
    str.resize(30);
#else
    str.resize(snprintf(nullptr,0,u64_fmt,ss));
#endif
    str.resize(snprintf(&str[0],str.size(),u64_fmt,ss)-1);
    return str;
}
FORCEDINLINE CString uinttostring(uint32 const& s)
{
    if(s==0)
        return "0";
    uint32 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,u32_fmt,ss));
    snprintf(&str[0],str.size(),u32_fmt,ss);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString uintstostring(uint16 const& s)
{
    if(s==0)
        return "0";
    uint16 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,u16_fmt,ss));
    snprintf(&str[0],str.size(),u16_fmt,ss);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString uintctostring(uint8 const& s)
{
    if(s==0)
        return "0";
    uint8 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,u8_fmt,ss));
    snprintf(&str[0],str.size(),u8_fmt,ss);
    str.resize(str.size()-1);
    return str;
}
/* Integer conversion */
FORCEDINLINE CString intltostring(int64 const& s)
{
    if(s==0)
        return "0";
    int64 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,i64_fmt,ss));
    snprintf(&str[0],str.size(),i64_fmt,ss);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString inttostring(int32 const& s)
{
    if(s==0)
        return "0";
    int32 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,i32_fmt,ss));
    snprintf(&str[0],str.size(),i32_fmt,ss);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString intstostring(int16 const& s)
{
    if(s==0)
        return "0";
    int16 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,i16_fmt,ss));
    snprintf(&str[0],str.size(),i16_fmt,ss);
    str.resize(str.size()-1);
    return str;
}
FORCEDINLINE CString intctostring(int8 const& s)
{
    if(s==0)
        return "0";
    int8 ss = s*10;
    CString str;
    str.resize(snprintf(nullptr,0,i8_fmt,ss));
    snprintf(&str[0],str.size(),i8_fmt,ss);
    str.resize(str.size()-1);
    return str;
}

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

    byte_t const* data = (byte_t const*)ptr;

    uint32 temp;
    szptr j = 0;
    for(szptr i=0;i<len/3; i++)
    {
        temp = (data[j++]) << 16;
        temp += (data[j++]) << 8;
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
    return false;
}

}

namespace StrUtil{

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

FORCEDINLINE CString lower(CString const& st)
{
    CString o;
    o.reserve(st.size());
    for(CString::value_type c : st)
        o.push_back(std::tolower(c));
    return o;
}

FORCEDINLINE CString upper(CString const& st)
{
    CString o;
    o.reserve(st.size());
    for(CString::value_type c : st)
        o.push_back(std::toupper(c));
    return o;
}

FORCEDINLINE CString propercase(CString const& input)
{
    CString out;
    out.reserve(input.size());
    szptr i = 0;
    for(CString::value_type c : input)
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
