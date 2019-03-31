from python.git_common import sshgit_to_https, git_get_origin
from python.ci.common import get_dep_list, create_target_matrix, get_script_locations
from python.common import try_get_key, configure_string

def jenkins_gen_config(build_info, src_dir):
    def mk_groovy_list(l):
        glist = ''
        for e in l:
            glist = "'%s', %s" % (e, glist)
        return glist

    makers_dir = '%s/toolchain/makers' % src_dir
    template_dir = '%s/toolchain/cmake/Templates' % src_dir

    deps = get_dep_list(build_info)

    linux_targets = create_target_matrix(makers_dir, 'linux', build_info)
    osx_targets = create_target_matrix(makers_dir, 'osx', build_info)
    windows_targets = create_target_matrix(makers_dir, 'windows', build_info)

    repo_url = sshgit_to_https(git_get_origin(src_dir))

    script_locations_unix = get_script_locations(build_info, 'unix')
    script_locations_windows = get_script_locations(build_info, 'windows')

    variables = {
        'PROJECT_NAME': try_get_key(build_info, 'name', 'coffee'),

        'LINUX_TARGETS': mk_groovy_list(linux_targets),
        'OSX_TARGETS': mk_groovy_list(osx_targets),
        'WINDOWS_TARGETS': mk_groovy_list(windows_targets),

        'DEPENDENCIES': deps.replace(";", "%"),
        'DEPENDENCIES_NIX': deps.replace(";", "%"),
        'REPO_URL': repo_url,

        'MAKEFILE_DIR': try_get_key(build_info, 'makefile_location', 'ci'),

        'WINDOWS_BUILD_SCRIPT': '${srcDir}\\' + script_locations_windows.build[0],
        'UNIX_BUILD_SCRIPT': '${srcDir}/' + script_locations_unix.build[0]
    }

    with open('%s/JenkinsTemplate.groovy' % template_dir, mode='r') as f:
        data = f.read()

    return configure_string(data, variables)
