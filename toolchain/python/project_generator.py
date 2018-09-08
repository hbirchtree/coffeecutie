from collections import namedtuple

_target_dir = ''
_source_dir = ''

RunSettings = namedtuple('RunSettings', ['dry', 'verbose'])

_settings = RunSettings(True, False)


class ProjectStep:
    def __init__(self):
        pass

    @staticmethod
    def get_source_dir():
        return _source_dir

    @staticmethod
    def get_target_dir():
        return _target_dir

    def __call__(self):
        pass


def set_or_default(val, default):
    if val is None:
        return default
    else:
        return val


class ProjectStructure:
    def __init__(self,
                 project_name=None,
                 display_name=None,

                 repo_dir=None,
                 target_dir=None,

                 directory_skeleton=None,
                 constant_files=None,
                 configurable_files=None,

                 configure_vars=None,

                 local_files=None,

                 project_steps=None
                 ):
        self.name = set_or_default(project_name, 'coffee_project')
        self.display_name = set_or_default(display_name, 'Coffee Project')

        self.dir_skel = set_or_default(directory_skeleton, [])
        self.file_skel = set_or_default(constant_files, {})
        self.cfg_skel = set_or_default(configurable_files, {})
        self.vars = set_or_default(configure_vars, {})
        self.locals = set_or_default(local_files, [])
        self.steps = set_or_default(project_steps, [])

        global _target_dir
        global _source_dir
        _target_dir = target_dir
        _source_dir = repo_dir

        resource_dir = 'rsrc/%s' % (project_name,)

        self.dir_skel.append(resource_dir)
        self.rsc_dir = resource_dir

    def generate(self, dry=True, verbose=False):
        from python.git_common import git_initialize
        from python.common import configure_file

        global _settings
        _settings = RunSettings(dry=dry, verbose=verbose)

        target_dir = ProjectStep.get_target_dir()
        repo_dir = ProjectStep.get_source_dir()

        if not _settings.dry:
            git_initialize(target_dir, self.locals)

        ProjectStructure.create_directory_structure(
            target_dir, self.dir_skel)
        ProjectStructure.copy_config_files(
            repo_dir, target_dir, self.file_skel)
        ProjectStructure.configure_all_files(
            repo_dir, target_dir, self.cfg_skel, self.vars)

        configure_file(None, '%s/%s/RESOURCES' % (target_dir, self.rsc_dir), {})

        for step in self.steps:
            step()

    @staticmethod
    def create_directory_structure(target_dir, directories):
        from python.common import run_command

        _dry_run = _settings.dry
        _verbose = _settings.verbose

        if type(directories) == list:
            for d in directories:
                dname = '%s/%s' % (target_dir, d)
                run_command('mkdir', [dname], dry_run=_dry_run, verbose=_verbose)
        elif type(directories) == dict:
            for d in directories:
                dname = '%s/%s' % (target_dir, d)
                run_command('mkdir', [dname], dry_run=_dry_run, verbose=_verbose)
        else:
            return False
        return True

    @staticmethod
    def copy_config_files(source_dir, target_dir, files):
        from os.path import isdir, isfile
        from glob import glob
        from python.common import run_command
        from python.git_common import git_add

        _dry_run = _settings.dry
        _verbose = _settings.verbose

        for file in files:
            src_name = '%s/%s' % (source_dir, file)
            if isdir(src_name):
                atype = type(files[file])
                if atype == list:
                    for e in files[file]:
                        src_name = '%s/%s/%s' % (source_dir, file, e)
                        trg_name = '%s/%s/%s' % (target_dir, file, e)
                        run_command('copy_dir', [src_name, trg_name],
                                    dry_run=_dry_run, verbose=_verbose)
                        if not _dry_run:
                            git_add(trg_name)
                elif atype == dict:
                    for e in files[file]:
                        src_name = '%s/%s/%s' % (source_dir, file, e)
                        trg_name = '%s/%s/%s' % (target_dir, file, files[file][e])
                        run_command('copy_dir', [src_name, trg_name],
                                    dry_run=_dry_run, verbose=_verbose)
                        if not _dry_run:
                            git_add(trg_name)
                elif files[file] is None:
                    src_name = '%s/%s' % (source_dir, file)
                    trg_name = '%s/%s' % (target_dir, file)
                    run_command('copy_dir', [src_name, trg_name],
                                dry_run=_dry_run, verbose=_verbose)
                    if not _dry_run:
                        git_add(trg_name)

            elif isfile(src_name):
                trg_name = None
                if type(files[file]) == str:
                    trg_name = '%s/%s' % (target_dir, files[file])
                elif files[file] is None:
                    trg_name = '%s/%s' % (target_dir, file)
                run_command('update_file', [src_name, trg_name],
                            dry_run=_dry_run, verbose=_verbose)
                if not _dry_run:
                    git_add(trg_name)
            else:
                assert ( type(files[file]) == str )
                glb_files = glob(src_name)
                trg_name = '%s/%s/' % (target_dir, files[file])
                if len(glb_files) > 1:
                    for g in glb_files:
                        run_command('update_file', [g, trg_name],
                                    dry_run=_dry_run, verbose=_verbose)
                    if not _dry_run:
                        git_add(trg_name)

    @staticmethod
    def configure_all_files(src_dir, trg_dir, files, variables):
        from python.common import configure_file
        from python.git_common import git_add

        for file in files:
            src_file = '%s/%s' % (src_dir, file)
            trg_file = '%s/%s' % (trg_dir, files[file])

            configure_file(src_file, trg_file, variables, _settings.verbose, _settings.dry)
            if not _settings.dry:
                git_add(trg_file)


