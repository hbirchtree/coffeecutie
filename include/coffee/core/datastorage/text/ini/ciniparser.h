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

    static Document Read(CResources::CResource const& source);
};

struct SimpleIniParser : IniParserDef
{
    struct Document
    {
        friend class SimpleIniParser;

        struct Variant
        {
            friend class Document;

            enum Type
            {
                Integer, /* Prepare for trouble with int size */
                Float,   /* Make it double */
                String,  /* To protect from lack of null-termination */
                Bool,    /* To denounce the evils of truth and false */
            };
            const Type type;

            int64 getInteger()
            {
                return v1;
            }
            bigscalar getFloat()
            {
                return v2;
            }
            cstring getString()
            {
                return v3.c_str();
            }
            bool getBool()
            {
                return v4;
            }

            int64& operator=(int64 const&v)
            {
                v1 = v;
            }
            bigscalar& operator=(bigscalar const&v)
            {
                v2 = v;
            }
            cstring operator=(cstring v)
            {
                v3 = v;
            }
            bool& operator=(bool v)
            {
                v4 = v;
            }

        protected:
            union
            {
                int64 v1;
                bigscalar v2;
                bool v4;
            };
            CString v3;

            Variant(int64 v):
                type(Integer),
                v1(v)
            {
            }
            Variant(bigscalar v):
                type(Float),
                v2(v)
            {
            }
            Variant(cstring v):
                type(String),
                v3(v)
            {
            }
            Variant(bool v):
                type(Bool),
                v4(v)
            {
            }
        };

        struct Section
        {
            friend class Document;

            void insertValue(cstring name, Variant* v)
            {
                m_values.insert(std::pair<CString,Variant*>(name,v));
            }
            Variant* value(cstring name)
            {
                auto it = m_values.find(name);
                if(it==m_values.end())
                    return nullptr;
                return it->second;
            }

        protected:
            Section()
            {
            }
            MultiMap<CString,Variant*> m_values;
        };

        /* Section creation */
        Section* newSection()
        {
            sections.push_front(Section());
            return &sections.front();
        }

        /* Value creation */
        Variant* newInteger(int64 v)
        {
            variantvalues.push_front(Variant(v));
            return &variantvalues.front();
        }
        Variant* newFloat(bigscalar v)
        {
            variantvalues.push_front(Variant(v));
            return &variantvalues.front();
        }
        Variant* newBool(bool v)
        {
            variantvalues.push_front(Variant(v));
            return &variantvalues.front();
        }
        Variant* newString(cstring v)
        {
            variantvalues.push_front(Variant(v));
            return &variantvalues.front();
        }

        /* Section insertion/extraction */
        void insertSection(cstring name, Section* s)
        {
            valuesections.insert(std::pair<CString,Section*>(name,s));
        }
        Section* section(cstring name)
        {
            auto it = valuesections.find(name);
            if(it==valuesections.end())
                return nullptr;
            return it->second;
        }
        /* Global values */
        void insertValue(cstring name, Variant* v)
        {
            valuevariants.insert(std::pair<CString,Variant*>(name,v));
        }
        Variant* value(cstring name)
        {
            auto it = valuevariants.find(name);
            if(it==valuevariants.end())
                return nullptr;
            return it->second;
        }

    protected:
        LinkList<CString> string_store; /* Store strings here! */

        LinkList<Section> sections;
        LinkList<Variant> variantvalues;

        MultiMap<CString,Variant*> valuevariants;
        MultiMap<CString,Section*> valuesections;
    };

    using Section = Document::Section*;
    using Variant = Document::Variant*;

    STATICINLINE Document Read(CResources::CResource const& source, bool unixmode = true)
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
        Document doc;

        /* Find all sections, global values and comments */
        Document::Section* csec = nullptr; /* Current section */
        CString tname;
        CString tvalu;
        Document::Variant* cval = nullptr;
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
                                if(tvalu=="false")
                                    btype = false;
                                else if(tvalu=="true")
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
    STATICINLINE bool Write(Document const& doc,CResources::CResource& target)
    {
        return false;
    }
};

}
}
}

using INI = CDataStorage::CTextStorage::CINI::SimpleIniParser;

}

#endif
