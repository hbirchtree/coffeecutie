#pragma once

#include "../types/tdef/integertypes.h"
#include "../types/tdef/stltypes.h"
#include "../plat/memory/stlstring_ops.h"
#include <coffee/core/CDebug>

namespace Coffee{

struct TablePrinter_Basic
{
    using ColSize = szptr;
    using ColumnSizes = Vector<ColSize>;
    using Column = Vector<CString>;
    using Header = Vector<CString>;
    using Table = Vector<Column>;

    template<typename T> STATICINLINE
    /*!
     * \brief Convert values to a column, formatting it as appropriate for the data
     * \param val
     * \param num
     * \return
     */
    Column GenColumn(T const* val, szptr num)
    {
        Column vec;
        for(szptr i=0;i<num;i++)
        {
            T const& ref = val[i];
            CString v = cStringFormat("{0}",ref);
            vec.push_back(v);
        }
        return vec;
    }

    template<typename T> STATICINLINE
    Column GenColumn(Vector<T> const& col)
    {
        Column vec;
        for(T const& v : col)
            vec.push_back(cStringFormat("{0}",v));
        return vec;
    }

    STATICINLINE
    /*!
     * \brief Generate a string containing a formatted and padded table
     * \param table
     * \return A string representing the input table
     */
    CString GenTable(Table const& table,Header const& head, CSize* size = nullptr)
    {
        const ColSize header_pad = 1;
        const byte_t newline = '\n';
        const byte_t row_sep = '#';
        const byte_t col_sep = '|';
        const byte_t pad_sep = ' ';

        ColumnSizes col_width;
        col_width.resize(table.size());

        ColSize acc = 0;
        ColSize row_count = table[0].size();

        for(ColSize i=0;i<table.size();i++)
        {
            Column const& col = table[i];
            ColSize& sz = col_width[i];
            sz = head[i].size();
            for(ColSize j=0;j<col.size();j++)
            {
                sz = CMath::max(col[j].size(),sz);
            }
            sz += header_pad;
            acc += sz;
        }

        acc += (table.size()-1)*2;

        std::stringstream ss;

        for(ColSize i=0;i<table.size();i++)
        {
            ss << head[i];
            ColSize pad = col_width[i]-head[i].size();
            for(ColSize j=0;j<pad;j++)
                ss << pad_sep;
            if(i<table.size()-1)
            {
                ss << col_sep;
                ss << pad_sep;
            }
        }

        ss << newline;

        for(ColSize i=0;i<acc;i++)
        {
            ss << row_sep;
        }

        ss << newline;

        for(ColSize i=0;i<row_count;i++)
        {
            for(ColSize j=0;j<table.size();j++)
            {
                CString const& str = table[j][i];
                ss << str;
                ColSize pad = col_width[j]-str.size();
                if(pad>0)
                for(ColSize k=0;k<pad;k++)
                    ss << pad_sep;
                if(j<table.size()-1)
                {
                    ss << col_sep;
                    ss << pad_sep;
                }
            }
            ss << newline;
        }

        if(size)
        {
            size->w = acc;
            size->h = row_count+2;
        }

        return ss.str();
    }
};

using Table = TablePrinter_Basic;

}