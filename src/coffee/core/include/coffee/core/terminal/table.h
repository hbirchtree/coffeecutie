#pragma once

#include <coffee/core/CDebug>
#include <coffee/core/stl_types.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace Coffee {

struct TablePrinter_Basic
{
    using ColSize     = szptr;
    using ColumnSizes = Vector<ColSize>;
    using Column      = Vector<CString>;
    using Header      = Vector<CString>;
    class Table : public Vector<Column>
    {
        Header const& m_header;

      public:
        Table(Header const& head) : Vector<Column>(), m_header(head)
        {
        }

        operator CString() const
        {
            return GenTable(*this, m_header);
        }
    };

    template<typename T>
    STATICINLINE
        /*!
         * \brief Convert values to a column, formatting it as appropriate for
         * the data \param val \param num \return
         */
        Column
        GenColumn(T const* val, szptr num)
    {
        Column vec;
        for(szptr i = 0; i < num; i++)
        {
            T const& ref = val[i];
            CString  v   = cStringFormat("{0}", ref);
            vec.push_back(v);
        }
        return vec;
    }

    template<typename T>
    STATICINLINE Column GenColumn(Vector<T> const& col)
    {
        Column vec;
        for(T const& v : col)
            vec.push_back(cStringFormat("{0}", v));
        return vec;
    }

    /*!
     * \brief Generate a string containing a formatted and padded table
     * \param table
     * \return A string representing the input table
     */
    static CString GenTable(
        Table const& table, Header const& head, CSize* size = nullptr);
};

using Table = TablePrinter_Basic;

} // namespace Coffee
