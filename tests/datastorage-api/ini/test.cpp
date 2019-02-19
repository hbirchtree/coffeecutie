#include <coffee/core/CIniParser>
#include <coffee/core/CUnitTesting>

using namespace Coffee;

static const cstring test_ini_doc =
    R"(
[Header1]
Key1 = Value 1
Key2 = 2
Key3 = true
Key4 = FALSE
)";

bool readwrite_file()
{
    auto testFile     = MkUrl("test.ini", RSCA::TemporaryFile);
    auto testFileDupe = MkUrl("test.ini.ini", RSCA::TemporaryFile);

    Resource testfile(testFile);

    testfile = Bytes::CreateString(test_ini_doc);

    FileCommit(testfile, RSCA::WriteOnly | RSCA::Discard | RSCA::NewFile);

    FileMap(testfile);
    Profiler::Profile("Mapping time");

    INI::Document doc2 = INI::Read(testfile);
    Profiler::Profile("Reading time");

    Resource rsc(testFileDupe);
    CString  docData = INI::Write(doc2);
    rsc              = Bytes::CreateString(docData.c_str());

    FileCommit(rsc, RSCA::Discard | RSCA::NewFile);
    Profiler::Profile("Write-back");

    FileUnmap(testfile);
    Profiler::Profile("Unmapping");
    return true;
}

/*!
 * \brief Compound test; both writing a file and creating a document.
 * \return
 */
bool write_file()
{
    file_error ec;
    Url        testfile = MkUrl("testoutfile.ini", RSCA::TemporaryFile);

    INI::Document doc;

    INI::Section t1 = doc.newSection();
    INI::Section t2 = doc.newSection();

    doc.insertSection("Test", t1);
    doc.insertSection("Rest", t2);

    INI::Variant v1 = doc.newString("Hello test string");
    INI::Variant v2 = doc.newBool(false);
    INI::Variant v3 = doc.newInteger(100);

    t1->insertValue("hello1", v1);
    t1->insertValue("hello2", v2);
    t1->insertValue("hello3", v3);

    Profiler::Profile("Creating and setting values");

    Resource rsc(testfile);
    Profiler::Profile("File object");
    CString docData = INI::Write(doc);
    rsc             = Bytes::CreateString(docData.c_str());
    Profiler::Profile("Writing object to file");
    FileCommit(rsc, RSCA::WriteOnly | RSCA::Discard | RSCA::NewFile);
    Profiler::Profile("Committing file");

    auto res = FileFun::Exists(testfile, ec);

    return res;
}

COFFEE_TESTS_BEGIN(2)

    {readwrite_file, "Reading INI file from disk"},
    {write_file, "Writing INI document to disk"}

COFFEE_TESTS_END()
