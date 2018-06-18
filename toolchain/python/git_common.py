import configure_ci
from python.common import *

_git_dir = None


def git_get_commit(repo_dir):
    result = run_command('git', ['-C', repo_dir, 'describe', '--tags'],
                         dry_run=False,
                         verbose=False)
    commit = result.stdout.replace('\n', '')
    if len(commit) < 1:
        return '0000'
    return commit


def git_get_slug(repo_dir):
    import re
    origin = configure_ci.git_get_origin(repo_dir)
    patt = re.compile('^.*[/:](\S+\/\S+)\.git$')
    match = patt.findall(origin)

    return match.pop()


def git_add(file):
    run_command('git', ['-C', _git_dir, 'add', file],
                dry_run=False,
                verbose=False)


def git_initialize(target_dir, patterns):
    global _git_dir
    _git_dir = target_dir
    run_command('git', ['-C', target_dir, 'init'],
                dry_run=False,
                verbose=False)

    gitignore_file = '%s/.gitignore' % target_dir

    if not isfile(gitignore_file):
        with open(gitignore_file, mode='w') as f:
            for p in patterns:
                f.write('%s\n' % p)
        git_add(gitignore_file)
