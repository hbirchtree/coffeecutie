# Updating MODULE_VERSION
execute_process (
    COMMAND "/home/havard/Code/coffeecutie/toolchain/desktop/version_generate.py"
    OUTPUT_VARIABLE DATE_VERSION
    )
execute_process (
    COMMAND "/home/havard/Code/coffeecutie/toolchain/desktop/../version.py" none
    OUTPUT_VARIABLE RELEASE_VERSION
    )
execute_process (
    COMMAND ${GITCOMMAND} rev-parse HEAD
    WORKING_DIRECTORY "/home/havard/Code/coffeecutie/toolchain/desktop"
    OUTPUT_VARIABLE GIT_VERSION
    )

string ( REPLACE "\n" "" RELEASE_VERSION "${RELEASE_VERSION}" )
string ( REPLACE "\n" "" DATE_VERSION "${DATE_VERSION}" )
string ( REPLACE "\n" "" GIT_VERSION "${GIT_VERSION}" )

set ( MODULE_VERSION "${RELEASE_VERSION}" )

if(GIT_VERSION)
    set ( MODULE_VERSION "${MODULE_VERSION}-${GIT_VERSION}" )
endif()
if(DATE_VERSION)
    set ( MODULE_VERSION "${MODULE_VERSION}-${DATE_VERSION}" )
endif()

message ( STATUS "Writing version ${MODULE_VERSION}" )

file ( WRITE "module_version.h"
    "#define MODULE_VERSION \"${MODULE_VERSION}\"\n"
    )
