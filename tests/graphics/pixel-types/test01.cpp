#include <coffee/core/CUnitTesting>
#include <coffee/core/types/edef/pixenum.h>

using namespace Coffee;

struct PixFmt_iterator
{
    PixFmt_iterator():
        pixfmt_counter(C_CAST<u32>(PixFmt::None))
    {
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


    return true;
}

bool regular_format_coverage()
{
    return true;
}

COFFEE_TEST_SUITE(2) = {{compressed_format_coverage,
                         "Compressed format coverage",
                         "Whether all compressed formats are covered by "
                         "functions relating to their properties"},
                        {regular_format_coverage, "Regular format coverage"}};

COFFEE_EXEC_TESTS()
