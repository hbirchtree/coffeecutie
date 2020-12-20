#!/usr/bin/env python

from argparse import ArgumentParser
from os.path import dirname
from os.path import isfile
from os import stat
from glob import glob

if __name__ == '__main__':
    args = ArgumentParser('DirToHeader')

    args.add_argument(
        '--output', type=str,
        help='Output header file')
    args.add_argument(
        'dirs', nargs='*')

    args = args.parse_args()

    files = {}

    max_vfs_name_length = 0
    total_vfs_length = 0

    for dir in args.dirs:
        prefix_len = len(dir)
        for file in glob(dir + '/**', recursive=True):
            if not isfile(file):
                continue

            vfs_path = file[prefix_len:]
            files[vfs_path] = file

            max_vfs_name_length = max(max_vfs_name_length, len(vfs_path))
            total_vfs_length = total_vfs_length + stat(file).st_size

    with open(args.output, 'w+') as output:
        output.write(
'''#include <array>
#if __cplusplus__ >= 201703L
#include <string_view>
#endif

#include <platforms/embed/file.h>

static const std::array<const unsigned char, %s> embed_vfs_data = {{''' % total_vfs_length)
        offset = 0

        descriptors = []

        for vfs_path in files:
            file_path = files[vfs_path]
            spacing = ''
            for i in range(max_vfs_name_length - len(vfs_path)):
                spacing = spacing + ' '
            print('Mapping {}{} -> {}'.format(vfs_path, spacing, file_path))

            with open(file_path, 'rb') as file:
                content = file.read()
                content_len = len(content)
                descriptors.append((vfs_path, offset, content_len))
                offset = offset + content_len
                for i, b in enumerate(content):
                    if i % 15 == 0:
                        output.write('\n    ')
                    output.write('0x%02x,' % b)

        output.write('''
}};

static const std::array<platform::file::embed::data_descriptor_t,''' + '{}'.format(len(files)) + '''> embed_vfs_descriptors = {{
''')
        for desc in descriptors:
            output.write('    {"%s", %s, %s},\n' % desc)

        output.write('''
}};

namespace platform {
namespace file {
namespace embed {

bool embeds_enabled = true;

bool file_lookup(file_reference_t query, semantic::Bytes& data)
{
    auto it = std::find_if(
        embed_vfs_descriptors.begin(),
        embed_vfs_descriptors.end(),
        [query](data_descriptor_t const& e) {
            return std::string(e.name) == query;
        });

    if(it == embed_vfs_descriptors.end())
        return false;

    data = semantic::Bytes::From(&embed_vfs_data.at(it->offset), it->size);
    return true;
}

}
}
}
''')
