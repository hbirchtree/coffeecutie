#include <coffee/core/CFiles>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>

#include <coffee/core/CUnitTesting>

using namespace Coffee;

const Url link_test =
    MkUrl("symlink.txt", RSCA::TemporaryFile | RSCA::NoDereference);
const Url target_test = MkUrl("target.txt", RSCA::TemporaryFile);

bool link_create_test()
{
    file_error ec;

    FileFun::Rm(target_test, ec);
    FileFun::Rm(link_test, ec);

    /* Creating a target file */
    assertTrue(FileFun::Touch(FileType::File, target_test, ec));
    assertEquals(FileFun::Stat(target_test, ec), FileType::File);
    /* Creating link to existing file */
    assertTrue(FileFun::Ln(target_test, link_test, ec));
    /* Getting correct result upon lstat() */
    assertEquals(FileFun::Stat(link_test, ec), FileType::Link);

    /* Verify that links are deletable */
    assertTrue(FileFun::Rm(link_test, ec));

    FileFun::Rm(target_test, ec);
    FileFun::Rm(link_test, ec);

    return true;
}

bool link_create_hanging_test()
{
    file_error ec;

    /* Creating link to non-existent file */
    assertTrue(FileFun::Ln(target_test, link_test, ec));
    /* Getting correct result upon lstat() */
    assertEquals(FileFun::Stat(link_test, ec), FileType::Link);

    /* Symlinks are verified deletable from last test */
    FileFun::Rm(link_test, ec);

    return true;
}

bool link_dereference_test()
{
    file_error ec;

    assertTrue(FileFun::Ln(target_test, link_test, ec));

    auto gotPath = FileFun::DereferenceLink(link_test, ec);

    assertEquals(gotPath, *target_test);

    /* Symlinks are verified deletable from last test */
    FileFun::Rm(link_test, ec);

    return true;
}

bool link_canonical_test()
{
    file_error ec;

    FileFun::Rm(target_test, ec);
    FileFun::Rm(link_test, ec);

    /* Create link to file */
    assertTrue(FileFun::Touch(FileType::File, target_test, ec));
    assertTrue(FileFun::Ln(target_test, link_test, ec));

    /* Grab canonical name, should point to target file */
    auto gotPath = FileFun::CanonicalName(link_test, ec);

    assertEquals(gotPath, *target_test);

    /* Symlinks are verified deletable from last test */
    assertTrue(FileFun::Rm(link_test, ec));
    assertTrue(FileFun::Rm(target_test, ec));

    return true;
}

COFFEE_TESTS_BEGIN(4)

    {link_create_test, "Normal symlink", nullptr, false, true},
    {link_create_hanging_test,
     "Creating a hanging symlink",
     nullptr,
     false,
     true},
    {link_dereference_test, "Dereferencing a symlink", nullptr, false, true},
    {link_canonical_test, "Canonicalizing symlink", nullptr, false, true}

COFFEE_TESTS_END()
