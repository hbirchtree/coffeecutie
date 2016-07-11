#include <coffee/core/plat/environment/windows/environment.h>

#include <coffee/core/plat/plat_memory.h>

#include <coffee/core/plat/plat_windows.h>
#include <Pathcch.h>
#include <cstdlib>

namespace Coffee {
	namespace Environment {
		namespace Windows {
			CString WindowsEnvFun::ExecutableName(cstring_w)
			{
				CString excname;

				excname.resize(MAX_PATH);
				GetModuleFileName(nullptr, &excname[0], excname.size());
				excname.resize(Search::ChrFind(excname.c_str(), 0) - excname.c_str());

				return excname;
			}

			CString WindowsEnvFun::GetUserData(cstring org, cstring app)
			{
				CString out;
				out = GetUserHome();
				out += GetPathSep();
				out += org;
				out += GetPathSep();
				out += app;
				return out;
			}

			CString WindowsEnvFun::GetVar(cstring v)
			{
				CString out;

				sbyte_t* var = std::getenv(v);

				if (var)
					out = var;

				return out;
			}

			WindowsEnvFun::Variables WindowsEnvFun::Environment()
			{
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
			}

			CString WindowsEnvFun::CurrentDir()
			{
				CString out;

				out.resize(GetCurrentDirectory(0, nullptr));
				GetCurrentDirectory(out.size(), &out[0]);
				out.resize(out.size() - 1);

				return out;
			}

			CString WindowsEnvFun::DirName(CString fn)
			{
				fn = CStrReplace(fn, "/", "\\");
				CWString fn_w(fn.begin(), fn.end());
				PathCchRemoveFileSpec(&fn_w[0], fn_w.size());
				CString out(fn_w.begin(), fn_w.end());
				out.resize(StrLen(out.c_str()) - 1);
				out = CStrReplace(out, "\\", "/");
				return out;
			}
			CString WindowsEnvFun::BaseName(CString fn)
			{
				CString sep = GetPathSep();
				cstring out_f = Search::ChrFindR(fn.c_str(), sep[0]);
				CString out;
				if (!out_f)
					out = fn;
				else
					out = out_f + 1;
				return out;
			}
		}
	}
}