#!/usr/bin/env python3

import re


VERSION_TEMPLATE = None
OVERLOADABLE_FUNCTIONS = [
    r'^Uniform',
    r'^ProgramUniform',
    r'Parameter[a-z]+$',
]

value_vector_pattern = r'(const GL(short|float|double|int|uint)) *'
vector_extract_pattern = r'^([^6]*?)(Matrix)?(([0-4](x[0-4])?)?[siufd]{1,2}(64)?_?v?)?(ARB|EXT|KHR|NV)?$'


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

    'GLsizei': 'i32',
}

type_guards = {
    'GLdouble': 'defined(GL_VERSION_4_1)',
    'GLint64': 'defined(GL_VERSION_4_1) || defined(GL_ES_VERSION_3_2)',
    'GLuint64': 'defined(GL_VERSION_4_1) || defined(GL_ES_VERSION_3_2)'
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
#define GLW_FPTR_CHECK(functionName) \\
    if constexpr(!gl::is_linked) \\
        if(!gl ## functionName ) \\
            Throw(undefined_behavior("undefined function " # functionName ));

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
        if m_type in ['bool'] or m_type in added:
            continue
        if gl_type in type_guards:
            guard = type_guards[gl_type]
            lines.append(f'#if {guard}')
        lines.append(f'static_assert(std::is_same_v<{gl_type}, ::libc_types::{m_type}>, "{gl_type} does not match {m_type}");')
        lines.append(f'using ::libc_types::{m_type};')
        if gl_type in type_guards:
            lines.append('#endif')
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
    type = type.strip()
    type = translate_type(type)
    return f'{type} {name}'


def extract_type(pointer_type):
    elements = pointer_type.split(' ')
    if len(elements) == 1:
        return '', translate_type(elements[0])
    segments = elements[:-1]
    assert(len(segments) <= 2)
    const = ''
    base_type = segments[-1]
    if len(segments) > 1:
        const, base_type = segments
    return const, translate_type(base_type)


def extract_pod_type(pointer_type: str):
    const, base_type = extract_type(pointer_type)
    return f'{const} {base_type}'.strip()


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
            ptr_output = [ p for p in outputs if p[0] == source_param ][0]
            output = [ p for p in outputs if p[0] == param_name ][0]
            outputs.remove(output)
            name = name_override if name_override is not None else source_param
            if 'void' in ptr_output[1]:
                inputs[i] = f'{name}.size() * sizeof(typename std::decay_t<{ptr_output[1]}>::value_type)'
            else:
                inputs[i] = f'{name}.size()'
        except ValueError:
            return False
        return True
    
    # TODO: Fix for when multiple params use the same count parameter

    for option in other_params + [meta, f'{source_param}Length']:
        if patch_param(option):
            return


def gen_concept_type(pod_type: str, const: bool = False, cls: str = None, dim: str = None):
    '''
        const_mat_2x2_f32
    '''
    if '::' in pod_type:
        pod_type = pod_type.split('::')[-1]
    const = 'const_' if const else ''
    dim = f'{dim}_' if dim is not None else ''
    cls = f'{cls}_' if cls is not None else ''
    return f'{const}{cls}{dim}{pod_type}'


def add_concept(templates: list, type: str, concept_name: str):
    templates.append([f'class {type}', f'semantic::concepts::{concept_name}'])


def coordinate_transform(params: list, i: int, output: list, inputs: list, template: list):
    name, type, _ = params[i]
    _, pod_type = extract_type(type)
    num = 1
    inputs.append(f'{name}.x()')
    try:
        if params[i+1][0] in ['y', 'v1', 'yoffset', 'green']:
            inputs.append(f'{name}.y()')
            num += 1
        if params[i+2][0] in ['z', 'v2', 'zoffset', 'blue']:
            inputs.append(f'{name}.z()')
            num += 1
        if params[i+3][0] in ['w', 'v3', 'alpha']:
            inputs.append(f'{name}.w()')
            num += 1
    except:
        pass

    if num == 1:
        inputs[-1] = name
        output.append(params[i])
        return 1

    vector_type = gen_concept_type(pod_type, False, 'vec', num)
    template.append([f'class {vector_type}', f'semantic::concepts::Vector<{vector_type}, {pod_type}, {num}>'])
    output.append([name, f'{vector_type} const&', None])
    return num


def dimensions_transform(params: list, i: int, output: list, inputs: list, template: list):
    name, type, _ = params[i]
    _, pod_type = extract_type(type)
    num = 1
    inputs.append(f'{name}[0]')
    try:
        if params[i+1][0] in ['height', 'h']:
            inputs.append(f'{name}[1]')
            num += 1
        if params[i+2][0] in ['depth']:
            inputs.append(f'{name}[2]')
            num += 1
    except:
        pass

    if num == 1:
        output.append([name, f'{pod_type}', None])
        inputs[-1] = name
        return 1

    vector_type = gen_concept_type(pod_type, False, 'size', num)
    if num == 2:
        template.append([f'class {vector_type}', f'semantic::concepts::Size2D<{vector_type}, {pod_type}>'])
    elif num == 3:
        template.append([f'class {vector_type}', f'semantic::concepts::Size2D<{vector_type}, {pod_type}>'])
    else:
        raise RuntimeError('dimension structure > 3 elements')
    output.append([name, f'{vector_type} const&', None])

    return num


def pointer_transform(params: list, i: int, output: list, inputs: list, template: list, usages: dict):
    name, type, meta = params[i]
    const, pod_type = extract_type(type)

    if meta[0] is not None and pod_type == 'GLenum':
        usages[meta[0]] = 1
        pod_type = 'group::' + enum_create_name(meta[0])

    span_type = 'span_' + gen_concept_type(pod_type, const == 'const')
    add_concept(template, span_type, f'Span<{span_type}>')
    if 'void' not in pod_type:
        template.append([None, f'std::is_same_v<std::decay_t<typename {span_type}::value_type>, std::decay_t<{pod_type}>>'])
    
    const = 'const&' if const != '' else ''
    output.append([name, f'{span_type} {const}', meta])
    inputs.append(f'{name}.size() ? reinterpret_cast<{type}>({name}.data()) : nullptr')

    patch_size_param(inputs, output, name, meta[1])

    return 1


def static_array_transform(params: list, i: int, static_size: str, output: list, inputs: list, template: list):
    name, type, meta = params[i]
    const, pod_type = extract_type(type)

    count_element = [ x for x in output if x[0] == meta[1] ]
    if len(count_element) == 1:
        count_element = count_element[0]
        output.remove(count_element)
        inputs[inputs.index(meta[1])] = f'{name}.size()'

    size = static_size.split('x')

    cls = None
    dim = None
    if len(size) == 2:
        cls = 'mat'
        dim = f'{size[0]}x{size[1]}'
    elif size[0] != '1':
        cls = 'vec'
        dim = f'{size[0]}'

    concept_type = 'span_' + gen_concept_type(pod_type, const == 'const', cls, dim)
    add_concept(template, concept_type, f'Span<{concept_type}>')
    if cls == 'mat':
        template.append([None, f'semantic::concepts::Matrix<typename {concept_type}::value_type, {pod_type}, {size[0]}, {size[1]}>'])
    elif cls == 'vec':
        template.append([None, f'semantic::concepts::Vector<typename {concept_type}::value_type, {pod_type}, {size[0]}>'])
    else:
        template.append([None, f'std::is_same_v<std::decay_t<typename {concept_type}::value_type>, std::decay_t<{pod_type}>>'])
    const = 'const&' if const != '' else ''
    output.append([name, f'{concept_type} {const}', None])
    inputs.append(f'reinterpret_cast<{type}>({name}.data())')

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
        output.append([name, 'group::' + enum_create_name(group), meta])
        inputs.append(f'static_cast<GLenum>({name})')
    return 1


def string_transform(params: list, i: int, output: list, inputs: list, lines: list, template: list):
    name, type, meta = params[i]

    if '*const*' in type:
        lines.append(f'auto [{name}_lens, {name}_cstr, {name}_store] = detail::transform_strings({name});')
        output.append([name, 'std::vector<std::string_view>', meta])
        inputs.append(f'{name}_cstr.data()')
        patch_size_param(inputs, output, name, meta[1], [], f'{name}_cstr')
    elif 'const' in type:
        output.append([name, 'std::string_view const&', meta])
        inputs.append(f'{name}.data()')
        patch_size_param(inputs, output, name, meta[1])
    elif meta[1] is not None:
        const, pod_type = extract_type(type)
        span_type = 'span_' + gen_concept_type(pod_type, const == 'const')
        add_concept(template, span_type, f'Span<{span_type}>')
        template.append([None, f'std::is_same_v<std::decay_t<typename {span_type}::value_type>, std::decay_t<{pod_type}>>'])
        output.append([name, span_type, meta])
        inputs.append(f'{name}.data()')
        patch_size_param(inputs, output, name, meta[1], ['bufSize'])
    return 1


def handle_transform(params: list, i: int, inputs: list, output: list, template: list):
    # TODO: Map handles to Span<...> type, including count
    name, type, meta = params[i]
    const, pod_type = extract_type(type)

    if '*' in type:
        span_type = 'span_' + gen_concept_type(pod_type, const == 'const')
        add_concept(template, span_type, f'Span<{span_type}>')
        template.append([None, f'std::is_same_v<std::decay_t<typename {span_type}::value_type>, std::decay_t<{pod_type}>>'])
        output.append([name, f'{span_type}', meta])
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

    output = []
    for i, param in enumerate(params):
        name, type, meta = param
        const, pod_type = extract_type(type)

        if has_arrays and i == count_idx:
            continue
        
        if i == offset_idx:
            inputs.append(f'reinterpret_cast<const void*>({name})')
            output.append([name, 'ptroff', meta])
            continue

        if has_arrays and param in arrays:
            lines.append(f'detail::assert_equal({name}.size(), {count[0]});')
            span_type = 'span_' + gen_concept_type(pod_type, const == 'const')
            add_concept(template, span_type, f'Span<{span_type}>')
            template.append([None, f'std::is_same_v<std::decay_t<typename {span_type}::value_type>, std::decay_t<{pod_type}>>'])
            inputs.append(f'{name}.data()')
            output.append([name, span_type, meta])
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
    
    debug_lines.append(f'''#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && !defined(GLEAM_USE_LINKED)
    if(glIs{cap_name}) glIs{cap_name}({name});
#endif''')


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
            i += pointer_transform(params, i, output, inputs, template_args, usages)
        elif '*' in type and static_size:
            i += static_array_transform(params, i, static_size, output, inputs, template_args)
        elif 'GLuint' in type and (func_name.startswith('Gen') or func_name.startswith('Create') or func_name.startswith('Delete')):
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
    excluded = excluded or len([ x for x in OVERLOADABLE_FUNCTIONS if re.findall(x, func_name) ]) == 0
    vector_match = re.findall(vector_extract_pattern, func_name)
    if len(vector_match) > 0 and not excluded:
        func_name = vector_match[0][0]

    func_name = snakeify(func_name)
    
    return func_name


def generate_function(command, usages: dict, version: tuple = None, override_name: str = None):
    func_name, return_data, params = command
    return_type, return_group = return_data

    return_var = 'out'
    if return_type is None:
        return_type = 'void'
    if return_group == 'String':
        return_type = 'stl_types::String'
        return_var = f'reinterpret_cast<const char*>({return_var})'
    return_type = return_type.strip()

    inputs = []
    lines = []
    debug_lines = []
    template_args = []

    if version is not None:
        template_args.append([
            None,
            f'MinimumVersion<Current, Version<{version[1][0]}, {version[1][1]}>>'
        ])

    params = preprocess_params(
        params, func_name,
        inputs,
        lines, debug_lines, template_args,
        usages)
    param_string = [ translate_param(p) for p in params ]
    param_string = ', '.join(param_string)
    input_string = ', '.join(inputs)
    if override_name is not None:
        visible_name = map_function_name(override_name)
    else:
        visible_name = map_function_name(func_name)

    templates = []
    [ templates.append(x) for x in template_args if x not in templates ]

    if len(templates) > 0:
        types = set()
        [ types.add(x[0]) for x in templates if x[0] is not None ]
        if len(types) == 0:
            types.add('typename Dummy = void')
        types = list(types)
        types.sort()
        concepts = [x[1] for x in templates if x[1] is not None]
        yield 'template<'
        yield '    ' + ',\n    '.join([ t for t in types if t is not None ])
        yield '>'
        if len(concepts) > 0:
            yield 'requires (\n    ' + ' &&\n    '.join(concepts) + ')'

    yield f'''STATICINLINE {return_type} {visible_name}({param_string})
{{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {{
        GLW_FPTR_CHECK({func_name})'''

    for chunk in debug_lines:
        for line in chunk.split('\n'):
            yield f'        {line}'
    yield '    }'
    for chunk in lines:
        for line in chunk.split('\n'):
            yield f'    {line}'
    
    ret_statement = 'auto out = ' if return_type != 'void' else ''
    yield f'    {ret_statement}gl{func_name}({input_string});'

    yield f'''    detail::error_check("{func_name}"sv);'''

    if ret_statement != '':
        yield f'    return {return_var};'

    yield '''}
'''

    # param_types = ', '.join([
    #     make_storable(translate_type(p[1]))
    #     for p in params 
    # ])

    # yield f'using {visible_name}_param_t = std::tuple<{param_types}>;'

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
    ILLEGAL_START = r'^[0-9]'
    DENIED_NAMES = [
        'byte', 'double', 'float', 'int', 'short', 
        'true', 'false', 'or', 'and', 'xor', 'bool',
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

    overflow_values = []
    for e in unique_values:
        real_value = [ int(value, base=16) for name, _, value in enum[1] if name == e ]
        if len(real_value) != 1:
            continue
        real_value = real_value[0]
        if real_value > 0xFFFFFFFF:
            overflow_values.append(e)
    [ unique_values.remove(x) for x in overflow_values ]

    for i, x in enumerate(unique_values):
        if not re.match(ILLEGAL_START, x):
            continue
        x = f'n{x}'
        unique_values[i] = x

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

    for value in overflow_values:
        snake_value = snakeify_underscores(value)
        yield f'''#ifdef {value}
constexpr auto {snake_name}_{snake_value} = {value};
#endif'''


