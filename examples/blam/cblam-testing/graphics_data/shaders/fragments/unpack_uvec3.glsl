/* Unpack r11g11b10u (r = low 11 bits, g = mid 11 bits, b = high 10 bits),
 * matching typing::pixels::r11g11b10u. Returns each component in [0, 1]. */
vec3 unpack_uvec3(in uint v)
{
    return vec3(
        float(v & 0x7FFu) / 2047.0,
        float((v >> 11) & 0x7FFu) / 2047.0,
        float((v >> 22) & 0x3FFu) / 1023.0
    );
}