_struct = ProjectStructure()
_prev_build_data = None


class StepCreateRebuild(ProjectStep):
    def __init__(self):
        pass

    def __call__(self):
        from python.git_common import git_add
        from python.common import run_command

        target_dir = ProjectStep.get_target_dir()

        for f in ('%s/rebuild.py' % target_dir, '%s/toolchain/configure_ci.py' % target_dir):
            run_command('add_execute', [f],
                        dry_run=_settings.dry,
                        verbose=_settings.verbose)
            if not _settings.dry:
                git_add(f)


class StepSaveLocalInfo(ProjectStep):
    def __init__(self):
        pass

    def __call__(self):
        from os.path import isfile
        import configure_ci
        from python.common import local_yml_filespec

        target_dir = ProjectStep.get_target_dir()

        # Leave local configuration for quick reconfiguration
        old_local_file = '%s/local.yml' % (target_dir,)
        local_file = '%s/%s' % (target_dir, local_yml_filespec)

        cfg = {}
        if isfile(old_local_file):
            cfg.update(configure_ci.parse_yaml(old_local_file))

        cfg.update({'source_dir': ProjectStep.get_source_dir()})
        yaml_data = configure_ci.render_yaml(cfg)
        if _settings.dry or _settings.verbose:
            print('Writing local config file:')
            print(yaml_data)

        if not _settings.dry:
            with open(local_file, mode='w') as local_config:
                local_config.write(yaml_data)


