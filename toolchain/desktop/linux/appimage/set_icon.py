#!/usr/bin/env python

#
# Source: https://stackoverflow.com/q/3341291
#

import sys
from gi.repository import Gio

if len(sys.argv) not in (2, 3):
    print 'Usage: {} FILE [ICON]'.format(sys.argv[0])
    print 'Leave out ICON to unset'
    sys.exit(0)

file = Gio.File.new_for_path(sys.argv[1])
icon_file = Gio.File.new_for_path(sys.argv[2]) if len(sys.argv) == 3 else None

# Get a file info object
info = file.query_info('metadata::custom-icon', 0, None)

if icon_file is not None:
    icon_uri = icon_file.get_uri()
    info.set_attribute_string('metadata::custom-icon', icon_uri)
else:
    # Change the attribute type to INVALID to unset it
    info.set_attribute('metadata::custom-icon',
        Gio.FileAttributeType.INVALID, '')

# Write the changes back to the file
file.set_attributes_from_info(info, 0, None)
