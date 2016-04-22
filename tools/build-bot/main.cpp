#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CMD>
#include <coffee/core/CArgParser>
#include <coffee/CAsio>

using namespace Coffee;

enum FailureCase
{
    Nothing,

    NoRepoSpecified  = 0x20,
    FeedFetchFailed = 0x21,

};

int32 coffee_main(int32 argc, cstring_w* argv)
{
    cstring builddir_ = ArgParse::Get(argc,argv,"builddirectory");
    cstring repodir_ = ArgParse::Get(argc,argv,"directory");
    cstring cmd_ = ArgParse::Get(argc,argv,"command");
    cstring timer_ = ArgParse::Get(argc,argv,"interval");

    cstring repo_ = ArgParse::Get(argc,argv,"repo");
    cstring branch_ = ArgParse::Get(argc,argv,"branch");

    CString builddir,repodir,cmd,repo,branch;
    uint64 interval = 3600;

    /* Specify build directory */
    if(!builddir_)
	builddir = Env::CurrentDir();
    else
	builddir = builddir_;
    if(!repodir_)
	repodir = Env::CurrentDir();
    else
	repodir = repodir_;
    /* Specify build command */
    if(!cmd_)
	cmd = cStringFormat("cmake --build {0}",builddir,repodir);
    else{
	CString tmp = cStringFormat("{0}",cmd_);
	cmd = cStringFormat(tmp.c_str(),builddir,repodir);
    }

    /* Specify repository (/user/repo) */
    if(!repo_)
	return NoRepoSpecified;
    else
	repo = repo_;
    /* Specify branch */
    if(!branch_)
	branch = "master";
    else
	branch = branch_;

    if(timer_)
    {
        bool ok = false;
        uint64 t__ = Convert::strtoull(timer_,10,&ok);
        if(ok)
            interval = t__;
    }

    cBasicPrint("command={0},directory={1},interval={2}",cmd,builddir,interval);

    REST::InitService();

    REST::Request request = cStringFormat("/{0}/commits/{1}.atom",repo,branch);

    while(true)
    {
        auto response = REST::RestRequest(REST::HTTPS,"github.com",request);

	if(response.code != 200)
	    return FeedFetchFailed;

        if(REST::GetContentType(response) == "application/atom+xml; charset=utf-8")
        {
            cDebug("Updated repository: {0}");
	    Proc::Execute({cmd.c_str()});
        }

        cDebug("Sleeping again, waking up at {0}",
               Time::LocalTimeString(Time::CurrentTimestamp()+interval*1000));
        Threads::sleepMillis(interval*1000);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main);
