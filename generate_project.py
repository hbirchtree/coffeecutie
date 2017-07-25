from os.path import dirname, basename, realpath, curdir

from os.path import isfile, isdir

from glob import glob
from subprocess import Popen, PIPE
from collections import namedtuple

from argparse import ArgumentParser

import configure_ci


ProcessResult = namedtuple('ProcessResult', 'code, stdout, stderr')


_dry_run = True
_verbose = False


def run_command(program, args, workdir=curdir):
    program_mapping = {
        'git': 'git',
        'mkdir': 'mkdir',
        'copy_dir': 'cp',
        'update_file': 'cp',
        'add_execute': 'chmod',
        'remove_file': 'rm'
    }

    program_args_mapping = {
        'git': [],
        'remove_file': [],
        'mkdir': ['-p'],
        'copy_dir': ['-urT'],
        'update_file': ['-u'],
        'add_execute': ['+x']
    }

    args = program_args_mapping[program] + args
    program = program_mapping[program]

    if _dry_run or _verbose:
        print('Executing %s %s' % (program, args))

        if _dry_run:
            return ProcessResult(0, '', '')

    proc = Popen([program, ] + args,
                 cwd=workdir, stdout=PIPE, stderr=PIPE)

    stdout, stderr = proc.communicate()

    proc.wait()

    if proc.returncode != 0:
        print('Attempted to run %s %s in directory %s' % (program, args, workdir))
        print('Process exited with error code: %s' % (proc.returncode,))
        print('stdout: \n%s' % (stdout,))
        print('stderr: \n%s' % (stderr,))
        exit(1)

    return ProcessResult(proc.returncode, stdout.decode(), stderr.decode())


_git_dir = None


def git_get_commit(repo_dir):
    result = run_command('git', ['-C', repo_dir, 'describe', '--tags'])
    commit = result.stdout.replace('\n', '')
    if len(commit) < 1:
        return '0000'
    return commit


def git_add(file):
    run_command('git', ['-C', _git_dir, 'add', file])


def git_initialize(target_dir, patterns):
    global _git_dir
    _git_dir = target_dir
    run_command('git', ['-C', target_dir, 'init'])

    gitignore_file = '%s/.gitignore' % target_dir

    if not isfile(gitignore_file):
        with open(gitignore_file, mode='w') as f:
            for p in patterns:
                f.write('%s\n' % p)
        git_add(gitignore_file)


def create_directory_structure(target_dir, directories):
    if type(directories) == list:
        for d in directories:
            dname = '%s/%s' % (target_dir, d)
            run_command('mkdir', [dname])
    elif type(directories) == dict:
        for d in directories:
            dname = '%s/%s' % (target_dir, d)
            run_command('mkdir', [dname])
    else:
        return False
    return True


def copy_config_files(source_dir, target_dir, files):
    for file in files:
        src_name = '%s/%s' % (source_dir, file)
        if isdir(src_name):
            atype = type(files[file])
            if atype == list:
                for e in files[file]:
                    src_name = '%s/%s/%s' % (source_dir, file, e)
                    trg_name = '%s/%s/%s' % (target_dir, file, e)
                    run_command('copy_dir', [src_name, trg_name])
                    git_add(trg_name)
            elif atype == dict:
                for e in files[file]:
                    src_name = '%s/%s/%s' % (source_dir, file, e)
                    trg_name = '%s/%s/%s' % (target_dir, file, files[file][e])
                    run_command('copy_dir', [src_name, trg_name])
                    git_add(trg_name)
            elif files[file] is None:
                src_name = '%s/%s' % (source_dir, file)
                trg_name = '%s/%s' % (target_dir, file)
                run_command('copy_dir', [src_name, trg_name])
                git_add(trg_name)

        elif isfile(src_name):
            trg_name = None
            if type(files[file]) == str:
                trg_name = '%s/%s' % (target_dir, files[file])
            elif files[file] is None:
                trg_name = '%s/%s' % (target_dir, file)
            run_command('update_file', [src_name, trg_name])
            git_add(trg_name)
        else:
            assert ( type(files[file]) == str )
            glb_files = glob(src_name)
            trg_name = '%s/%s/' % (target_dir, files[file])
            if len(glb_files) > 1:
                for g in glb_files:
                    run_command('update_file', [g, trg_name])
                git_add(trg_name)


def configure_file(src_file, trg_file, variables):
    if isfile(trg_file):
        if _verbose:
            print('Skipping configuration of %s, it already exists' % trg_file)
        return

    for var in variables:
        assert ( type(variables[var]) == str )
    src_text = None
    if src_file is not None:
        with open(src_file, 'r') as src_fd:
            src_text = src_fd.read()
            for var in variables:
                src_text = src_text.replace('@%s@' % (var,),
                                            variables[var])

    if _dry_run or _verbose:
        print('Writing file %s:' % trg_file)
        print(src_text)
        if _dry_run:
            return

    with open(trg_file, 'w') as trg_fd:
        if src_text is not None:
            trg_fd.write(src_text)
    git_add(trg_file)


