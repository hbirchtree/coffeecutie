#!/usr/bin/env python3
import re
from copy import deepcopy
from collections import defaultdict

TRANSLATE_TYPE_RGX = r'^(.*?)(const)?(\*)$'
CONST_EXTRACT_RGX = r'^(const )?(.*)$'

# Mapping of GL types
TYPE_MAP = {
    'GLubyte': 'libc_types::u8',
    'GLbyte': 'libc_types::i8',
    'GLushort': 'libc_types::u16',
    'GLshort': 'libc_types::i16',
    'GLbitfield': 'libc_types::u32',
    'GLuint': 'libc_types::u32',
    'GLint': 'libc_types::i32',
    'GLuint64': 'libc_types::u64',
    'GLuint64EXT': 'libc_types::u64',
    'GLint64': 'libc_types::i64',
    
    'GLsizei': 'libc_types::i32',
    'GLsizeiptr': 'libc_types::ptroff',
    'GLintptr': 'libc_types::ptroff',
    
    'GLfloat': 'libc_types::f32',
    'GLdouble': 'libc_types::f64',
    'GLboolean': 'libc_types::u8',

    'const GLchar *': 'libc_types::cstring ',
    'GLchar *': 'libc_types::cstring_w ',
    'void *': 'libc_types::c_ptr',
    'const void *': 'libc_types::c_cptr ',

    'GLiptr': 'libc_types::ptroff'
}
#

class GLBaseType:
    def __init__(self, type_name='libc_types::i32', is_const=False):
        self.type_name = type_name
        self.is_const = is_const

    def is_base(self):
        return True

    def underlying_type(self):
        return str(self)

    @staticmethod
    def from_string(type_string=''):
        return GLBaseType('', False)

    def __str__(self):
        return ('const ' if self.is_const else '') + self.type_name

class GLBaseTypes:
    GLenum = GLBaseType(type_name='libc_types::i32')
    GLfloat = GLBaseType(type_name='libc_types::f32')
    GLint = GLBaseType(type_name='libc_types::i32')

# Translate base types of possibly composite types
def translate_type(gl_type, command_name = None):
    if command_name is not None and 'GetString' in command_name:
        gl_type = GLBaseType(TYPE_MAP['const GLchar *'])
        return gl_type

    if gl_type.is_base():
        if gl_type.type_name in TYPE_MAP:
            gl_type.type_name = TYPE_MAP[gl_type.type_name]
    else:
        translate_type(gl_type.base_type, command_name)

    return gl_type

# Extract recursive type information
def get_types(string_type):
    inner_type = re.findall(TRANSLATE_TYPE_RGX, string_type.strip())

    # We're at the inner type
    if not inner_type:
        # Attempt to find a const on it
        const_des = re.findall(CONST_EXTRACT_RGX, string_type.strip())
        const_des = const_des[0]

        return GLBaseType(is_const=(const_des[0].strip() == 'const'), type_name=const_des[1])
    else:
        inner_type = inner_type[0]
        return (inner_type[1] == 'const',
                inner_type[2] == '*',
                get_types(inner_type[0]))

class GLType:
    def __init__(self, base_type=None, is_const=False, is_ptr=False):
        self.base_type = base_type
        self.is_const = is_const
        self.is_ptr = is_ptr

    def is_base(self):
        return False

    def underlying_type(self):
        return self.base_type.underlying_type()

    @staticmethod
    def recursive_type(tuple_info):
        if isinstance(tuple_info[2], GLBaseType):
            return GLType(is_const=tuple_info[0],
                    is_ptr=tuple_info[1],
                    base_type=tuple_info[2])
        else:
            return GLType(is_const=tuple_info[0],
                    is_ptr=tuple_info[1],
                    base_type=GLType.recursive_type(tuple_info[2]))

    @staticmethod
    def from_string(type_string=''):
        out = GLType()
        declaration = get_types(type_string)

        if isinstance(declaration, GLBaseType):
            out.base_type = declaration
        else:
            out = GLType.recursive_type(declaration)

        return out

    def __str__(self):
        return str(self.base_type) + (' *' if self.is_ptr else '') + ('const' if self.is_const and self.is_ptr else '')



