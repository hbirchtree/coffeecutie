#include <coffee/core/datastorage/text/ini/ciniparser.h>

#include <coffee/core/CDebug>
#include <coffee/core/string_casting.h>

namespace Coffee {
namespace DataStorage {
namespace TextStorage {
namespace CINI {

static SimpleIniParser::variant_t* get_value(
    SimpleIniParser::document_t& doc, cstring tvalu)
{
    bool mismatch = false;

    auto itype = quiet_exception::call<std::invalid_argument>(
        str::from_string<i64>, mismatch, tvalu);

    if(!mismatch)
        return doc.newInteger(itype);

    auto dtype = quiet_exception::call<std::invalid_argument>(
        str::from_string<bigscalar>, mismatch, tvalu);
    if(!mismatch)
        return doc.newFloat(dtype);

    bool btype = quiet_exception::call<std::invalid_argument, bool>(
        str::from_string<bool>, mismatch, tvalu);
    if(!mismatch)
        return doc.newBool(btype);

    return doc.newString(tvalu);
}

SimpleIniParser::document_t SimpleIniParser::Read(
    const Bytes& source, bool unixmode)
{
    const constexpr cstring sec_ld = "[";
    const constexpr cstring sec_rd = "]";
    const constexpr cstring del_ct = ";";
    const constexpr cstring del_vl = "=";

    cstring t1, t2, t3, t4; /* Temporaries */

    cstring linesep = "\n";
    if(!unixmode)
        linesep = "\r";

    /* Retrieve data */
    cstring data = C_FCAST<cstring>(source.data);
    cstring ref  = data;
    cstring end  = data + source.size;

    /* We populate the document on-the-go */
    document_t doc;

    /* Find all sections, global values and comments */
    section_t*      csec = nullptr; /* Current section */
    CString         tname;
    CString         tvalu;
    variant_t*      cval = nullptr;
    Vector<cstring> cmtlines;
    t2 = nullptr;
    t3 = nullptr;
    t4 = nullptr;
    while(ref && ref < end)
    {
        t1 = str::find(ref, linesep);
        if(t1)
        {
            switch(ref[0])
            {
            /* Sections */
            case sec_ld[0]:
                t2 = str::find(ref, sec_rd);
                if(t2 && t2 < t1)
                {
                    /* Create new section, insert name */
                    csec = doc.newSection();
                    tname.clear();
                    tname.insert(
                        0, ref + 1, t2 - 1 - ref); /* Remove square brackets */
                    doc.insertSection(tname.c_str(), csec);
                }
                break;
                /* Comments */
            case del_ct[0]:
                cmtlines.push_back(ref);
                break;
                /* Values */
            default:
            {
                /* Find name-value delimiter */
                t2 = str::find(ref, del_vl);
                if(t2 && t2 < t1)
                {
                    /* Extract value name */
                    tname.clear();
                    tname.insert(0, ref, t2 - ref);
                    str::trim::both(tname);

                    /* Extract value, create variant */
                    cval = nullptr;
                    t2 += 1;
                    tvalu.clear();
                    tvalu.insert(0, t2, t1 - t2);
                    str::trim::both(tvalu);

                    cval = get_value(doc, tvalu.c_str());

                    /* If a section has been found previously, put it in there
                     */
                    if(csec /*&&cval*/)
                    {
                        csec->insertValue(tname.c_str(), cval);
                    } else if(cval)
                    {
                        doc.insertValue(tname.c_str(), cval);
                    }
                }
                break;
            }
            }
            ref = t1 + 1;
        }
    }

    return doc;
}

CString SimpleIniParser::Write(const SimpleIniParser::document_t& doc)
{
    const constexpr cstring linesep = "\n";

    CString outdata;

    CString t1;
    for(Pair<CString, variant_t*> const& v : doc.values())
    {
        PairToString(v, t1, outdata, linesep);
    }

    for(Pair<CString, section_t*> const& s : doc.sections())
    {
        if(outdata.size() > 0)
            outdata.append(linesep);
        outdata.append(cStringFormat("[{0}]{1}", s.first, linesep));
        for(Pair<CString, variant_t*> const& v : s.second->values())
        {
            PairToString(v, t1, outdata, linesep);
        }
    }

    return outdata;
}

void SimpleIniParser::PairToString(
    const Pair<CString, variant_t*>& v,
    CString&                         t1,
    CString&                         outdata,
    cstring                          linesep)
{
    t1.clear();
    switch(v.second->type)
    {
    case variant_t::Bool:
        t1 = cast_pod(v.second->getBool());
        break;
    case variant_t::Float:
        t1 = cast_pod(v.second->getFloat());
        break;
    case variant_t::Integer:
        t1 = cast_pod(v.second->getInteger());
        break;
    case variant_t::String:
    {
        cstring cs = v.second->getString();
        t1.insert(0, cs, str::len(cs));
        break;
    }
    }
    outdata.append(cStringFormat("{0} = {1}{2}", v.first, t1, linesep));
}

} // namespace CINI
} // namespace TextStorage
} // namespace DataStorage
} // namespace Coffee
