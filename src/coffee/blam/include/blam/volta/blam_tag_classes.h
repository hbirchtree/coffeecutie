#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/type_safety.h>

#if !defined(COFFEE_NO_ENDIAN_OPS)
#include <peripherals/libc/endian_ops.h>
#endif

namespace blam {

using namespace libc_types;

enum class tag_class_t : u32
{
    none = 0xffffffff,

    actr = 0x61637472, // rtca 1633907826 Actor
    actv = 0x61637476, // vtca 1633907830 Actor variant
    ant  = 0x616e7421, // !tna 1634628641 Antenna
    antr = 0x616e7472, // rtna 1634628722 Animation trigger
    bipd = 0x62697064, // dpib 1651077220 Biped
    bitm = 0x6269746d, // mtib 1651078253 Bitmap
    coll = 0x636f6c6c, // lloc 1668246636 Collision mesh
    colo = 0x636f6c6f, // oloc 1668246639 Color group
    cont = 0x636f6e74, // tnoc 1668247156 Contrail
    ctrl = 0x6374726c, // lrtc 1668575852 Control/button/panel
    deca = 0x64656361, // aced 1684366177 Decal
    DeLa = 0x44654c61, // aLeD 1147489377 UI item
    devc = 0x64657663, // cved 1684371043 Device?
    dobc = 0x646f6263, // cbod 1685021283 Detail object collection
    effe = 0x65666665, // effe 1701209701 Effect
    elec = 0x656c6563, // cele 1701602659 Lightning
    eqip = 0x65716970, // piqe 1701931376 Equipment
    flag = 0x666c6167, // galf 1718378855 Flag of some sort?
    fog  = 0x666f6720, //  gof 1718576928 Fog parameters
    font = 0x666f6e74, // tnof 1718578804 Font.
    foot = 0x666f6f74, // toof 1718579060 Material effect?
    garb = 0x67617262, // brag 1734439522 Garbagio
    glw  = 0x676c7721, // !wlg 1735161633 Glow
    grhi = 0x67726869, // ihrg 1735551081 Grenade HUD interface
    hmt  = 0x686d7420, //  tmh 1752003616 HUD message text
    hud  = 0x68756423, // #duh 1752523811 HUD number/element?
    hudg = 0x68756467, // gduh 1752523879 HUD global
    item = 0x6974656d, // meti            Item
    itmc = 0x69746d63, // cmti 1769237859 Item collection
    jpt  = 0x6a707421, // !tpj 1785754657 Damage
    lens = 0x6c656e73, // snel 1818586739 Lens flare
    lifi = 0x6c696669, // ifil 1818846825 Light fixture
    ligh = 0x6c696768, // hgil 1818847080 Light
    lsnd = 0x6c736e64, // dnsl 1819504228 Looping sound
    mach = 0x6d616368, // hcam 1835098984 Machine
    matg = 0x6d617467, // gtam 1835103335 Game globals
    metr = 0x6d657472, // rtem 1835365490 Meter, as in weapon meter
    mgs2 = 0x6d677332, // 2sgm 1835496242 Light volume
    mod2 = 0x6d6f6432, // 2dom 1836016690 Model
    mode = 0x6d6f6465, // edom 1836016690 Model
    mply = 0x6d706c79, // ylpm 1836084345 Multiplayer scenario
    obje = 0x6f626a65, // ejbo            Object
    part = 0x70617274, // trap 1885434484 Particle
    pctl = 0x7063746c, // ltcp 1885566060 Particle system
    phys = 0x70687973, // syhp 1885895027 Physics
    pphy = 0x70706879, // yhpp 1886414969 Point physics
    proj = 0x70726f6a, // jorp 1886547818 Projectile
    rain = 0x7261696e, // niar 1918986606 Rain effect
    sbsp = 0x73627370, // psbs 1935831920 Structured BSP
    scen = 0x7363656e, // necs 1935893870 Scenery item
    scex = 0x73636578, // xecs 1935893880 Extended Chicago shader
    schi = 0x73636869, // ihcs 1935894633 Chicago shader
    scnr = 0x73636e72, // rncs 1935896178 Scenario
    senv = 0x73656e76, // vnes 1936027254 Shader environment
    sgla = 0x73676c61, // algs 1936157793 Glass shader
    shdr = 0x73686472, // rdhs Shader
    sky  = 0x736b7920, //  yks 1936423200 Skybox
    smet = 0x736d6574, // tems 1936549236 Metal shader
    snd  = 0x736e6421, // !dns 1936614433 Sound
    snde = 0x736e6465, // edns 1936614501 Sound environment
    soso = 0x736f736f, // osos 1936683887 Shader model
    sotr = 0x736f7472, // sotr            Shader generic transparency
    Soul = 0x536f756c, // luoS 1399813484 UI item collection
    spla = 0x73706c61, // alps 1936747617 Plasma shader
    ssce = 0x73736365, // ecss 1936941925 Sound scenery
    str  = 0x73747223, // #rts 1937011235 String
    swat = 0x73776174, // taws 1937203572 Water shader
    tagc = 0x74616763, // cgat 1952540515 Tag collection?
    trak = 0x7472616b, // kart 1953653099 Camera track
    udlg = 0x75646c67, // gldu 1969515623 (AI?) Dialog
    unhi = 0x756e6869, // ihnu 1970169961 Unit HUD interface
    unit = 0x756E6974, // tinu Unit, possibly a vehi, bipd
    ustr = 0x75737472, // rtsu 1970500722 Unicode string
    vcky = 0x76636b79, // ykcv 1986227065 Virtual keyboard!?
    vehi = 0x76656869, // ihev 1986357353 Vehicle
    weap = 0x77656170, // paew 2003132784 Weapon
    wind = 0x77696e64, // dniw 2003398244 Wind effect
    wphi = 0x77706869, // ihpw 2003855465 Weapon HUD interface

