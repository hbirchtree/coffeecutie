#!/usr/bin/env python3

from sys import argv
from os.path import realpath

import json

with open(argv[1]) as in_file:
    content = json.loads(in_file.read())

if 'sources' not in content:
    print('-- File has no "sources" list')
    exit(1)

content['sourcesContent'] = []

for source_file in content['sources']:
    try:
        with open(source_file) as source:
            content['sourcesContent'].append(source.read())
    except IOError:
        print('%s: No such file' % realpath(source_file))
    except UnicodeDecodeError as e:
        print('%s: Decode error: %s' % (realpath(source_file), e))

with open(argv[1], 'w') as out_file:
    out_file.write(json.dumps(content))
