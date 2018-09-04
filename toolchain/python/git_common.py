from python.common import *

_git_dir = None


def sshgit_to_https(url):
    import re
    # git@github.com:hbirchtree/coffeecutie.git
    # ssh://git@github.com/hbirchtree/coffeecutie.git
    patterns = [re.compile('^.*git@([^/:]+)[:/](.+)'),
                # re.compile('ssh://git@([^/]+)/(.+)')
                ]

    for patt in patterns:
        match = patt.findall(url)
        if match:
            return 'https://%s/%s' % (match[0][0], match[0][1])
    return url


def git_get_origin(repo_dir):
    import re
    with open('%s/.git/config' % repo_dir, mode='r') as f:
        data = f.read()

        remote_patt = re.compile('\[remote .*')
        prop_patt = re.compile('^\s+([a-z]+)\s*=\s*([\W\w]+)$')

        block_started = True
        for line in data.split('\n'):
            if len(line) < 1:
                continue
            if remote_patt.match(line):
                block_started = True
                continue
            if block_started:
                match = prop_patt.findall(line)
                if not match:
                    block_started = False
                    continue
                if match[0][0] == 'url':
                    return match[0][1]
                print(match)


        raise RuntimeError('Failed to parse')


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
    origin = git_get_origin(repo_dir)
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
