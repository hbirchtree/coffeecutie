#ifndef ENVIRONMENT_DETAILS
#define ENVIRONMENT_DETAILS

#include "coffee/core/CTypes"

namespace Coffee{

extern cstring_w coffee_get_env_variable(cstring var);

extern cstring coffee_get_path_sep();

extern cstring_w coffee_concat_plat_path(cstring_w target, cstring v2);

extern cstring_w coffee_get_user_home_dir();

extern cstring_w coffee_get_userdata_dir(cstring orgname, cstring appname);

extern cstring_w coffee_get_application_dir();

extern cstring_w coffee_get_current_dir();

} //Coffee

#endif
