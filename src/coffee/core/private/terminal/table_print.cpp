#include <coffee/core/terminal/table-print.h>
#include <coffee/core/types/cdef/geometry.h>
#include <coffee/core/CMath>

namespace Coffee{

CString TablePrinter_Basic::GenTable(const TablePrinter_Basic::Table &table, const TablePrinter_Basic::Header &head, CSize *size)
{
#ifndef COFFEE_LOWFAT
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
        size->w = C_FCAST<i32>(acc);
        size->h = C_FCAST<i32>(row_count+2);
    }

    return ss.str();
#else
    return {};
#endif
}

}
