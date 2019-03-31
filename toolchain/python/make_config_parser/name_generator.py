def conditional_append(v1, v2):
    if len(v1) == 0:
        return v2

    if v2 == '.':
        return v1

    return v1 + "." + v2

#
# Takes this structure:
#coffee:
#    android:
#      armv8a:
#        nougat:
#          - gles3
#        lollipop:
#          - gles3
#      armv7a:
#        nougat:
#          - gles2
#          - gles3
#        lollipop:
#          - gles3
#        kitkat:
#          - gles2
#
# and generates names such as coffee.android.nougat.gles3
#

def generate_target_names(target_description, prefix =''):

    if type(target_description) == dict:
        for key in sorted(list(target_description.keys())):
            for f in generate_target_names(target_description[key], conditional_append(prefix, key)):
                yield f
        return

    if type(target_description) == list:
        for name in target_description:
            yield conditional_append(prefix, name)
        return

    raise TypeError("invalid value in structure")

if __name__ == '__main__':
    test_structure = {
        'android':  ['.', 'gles3'],
        'apple': {
            'ios': ['armv7', 'arm64', '.']
        }
    }

    for target in generate_target_names(test_structure):
        print(target)