# Filtering the OpenGL function names!

ACTION_TYPE_MAP = {
    'Gen': 'Alloc',
    'Create': 'AllocEx',
    'Delete': 'Free',
    'Bind': 'Bind'
}

RESOURCE_TYPE_PRIORITY_MAP = [
    'Framebuffers',
    'Framebuffer',

    'Textures',
    'Samplers',
    
    'ShaderProgramv',
    'ProgramPipelines',
    'Pipelines',
    'ActiveUniformBlock',
    'ActiveUniform',
    'UniformBlock',
    'ProgramUniform',
    'ShaderStorageBlock',
    'ActiveAttrib',

    'VertexArrayAttrib',
    'VertexAttrib',
    'VertexArrays',
    'VertexArray',

    'VertexBuffers',
    'VertexBuffer',
    
    'Buffers',
    'NamedBuffer',
   
    'Renderbuffers',
    'NamedFramebuffer',
    'NamedRenderbuffer',
    
    'ElementBuffer',
    
    'Attrib',
    'TransformFeedbacks',
]

RESOURCE_TYPE_MAP = {
    'Texture': 'Tex',
    'Textures': 'Tex',

    'ProgramPipeline': 'Pipeline',
    'ProgramPipelines': 'Pipeline',
    'Pipelines': 'Pipeline',
    'Uniform': 'Unif',
    'ProgramUniform': 'Unif',
    'ActiveUniformBlock': 'ActiveUnifBlock',
    'UniformBlock': 'UnifBlock',
    'ShaderStorageBlock': 'SSBO',
    'ActiveAttrib': 'ActiveAttrib',
    'ActiveUniform': 'ActiveUnif',
    
    'Buffers': 'Buf',
    'Buffer': 'Buf',
    'NamedBuffer': 'Buf',
    
    'Renderbuffer': 'RBuf',
    'Renderbuffers': 'RBuf',
    'NamedRenderbuffer': 'RBuf',
    'Framebuffer': 'FB',
    'Framebuffers': 'FB',
    'NamedFramebuffer': 'FB',

    'TransformFeedbacks': 'XF',
    'TransformFeedback': 'XF',
    
    'Boolean': 'Bool',
    'Float': 'Scalarf',
    'Double': 'Scalar',

    'VertexArrayAttrib': 'VAO',
    'VertexAttrib': 'VAO',
    'VertexArray': 'VAO',
    'VertexArrays': 'VAO',
    'Vertex': 'VAO',

    'ElementBuffer': 'ElementBuf',
    'VertexBuffer': 'VertBuf',
    'VertexBuffers': 'VertBuf',

    'Queries': 'Query',
    'Samplers': 'Sampler'
}

RESOURCE_TYPES_LIST = [
    'CompressedTexture'
    'CompressedTex|Textures|Texture|Tex',
    'Samplers|Sampler|ImageTexture',

    'ConditionalRender',

    'Sync',

    'VertexBuffers',
    'VertexArrayAttrib',
    'VertexArrays|VertexArray|VertexAttrib|Vertex',
    
    'ActiveUniforms|ActiveUniform',
    'ActiveUniformBlock|UniformBlock',
    'ActiveAttrib',
    'ProgramUniform|Uniform',
    'ProgramPipelines|ProgramPipeline|Program|AttachedShaders|Shader',
    'ShaderStorageBlock',
    'ShaderProgramv',
    
    'Float',
    'Integer|Double|Boolean',
    
    'NamedRenderbuffer|Renderbuffers|Renderbuffer',
    'NamedFramebuffer|Framebuffers|Framebuffer',
    
    'Query|Queries|NamedBuffer|Buffers|Buffer',
    
    'TransformFeedbacks|TransformFeedback',
    
    'Error'
]
    
RESOURCE_TYPES = ''

