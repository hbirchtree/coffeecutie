#ifndef COFFEE_CORE_DATASTORAGE_TEXT_INI_H
#define COFFEE_CORE_DATASTORAGE_TEXT_INI_H

#include <coffee/core/CFiles>
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace DataStorage{
namespace TextStorage{
namespace CINI{

struct IniParserDef
{
    struct Document;

    static Document Read(CResources::Resource const& source);
};

struct SimpleIniParser : IniParserDef
{
    struct document_t;
    struct section_t;

    struct variant_t
    {
        friend struct document_t;

        enum Type
        {
            Integer, /* Prepare for trouble with int size */
            Float,   /* Make it double */
            String,  /* To protect from lack of null-termination */
            Bool,    /* ... */
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
        friend struct SimpleIniParser;

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

    static document_t Read(CResources::Resource const& source, bool unixmode = true);


    static bool Write(document_t const& doc,CResources::Resource& target);

    using Document = document_t;
    using Variant = variant_t*;
    using Section = section_t*;

protected:
    static void PairToString(std::pair<CString,variant_t*> const& v,
                             CString& t1, CString& outdata, cstring linesep);

};

}
}
}

using INI = DataStorage::TextStorage::CINI::SimpleIniParser;

}

#endif
