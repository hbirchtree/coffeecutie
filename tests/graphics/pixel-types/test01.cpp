#include <coffee/core/CUnitTesting>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/interfaces/cgraphics_pixops.h>

using namespace Coffee;

struct PixFmt_iterator
{
    PixFmt_iterator() : pixfmt_counter(C_CAST<u32>(PixFmt::None))
    {
        ++(*this);
    }

    PixFmt_iterator& operator++()
    {
        pixfmt_counter++;
        return *this;
    }

    operator bool()
    {
        return pixfmt_counter < C_CAST<u32>(PixFmt::MAX_PIXFMT);
    }

    operator PixFmt()
    {
        if(!*this)
            throw undefined_behavior("invalid enum value");

        return C_CAST<PixFmt>(pixfmt_counter);
    }

    u32 pixfmt_counter;
};

bool compressed_format_coverage()
{
    PixFmt_iterator fmt;
    while(fmt)
    {
        if(IsPixFmtCompressed(fmt))
        {
            /* TODO: Do something */
        }
        ++fmt;
    }

    return true;
}

bool regular_format_coverage()
{
    PixFmt_iterator fmt;

    while(fmt)
    {
        if(!IsPixFmtCompressed(fmt) && fmt != PixFmt::None)
        {
            auto sampleType = GetPixSampleType(fmt);

            if(sampleType == PixFlg::None)
                throw implementation_error("no sample type");

            auto component = GetPixComponent(fmt);
            auto bitFmt    = GetPreferredBitFmt(fmt);

            if(component == PixCmp::None)
                throw implementation_error("no sample type");

            GetPixSize(bitFmt, component, 16);
        }
        ++fmt;
    }

    return true;
}

COFFEE_TEST_SUITE(2) = {{compressed_format_coverage,
                         "Compressed format coverage",
                         "Checking compatibility with functions"},
                        {regular_format_coverage, "Regular format coverage"}};

COFFEE_EXEC_TESTS()