for rsrc_type in RESOURCE_TYPES_LIST:
    RESOURCE_TYPES += rsrc_type + '|'

RESOURCE_TYPES = RESOURCE_TYPES[:-1]
RESOURCE_TYPES = RESOURCE_TYPES.split('|')

def map_action_type(action):
    try:
        return ACTION_TYPE_MAP[action]
    except KeyError:
        raise RuntimeError(action)
        return action

def map_resource_type(rsrc_type):
    try:
        return RESOURCE_TYPE_MAP[rsrc_type]
    except KeyError:
        return rsrc_type

def translate_move_get(cmd_name):
    GETTER_RGX = r"^(Getn|Get)(%s)(.*)$"
    # Move use of Get* in command name
    for rtype in RESOURCE_TYPES:
        get = re.findall(GETTER_RGX % rtype, cmd_name)
        if len(get):
            get = get[0]
            cmd_name = '%s%s%s' % (get[1], get[0], get[2])
    return cmd_name

def translate_move_action(cmd_name):
    ACTION_RGX = r"^(Bind|Gen|Create|Delete)(%s)$"
    # Rename CreateBuffers to BufAlloc and similar
    for rtype in RESOURCE_TYPES:
        get = re.findall(ACTION_RGX % rtype, cmd_name)
        if len(get):
            get = get[0]
            cmd_name = '%s%s' % (map_action_type(get[0]), get[1])
    return cmd_name

def translate_move_object(cmd_name):
    OBJECT_RGX = r"^(.*)(%s)(.*)$"
    # Do prepass for prioritized types
    for rtype in RESOURCE_TYPE_PRIORITY_MAP:
        get = re.findall(OBJECT_RGX % rtype, cmd_name)
        if len(get):
            get = get[0]
            if 'Is' in get[0]:
                continue
            cmd_name = '%s%s%s' % (map_resource_type(get[1]), get[0], get[2])
            return cmd_name
    # Rename InvalidateBufferSubdata into BufInvalidateSubdata and similar
    for rtype in RESOURCE_TYPES:
        get = re.findall(OBJECT_RGX % rtype, cmd_name)
        if len(get):
            get = get[0]
            if 'Is' in get[0]:
                continue
            cmd_name = '%s%s%s' % (map_resource_type(get[1]), get[0], get[2])
            return cmd_name
    return cmd_name
 
UNIF_RGX = '^Unif(Matrix)?([0-9](x[0-9])?)(.*v)$'

def translate_cmd_name(cmd_name):
    cmd_name = cmd_name[2:]

    if 'Draw' in cmd_name:
        return cmd_name
   
    #cmd_name = translate_move_get(cmd_name)
    cmd_name = translate_move_action(cmd_name)
    cmd_name = translate_move_object(cmd_name)

    if 'Unif' in cmd_name:
        uni = re.findall(UNIF_RGX, cmd_name)

        if uni:
            cmd_name = 'Unif%s' % uni[0][3]

    return cmd_name
#

TRANSLATE_GL_GROUP = {
    'TextureTarget': 'Texture',
    'BufferStorageTarget': 'BufType',
    'BufferTargetARB': 'BufType',
    'FramebufferTarget': 'FramebufferT',
    'QueryTarget': 'QueryT'
}

class InlineArgExpression:
    def __init__(self, aname, atype, expression=None):
        self.name = aname
        self.exp = expression
        self.atype = atype

    def __repr__(self):
        return '%s %s -> %s' % (self.name, self.atype, self.exp)

    def as_typed(self):
        return '%s %s' % (self.atype, self.name)

    def as_name(self):
        return self.exp if self.exp is not None else self.name

InArgExp = InlineArgExpression

