from os.path import curdir
from collections import namedtuple
from os.path import isfile, isdir, dirname
from subprocess import Popen, PIPE

from platform import uname

local_yml_filespec = '.local.yml'
build_yml_filespec = '.build.yml'

ProcessResult = namedtuple('ProcessResult', 'code, stdout, stderr')


def configure_string(src_text, variables):
    for var in variables:
        src_text = src_text.replace('@%s@' % (var,),
                                    variables[var])
    return src_text


def try_get_key(d, k, v):
    try:
        return d[k]
    except KeyError:
        return v


def run_command(program, args, workdir=curdir, dry_run=True, verbose=False):
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

    # On OS X, cp is quite primitive
    # So we switch to rsync
    if uname().system == 'Darwin':
        program_mapping['copy_dir'] = 'rsync'
        program_mapping['update_file'] = 'rsync'
        program_args_mapping['copy_dir'] = ['-av']
        program_args_mapping['update_file'] = ['-u']

        if program == 'copy_dir':
            args[0] = args[0] + '/'

    args = program_args_mapping[program] + args
    program = program_mapping[program]

    if dry_run or verbose:
        print('Executing %s %s' % (program, args))

        if dry_run:
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


def configure_file(src_file, trg_file, variables, _verbose=False, _dry_run=False):
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
            src_text = configure_string(src_text, variables)

    if _dry_run or _verbose:
        print('Writing file %s:' % trg_file)
        print(src_text)
        if _dry_run:
            return
    
    if not isdir(dirname(trg_file)):
        run_command("mkdir", [dirname(trg_file)], dry_run=False)

    with open(trg_file, 'w') as trg_fd:
        if src_text is not None:
            trg_fd.write(src_text)

