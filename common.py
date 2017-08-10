from os.path import dirname, basename, realpath, curdir

from os.path import isfile, isdir


local_yml_filespec = '.local.yml'
build_yml_filespec = '.build.yml'


def configure_string(src_text, variables):
    for var in variables:
        src_text = src_text.replace('@%s@' % (var,),
                                    variables[var])
    return src_text


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

    with open(trg_file, 'w') as trg_fd:
        if src_text is not None:
            trg_fd.write(src_text)