class ArgumentTransform:
    def __init__(self, args, cmd_name, og_name):
        self.cmd_name = cmd_name
        self.og_name = og_name
        self.args = deepcopy(args)
        self.args_consumed = [None for x in range(len(args))]
        self.cmd_inputs = [None for x in range(len(args))]
        self.function_parameters = [None for x in range(len(args))]
        self.return_type = None

    def consume(self, arg):
        self.args_consumed += [arg]

    def is_consumed(self, arg):
        return arg in self.args_consumed

    def emit_command_input(self, arg, argex):
        self.cmd_inputs[self.args.index(arg)] = argex

    def emit_function_param(self, arg, argex):
        self.function_parameters[self.args.index(arg)] = argex

    def remap_type(self, arg, new_type, new_expression):
        self.emit_command_input(arg, InArgExp(arg.name, arg.atype, new_expression))
        self.emit_function_param(arg, InArgExp(arg.name, new_type))
        self.consume(arg)

    def find_by_name(self, name=''):
        for arg in self.args:
            if self.is_consumed(arg):
                continue
            if arg.name == name:
                return arg
        return None

    def parse(self):
        # Full-set argument transform, consume multiple args
        arg1 = self.find_by_name('data')
        arg2 = self.find_by_name('size')

        if arg1 is None and arg2 is None:
            arg1 = self.find_by_name('binary')
            arg2 = self.find_by_name('bufSize')

        # Replace size and data with a Bytes const&
        if arg1 is not None and arg2 is not None:
            self.consume(arg1)
            self.consume(arg2)
            argument_type = 'semantic::Bytes'
            if 'const' in arg1.atype.underlying_type():
                argument_type = 'semantic::BytesConst'
            self.emit_command_input(arg1, InArgExp(arg1.name, arg1.atype, '%s.data' % arg1.name))
            self.emit_command_input(arg2, InArgExp(arg2.name, arg2.atype, 'C_FCAST<%s>(%s.size)' % (arg2.atype, arg1.name)))
            self.emit_function_param(arg1, InArgExp(arg1.name, GLBaseType('%s const&' % argument_type)))

        arg1 = self.find_by_name('format')
        arg2 = self.find_by_name('type')

        if arg1 is not None and arg2 is not None:
            self.consume(arg1)
            self.consume(arg2)
            self.emit_command_input(arg2, InArgExp(arg2.name, arg2.atype, 'to_enum(%s)' % arg2.name))
            if arg1.group == 'PixelFormat':
                self.emit_command_input(arg1, InArgExp(arg1.name, arg1.atype, 'to_enum(%s, typing::pixels::PixFmt::None)' % arg1.name))
                self.emit_function_param(arg1, InArgExp(arg1.name, GLBaseType('typing::PixCmp')))
            else:
                self.emit_command_input(arg1, InArgExp(arg1.name, arg1.atype, 'to_enum(%s)' % arg1.name))
                self.emit_function_param(arg1, InArgExp(arg1.name, GLBaseType('typing::pixels::PixFmt')))
            self.emit_function_param(arg2, InArgExp(arg2.name, GLBaseType('typing::pixels::BitFmt')))

        # Using BitFmt and TypeEnum

        arg1 = self.find_by_name('value')
        arg2 = self.find_by_name('count')

        if arg1 is None:
            arg1 = self.find_by_name('values')

        # For gl*Uniform*v calls, use a typed Span<T>, and include the count parameter
        if arg1 is not None and arg2 is not None and not arg1.atype.is_base() and 'Unif' in self.cmd_name:
            UNIF_RGX = r'^.*?Uniform(Matrix)?([2-9](x[2-9])?)(.*)v$'
            self.consume(arg1)
            self.consume(arg2)
            span_type = arg1.atype.base_type
            unif_info = re.findall(UNIF_RGX, self.og_name)
            if unif_info:
                unif_info = unif_info[0]
                if unif_info[0] == 'Matrix':
                    span_type = 'typing::vector_types::Mat'
                else:
                    span_type = 'typing::vector_types::Vec'
                span_type += unif_info[3]
                span_type += unif_info[1].replace('x', '_')
                
            self.emit_command_input(arg1, InArgExp(arg1.name, arg1.atype, 'C_RCAST<%s*>(%s.data())' % (arg1.atype.base_type, arg1.name)))
            self.emit_command_input(arg2, InArgExp(arg2.name, arg2.atype, 'C_FCAST<%s>(%s.size())' % (arg2.atype, arg1.name)))
            self.emit_function_param(arg1, InArgExp(arg1.name, GLBaseType('semantic::Span<%s> const&' % (span_type, ))))

        arg1 = self.find_by_name('x') or self.find_by_name('v0')
        arg2 = self.find_by_name('y') or self.find_by_name('v1')
        arg3 = self.find_by_name('z') or self.find_by_name('v2')
        arg4 = self.find_by_name('w') or self.find_by_name('v3')

        if arg1 is not None and arg2 is not None:
            self.consume(arg1)
            self.consume(arg2)

            vec_template = 'typing::vectors::tvector<%s, %s>'
            vec_base = str(arg1.atype.base_type)

            vec_type = vec_template % (vec_base, 2)
            if arg3 is not None:
                vec_type = vec_template % (vec_base, 3)
                self.consume(arg4)
                self.emit_command_input(arg3, InArgExp(arg1.name, arg1.atype, '%s[2]' % arg1.name))
            if arg4 is not None:
                vec_type = vec_template % (vec_base, 4)
                self.consume(arg4)
                self.emit_command_input(arg4, InArgExp(arg1.name, arg1.atype, '%s[3]' % arg1.name))
            self.emit_command_input(arg1, InArgExp(arg1.name, arg1.atype, '%s[0]' % arg1.name))
            self.emit_command_input(arg2, InArgExp(arg1.name, arg1.atype, '%s[1]' % arg1.name))
            self.emit_function_param(arg1, InArgExp(arg1.name, GLBaseType(vec_type)))

        # For passing N IDs to a function, use Span<T>
        if 'Alloc' in self.cmd_name or 'Free' in self.cmd_name or 'Arrayv' in self.cmd_name or 'Debug' in self.cmd_name:
            arg1 = self.find_by_name('n') or self.find_by_name('count')

            try:
                arg2 = self.args[self.args.index(arg1) + 1]
            except IndexError:
                arg2 = None
            except ValueError:
                arg2 = None

            if arg2 is not None and \
                    arg1 is not None and \
                    arg1.atype.base_type.is_base() and \
                    arg1.atype.base_type.type_name in ['libc_types::ptroff', 'libc_types::i32']:
                self.consume(arg1)
                self.consume(arg2)

                self.emit_command_input(arg1, InArgExp(arg1.name, arg1.atype, 'C_FCAST<%s>(%s.size())' % (arg1.atype, arg2.name)))
                self.emit_command_input(arg2, InArgExp(arg2.name, arg2.atype, '%s.data()' % arg2.name))
                self.emit_function_param(arg2, InArgExp(arg2.name, GLBaseType('semantic::Span<%s>&&' % arg2.atype.base_type)))


        arg1 = self.find_by_name('width')
        arg2 = self.find_by_name('height')
        arg3 = self.find_by_name('depth')
 
        # Make us of Size* structs when possible
        if arg1 is not None and arg2 is not None:
            self.consume(arg1)
            self.consume(arg2)

            arg_base = 'size' if self.find_by_name('size') is not None else arg1.name
            
            if arg3 is not None:
                self.consume(arg3)
                self.emit_command_input(arg1, InArgExp(arg_base, arg1.atype, '%s.width' % arg_base))
                self.emit_command_input(arg2, InArgExp(arg_base, arg1.atype, '%s.height' % arg_base))
                self.emit_command_input(arg3, InArgExp(arg_base, arg1.atype, '%s.depth' % arg_base))
                self.emit_function_param(arg1, InArgExp(arg_base, GLBaseType('typing::geometry::size_3d<libc_types::i32> const&')))
            else:
                self.emit_command_input(arg1, InArgExp(arg_base, arg1.atype, '%s.w' % arg_base))
                self.emit_command_input(arg2, InArgExp(arg_base, arg1.atype, '%s.h' % arg_base))
                self.emit_function_param(arg1, InArgExp(arg_base, GLBaseType('typing::geometry::size_2d<libc_types::i32> const&')))

        arg1 = self.find_by_name('xoffset') or self.find_by_name('x')
        arg2 = self.find_by_name('yoffset') or self.find_by_name('y')
        arg3 = self.find_by_name('zoffset')
 
        if arg1 is not None and arg2 is not None and 'i32' in str(arg1.atype.base_type):
            self.consume(arg1)
            self.consume(arg2)

            arg_base = 'offset' if self.find_by_name('offset') is not None else arg1.name

            if arg3 is not None:
                self.consume(arg3)
                self.emit_command_input(arg1, InArgExp(arg_base, arg1.atype, '%s.x' % arg_base))
                self.emit_command_input(arg2, InArgExp(arg_base, arg1.atype, '%s.y' % arg_base))
                self.emit_command_input(arg3, InArgExp(arg_base, arg1.atype, '%s.z' % arg_base))
                self.emit_function_param(arg1, InArgExp(arg_base, GLBaseType('typing::geometry::point_3d<libc_types::i32> const&')))
            else:
                self.emit_command_input(arg1, InArgExp(arg_base, arg1.atype, '%s.x' % arg_base))
                self.emit_command_input(arg2, InArgExp(arg_base, arg1.atype, '%s.y' % arg_base))
                self.emit_function_param(arg1, InArgExp(arg_base, GLBaseType('typing::geometry::point_2d<libc_types::i32> const&')))

        arg1 = self.find_by_name('red')
        arg2 = self.find_by_name('green')
        arg3 = self.find_by_name('blue')
        arg4 = self.find_by_name('alpha')

        if arg1 is not None and arg2 is not None and\
                arg3 is not None and arg4 is not None:
            self.consume(arg1)
            self.consume(arg2)
            self.consume(arg3)
            self.consume(arg4)
            arg_base = 'color' if self.find_by_name('color') is None else arg1.name
            self.emit_command_input(arg1, InArgExp(arg_base, arg1.atype, '%s.r()' % arg_base))
            self.emit_command_input(arg2, InArgExp(arg_base, arg2.atype, '%s.g()' % arg_base))
            self.emit_command_input(arg3, InArgExp(arg_base, arg3.atype, '%s.b()' % arg_base))
            self.emit_command_input(arg4, InArgExp(arg_base, arg4.atype, '%s.a()' % arg_base))
            self.emit_function_param(arg1, InArgExp(arg_base, GLBaseType('typing::vectors::tvector<%s,4> const&' % arg1.atype)))
 
        arg1 = None
        arg2 = None
        arg3 = None
        arg4 = None

        # Single-argument transforms
        for arg in self.args:
            if self.is_consumed(arg):
                continue

            if arg.name in ['access', 'usage', 'flags']:
                # Use RSCA in place of access and usage enums with RSCA
                # We may use different to_enum* functions, varying
                enum_idx = 1
                if arg.name in ['access', 'flags']:
                    enum_idx = 2

                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'to_enum%s(%s)' % (enum_idx, arg.name)))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('semantic::RSCA')))
                self.consume(arg)
            elif arg.name == 'border' and 'Tex' in self.cmd_name:
                # Remove the border argument for texture functions
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, '0'))
                self.consume(arg)
            elif arg.name in ['texture', 'buffer', 'framebuffer', 'renderbuffer', 'sampler', 'vaobj', 'array', 'program', 'shader', 'pipeline', 'id'] and str(arg.atype) == 'libc_types::u32':
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'C_OCAST<libc_types::u32>(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('glhnd const&')))
                self.consume(arg)
            elif arg.group == 'EnableCap':
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'to_enum(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('Feature')))
                self.consume(arg)
            elif arg.group == 'PolygonMode':
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'to_enum(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('DrawMode')))
                self.consume(arg)
            elif arg.group in ['MaterialFace', 'CullFaceMode']:
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'to_enum(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('Face')))
                self.consume(arg)
            elif arg.group in ['PatchParameterName']:
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'to_enum(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('PatchProperty')))
                self.consume(arg)
            elif arg.group in ['ShaderType']:
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'to_enum1(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('ShaderStage')))
                self.consume(arg)
            elif arg.group in ['UseProgramStageMask']:
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'to_enum2(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('ShaderStage')))
                self.consume(arg) 
            elif arg.group == 'InternalFormat' and 'Compressed' not in self.cmd_name:
                # Replace GLenum internalformat with PixFmt for Tex* functions
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'to_enum(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('typing::pixels::PixFmt')))
                self.consume(arg)
            elif arg.group == 'InternalFormat' and 'Compressed' in self.cmd_name:
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype,
                    'to_enum(%s.base_fmt, %s.p_flags, %s.c_flags)' % (arg.name, arg.name, arg.name)))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('typing::pixels::CompFmt')))
                self.consume(arg)
            elif arg.group == 'PixelFormat' and 'Compressed' in self.cmd_name:
                # Compressed formats have more information for the PixFmt
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype,
                    'to_enum(%s.base_fmt, %s.p_flags, %s.c_flags)' % (arg.name, arg.name, arg.name)))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('typing::pixels::CompFmt')))
                self.consume(arg) 
            elif arg.group == 'PrimitiveType' and arg.name == 'mode': # there's a bug in the spec for this!
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'to_enum(%s.t, %s.c)' % (arg.name, arg.name)))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('DrwMd const&')))
                self.consume(arg)
            elif arg.group in ['DrawElementsType', 'PrimitiveType'] and arg.name == 'type':
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'to_enum(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('TypeEnum')))
                self.consume(arg)
            elif 'ConditionalRender' in self.cmd_name and arg.name == 'mode':
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'to_enum1(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('Delay')))
                self.consume(arg)
            elif 'Draw' in self.cmd_name and arg.name in ['indirect', 'indices'] and '*' in str(arg.atype):
                self.remap_type(arg, GLBaseType('libc_types::uintptr'), 'C_RCAST<%s>(%s)' % (arg.atype, arg.name))
            elif arg.atype.is_ptr and arg.name == 'value' and not arg.atype.is_base():
                # Use Span<T> on 'T* value' arguments
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, '%s.data()' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('semantic::Span<%s> const&' % (arg.atype.base_type,))))
                self.consume(arg)
            elif arg.group == 'TextureTarget':
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'texture_to_enum(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('TexComp::tex_flag const&')))
                self.consume(arg)
            elif arg.group in ['BufferStorageTarget', 'BufferTargetARB']:
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'buffer_to_enum(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('BufferComp::buf_flag const&')))
                self.consume(arg)
            elif arg.group == 'QueryTarget':
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, 'query_to_enum(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('QueryComp::query_flag const&')))
                self.consume(arg)
            elif arg.group in TRANSLATE_GL_GROUP:
                # Replace GLenum from functions utilizing state, eg. buffer bindings
                replacement = TRANSLATE_GL_GROUP[arg.group]
                self.emit_command_input(arg,
                        InArgExp(arg.name, arg.atype, 'to_enum(%s)' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, replacement))
                self.consume(arg)
            elif str(arg.atype) == 'u8':
                # Use bool on signature, GL_TRUE/GL_FALSE on function
                self.emit_command_input(arg, InArgExp(arg.name, GLBaseType('GLboolean'), '%s ? GL_TRUE : GL_FALSE' % arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, GLBaseType('bool')))
                self.consume(arg)
            else:
                # Unchanged argument
                self.emit_command_input(arg, InArgExp(arg.name, arg.atype, arg.name))
                self.emit_function_param(arg, InArgExp(arg.name, arg.atype, arg.name))

        return (self.function_parameters, self.cmd_inputs)

def translate_arg_set(args, cmd_name, og_name):
    return ArgumentTransform(args, cmd_name, og_name).parse()
