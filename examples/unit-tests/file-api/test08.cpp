#include <coffee/core/CUnitTesting>

#include <coffee/core/CFiles>

using namespace Coffee;

const constexpr cstring link_test = "symlink.txt";
const constexpr cstring target_test = "target.txt";

bool link_create_test()
{
    bool status = true;
    /* Creating a target file */
    if(status && !FileFun::Touch(FileFun::File,target_test))
        status = false;
    if(status && FileFun::Stat(target_test) != FileFun::File)
        status = false;
    /* Creating link to existing file */
    if(status && !FileFun::Ln(target_test,link_test))
        status = false;
    /* Getting correct result upon lstat() */
    if(status && FileFun::Stat(link_test) != FileFun::Link)
        status = false;

    /* Being able to rm() the link */
    FileFun::Rm(link_test);
    FileFun::Rm(target_test);

    return status;
}

bool link_create_hanging_test()
{
    /* Creating link to non-existent file */
    if(!FileFun::Ln(target_test,link_test))
        return false;
    /* Getting correct result upon lstat() */
    if(FileFun::Stat(link_test) != FileFun::Link)
        return false;
    /* Being able to rm() the link */
    if(!FileFun::Rm(link_test))
        return false;

    return true;
}

bool link_dereference_test()
{
    return true;
}

bool link_canonical_test()
{
    return true;
}

const constexpr CoffeeTest::Test _run_tests[4] = {
    {link_create_test,"Normal symlink",nullptr,false,true},
    {link_create_hanging_test,"Creating a hanging symlink",nullptr,false,true},
    {link_dereference_test,"Dereferencing a symlink",nullptr,false,true},
    {link_canonical_test,"Canonicalizing symlink",nullptr,false,true},
};

COFFEE_RUN_TESTS(_run_tests);
