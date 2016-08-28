#include <coffee/core/plat/environment/windows/environment.h>

#include <coffee/core/plat/plat_memory.h>

#include <coffee/core/plat/plat_windows.h>
#include <Pathcch.h>
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

				CString k, v;

				while (env[0] != TCHAR(0))
				{
					cstring_w p_r = nullptr;
					cstring p = nullptr;

					p_r = Convert::WideNarrow(env);

					if (!p_r)
						p = (cstring)env;
					else
						p = p_r;

					k.clear();
					{
						szptr len = Search::ChrFind(p, '=') - p;
						k.resize(len);
						k.insert(0, p, len);
					}
					v.clear();
					{
						szptr off = Search::ChrFind(p, '=') - p + 1;
						szptr len = StrLen(p) - off;
						v.resize(len);
						v.insert(0, &p[off], len);
					}


					var[k] = v;

					if (p_r)
						CFree(p_r);

					env = (TCHAR*)Search::ChrFind(env, TCHAR(0));
					env += 1; // Skipping the expected \0
				}

				return var;
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
#ifdef COFFEE_WINDOWS_UWP
				return {};
#else
                CString fn_ = fn;
                fn_ = CStrReplace(fn, "/", "\\");
                CWString fn_w(fn_.begin(), fn_.end());
				PathCchRemoveFileSpec(&fn_w[0], fn_w.size());
				CString out(fn_w.begin(), fn_w.end());
                out.resize(StrLen(out.c_str()));
				out = CStrReplace(out, "\\", "/");
				return out;
#endif
			}
            CString WindowsEnvFun::BaseName(cstring fn)
			{
                const constexpr cstring sep = "/";
                cstring out_f = Search::ChrFindR(fn, sep[0]);
                if((out_f - fn) < 0)
                    return fn;
				CString out;
				if (!out_f)
					out = fn;
				else
					out = out_f + 1;
                if(out.empty())
                    out = ".";
				return out;
			}
		}
	}
}
