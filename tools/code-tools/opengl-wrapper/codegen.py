#!/usr/bin/env python3

from enum import unique
import re


OVERLOADABLE_FUNCTIONS = [
    'Uniform',
    'ProgramUniform',
]

value_vector_pattern = r'(const GL(short|float|double|int|uint)) *'
vector_extract_pattern = r'^([^6]*?)(Matrix)?(([0-4](x[0-4])?)?[siufd]{1,2}(64)?_?v?)?(ARB|NV)?$'


span_type = 'template<typename> class Span'
span_concept = 'semantic::concepts::Span<Span>'

vector_type = 'template<typename, size_t> class vec'
matrix_type = 'template<typename, size_t, size_t> class mat'
vector_concept = 'semantic::concepts::Vector<vec>'
matrix_concept = 'semantic::concepts::Matrix<mat>'


type_map = {
    'GLubyte': 'u8',
    'GLbyte': 'i8',
    
    'GLushort': 'u16',
    'GLshort': 'i16',
    
    'GLuint': 'u32',
    'GLint': 'i32',
    
    'GLuint64': 'u64',
    'GLint64': 'i64',

    'GLfloat': 'f32',
    'GLdouble': 'f64',
    'GLboolean': 'bool',

    # 'GLsizei': 'u64',
}


def generate_header(includes, lines):
    output = ''
    output = output + '#pragma once\n'
    output = output + '\n'
    output = output + '#include "base.h"\n\n'
    output = output + '#include "groups.h"\n\n'

    includes.append('cstddef')
    includes.append('tuple')
    includes.append('string_view')
    includes.append('vector')
    includes.append('peripherals/concepts/span.h')
    includes.append('peripherals/concepts/vector.h')
    for include in includes:
        output = output + f'#include <{include}>\n'

    lines.append('''
namespace gl {

namespace detail {
inline std::tuple<
    std::vector<GLint>,
    std::vector<const char*>,
    std::vector<std::string>>
transform_strings(std::vector<std::string_view> const& strings)
{
    std::vector<GLint> string_lengths;
    std::vector<const char*> cstrings;
    std::vector<std::string> stringstorage;

    for(auto const& view : strings)
    {
        stringstorage.push_back(std::string(view.begin(), view.end()));
        cstrings.push_back(stringstorage.back().c_str());
        string_lengths.push_back(stringstorage.back().size());
    }

    return std::make_tuple(
        std::move(string_lengths), 
        std::move(cstrings), 
        std::move(stringstorage));
}

inline std::string error_to_hex()
{
    return "0x0000";
}

template<typename T1, typename T2>
inline void assert_equal(T1 const& v1, T2 const& v2)
{
    if (v1 != v2)
        Throw(std::runtime_error("assertion failed"));
}
}
''')
    added = []
    for gl_type in type_map.keys():
        m_type = type_map[gl_type]
        if m_type in ['bool']:
            continue
        lines.append(f'static_assert(std::is_same_v<{gl_type}, ::libc_types::{m_type}>, "{gl_type} does not match {m_type}");')
        if m_type in added:
            continue
        lines.append(f'using ::libc_types::{m_type};')
        added.append(m_type)

    lines.append('''
}
''')

    for line in lines:
        output = output + f'{line}\n'
    
    return output


def snakeify(name: str):
    splitter = r'([A-Z][0-9a-z]+)'
    if '1D' in name or '2D' in name or '3D' in name:
        splitter = r'([A-Z0-9][a-z]+)'
    segments = re.split(splitter, name)
    segments = [ seg for seg in segments if len(seg) > 0 ]
    segments = [ seg.replace('_', '') for seg in segments ]
    return '_'.join(segments).lower()


def enum_create_name(name: str):
    name = name.replace('PName', 'Prop')
    name = name.replace('IType', 'Int')
    name = name.replace('LType', 'Long')
    return snakeify(name)


def snakeify_underscores(name: str):
    return '_'.join(name.split('_')[1:]).lower()


def translate_type(type: str):
    if type in type_map:
        return type_map[type]
    
    return type


def translate_param(param: tuple):
    name, type, group = param
    type = translate_type(type)
    return f'{type} {name}'


def extract_pod_type(pointer_type):
    elements = pointer_type.split(' ')
    if len(elements) == 1:
        return elements[0]
    return translate_type(' '.join(elements[:-1]))


def make_storable(type: str):
    type = type.strip()
    if type.endswith('&&'):
        type = type[:-2]
    if type.endswith('&'):
        type = type[:-1]
    return type


def patch_size_param(inputs: list, outputs: list, source_param: str, meta: str, other_params: list = [], name_override: str = None):
    def patch_param(param_name: str):
        try:
            i = inputs.index(param_name)
            output = [ p for p in outputs if p[0] == param_name ][0]
            print(inputs[i], output)
            outputs.remove(output)
            name = name_override if name_override is not None else source_param
            inputs[i] = f'{name}.size()'
        except ValueError:
            return False
        return True
    
    for option in other_params + [meta, f'{source_param}Length']:
        if patch_param(option):
            return


