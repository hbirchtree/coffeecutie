#ifndef COFFEE_CORE_DATASTORAGE_TEXT_INI_H
#define COFFEE_CORE_DATASTORAGE_TEXT_INI_H

#include <coffee/core/CFiles>
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace CDataStorage{
namespace CTextStorage{
namespace CINI{

struct IniParserDef
{
    struct Document;

    static Document Read(CResources::Resource const& source);
};

struct SimpleIniParser : IniParserDef
{
    class document_t;
    class section_t;

    struct variant_t
    {
	friend class document_t;

	enum Type
	{
	    Integer, /* Prepare for trouble with int size */
	    Float,   /* Make it double */
	    String,  /* To protect from lack of null-termination */
	    Bool,    /* To denounce the evils of truth and false */
	};
	const Type type;

	int64 getInteger() const
	{
	    return v1;
	}
	bigscalar getFloat() const
	{
	    return v2;
	}
	cstring getString() const
	{
	    return v3.c_str();
	}
	bool getBool() const
	{
	    return v4;
	}

	int64& operator=(int64 const&v)
	{
            return v1 = v;
	}
	bigscalar& operator=(bigscalar const&v)
	{
            return v2 = v;
	}
	cstring operator=(cstring v)
	{
            v3 = v;
            return v3.c_str();
	}
	bool& operator=(bool v)
	{
            return v4 = v;
	}

    protected:
	union
	{
	    int64 v1;
	    bigscalar v2;
	    bool v4;
	};
	CString v3;

	variant_t(int64 v):
	    type(Integer),
	    v1(v)
	{
	}
	variant_t(bigscalar v):
	    type(Float),
	    v2(v)
	{
	}
	variant_t(cstring v):
	    type(String),
	    v3(v)
	{
	}
	variant_t(bool v):
	    type(Bool),
	    v4(v)
	{
	}
    };

    struct section_t
    {
	friend class document_t;

	void insertValue(cstring name, variant_t* v)
	{
	    m_values.insert(std::pair<CString,variant_t*>(name,v));
	}
	variant_t* value(cstring name)
	{
	    auto it = m_values.find(name);
	    if(it==m_values.end())
		return nullptr;
	    return it->second;
	}

	MultiMap<CString,variant_t*> const& values() const
	{
	    return m_values;
	}

    protected:
	section_t()
	{
	}
	MultiMap<CString,variant_t*> m_values;
    };

    struct document_t
    {
	friend class SimpleIniParser;

        /* Section creation */
	section_t* newSection()
        {
	    section_containment.push_front(section_t());
	    return &section_containment.front();
        }

        /* Value creation */
	variant_t* newInteger(int64 v)
        {
	    variant_containment.push_front(variant_t(v));
	    return &variant_containment.front();
        }
	variant_t* newFloat(bigscalar v)
        {
	    variant_containment.push_front(variant_t(v));
	    return &variant_containment.front();
        }
	variant_t* newBool(bool v)
        {
	    variant_containment.push_front(variant_t(v));
	    return &variant_containment.front();
        }
	variant_t* newString(cstring v)
        {
	    variant_containment.push_front(variant_t(v));
	    return &variant_containment.front();
        }

        /* Section insertion/extraction */
	void insertSection(cstring name, section_t* s)
        {
	    m_section_map.insert(std::pair<CString,section_t*>(name,s));
        }
	section_t* section(cstring name)
        {
	    auto it = m_section_map.find(name);
	    if(it==m_section_map.end())
                return nullptr;
            return it->second;
        }
        /* Global values */
	void insertValue(cstring name, variant_t* v)
        {
	    m_variant_map.insert(std::pair<CString,variant_t*>(name,v));
        }
	variant_t* value(cstring name)
        {
	    auto it = m_variant_map.find(name);
	    if(it==m_variant_map.end())
                return nullptr;
            return it->second;
        }

	MultiMap<CString,variant_t*> const& values() const
	{
	    return m_variant_map;
	}
	MultiMap<CString,section_t*> const& sections() const
	{
	    return m_section_map;
	}

    protected:
	LinkList<section_t> section_containment;
	LinkList<variant_t> variant_containment;

	MultiMap<CString,variant_t*> m_variant_map;
	MultiMap<CString,section_t*> m_section_map;
    };

    STATICINLINE document_t Read(CResources::Resource const& source,
			       bool unixmode = true)
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
        cstring data = (cstring)source.data;
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
	t2 = nullptr,t3 = nullptr,t4 = nullptr;
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
                                bool btype;
				if(tvalu=="false"||tvalu=="False")
                                    btype = false;
				else if(tvalu=="true"||tvalu=="True")
                                    btype = true;
                                else
                                    eval = false;
                                if(eval)
                                {
                                    cval = doc.newBool(btype);
                                }else
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


    STATICINLINE bool Write(document_t const& doc,CResources::Resource& target)
    {
	const constexpr cstring linesep = "\n";

	CString outdata;

	CString t1;
	for(std::pair<CString,variant_t*> const& v : doc.values())
	{
	    PairToString(v,t1,outdata,linesep);
	}

	for(std::pair<CString,section_t*> const& s : doc.sections())
	{
	    if(outdata.size() > 0)
		outdata.append(linesep);
	    outdata.append(cStringFormat("[{0}]{1}",s.first,linesep));
	    for(std::pair<CString,variant_t*> const& v : s.second->values())
	    {
		PairToString(v,t1,outdata,linesep);
	    }
	}

	target.size = outdata.size();
	target.data = Alloc(target.size);
	MemCpy(target.data,&outdata[0],outdata.size());

	return true;
    }

    using Document = document_t;
    using Variant = variant_t*;
    using Section = section_t*;

protected:
    STATICINLINE void PairToString(
	    std::pair<CString,variant_t*> const& v,
	    CString& t1, CString& outdata, cstring linesep)
    {
	t1.clear();
	switch(v.second->type)
	{
	case variant_t::Bool:
	    t1 = Convert::booltostring(v.second->getBool());
	    break;
	case variant_t::Float:
	    t1 = Convert::scalartostring(v.second->getFloat());
	    break;
	case variant_t::Integer:
	    t1 = Convert::inttostring(v.second->getInteger());
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

};

}
}
}

using INI = CDataStorage::CTextStorage::CINI::SimpleIniParser;

}

#endif
