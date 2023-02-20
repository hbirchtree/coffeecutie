#!/usr/bin/env python

from sys import argv
import xml.etree.ElementTree as etree
from xml.dom import minidom

def main():
    svg1 = etree.parse(argv[1]).getroot()
    svg2 = etree.parse(argv[2]).getroot()

    elements = []
    for svg in [svg1, svg2]:
        elements = elements + svg.findall('./{http://www.w3.org/2000/svg}defs')
        elements = elements + svg.findall('./{http://www.w3.org/2000/svg}g')

    root = etree.Element('svg')
    root.set('viewBox', '0 0 108 108')
    root.set('xmlns', 'http://www.w3.org/2000/svg')
    root.set('xmlns:svg', 'http://www.w3.org/2000/svg')
    root.set('version', '1.1')
    for g in elements:
        root.append(g)

    out = etree.tostring(root, encoding='UTF-8').decode('utf-8')
    print(out.replace('<ns0:', '<').replace('</ns0:', '</'))

if __name__ == '__main__':
    main()
