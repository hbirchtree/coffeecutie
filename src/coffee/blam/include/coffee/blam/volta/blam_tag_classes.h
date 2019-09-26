#pragma once

#include <coffee/core/libc_types.h>

#if !defined(COFFEE_NO_ENDIAN_OPS)
#include <peripherals/libc/endian_ops.h>
#endif

namespace Coffee {
namespace Blam {

/*!
 * \brief Blam tag classes, sourced from SparkEdit source code
 */
enum class tag_class_t
{
    obje = 0x656a626f, /* TAG_OBJECT */
    unit = 0x2,        /* TAG_UNIT */
    bipd = 0x64706962, /* TAG_BIPED */
    vehi = 0x69686576, /* TAG_VEHICLE */
    item = 0x6d657469, /* TAG_ITEM */
    weap = 0x70616577, /* TAG_WEAPON */
    eqip = 0x70697165, /* TAG_EQUIPMENT */
    garb = 0x62726167, /* TAG_GARBAGE */
    proj = 0x6A6F7270, /* TAG_PROJECTILE */
    scen = 0x6E656373, /* TAG_SCENERY */
    ssce = 0x4,        /* TAG_SOUND_SCENERY */
    devi = 0x5,        /* TAG_DEVICE */
    mach = 0x6,        /* TAG_MACHINE */
    ctrl = 0x7,        /* TAG_CONTROL */
    lifi = 0x8,        /* TAG_LIGHT_FIXTURE */
    plac = 0x9,        /* TAG_PLACEHOLDER */
    bitm = 0x6d746962, /* TAG_BITMAP */
    shdr = 0x72646873, /* TAG_SHADER */
    snd  = 0x21646e73, /* TAG_SOUND */
    ustr = 0x72747375, /* TAG_UNICODE_STR */
    scnr = 0x726E6373, /* TAG_SCENARIO */
    sbsp = 0x70736273, /* TAG_STRUCTURED_BSP */
    mply = 0x796c706d, /* TAG_MULTIPLAYER_SCENARIO */
    itmc = 0x636d7469, /* TAG_ITEM_COLLECTION */
    hmt  = 0x20746D68, /* TAG_HUD_MSG_TEXT */
    trak = 0x6B617274, /* TAG_CAMERA_TRACK */
    mode = 0x65646F6D, /* TAG_MODEL */
    actr = 0x72746361, /* TAG_ACTOR */
    actv = 0x76746361, /* TAG_ACTOR_VARIANT */
    antr = 0x72746E61, /* TAG_ANIMATION_TRIGGER */
    sky  = 0x20796B73, /* TAG_SKY */
    lens = 0x736E656C, /* TAG_LENSFLARE */
    grhi = 0x69687267, /* TAG_GRENADE_HUD_INTERFACE */
    unhi = 0x69686E75, /* TAG_UNIT_HUD_INTERFACE */
    wphi = 0x69687077, /* TAG_WEAPON_HUD_INTERFACE */
    coll = 0x6C6C6F63, /* TAG_COLLISION_MODEL */
    cont = 0x746E6F63, /* TAG_CONTRAIL */
    deca = 0x61636564, /* TAG_DECAL */
    effe = 0x65666665, /* TAG_EFFECT */
    part = 0x74726170, /* TAG_PARTICLE */
    pctl = 0x6C746370, /* TAG_PARTICLE_SYSTEM */
    rain = 0x13,       /* TAG_WEATHER_PARTICLE */
    matg = 0x6774616D, /* TAG_GAME_GLOBALS */
    hud  = 0x23647568, /* TAG_HUD_NUMBER */
    hudg = 0x67647568, /* TAG_HUD_GLOBALS */
    jpt  = 0x2174706A, /* TAG_DAMAGE */
    ligh = 0x6867696C, /* TAG_LIGHT */
    glw  = 0x14,       /* TAG_GLOW */
    mgs2 = 0x3273676D, /* TAG_LIGHT_VOLUME */
    lsnd = 0x646E736C, /* TAG_LOOPING_SOUND */
    snde = 0x65646E73, /* TAG_SOUND_ENVIRONMENT */
    pphy = 0x79687070, /* TAG_PHYSICS */
                       //    pphy = 0x79687070,  /* TAG_POINT_PHYSICS */
    fog  = 0x20676F66, /* TAG_FOG */
    wind = 0x646E6977, /* TAG_WIND */
    senv = 0x766E6573, /* TAG_SHADER_ENVIRONMENT */
    dobc = 0x63626F64, /* TAG_DETAIL_OBJ_COLLECTION */
    font = 0x746E6F66, /* TAG_FONT */
    udlg = 0x676C6475, /* TAG_DIALOG */
    DeLa = 0x614C6544, /* TAG_UI_ITEM */
    Soul = 0x6C756F53, /* TAG_UI_ITEM_COLLECTION */
    soso = 0x6F736F73, /* TAG_SHADER_MODEL */
    mdle = 0x6F736F73, /* TAG_SHADER_MODEL_LE */
    sotr = 0x72746F73, /* TAG_SHADER_TRANSPARENCY */
    swat = 0x74617773, /* TAG_SHADER_WATER */
    sgla = 0x616C6773, /* TAG_SHADER_GLASS */
    smet = 0x74656D73, /* TAG_SHADER_METAL */
    spla = 0x616C7073, /* TAG_SHADER_PLASMA */
    ant  = 0x21746E61, /* TAG_ANTENNA */
    str  = 0x23727473, /* TAG_STRING_LIST */
    colo = 0x6F6C6F63, /* TAG_COLOR_GROUP */
    elec = 0x15,       /* TAG_LIGHTNING */
    metr = 0x7274656D, /* TAG_METER */
    schi = 0x69686373, /* TAG_SHADER_TRANSP_VARIANT */
    flag = 0x16,       /* TAG_FLAG */
    foot = 0x746F6F66, /* TAG_MATERIAL_EFFECT */
    vcky = 0x17,       /* TAG_VIRTUAL_KEYBOARD */
    mod2 = 0x32646F6D, /* TAG_MODEL2 */
};

FORCEDINLINE bool tag_class_cmp(tag_class_t v1, tag_class_t v2)
{
    auto v1_i = libc::endian::to<libc::endian::u32_host>(C_CAST<u32>(v1));
    auto v2_i = C_CAST<u32>(v2);

    return v1_i == v2_i || v1 == v2;
}

#if !defined(COFFEE_MSVCXX)
FORCEDINLINE bool operator==(tag_class_t t1, tag_class_t t2)
{
    return tag_class_cmp(t1, t2);
}

FORCEDINLINE bool operator!=(tag_class_t t1, tag_class_t t2)
{
    return !(t1 == t2);
}
#endif

} // namespace Blam
} // namespace Coffee