    devi = devc,

    undefined = 0xFFFFFFFF,

    actor                    = actr,
    actor_variant            = actv,
    antenna                  = ant,
    animation_trigger        = antr,
    biped                    = bipd,
    bitmap                   = bitm,
    collision_mesh           = coll,
    color_group              = colo,
    contrail                 = cont,
    control_object           = ctrl,
    decal                    = deca,
    ui_element               = DeLa,
    device                   = devc,
    detail_object_collection = dobc,
    effect                   = effe,
    lightning                = elec,
    equipment                = eqip,
    flag_object              = flag,
    fog_parameters           = fog,
    // foot?
    // garb?
    glow                  = glw,
    grenade_hud_interface = grhi,
    hud_message_text      = hmt,
    hud_number            = hud,
    hud_global            = hudg,
    // item = item
    item_collection         = itmc,
    damage_descriptor       = jpt,
    lens_flare              = lens,
    light_fixture           = lifi,
    light                   = ligh,
    machine                 = mach,
    meter                   = metr,
    light_volume            = mgs2,
    model2                  = mod2,
    model                   = mode,
    multiplayer_scenario    = mply,
    object                  = obje,
    particle                = part,
    particle_system         = pctl,
    physics                 = phys,
    point_physics           = pphy,
    projectile              = proj,
    rain_effect             = rain,
    structured_bsp          = sbsp,
    scenery_item            = scen,
    shader_chicago_extended = scex,
    shader_chicago          = schi,
    scenario                = scnr,
    shader_environment      = senv,
    shader_glass            = sgla,
    shader_generic          = shdr,
    skybox                  = sky,
    shader_meter            = smet,
    sound                   = snd,
    sound_environment       = snde,
    shader_model            = soso,
    shader_transparent      = sotr,
    ui_item_collection      = Soul,
    shader_plasma           = spla,
    sound_scenery           = ssce,
    string                  = str,
    shader_water            = swat,
    tag_collection          = tagc,
    camera_track            = trak,
    unit_dialog             = udlg,
    unit_hud_interface      = unhi,
    unicode_string          = ustr,
    virtual_keyboard        = vcky,
    vehicle                 = vehi,
    weapon                  = weap,
    wind_effect             = wind,
    weapon_hud_interface    = wphi,
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

} // namespace blam
