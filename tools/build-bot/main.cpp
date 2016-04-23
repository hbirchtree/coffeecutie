#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CMD>
#include <coffee/core/CArgParser>
#include <coffee/core/CXmlParser>
#include <coffee/core/CJSONParser>
#include <coffee/CAsio>

using namespace Coffee;

enum FailureCase
{
    Nothing,

    NoRepoSpecified = 20,
    FeedFetchFailed = 21,

    ProcessFailed   = 22,
    XMLParseFailed  = 23,
};

enum UpdateSource
{
    GithubRSS = 1,
};

enum BuildTypes
{
    CMakeSystem = 1,
};

struct Repository
{
    CString repository;
    CString branch;

    CString build;
    CString repodir;

    Vector<Proc_Cmd> command_queue;

    uint64 interval;
};

struct Repository_tmp
{
    REST::Request request;
    REST::RestResponse response;
    CString expect_type;

    Timestamp wakeup;
};

struct DataSet
{
    Repository repo;
    Repository_tmp temp;
};

DataSet create_item(cstring file)
{
    DataSet repo;

    repo.repo.branch = "master";
    repo.repo.interval = 3600;
    repo.repo.repodir = Env::CurrentDir();
    repo.repo.build = Env::CurrentDir();

    CResources::Resource resc(file);
    CResources::FileMap(resc);

    if(!resc.data)
        return {};

    CString data_string;
    data_string.insert(0,(cstring)resc.data,resc.size);

    CResources::FileUnmap(resc);

    CString build_sys = "cmake";

    JSON::Document doc = JSON::Read(data_string.c_str());

    if(doc.HasMember("repository"))
        repo.repo.repository = doc["repository"].GetString();
    if(doc.HasMember("branch"))
        repo.repo.branch = doc["branch"].GetString();
    if(doc.HasMember("build-dir"))
        repo.repo.build = doc["build-dir"].GetString();
    if(doc.HasMember("repo-dir"))
        repo.repo.repodir = doc["repo-dir"].GetString();
    if(doc.HasMember("interval"))
        repo.repo.interval = doc["interval"].GetUint64();
    if(doc.HasMember("build-type"))
        build_sys = doc["build-type"].GetString();

    if(build_sys == "cmake")
    {
        repo.repo.command_queue.push_back({"git",{"pull"},{}});
        repo.repo.command_queue.push_back({"cmake",{"--build",repo.repo.build.c_str()},{}});
    }else{
        cWarning("Unrecognized build system: {0}",build_sys);
    }

    repo.temp.expect_type = "application/atom+xml; charset=utf-8";
    repo.temp.request = cStringFormat("/{0}/commits/{1}.atom",
                                      repo.repo.repository,repo.repo.branch);

    cBasicPrint("\nConfigured target:\n"
                "repository = {0}\n"
                "branch = {5}\n"
                "repository-dir = {1}"
                "build-dir = {2}\n"
                "interval = {3}\n"
                "build-system = {4}\n",
                repo.repo.repository,
                repo.repo.repodir,
                repo.repo.build,
                repo.repo.interval,
                build_sys,
                repo.repo.branch);

    return repo;
}

FailureCase update_item(Repository const& data, Repository_tmp* workarea)
{
    if(Time::CurrentTimestamp() < workarea->wakeup)
        return Nothing;

    uint64 const& interval = data.interval;
    REST::Request const& request = workarea->request;
    CString const& expect_type = workarea->expect_type;

    CString repo = data.repository;
    Vector<Proc_Cmd> const command_queue = data.command_queue;

    cBasicPrint("WORKING ON: {0}",repo);

    auto response = REST::RestRequest(REST::HTTPS,"github.com",request);

    if(response.code != 200)
        return FeedFetchFailed;

    if(REST::GetContentType(response) == expect_type)
    {
        cBasicPrint("Updated repository: {0}",repo);
        CString log;
        for(Proc_Cmd const& cmd : command_queue)
            if(Proc::ExecuteLogged(cmd,&log) != 0)
            {
                cBasicPrint("Failed with:\n{0}",log);
                return ProcessFailed;
            }
    }else{
        cWarning("Content mismatch: \"{0}\", expected \"{1}\"",
                 REST::GetContentType(response),
                 expect_type);
        return XMLParseFailed;
    }

    workarea->wakeup = Time::CurrentTimestamp()+interval;

    cBasicPrint("Sleeping again, waking up at {0}",
           Time::LocalTimeString(workarea->wakeup));

    cBasicPrint("");

    return Nothing;
}

int32 coffee_main(int32 argc, cstring_w* argv)
{
    argc--;
    argv = &argv[1];

    Vector<DataSet> datasets;
    Vector<DataSet> complaints;

    for(int32 i=0;i<argc;i++)
    {
        datasets.push_back(create_item(argv[i]));
    }

    if(datasets.size() == 0)
        return 0;

    REST::InitService();

    while(true)
    {
        for(DataSet& e : datasets)
        {
            if(update_item(e.repo,&e.temp)!=Nothing)
                return 1;
        }
        Threads::sleepMillis(250);
    }
}

COFFEE_APPLICATION_MAIN(coffee_main);
