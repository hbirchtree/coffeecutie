#include <coffee/CAsio>
#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CMD>
#include <coffee/core/CArgParser>
#include <coffee/core/CXmlParser>
#include <coffee/core/CJSONParser>

using namespace Coffee;

cstring git_program = "git";
cstring cmake_program = "cmake";

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

enum BuildSpecs
{
	IgnoreFailure = 0x1,
	CleanAlways   = 0x2,
};

struct GitCommit
{
    CString hash;
    Timestamp ts;
};

bool operator<(GitCommit const& c1, GitCommit const& c2)
{
    return c1.ts < c2.ts && c1.hash != c2.hash;
}

struct Repository
{
    CString repository;
    CString branch;

    CString build;
    CString repodir;

    CString upstream;

    Vector<Proc_Cmd> command_queue;

    uint64 interval;

	uint32 flags;
};

struct Repository_tmp
{
    REST::Request request;
    REST::RestResponse response;
    CString expect_type;

    GitCommit last_commit;

    Timestamp wakeup;
};

struct DataSet
{
    Repository repo;
    Repository_tmp temp;
};

GitCommit ParseEntry(XML::Element const* el)
{
    if(!el)
	return {};

    XML::Element const* cts = el->FirstChildElement("updated");
    XML::Element const* cid = el->FirstChildElement("id");

    if(!cts || !cid)
	return {};

    Timestamp ts = Time::ParseTimeStdTime(cts->GetText());
    Regex::Pattern patt = Regex::Compile(".+Commit\\/([0-9a-fA-F]+)");

    auto cap = Regex::Match(patt,cid->GetText(),true);

    if(cap.size() < 2)
	return {};

    return {cap[1].s_match[0],ts};
}

DataSet create_item(cstring file)
{
    DataSet repo = {};

    repo.temp = {};
    repo.temp.last_commit = {};

    repo.repo.branch = "master";
    repo.repo.upstream = "origin";
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
    if(doc.HasMember("upstream"))
	repo.repo.upstream = doc["upstream"].GetString();
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

    cDebug("Upstream: {0}",repo.repo.upstream);
    cDebug("Branch: {0}",repo.repo.branch);

	Vector<Proc_Cmd>& cmd_queue = repo.repo.command_queue;

    if(build_sys == "cmake")
    {
		cstring upstream = repo.repo.upstream.c_str();
		cstring branch = repo.repo.branch.c_str();
		cstring build_dir = repo.repo.build.c_str();

	cmd_queue.push_back({git_program,{"pull",upstream,branch},{}});
	cmd_queue.push_back({ cmake_program,{ "--clean",},{} });
	cmd_queue.push_back({cmake_program,{"--build",build_dir},{}});
    }else{
	cWarning("Unrecognized build system: {0}",build_sys);
    }

    repo.temp.expect_type = "application/atom+xml; charset=utf-8";
    repo.temp.request = cStringFormat("/{0}/commits/{1}.atom",
				      repo.repo.repository,repo.repo.branch);

    cBasicPrint("\nConfigured target:\n"
		"repository = {0}\n"
		"branch = {5}\n"
		"repository-dir = {1}\n"
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
	XML::Document* doc = XML::XMLRead(
		    BytesConst{response.payload.size(),(byte_t*)response.payload.c_str()});

	XML::Element* feed = doc->FirstChildElement("feed");

	if(!feed)
	    return XMLParseFailed;

	GitCommit cmt = ParseEntry(feed->FirstChildElement("entry"));

	if(workarea->last_commit < cmt)
	{
	    workarea->last_commit = cmt;

	    cBasicPrint("Updated repository: {0}, commit={1}, ts={2}",
			repo,cmt.hash,cmt.ts);
	    CString log;
	    for (Proc_Cmd const& cmd : command_queue)
	    {
		cBasicPrintNoNL("Running: {0}",cmd.program);
		for(CString const& a : cmd.argv)
		    cBasicPrintNoNL(" {0}",a);
		cBasicPrintNoNL("\n");
		int sig = Proc::ExecuteLogged(cmd, &log);
		if (sig != 0)
		{
		    cBasicPrint("Failed with signal {0}:\n{1}", sig, log);
		    return ProcessFailed;
		}
	    }
	}else{
	    cWarning("Mismatch commit, latest is commit={0}, ts={1}",
		     workarea->last_commit.hash,workarea->last_commit.ts);
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
    ArgumentCollection args;

    args.registerArgument(ArgumentCollection::Argument,"gitbin");
    args.registerArgument(ArgumentCollection::Argument,"cmakebin");

    args.parseArguments(argc,argv);

    for(std::pair<CString,cstring> const& arg : args.getArgumentOptions())
    {
	if(arg.first == "gitbin" && arg.second)
	    git_program = arg.second;
	else if(arg.first == "cmakebin" && arg.second)
	    cmake_program = arg.second;
    }

    cDebug("Launched BuildBot");

    Vector<DataSet> datasets;

    for(cstring it : args.getPositionalArguments())
    {
	datasets.push_back(create_item(it));
    }

    if(datasets.size() == 0)
	return 0;

    cDebug("Got {0} datasets\n",datasets.size());

    REST::InitService();

    while(true)
    {
	for(DataSet& e : datasets)
	{
	    int sig = update_item(e.repo, &e.temp);
	    if (sig != Nothing)
		return sig;
	}
	Threads::sleepMillis(250);
    }
}

COFFEE_APPLICATION_MAIN(coffee_main);