def coordinate_transform(params: list, i: int, output: list, inputs: list, template: list):
    name, type, _ = params[i]
    pod_type = extract_pod_type(type)
    num = 1
    inputs.append(f'{name}.x()')
    try:
        if params[i+1][0] in ['y', 'v1', 'yoffset', 'green']:
            inputs.append(f'{name}.y()')
            num += 1
        template.append([vector_type, vector_concept])
        if params[i+2][0] in ['z', 'v2', 'zoffset', 'blue']:
            inputs.append(f'{name}.z()')
            num += 1
        if params[i+3][0] in ['w', 'v3', 'alpha']:
            inputs.append(f'{name}.w()')
            num += 1
    except:
        pass

    if num == 1:
        inputs.pop()
        inputs.append(name)
        output.append(params[i])
        return 1

    output.append([name, f'vec<{pod_type}, {num}> const&', None])
    return num


def dimensions_transform(params: list, i: int, output: list, inputs: list, template: list):
    name, type, _ = params[i]
    pod_type = type.strip()
    num = 1
    inputs.append(f'{name}.w')
    try:
        if params[i+1][0] in ['height', 'h']:
            inputs.append(f'{name}.h')
            num += 1
        if params[i+2][0] in ['depth']:
            inputs.append(f'{name}.d')
            num += 1
    except:
        pass

    template.append([vector_type, vector_concept])
    output.append([name, f'vec<{pod_type}, {num}> const&', None])
    return num


def pointer_transform(params: list, i: int, output: list, inputs: list, template: list):
    name, type, meta = params[i]
    pod_type = extract_pod_type(type)

    if 'void' in pod_type:
        constness = 'const ' if 'const' in pod_type else ''
        pod_type = f'{constness}std::byte'

    template.append([span_type, span_concept])
    output.append([name, f'Span<{pod_type}>', meta])
    inputs.append(f'reinterpret_cast<{type}>({name}.data())')

    patch_size_param(inputs, output, name, meta[1])

    return 1


def static_array_transform(params: list, i: int, static_size: str, output: list, inputs: list, template: list):
    name, type, meta = params[i]
    pod_type = extract_pod_type(type)
    constness = 'const ' if 'const' in pod_type else ''

    count_element = [ x for x in output if x[0] == meta[1] ]
    if len(count_element) == 1:
        count_element = count_element[0]
        output.remove(count_element)
        inputs[inputs.index(meta[1])] = f'{name}.size()'

    static_size = static_size.split('x')

    template.append([span_type, span_concept])
    if len(static_size) == 2:
        template.append([matrix_type, matrix_concept])
        output.append([name, f'Span<{constness}mat<std::decay_t<{pod_type}>, {static_size[0]}, {static_size[1]}>> const&', None])
    else:
        if static_size[0] != '1':
            template.append([vector_type, vector_concept])
            output.append([name, f'Span<{constness}vec<std::decay_t<{pod_type}>, {static_size[0]}>> const&', None])
        else:
            output.append([name, f'Span<{pod_type}> const&', None])

    inputs.append(f'reinterpret_cast<{pod_type}*>({name}.data())')

    return 1


def enum_transform(params: list, i: int, output: list, inputs: list, usages: dict):
    name, type, meta = params[i]

    # TODO: Mapping enums to enum/enum class

    group = meta[0]

    if group is not None:
        for grp in group.split(','):
            usages[grp] = 1

    if group is None:
        output.append(params[i])
        inputs.append(name)
    else:
        output.append([name, 'groups::' + enum_create_name(group), meta])
        inputs.append(f'static_cast<GLenum>({name})')
    return 1


def string_transform(params: list, i: int, output: list, inputs: list, lines: list, template: list):
    name, type, meta = params[i]

    if '*const*' in type:
        lines.append(f'auto [{name}_lens, {name}_cstr, {name}_store] = detail::transform_strings({name});')
        patch_size_param(inputs, output, name, meta[1], [], f'{name}_cstr')
        output.append([name, 'std::vector<std::string_view>', meta])
        inputs.append(f'{name}_cstr.data()')
    elif 'const' in type:
        patch_size_param(inputs, output, name, meta[1])
        output.append([name, 'std::string_view const&', meta])
        inputs.append(f'{name}.data()')
    elif meta[1] is not None:
        pod_type = extract_pod_type(type)
        template.append([span_type, span_concept])
        output.append([name, f'Span<{pod_type}>', meta])
        inputs.append(f'{name}.data()')
        patch_size_param(inputs, output, name, meta[1], ['bufSize'])
    return 1


