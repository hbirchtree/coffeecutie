#!/usr/bin/env python3

from sys import argv

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
        print('%s: No such file' % source_file)


with open(argv[1], 'w') as out_file:
    out_file.write(json.dumps(content))
