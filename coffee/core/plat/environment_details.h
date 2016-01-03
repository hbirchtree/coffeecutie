#ifndef ENVIRONMENT_DETAILS
#define ENVIRONMENT_DETAILS

#include "coffee/core/CTypes"

namespace Coffee{

extern cstring_w env_get_variable(cstring var);

extern bool env_set_variable(cstring var, cstring value);

extern bool env_unset_variable(cstring var);

extern bool env_clear_all();

extern cstring env_get_path_separator();

extern cstring_w env_concatenate_path(cstring_w target, cstring v2);

extern cstring_w env_get_user_home();

extern cstring_w env_get_user_data(cstring orgname, cstring appname);

extern cstring_w env_get_application_dir();

extern cstring_w env_get_current_dir();

} //Coffee

#endif