def handle_transform(params: list, i: int, inputs: list, output: list, template: list):
    # TODO: Map handles to Span<...> type, including count
    name, type, meta = params[i]
    pod_type = extract_pod_type(type)

    if '*' in type:
        template.append([span_type, span_concept])
        output.append([name, f'Span<{pod_type}>&&', meta])
        inputs.append(f'{name}.data()')
        patch_size_param(inputs, output, name, meta[1])
    else:
        output.append(params[i])
        inputs.append(name)

    return 1


def draw_transform(params: list, inputs: list, lines: list, template: list, usages: dict):
    
    offset_ptrs = [ x for x in params if 'const void *' in x[1] ]
    assert(len(offset_ptrs) <= 1)
    offset = offset_ptrs[0] if len(offset_ptrs) == 1 else None
    offset_idx = params.index(offset) if offset is not None else -1

    remaining = [ x for x in params if x not in offset_ptrs ]
    arrays = [ x for x in remaining if '*' in x[1] ]
    values = [ x for x in remaining if x not in arrays ]

    count = [ x for x in values if 'count' in x[0] ]
    has_arrays = len(arrays) >= 1 and len(count) == 1
    if has_arrays:
        count = count[0]
        count_idx = params.index(count)

        lines.append(f'{count[1]} {count[0]} = {arrays[0][0]}.size();')
        template.append([span_type, span_concept])

    output = []
    for i, param in enumerate(params):
        name, type, meta = param
        pod_type = extract_pod_type(type)

        if has_arrays and i == count_idx:
            continue
        
        if i == offset_idx:
            inputs.append(f'reinterpret_cast<const void*>({name})')
            output.append([name, 'ptroff', meta])
            continue

        if has_arrays and param in arrays:
            lines.append(f'detail::assert_equal({name}.size(), {count[0]});')
            inputs.append(f'{name}.data()')
            output.append([name, f'Span<{pod_type}>', meta])
        else:
            if type in ['GLenum', 'GLbitfield']:
                enum_transform(params, i, output, inputs, usages)
            else:
                inputs.append(name)
                output.append(param)
    return output


def reference_transform(params: list, i: int, output: list, inputs: list):
    name, type, group = params[i]
    pod_type = extract_pod_type(type)
    inputs.append(f'&{name}')
    output.append([name, f'{pod_type}&', [group[0], None]])
    return 1


def add_handle_check(params: list, i: int, debug_lines: list):
    name, _, _ = params[i]

    if name not in [
        'array',
        'buffer',
        'framebuffer',
        'pipeline',
        'program',
        'renderbuffer',
        'sampler',
        'shader',
        'texture',
        'vaobj',
        'xfb']:
        return

    cap_name = name[:1].upper() + name[1:]
    map_names = {
        'vaobj': 'VertexArray',
        'array': 'VertexArray',
        'pipeline': 'ProgramPipeline',
        'xfb': 'TransformFeedback',
    }
    if name in map_names.keys():
        cap_name = map_names[name]
    
    debug_lines.append(f'glIs{cap_name}({name});')


def preprocess_params(
        params: list,
        func_name: str,
        inputs: list,
        lines: list,
        debug_lines: list,
        template_args: list,
        usages: dict):
    output = []
    i = 0

    if func_name.startswith('DrawArrays') or func_name.startswith('DrawElements') or func_name.startswith('MultiDraw'):
        return draw_transform(params, inputs, lines, template_args, usages)

    # if func_name.startswith('Gen') or func_name.startswith('Create') or func_name.startswith('Delete'):
        # return 

    while i < len(params):
        name, type, group = params[i]
        
        value_vector = re.findall(value_vector_pattern, type)
        vector_extract = re.findall(vector_extract_pattern, func_name)
        static_size = None

        # print(name, group, value_vector)

        if len(vector_extract) > 0:
            vector_extract = vector_extract[0]
            static_size = vector_extract[3] if vector_extract[3] != '' else None
            if 'x' not in vector_extract[3] and vector_extract[1] == 'Matrix':
                static_size = f'{static_size}x{static_size}'
        
        if type == 'GLuint':
            add_handle_check(params, i, debug_lines)

        if name in ['x', 'v0', 'xoffset', 'red']:
            i += coordinate_transform(params, i, output, inputs, template_args)
        elif name in ['width', 'w']:
            i += dimensions_transform(params, i, output, inputs, template_args)
        elif type in ['GLchar *', 'const GLchar *', 'const GLchar *const*']:
            i += string_transform(params, i, output, inputs, lines, template_args)
        elif '1' in group and 'const' not in type and 'void' not in type:
            i += reference_transform(params, i, output, inputs)
        elif '*' in type and static_size is None:
            i += pointer_transform(params, i, output, inputs, template_args)
        elif '*' in type and static_size:
            i += static_array_transform(params, i, static_size, output, inputs, template_args)
        elif func_name.startswith('Gen') or func_name.startswith('Create') or func_name.startswith('Delete'):
            i += handle_transform(params, i, inputs, output, template_args)
        # elif name in ['n'] and '*' in params[i+1][1]:
            # i += handle_transform(params, i, func_name, output, inputs)
        elif type in ['GLenum', 'GLbitfield']:
            i += enum_transform(params, i, output, inputs, usages)
        else:
            inputs.append(name)
            output.append(params[i])
            i += 1

    return output


