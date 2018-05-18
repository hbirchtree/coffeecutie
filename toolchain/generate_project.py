#!/usr/bin/env python3

from argparse import ArgumentParser
from os.path import dirname, realpath

from python.git_common import *

from python.project_generator import *
from python.project_generator import _settings as run_settings

_dry_run = True
_verbose = False


project_name_compliance = r'[A-Za-z_][A-Za-z0-9_]+'


def check_project_name(name):
    import re
    pattern = re.compile(project_name_compliance)
    return pattern.match(name) is not None


def convert_sh_format(target_dir):
    try:
        gitignore_file = '%s/.gitignore' % (target_dir, )

        gitignore_data = None
        with open(gitignore_file, mode='r') as f:
            gitignore_data = f.read()

        gitignore_data = gitignore_data.replace('reconfig.sh', '#reconfig.sh')

        with open(gitignore_file, mode='w') as f:
            f.write(gitignore_data)
            f.write('\nlocal.yml')
    except FileNotFoundError:
        pass

    run_command('remove_file', ['%s/reconfig.sh' % target_dir],
                dry_run=_dry_run, verbose=_verbose)


def main():

    args = ArgumentParser(description='Create Coffee project files')

    args.add_argument('project_name', metavar='project-name', type=str,
                      help='name used when inserting placeholders')
    args.add_argument('display_name', metavar='display-name', type=str,
                      help='name used when inserting user-facing placeholders')

    args.add_argument('-d', '--project-directory', metavar='project-directory', type=str,
                      dest='project_directory',
                      help='directory to create project files in, defaults to cwd',
                      default=curdir)

    args.add_argument('--repo-directory', metavar='repo_dir', type=str,
                      dest='repo_dir',
                      help='directory from which template files will be pulled \
                            defaults to directory where this program resides',
                      default=dirname(realpath(dirname(__file__))))

    args.add_argument('--execute', action='store_false',
                      dest='dry_run', help='do not perform any actions, only print commands and files',
                      default=True)
    args.add_argument('--verbose', action='store_true',
                      dest='verbose', help='print information on actions being performed',
                      default=False)
    args.add_argument('--convert-sh', action='store_true',
                      dest='convert_sh', help='convert from reconfig.sh format to rebuild.py',
                      default=False)

    args = args.parse_args()

    repo_dir = dirname(realpath(dirname(__file__)))
    target_dir = realpath(args.project_directory)
    project_name = args.project_name
    display_name = args.display_name

    args.project_directory = target_dir

    global _dry_run
    if args.dry_run == 0:
        _dry_run = False
    global _verbose
    if args.verbose == 1:
        _verbose = True
    if args.convert_sh:
        convert_sh_format(target_dir)

    # Check args and stuff
    if not check_project_name(project_name):
        print('Invalid project name, please comply with %s' % (project_name_compliance,))
        return 1

    if repo_dir == target_dir:
        print('Cannot set up new project within existing project, dawg')
        return 1

    ProjectStructure(project_name=project_name,
                     display_name=display_name,
                     repo_dir=repo_dir,
                     target_dir=target_dir,
                     directory_skeleton=[
                         'cmake', 'src', 'internal',
                         'ci', 'include',
                         'cmake/Templates'
                     ],
                     constant_files={
                         'cmake': [
                             'Find/', 'Find_Windows/',
                             'Modules/', 'Packaging/', 'Preload/',
                             'Toolchains/'
                         ],
                         'desktop/': None,
                         'internal/templates/': None,
                         'tools/makers/Makefile.*': 'ci',
                         'tools/makers/targets.yml': 'ci',
                         'tools/ci/*.sh': 'ci',
                         'tools/ci/github_api.py': 'ci',
                         'tools/ci/*.ps1': 'ci',
                         'configure_ci.py': None,
                         'tools/python/common.py': 'common.py',
                         'cmake/Templates/JenkinsTemplate.groovy': None,
                         'cmake/Templates/reconfig.py': 'rebuild.py',
                         'cmake/Templates/Config*.cmake.in': 'cmake/Templates',
                         'buildinfo.py': None,
                         'version.py': None,
                         'git-gen-release.sh': None,
                         'quick-build.sh': None,
                         'quick-build.ps1': None,
                         'tools/ci/get_matching_release.py': 'ci/travis-helper.py',
                     },
                     configurable_files={
                         'cmake/Templates/TemplateProject.txt': 'CMakeLists.txt',
                         'cmake/Templates/main.cpp': 'src/main.cpp'
                     },
                     configure_vars={
                         'PJNAME': project_name,
                         'TITLE': display_name,
                         'SRCS': 'src/main.cpp',
                         'COMPANY': 'Coffee',
                         'DESC': 'Coffee Application',
                         'COPYRIGHT': 'Coffee',
                         'LIBS': '''
${COFFEE_CORE_LIBRARY};${COFFEE_GLEAM_COMMON_LIBRARY};
${COFFEE_OPENAL_LIBRARY};${COFFEE_ANDROID_LIBRARY};
${STANDARDLIBS_LIBRARIES};
${SDL2_LIBRARY};${SDL2_LIBRARIES}''',
                         'BUNDLE_LIBS': ''
                     },
                     local_files=[
                         '__pycache__',
                         '*~',
                         '*.user',
                         '*.trace',
                         'vgcore.*',
                         'core',
                         '*.vfs',
                         '*.pyc',
                         'multi_build',
                         '*.out*',
                         '*.ini',
                         local_yml_filespec
                     ],
                     project_steps=[
                         StepCreateRebuild(),
                         StepSaveLocalInfo(),
                         StepUpdateBuildInfo('cmake/Templates/coffee-build.yml',
                                             configure_ci.CI_SERVICES),
                         #StepGenerateCMakeScript(),
                         #StepCorrectPythonScriptPaths(),
                         StepListGitStatus(),
                     ]
                     ).generate(dry=args.dry_run,
                                verbose=args.verbose)

    return 0

exit(main())
