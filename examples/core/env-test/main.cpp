#include <coffee/core/CApplication>
#include <coffee/core/CPlatform>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>

using namespace Coffee;

int32 coffee_main(int32, cstring_w*)
{
    CResources::FileResourcePrefix("sample_data/");

    Profiler::PushContext("Configuration data");
    {
        /* Check out system directory strings and user data directories */
        CString cfg_dir  = Env::GetUserData(
                    "hbirchtree",
                    "Best Coffee of All Time");
        Profiler::Profile("Get userdata directory");

        CString app_dir  = Env::ApplicationDir();
        CString exe_name = Env::ExecutableName();
        Profiler::Profile("Get application location");

        cDebug("Settings directory: {0}",cfg_dir);
        cDebug("Program directory:  {0}",app_dir);
        cDebug("Launching from      {0}",exe_name);
		cDebug("Current directory:  {0}",Env::CurrentDir());

        Profiler::Profile("Print some data");

        if(!CResources::FileMkdir(cfg_dir.c_str(),true))
            cWarning("Failed to create settings directory");
        else
        {
            CString test_file = Env::ConcatPath(cfg_dir.c_str(),"test_file.sav");
            FileFun::Touch(FileFun::File,test_file.c_str());
        }
        Profiler::Profile("Create directory recursively");
    }
    Profiler::PopContext();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
