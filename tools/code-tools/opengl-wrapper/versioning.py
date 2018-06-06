import re

VERSION_RGX = r'GL_(ES_)*VERSION_([0-9]+)_([0-9]+)'

API_FILTER = [
    'gl',
    'glcore',
    'gles2'
]

EXTENSION_FILTER = [
    'GL_ARB', 
    'GL_KHR',
]

# Mostly blacklisting outdated ARB extensions
EXTENSION_BLACKLIST = [
    'GL_ARB_cl_event',
    'GL_ARB_vertex_program',
    'GL_ARB_vertex_shader',
    'GL_ARB_window_pos',
    'GL_ARB_fragment_program',
    'GL_ARB_multitexture',
    'GL_ARB_vertex_type_2_10_10_10_rev',
    'GL_ARB_sample_shading',
    'GL_ARB_sync',
    'GL_ARB_imaging',
    'GL_ARB_vertex_buffer_object',
    'GL_ARB_vertex_blend',
    'GL_ARB_color_buffer_float',
    'GL_ARB_transpose_matrix',
    'GL_ARB_shader_subroutine',
    'GL_ARB_sparse_buffer',
    'GL_ARB_sample_locations',
    'GL_ARB_matrix_palette'
]

# The APIs and extensions we are interested in
class GLVersion:
    def __init__(self):
        self.api = ''
        self.major = 9
        self.minor = 9
        self.isnone = True
    
    def template_str(self):
        return 'GL' + self.api + 'VER_' + str(self.major) + str(self.minor)
        
    def compile_str(self):
        if self.isnone:
            return 'GL_VERSION_NONE'
        return '0x' + str(self.major) + str(self.minor) + '0'
    
    def islowerthan(self, other):
        return self.major < other.major or \
            (self.major == other.major and self.minor < other.minor)

# Check if we want  to use this extension
def is_accepted_extension(ext_name):
    if 'texture_compression' in ext_name:
        if 'GL_3DFX_' in ext_name:
            return False
        if 'GL_NV_' in ext_name:
            return False
        return True
    return ext_name[0:6] in EXTENSION_FILTER

# Check if we want to use this API, ignores glsc and gles1
def is_accepted_api(api_name):
    return api_name in API_FILTER

# Create a better representation of GL versions
def accepted_version(version_name):
    version = GLVersion()
    match = re.findall(VERSION_RGX, version_name)
    
    if len(match) == 0:
        return None
        
    match = match[0]
    
    if len(match[0]) > 2:
        version.api = match[0][0:2]
    version.major = int(match[1])
    version.minor = int(match[2])
    
    version.isnone = False
    
    return version

