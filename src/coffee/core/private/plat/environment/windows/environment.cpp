#include <coffee/core/plat/environment/windows/environment.h>

#include <coffee/core/coffee.h>
#include <coffee/core/plat/plat_memory.h>
#include <coffee/core/plat/plat_windows.h>
#include <stdlib.h>
#include <wchar.h>

namespace Coffee {
namespace Environment {
namespace Windows {
CString WindowsEnvFun::ExecutableName(cstring_w)
{
#ifdef COFFEE_WINDOWS_UWP
    CWString excname;
#else
    CString excname;
#endif
    excname.resize(MAX_PATH);

    GetModuleFileName(nullptr, &excname[0], excname.size());
    excname.resize(Search::ChrFind(excname.c_str(), 0) - excname.c_str());
#ifdef COFFEE_WINDOWS_UWP
    excname = CStrReplace(excname, L"\\", L"/");
#else
    excname = CStrReplace(excname, "\\", "/");
#endif

#ifdef COFFEE_WINDOWS_UWP
    CString out(excname.begin(),excname.end());
    return out;
#else
    return excname;
#endif
}

CString WindowsEnvFun::GetUserData(cstring org, cstring app)
{
    if(!org && !app)
    {
        org = ApplicationData().organization_name.c_str();
        app = ApplicationData().application_name.c_str();
    }

    CString out;
    out = GetUserHome();
    out = ConcatPath(out.c_str(),"AppData");
    out = ConcatPath(out.c_str(),"Local");
    out = ConcatPath(out.c_str(),org);
    out = ConcatPath(out.c_str(),app);
    out = CStrReplace(out, "\\", "/");
    return out;
}

CString WindowsEnvFun::GetVar(cstring v)
{
    CString out;

#ifndef COFFEE_WINDOWS_UWP
    cstring_w var = getenv(v);

    if (var)
        out = var;
#endif

    return out;
}

WindowsEnvFun::Variables WindowsEnvFun::Environment()
{
#ifdef COFFEE_WINDOWS_UWP
    return {};
#else
    TCHAR* env = GetEnvironmentStrings();
    if (!env)
        return{};
    Variables var;

    while (env[0] != TCHAR(0))
    {
        CString p_r;

        p_r = env;

        szptr key_end = p_r.find('=');

        var[p_r.substr(0, key_end)] = p_r.substr(key_end);

        env = (TCHAR*)Search::ChrFind(env, TCHAR(0));
        env += 1; // Skipping the expected \0
    }

    return var;
#endif
}

bool WindowsEnvFun::ExistsVar(cstring v)
{
#ifdef COFFEE_WINDOWS_UWP
	return false;
#else
	return getenv(v) != nullptr;
#endif
}

CString WindowsEnvFun::CurrentDir()
{
#ifdef COFFEE_WINDOWS_UWP
    CWString out;
#else
    CString out;
#endif

    out.resize(GetCurrentDirectory(0, nullptr));
    GetCurrentDirectory(out.size(), &out[0]);
    out.resize(out.size() - 1);
#ifdef COFFEE_WINDOWS_UWP
    out = CStrReplace(out, L"\\", L"/");
    return CString(out.begin(),out.end());
#else
    out = CStrReplace(out, "\\", "/");
    return out;
#endif
}

CString WindowsEnvFun::DirName(cstring fn)
{
	CString fn_ = fn;

	auto idx = fn_.rfind('/');

	if (idx != CString::npos)
		return fn_.substr(0, idx);
	else
	{
		if (!fn_.size())
			return ".";
		return fn_;
	}

//#ifdef COFFEE_WINDOWS_UWP
//    return {};
//#else
//    CString fn_ = fn;
//    fn_ = CStrReplace(fn, "/", "\\");
//    CWString fn_w(fn_.begin(), fn_.end());
//    //PathCchRemoveFileSpec(&fn_w[0], fn_w.size());
//    CString out(fn_w.begin(), fn_w.end());
//    out.resize(StrLen(out.c_str()));
//    out = CStrReplace(out, "\\", "/");
//    return out;
//#endif
}
CString WindowsEnvFun::BaseName(cstring fn)
{
    const constexpr cstring sep = "/";
    
	if (!fn)
		return ".";

	CString out = fn;
	auto idx = out.rfind(sep);

	if (out == "/")
		return out;

	if (idx == CString::npos)
	{
		if (!out.size())
			return ".";
		return out;
	}

    return out.substr(idx + 1, out.size() - idx - 1);
}
}
}
}