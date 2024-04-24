vec3 unpack_uvec3(in uint v)
{
    return vec3(
        float(v >> 21) / 2047.0,
        float((v >> 10) & 0x7FF) / 2047.0,
        float(v & 0x3FF) / 1023.0
    );
}
