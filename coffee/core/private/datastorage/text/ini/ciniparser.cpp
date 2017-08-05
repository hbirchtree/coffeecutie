#include <coffee/core/datastorage/text/ini/ciniparser.h>

#include <coffee/core/string_casting.h>
#include <coffee/core/CDebug>

namespace Coffee{
namespace DataStorage{
namespace TextStorage{
namespace CINI{

SimpleIniParser::document_t SimpleIniParser::Read(const CResources::Resource &source, bool unixmode)
{
    const constexpr cstring sec_ld = "[";
    const constexpr cstring sec_rd = "]";
    const constexpr cstring del_ct = ";";
    const constexpr cstring del_vl = "=";

    cstring t1,t2,t3,t4; /* Temporaries */

    cstring linesep = "\n";
    if(!unixmode)
        linesep = "\r";

    /* Retrieve data */
    cstring data = C_CAST<cstring>(source.data);
    cstring ref = data;
    cstring end = data+source.size;

    /* We populate the document on-the-go */
    document_t doc;

    /* Find all sections, global values and comments */
    section_t* csec = nullptr; /* Current section */
    CString tname;
    CString tvalu;
    variant_t* cval = nullptr;
    Vector<cstring> cmtlines;
    t2 = nullptr; t3 = nullptr; t4 = nullptr;
    while(ref&&ref<end)
    {
        t1 = StrFind(ref,linesep);
        if(t1)
        {
            switch(ref[0])
            {
            /* Sections */
            case sec_ld[0]:
                t2 = StrFind(ref,sec_rd);
                if(t2&&t2<t1)
                {
                    /* Create new section, insert name */
                    csec = doc.newSection();
                    tname.clear();
                    tname.insert(0,ref+1,t2-1-ref); /* Remove square brackets */
                    doc.insertSection(tname.c_str(),csec);
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
                t2 = StrFind(ref,del_vl);
                if(t2&&t2<t1)
                {
                    /* Extract value name */
                    tname.clear();
                    tname.insert(0,ref,t2-ref);
                    StrUtil::trim(tname);

                    /* Extract value, create variant */
                    cval = nullptr;
                    t2 += 1;
                    tvalu.clear();
                    tvalu.insert(0,t2,t1-t2);
                    StrUtil::trim(tvalu);
                    bool eval;

                    int64 itype = Convert::strtoll(tvalu.c_str(),10,&eval);
                    if(eval)
                    {
                        cval = doc.newInteger(itype);
                    }else
                    {
                        bigscalar dtype = Convert::strtoscalar(tvalu.c_str(),&eval);
                        if(eval)
                        {
                            cval = doc.newFloat(dtype);
                        }else{
                            eval = true;
                            bool btype = Convert::strtobool(tvalu.c_str(),&eval);
                            if(eval)
                                cval = doc.newBool(btype);
                            else
                                cval = doc.newString(tvalu.c_str());
                        }
                    }

                    /* If a section has been found previously, put it in there */
                    if(csec/*&&cval*/)
                    {
                        csec->insertValue(tname.c_str(),cval);
                    }else if(cval){
                        doc.insertValue(tname.c_str(),cval);
                    }
                }
                break;
            }
            }
            ref = t1+1;
        }
    }

    return doc;
}

bool SimpleIniParser::Write(const SimpleIniParser::document_t &doc, CResources::Resource &target)
{
    const constexpr cstring linesep = "\n";

    CString outdata;

    CString t1;
    for(Pair<CString,variant_t*> const& v : doc.values())
    {
        PairToString(v,t1,outdata,linesep);
    }

    for(Pair<CString,section_t*> const& s : doc.sections())
    {
        if(outdata.size() > 0)
            outdata.append(linesep);
        outdata.append(cStringFormat("[{0}]{1}",s.first,linesep));
        for(Pair<CString,variant_t*> const& v : s.second->values())
        {
            PairToString(v,t1,outdata,linesep);
        }
    }

    target.size = outdata.size();
    target.data = Alloc(target.size);
    MemCpy(target.data,&outdata[0],outdata.size());

    return true;
}

void SimpleIniParser::PairToString(const Pair<CString, variant_t *> &v,
                                   CString& t1, CString& outdata, cstring linesep)
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
        t1.insert(0,cs,StrLen(cs));
        break;
    }
    }
    outdata.append(cStringFormat("{0} = {1}{2}",v.first,t1,linesep));
}

}
}
}
}
