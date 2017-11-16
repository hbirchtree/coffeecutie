#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>


using namespace Coffee;

const Url link_test = MkUrl("symlink.txt",
                            ResourceAccess::SpecifyStorage
                            |ResourceAccess::TemporaryFile);
const Url target_test = MkUrl("target.txt",
                              ResourceAccess::SpecifyStorage
                              |ResourceAccess::TemporaryFile);

bool link_create_test()
{
    bool status = true;
    /* Creating a target file */
    if(!FileFun::Touch(FileFun::File,target_test))
        status = false;
    if(status && FileFun::Stat(target_test) != FileFun::File)
        status = false;
    /* Creating link to existing file */
    if(status && !FileFun::Ln(target_test,link_test))
        status = false;
    /* Getting correct result upon lstat() */
    if(status && FileFun::Stat(link_test) != FileFun::Link)
        status = false;

    /* Verify that links are deletable */
    status = status && FileFun::Rm(link_test);
    FileFun::Rm(target_test);
    FileFun::Rm(link_test);

    return status;
}

bool link_create_hanging_test()
{
    bool status = true;
    /* Creating link to non-existent file */
    if(!FileFun::Ln(target_test,link_test))
        status = false;
    /* Getting correct result upon lstat() */
    if(FileFun::Stat(link_test) != FileFun::Link)
        status = false;

    /* Symlinks are verified deletable from last test */
    FileFun::Rm(link_test);

    return status;
}

bool link_dereference_test()
{
    bool status = true;

    if(!FileFun::Ln(target_test,link_test))
        status =  false;

    auto gotPath = FileFun::DereferenceLink(link_test);

    if(status && gotPath != *target_test)
        status = false;

    /* Symlinks are verified deletable from last test */
    FileFun::Rm(link_test);

    return status;
}

bool link_canonical_test()
{
    bool status = true;

    FileFun::Touch(FileFun::File,target_test);
    if(!FileFun::Ln(target_test,link_test))
        status =  false;

    auto gotPath = FileFun::CanonicalName(link_test);

    if(status && gotPath != *target_test)
    {
        cWarning("Canonical name test:\n {0} != {1}",
                 FileFun::CanonicalName(link_test),
                 target_test);
        status = false;
    }

    /* Symlinks are verified deletable from last test */
    FileFun::Rm(link_test);
    FileFun::Rm(target_test);

    return status;
}

const constexpr CoffeeTest::Test _run_tests[4] = {
    {link_create_test,"Normal symlink",nullptr,false,true},
    {link_create_hanging_test,"Creating a hanging symlink",nullptr,false,true},
    {link_dereference_test,"Dereferencing a symlink",nullptr,false,true},
    {link_canonical_test,"Canonicalizing symlink",nullptr,false,true},
};

COFFEE_RUN_TESTS(_run_tests);