def configure_all_files(src_dir, trg_dir, files, variables):
    for file in files:
        src_file = '%s/%s' % (src_dir, file)
        trg_file = '%s/%s' % (trg_dir, files[file])

        configure_file(src_file, trg_file, variables)


def create_ci_config_file(src_dir, trg_dir, template, args,
                          ci_services=[]):
    src_file = '%s/%s' % (src_dir, template)
    trg_file = '%s/build.yml' % (trg_dir,)

    structure = configure_ci.parse_yaml(src_file)

    structure['name'] = args.project_name
    structure['display_name'] = args.display_name
    structure['engine_version'] = git_get_commit(args.repo_dir)

    config_files = configure_ci.generate_config_files(ci_services,
                                                      structure)

    yaml_data = configure_ci.render_yaml(structure)

    if _dry_run or _verbose:
        for config in config_files:
            yaml_config = configure_ci.render_yaml(config_files[config])
            print('Writing file %s/%s:' % (trg_dir, config))
            print(yaml_config)
        print('Writing file %s:' % trg_file)
        print(yaml_data)
        if _dry_run:
            return

    if not isfile(trg_file):
        with open(trg_file, mode='w') as trg_fd:
            trg_fd.write(yaml_data)
        git_add(trg_file)

    for config in config_files:
        ci_file = '%s/%s' % (trg_dir, config)
        if isfile(ci_file):
            continue
        yaml_config = configure_ci.render_yaml(config_files[config])
        with open(ci_file, mode='w') as ci_fd:
            ci_fd.write(yaml_config)
        git_add(ci_file)


project_name_compliance = r'[A-Za-z_][A-Za-z0-9_]+'


def check_project_name(name):
    import re

    pattern = re.compile(project_name_compliance)

    return pattern.match(name) is not None


def convert_sh_format(target_dir, source_dir):
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

    run_command('remove_file', ['%s/reconfig.sh' % target_dir])


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
                      default=realpath(dirname(__file__)))

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

    repo_dir = realpath(dirname(__file__))
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
        convert_sh_format(target_dir, repo_dir)

    # Check args and stuff
    if not check_project_name(project_name):
        print('Invalid project name, please comply with %s' % (project_name_compliance,))
        return 1

    if repo_dir == target_dir:
        print('Cannot set up new project within existing project, dawg')
        return 1

    directories = [
        'cmake', 'src', 'internal',
        'ci', 'include',
        'cmake/Templates'
    ]
    files = {
            'cmake': [
                'Find', 'Find_Android', 'Find_Windows',
                'Modules', 'Packaging', 'Preload',
                'Toolchains'
            ],
            'desktop': None,
            'internal/templates': None,
            'tools/makers/Makefile.*': 'ci',
            'cmake/Templates/travis-build.sh': 'ci',
            'cmake/Templates/travis-deps.sh': 'ci',
            'configure_ci.py': None,
            'cmake/Templates/reconfig.py': 'rebuild.py'
        }

    variables = {
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
    }

    gitignore_patterns = [
        '*~',
        '*.user',
        'local.yml'
    ]

    services = {
        configure_ci.ConfigCreator(configure_ci.travis_gen_config,
                                   'Travis CI', configure_ci.travis_targets,
                                   '.travis.yml'),
        configure_ci.ConfigCreator(configure_ci.appveyor_gen_config,
                                   'Appveyor CI', configure_ci.appveyor_targets,
                                   'appveyor.yml'),
    }

    configurable_files = {
        'cmake/Templates/TemplateProject.txt': 'CMakeLists.txt',
        'cmake/Templates/main.cpp': 'src/main.cpp'
    }

    resource_dir = 'rsrc/%s' % (project_name,)
    directories += [resource_dir]

    git_initialize(target_dir, gitignore_patterns)
    create_directory_structure(target_dir, directories)
    copy_config_files(repo_dir, target_dir, files)
    configure_all_files(repo_dir, target_dir, configurable_files, variables)

    configure_file(None, '%s/%s/RESOURCES' % (target_dir, resource_dir), {})

    create_ci_config_file(repo_dir, target_dir, 'cmake/Templates/coffee-build.yml',
                          args=args, ci_services=services)

    # Leave local configuration for quick reconfiguration

    with open('%s/local.yml' % target_dir, mode='w') as local_config:
        cfg = {
            'source_dir': repo_dir
        }
        yaml_data = configure_ci.render_yaml(cfg)
        if _dry_run or _verbose:
            print('Writing local config file:')
            print(yaml_data)

        if not _dry_run:
            local_config.write(yaml_data)

    for f in ('%s/rebuild.py' % target_dir, '%s/configure_ci.py' % target_dir):
        run_command('add_execute', [f])
        git_add(f)

    return 0

exit(main())