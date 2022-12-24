#!/usr/bin/env python3

from argparse import ArgumentParser
from os.path import dirname
from os.path import isfile
from os import stat
from glob import glob

def padding(size):
    return 8 - (size & 0x7)

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
            file_size = stat(file).st_size
            total_vfs_length = total_vfs_length + file_size + padding(file_size)

    with open(args.output, 'w+') as output:
        output.write(
'''#include <array>
#include <string_view>

#include <platforms/embed/file.h>

using namespace std::string_view_literals;

static const std::array<const libc_types::u8, %s> embed_vfs_data = {{''' % total_vfs_length)
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
                output.write('\n\n    ')
                for i in range(padding(content_len)):
                    if i == 0:
                        output.write('/* padding */ ')
                    output.write('0x0,')

        output.write('''
}};

static const std::array<platform::file::embed::data_descriptor_t,''' + str(len(files)) + '''> embed_vfs_descriptors = {{
''')
        for desc in descriptors:
            output.write('    {"%s"sv, %s, %s},\n' % desc)

        output.write('''
}};

namespace platform::file::embed {

const bool embeds_enabled = true;

namespace detail {

semantic::Span<const data_descriptor_t> files_listing()
{
    if(!embeds_enabled || !embed_vfs_descriptors.size())
        return {};
    return semantic::Span<const data_descriptor_t>(embed_vfs_descriptors);
}

semantic::Span<const libc_types::u8> files_data()
{
    if(!embeds_enabled || !embed_vfs_descriptors.size())
        return {};
    return semantic::Span<const libc_types::u8>(embed_vfs_data);
}

} // namespace detail
} // namespace platform::file::embed
''')