def map_function_name(func_name: str):
    vattr_exclude = re.findall(r'P[0-4]', func_name)
    excluded = len(vattr_exclude) > 0
    excluded = excluded or len([ x for x in OVERLOADABLE_FUNCTIONS if func_name.startswith(x) ]) == 0
    vector_match = re.findall(vector_extract_pattern, func_name)
    if len(vector_match) > 0 and not excluded:
        func_name = vector_match[0][0]

    func_name = snakeify(func_name)
    
    return func_name


def generate_function(command, usages: dict, version: tuple):
    func_name, return_type, params = command

    if return_type is None:
        return_type = 'void'
    return_type = return_type.strip()

    inputs = []
    lines = []
    debug_lines = []
    template_args = [
        ['class Current', f'MinimumVersion<Current, Version<{version[1][0]}, {version[1][1]}>>']
    ]
    params = preprocess_params(
        params, func_name,
        inputs,
        lines, debug_lines, template_args,
        usages)
    param_string = [ translate_param(p) for p in params ]
    param_string = ', '.join(param_string)
    input_string = ', '.join(inputs)
    visible_name = map_function_name(func_name)

    templates = []
    [ templates.append(x) for x in template_args if x not in templates ]

    if len(templates) > 0:
        types = [x[0] for x in templates]
        concepts = [x[1] for x in templates if x[1] is not None]
        yield 'template<'
        yield '    ' + ',\n    '.join(types)
        yield '>'
        if len(concepts) > 0:
            yield 'requires (' + ' && '.join(concepts) + ')'

    yield f'''STATICINLINE {return_type} {visible_name}({param_string})
{{
    if constexpr(compile_info::debug_mode)
    {{
        if constexpr(gl::is_linked)
            if(!gl{func_name})
                Throw(undefined_behavior("unloaded function {func_name}"));'''

    for chunk in debug_lines:
        for line in chunk.split('\n'):
            yield f'        {line}'
    yield '    }'
    for chunk in lines:
        for line in chunk.split('\n'):
            yield f'    {line}'
    
    ret_statement = 'auto out = ' if return_type != 'void' else ''
    yield f'    {ret_statement}gl{func_name}({input_string});'

    yield '''    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }'''

    if ret_statement != '':
        yield '    return out;'

    yield '''}
'''

    return

    param_types = ', '.join([ make_storable(p[1]) for p in params ])
    param_names = ', '.join([ p[0] for p in params ])
    invocation = ',\n            '.join([ f'std::get<{i}>(params)' for i, _ in enumerate(params) ])
    yield f'''struct cmd_{visible_name}
{{
    using return_type = {return_type};
    using parameter_tuple = std::tuple<{param_types}>;



    STATICINLINE cmd_{visible_name} from_invocation({param_string})
    {{
        cmd_{visible_name} out;
        out.params = std::make_tuple<{param_types}>({param_names});
        return out;
    }}

    inline return_type operator()()
    {{
        {visible_name}(
            {invocation}
            );
    }}
}};
'''


def generate_enum(enum: tuple, usages: dict, deprecated_symbols: set):
    DENIED_NAMES = [
        'byte', 'double', 'float', 'int', 'short', 'true', 'false', 'or', 'and', 'xor',
    ]

    name, values, meta = enum

    if name not in usages:
        return

    snake_name = enum_create_name(name)
    yield f'''
// {name}
enum class {snake_name} : ::libc_types::u32 {{'''

    # Remove duplicates, they happen?
    unique_values = set()
    [ unique_values.add(x[0]) for x in values ]
    unique_values = list(unique_values)
    unique_values.sort()

    unique_values = [ x for x in unique_values if x not in deprecated_symbols]

    if len(unique_values) == 0:
        return

    for value in unique_values:
        snake_value = snakeify_underscores(value)
        if snake_value.startswith(snake_name):
            snake_value = snake_value[len(snake_name)+1:]
        if snake_value in DENIED_NAMES:
            snake_value = f'{snake_value}_'
        yield f'''#ifdef {value}
    {snake_value} = {value},
#endif'''
    
    yield f'''}}; // enum class {snake_name}'''

    if meta[0] == 'bitmask':
        yield f'C_FLAGS({snake_name}, ::libc_types::u32);'
