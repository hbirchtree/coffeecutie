#!/usr/bin/env python

from base64 import b64encode
from os import walk
from os.path import basename
from sys import argv

source_dir = argv[1]
output_file = argv[2]

print(f'Traversing directory: {source_dir}')

images = set()
logs = set()
test_runs = set()

for (dirpath, _, filenames) in walk(source_dir):
    if dirpath == source_dir:
        continue
    test_runs.add(dirpath)
    for f in filenames:
        if f.endswith('.jpg'):
            images.add(basename(f))
        if f.endswith('.log'):
            logs.add(basename(f))

images = sorted(list(images))
logs = sorted(list(logs))
test_runs = sorted(list(test_runs))

print(f'''Found test runs: {', '.join(test_runs)}
Found images: {', '.join(images)}
Found logs: {', '.join(logs)}
''')

with open(output_file, 'w+') as results:
    results.write('''<html>
<head>
    <title>Test results</title>
    <style>
html {
    background-color: black;
    color: white;
}
th {
    font-weight: 2000;
}
td {
    font-weight: 100;
}
td.fail {
    background-color: #800;
    text-align: center;
}
textarea {
    background-color: black;
    color: white;
}
    </style>
</head>
<body>
<table>
    <tr>
        <th>Screenshot</th>''')
    tab2 = ' ' * 8
    for test_run in test_runs:
        results.write(f'\n{tab2}<th>{basename(test_run)}</th>')
    results.write('\n    </tr>')
    for img in images:
        results.write('\n    <tr>')
        results.write(f'\n{tab2}<td>{img}</td>')
        for test_run in test_runs:
            img_path = f'{test_run}/{img}'
            try:
                with open(img_path, 'rb+') as img_file:
                    results.write(f'\n{tab2}<td><img src="data:image/jpeg;base64,{b64encode(img_file.read()).decode()}"/></td>')
            except FileNotFoundError:
                print(f'ERROR: Image "{img_path}" expected, but not found')
                results.write(f'\n{tab2}<td class="fail">DNF</td>')
        results.write('\n    </tr>')
    results.write('\n    <tr><th>Logs</th></tr>')
    for log in logs:
        results.write('\n    <tr>')
        results.write(f'\n{tab2}<td>{log}</td>')
        for test_run in test_runs:
            log_path = f'{test_run}/{log}'
            try:
                with open(log_path, 'r+') as log_file:
                    results.write(f'\n{tab2}<td><textarea rows="100" cols="80">{log_file.read()}</textarea></td>')
            except FileNotFoundError:
                results.write(f'\n{tab2}<td>No logs</td>')
        results.write('\n    </tr>')
    results.write('''
</table>
</body>
</html>''')