class StepUpdateBuildInfo(ProjectStep):
    def __init__(self, template, ci_services):
        self.template = template
        self.ci_services = ci_services

    def __call__(self):
        from copy import deepcopy
        from python.git_common import git_add, git_get_commit, git_get_slug
        from python.common import run_command, build_yml_filespec
        from os.path import isfile
        import configure_ci

        src_dir = ProjectStep.get_source_dir()
        trg_dir = ProjectStep.get_target_dir()

        src_file = '%s/%s' % (src_dir, self.template)
        trg_file = '%s/%s' % (trg_dir, build_yml_filespec)
        old_trg_file = '%s/%s' % (trg_dir, 'build.yml')

        structure = None
        # Get current version of engine
        engine_version = git_get_commit(src_dir)
        engine_slug = git_get_slug(src_dir)

        # If file exists, update the contents
        if isfile(trg_file) or isfile(old_trg_file):
            if isfile(trg_file):
                structure = configure_ci.parse_yaml(trg_file)
            if isfile(old_trg_file):
                structure = configure_ci.parse_yaml(old_trg_file)
                run_command('remove_file', [old_trg_file],
                            dry_run=_settings.dry,
                            verbose=_settings.verbose)

            if 'dependencies' in structure:
                v = structure['dependencies']

                # If using a list, convert to dictionary
                if type(v) == list:
                    dv = {}
                    print('Updating %s structure' % (build_yml_filespec,))
                    structure.pop('engine_version', None)
                    for e in v:
                        try:
                            dv[e.split(':')[1]] = '*'
                        except IndexError:
                            pass
                    structure['dependencies'] = dv
                    v = dv

                if engine_slug in v:
                    structure['dependencies'][engine_slug] = engine_version
        # If it does not exist, configure a new one
        else:
            structure = configure_ci.parse_yaml(src_file)
            configure_ci.parse_linux_targets()

            structure['dependencies'][engine_slug] = engine_version

            structure['name'] = _struct.name
            structure['display_name'] = _struct.display_name
            structure['version'] = {
                     "major": 0,
                     "minor": 0,
                     "release": 1,
                     "patch": 0,
                     "hotfix": 0,
                }

        config_files = configure_ci.generate_config_files(self.ci_services,
                                                          deepcopy(structure),
                                                          trg_dir)

        global _prev_build_data
        _prev_build_data = structure
        yaml_data = configure_ci.render_yaml(structure)

        if _settings.dry or _settings.verbose:
            for config in config_files:
                yaml_config = configure_ci.render_yaml(config_files[config])
                print('Writing file %s/%s:' % (trg_dir, config))
                print(yaml_config)
            print('Writing file %s:' % trg_file)
            print(yaml_data)
            if _settings.dry:
                return

        configure_ci.process_configs(config_files,
                                     print_config=_settings.verbose,
                                     cur_dir=trg_dir)

        if not _settings.dry:
            for config in config_files:
                git_add('%s/%s' % (trg_dir, config))
            with open(trg_file, mode='w') as f:
                f.write(yaml_data)
            git_add(trg_file)


class StepListGitStatus(ProjectStep):
    def __init__(self):
        pass

    def __call__(self):
        from python.common import run_command

        target_dir = ProjectStep.get_target_dir()

        print(run_command('git', ['status'], target_dir,
                          dry_run=_settings.dry,
                          verbose=_settings.verbose
                          ).stdout
              )


class StepGenerateCMakeScript(ProjectStep):
    def __init__(self, exports=None):
        self.exports = set_or_default(exports, {})

    def __call__(self):
        from python.cmake_common import cmake_generate_findscript
        from configure_ci import try_get_key
        from python.git_common import git_add

        cmake_script = cmake_generate_findscript(try_get_key(_prev_build_data,
                                                             'exports',
                                                             {})
                                                 )

        pname = try_get_key(_prev_build_data, 'name', 'coffee_project')
        cmake_script = cmake_script.replace('@PROJECT_NAME@',
                                            pname)

        target_dir = ProjectStep.get_target_dir()
        script_name = '%s/Find%s.cmake' % (target_dir, pname)

        if _settings.dry or _settings.verbose:
            print('Writing %s:\n%s' % (script_name, cmake_script))

        if not _settings.dry:
            with open(script_name, mode='w') as f:
                f.write(cmake_script)
            git_add(script_name)


class StepCorrectPythonScriptPaths(ProjectStep):
    def __init__(self):
        pass

    def __call__(self):
        from glob import glob
        if _settings.dry:
            return
        for f in glob('%s/*.py' % ProjectStep.get_target_dir()):
            file_data = open(f).read()
            file_data = file_data.replace('tools.python.', '')
            file_data = file_data.replace('tools/python/', '')
            with open(f, mode='w') as fh:
                fh.write(file_data)
