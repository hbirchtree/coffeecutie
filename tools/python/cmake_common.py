from tools.python.common import *
from configure_ci import try_get_key


def cmake_generate_findscript(exports):

    script = '''
#
# Autogenerated by generate_project.py
# Changes made to this file will NOT be saved
# This find-script will make it easier to add several libraries for external use 
#

set ( COFFEE_SEARCH_PATHS
    /usr/local
    /usr
    /opt

    ~/Library/Frameworks
    /Library/Frameworks

    ${COFFEE_ROOT_DIR}
    $ENV{COFFEE_ROOT_DIR}
    )
set ( COFFEE_LIBRARY_SUFFIXES
    lib
    lib64
    lib/${ANDROID_ABI}
    lib/${CMAKE_LIBRARY_ARCHITECTURE}
    )

    '''

    for k in exports:
        data = exports[k]
        type_k = try_get_key(data, 'type', '')
        if type_k == 'library':
            script += '''
find_library ( %s_TMP
    %s
    
    PATHS ${COFFEE_SEARCH_PATHS}
    PATH_SUFFIXES ${COFFEE_LIBRARY_SUFFIXES}
    )

if(%s_TMP)
    set ( %s "${%s_TMP}" CACHE PATH "" )
endif()
''' % (k, try_get_key(data, 'file', 'CoffeeLib'), k, k, k)
        elif type_k == 'header':
            script += '''
find_path ( %s_TMP
    %s/%s

    PATHS
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    include
    )

if(%s_TMP)
    set ( %s "${%s_TMP}" CACHE PATH "" )
endif()
''' % (k, try_get_key(data, 'path', '.'),
       try_get_key(data, 'file', 'CoffeeLib'),
       k, k, k)
        else:
            print('ERROR: Invalid export type detected: %s' % type_k)

    script += '''
find_package_handle_standard_args (
    @PROJECT_NAME@

    REQUIRED_VARS
'''

    for k in exports:
        if try_get_key(exports[k], 'required', False):
            script += '%s\n' % k

    script += '''
    )
'''

    return script
