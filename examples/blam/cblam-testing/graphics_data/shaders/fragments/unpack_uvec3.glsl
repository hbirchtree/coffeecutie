/* Decode one signed (two's-complement) bitfield of `bits` width into [-1, 1].
 * Xbox packs normals/tangents this way: components near 0 sit either just
 * above 0 or just below 2^bits, so they must be sign-extended (an unsigned
 * decode flips them to ~±1 and produces speckle noise). */
float unpack_signed(in uint b, in uint bits)
{
    uint sign_bit = 1u << (bits - 1u);
    uint range    = 1u << bits;
    float v = b >= sign_bit ? float(b) - float(range) : float(b);
    return v / float(sign_bit - 1u);
}

/* Unpack a signed r11g11b10 vector (r = low 11 bits, g = mid 11, b = high 10),
 * matching typing::pixels::r11g11b10u. Returns each component in [-1, 1]. */
vec3 unpack_uvec3(in uint v)
{
    return vec3(
        unpack_signed(v & 0x7FFu, 11u),
        unpack_signed((v >> 11) & 0x7FFu, 11u),
        unpack_signed((v >> 22) & 0x3FFu, 10u)
    );
}
