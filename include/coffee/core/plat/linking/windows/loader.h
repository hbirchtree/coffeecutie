#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_WINDOWS

#include "../libraries.h"

namespace Coffee {
	namespace Library {
		namespace Windows {

			struct WindowsFunctionLoader : FunctionLoad_def
			{

			};
			struct WindowsObjectLoader : ObjectLoader_def<WindowsFunctionLoader>
			{

			};

		}

		using FunctionLoader = Windows::WindowsFunctionLoader;
		using ObjectLoader = Windows::WindowsObjectLoader;

	}
}

#endif