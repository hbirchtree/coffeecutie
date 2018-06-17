from versioning import GLVersion
from command_filtering import GLType, GLBaseType, GLBaseTypes
import re

class GLExtension:
    def __init__(self):
        self.name = None # str
        self.api = [] # [str]
    
    def __str__(self):
        return 'GLExtension(' + self.name + ')'

    def __repr__(self):
        return str(self)

class GLArgument:
    def __init__(self, arg_type=None, arg_name='', arg_group=None):
        self.atype = arg_type
        self.name = arg_name
        self.group = arg_group

    def as_typed(self):
        return '%s %s' % (str(self.atype), self.name)

    def as_name(self):
        return self.name

    def as_grouped(self):
        return '%s %s' % (self.group, self.name)

def arguments_to_string(arguments, typed=True):
    out = ''

    for arg in arguments:
        if arg is None:
            continue
        out += (arg.as_name() if not typed else arg.as_typed()) + ', '

    if arguments:
        out = out[:-2]

    return out

class GLCommand:
    def __init__(self):
        self.ret_type = None # str
        self.name = None # str
        self.original_name = None # str
        self.args = [] # [(name, type, group), ...]
        self.min_api = [GLVersion(), GLVersion()]
        self.max_api = [GLVersion(), GLVersion()]
        self.extensions = []
        self.fallback = []
        self.source_element = None
   
    # Whether this is an extension-only function
    def is_extension(self):
        return self.min_api[0].isnone and \
                self.min_api[1].isnone and \
                self.extensions

    def template_check(self):
        if self.is_extension():
            # No version, extension only
            return '/* No template check available */'
        if self.min_api[0].isnone and not self.min_api[1].isnone:
            # ES-exclusive function
            return 'GL_VERSION_REQ_ES(%s)' % self.min_api[1].template_str()
        elif not self.min_api[0].isnone and self.min_api[1].isnone:
            # Desktop-exclusive function
            return 'GL_VERSION_REQ_DESKTOP(%s)' % self.min_api[0].template_str()
        else:
            return 'GL_VERSION_REQ_COMBO(%s, %s)' % \
                (self.min_api[0].template_str(), self.min_api[1].template_str())

    def compile_check(self):
        if self.is_extension():
            return '0'
        else:
            return 'GL_VERSION_VERIFY(%s, %s)' % \
                    (self.min_api[0].compile_str(),
                     self.min_api[1].compile_str())

    def extension_check(self):
        if len(self.extensions) and False:
            out = ''
            for ext in self.extensions:
                out += ' || (defined(%s) && %s)' % (ext.name, ext.name)
            return out
        return ''

    def ptr_check(self):
        base = 'if(!%s)\n    {' % self.original_name

        for ext in self.fallback:
            base += '\n#if defined(%s) && %s' % (ext[1].name, ext[1].name)
            base += '\n        if(%s) Throw(undefined_behavior("extension %s is available"));' %\
                    (ext[0].original_name, ext[1].name)
            base += '\n#endif'

        base += '\n        Throw(undefined_behavior("function not loaded!"));'

        base += '\n    }'

        return base

    def returns_value(self):
        return str(self.ret_type) != 'void'
    
    def param_string(self, function_prefix='', function_arguments=None, command_arguments=None):
        return \
'''
#if %s%s
%s
%s%s %s(%s)
{
#ifndef NDEBUG
    %s
#endif
    %s%s(%s);
}
#endif
''' % (self.compile_check(), self.extension_check(), self.template_check(),
       function_prefix,
       self.ret_type, self.name,
       arguments_to_string(self.args) if function_arguments is None else function_arguments,
       self.ptr_check(),
       'return ' if self.returns_value() else '',
       self.original_name,
       arguments_to_string(self.args, False) if command_arguments is None else command_arguments
       )

    def __str__(self):
        return self.param_string()

    def __repr__(self):
        return self.name

def set_cmd_version(cmd_obj, version_desc):
    if version_desc.api == 'ES' and \
        version_desc.islowerthan(cmd_obj.min_api[1]):
        cmd_obj.min_api[1] = version_desc
    elif version_desc.islowerthan(cmd_obj.min_api[0]):
        cmd_obj.min_api[0] = version_desc

def extract_commands(registry, commands, cmd_names, version_desc):
    for cmd in registry.find('commands').findall('command'):
        cmd_name = cmd.find('proto').findtext('name')
        
        if cmd_name not in cmd_names:
            continue
        
        if cmd_name in commands:
            cmd_obj = commands[cmd_name]
            set_cmd_version(cmd_obj, version_desc)
            
        else:
            ret_type = ''.join(''.join(cmd.find('proto').itertext()).rsplit(cmd_name, 1))

            cmd_obj = GLCommand()
            cmd_obj.ret_type = GLType.from_string(ret_type.strip())
            cmd_obj.original_name = cmd_name
            cmd_obj.name = cmd_name
            cmd_obj.source_element = cmd
            
            for arg in cmd.findall('param'):
                arg_obj = GLArgument()
                arg_obj.name = arg.findtext('name')
                arg_obj.atype = GLType.from_string(''.join(''.join(arg.itertext()).rsplit(arg_obj.name, 1)))
                arg_obj.group = arg.get('group')

                cmd_obj.args += [arg_obj]
            
            set_cmd_version(cmd_obj, version_desc)
                    
            commands[cmd_name] = cmd_obj

#
