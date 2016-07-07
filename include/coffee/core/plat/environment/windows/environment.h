#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#ifndef COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H
#define COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H

#include "../environment_details.h"
#include "../../plat_windows.h"
#include <cstdlib>
#include <Pathcch.h>

namespace Coffee{
namespace Environment{
	namespace Windows {

		struct WindowsEnvFun : EnvInterface
		{
			STATICINLINE CString ExecutableName(cstring_w = nullptr)
			{
				CString excname;

				excname.resize(MAX_PATH);
				GetModuleFileName(nullptr, &excname[0], excname.size());
				excname.resize(Search::ChrFind(excname.c_str(), 0) - excname.c_str());

				return excname;
			}

			STATICINLINE Variables Environment()
			{
				TCHAR* env = GetEnvironmentStrings();
				if (!env)
					return {};
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
						szptr len = StrLen(p)-off;
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
			
			STATICINLINE CString GetVar(cstring v)
			{
				CString out;

				sbyte_t* var = std::getenv(v);

				if(var)
					out = var;

				return out;
			}
			STATICINLINE bool SetVar(cstring, cstring)
			{
				return false;
			}
			STATICINLINE bool UnsetVar(cstring)
			{
				return false;
			}
			STATICINLINE bool ClearEnv()
			{
				return false;
			}

			STATICINLINE CString GetPathSep()
			{
				return "\\";
			}
			STATICINLINE CString ConcatPath(cstring f, cstring fv)
			{
				CString out;
				out += f;
				out += GetPathSep();
				out += fv;
				return out;
			}
			STATICINLINE CString GetUserHome()
			{
				return GetVar("USERPROFILE");
			}

			STATICINLINE CString GetUserData(cstring org, cstring app)
			{
				CString out;
				out = GetUserHome();
				out += GetPathSep();
				out += org;
				out += GetPathSep();
				out += app;
				return out;
			}

			STATICINLINE CString ApplicationDir()
			{
				return DirName(ExecutableName());
			}
			STATICINLINE CString CurrentDir()
			{
				CString out;

				out.resize(GetCurrentDirectory(0, nullptr));
				GetCurrentDirectory(out.size(), &out[0]);
				out.resize(out.size() - 1);

				return out;
			}
			STATICINLINE CString DirName(CString fn)
			{
				std::wstring fn_w(fn.begin(), fn.end());
				PathCchRemoveFileSpec(&fn_w[0], fn_w.size());
				CString out(fn_w.begin(), fn_w.end());
				out.resize(StrLen(out.c_str()) - 1);
				return out;
			}
			STATICINLINE CString BaseName(CString fn)
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
		};
	}

}

using ColorMap_ = Environment::EnvColorCodes;
using Env_ = Environment::Windows::WindowsEnvFun;

}

#endif
#endif
