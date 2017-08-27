#!/usr/bin/env python3

from lxml import etree

from argparse import ArgumentParser


def read_xml(file):
    with open(file, mode='r') as f:
        return etree.fromstring(f.read())


def add_resource(element, args):
    rsc_tree = None
    for e in element:
        if e.tag.endswith('ItemGroup'):
            for e2 in e:
                if e2.tag.endswith('AppxPackagedFile'):
                    rsc_tree = e
                    break

    if rsc_tree is None:
        return

    ns = element.nsmap[None]

    new_rsc = etree.Element('{%s}AppxPackagedFile' % ns)
    new_rsc.attrib['Include'] = args.pop(0)

    ppath = etree.Element('{%s}PackagePath' % ns)
    ppath.text = args.pop(0)
    rereg = etree.Element('{%s}ReRegisterAppIfChanged' % ns)
    rereg.text = 'true'

    new_rsc.append(ppath)
    new_rsc.append(rereg)

    rsc_tree.append(new_rsc)


def main():
    args = ArgumentParser('AppX recipe manipulator',
                          description='Allows inserting information into AppX recipes')

    args.add_argument('input_file', metavar='input', type=str,
                      help='path to *.appxrecipse file')

    args.add_argument('command',  type=str, choices=['add_resource'],
                      help='command to modify file with')

    args.add_argument('extra_args', nargs='+',
                      help='depends on command')

    args = args.parse_args()

    tree = read_xml(args.input_file)

    if args.command == 'add_resource':
        add_resource(tree, args.extra_args)

    with open('test.xml', mode='w') as f:
        f.write(etree.tostring(tree, pretty_print=True).decode())

    return 0


if __name__ == '__main__':
    exit(main())
