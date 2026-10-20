#pragma once

#include "endian.h"

#include <peripherals/libc/types.h>

#include <array>
#include <cstring>
#include <string_view>

namespace blam {

consteval libc_types::u32 tag_fourcc(char const (&s)[5])
{
    return (static_cast<libc_types::u32>(static_cast<unsigned char>(s[0])) << 24) |
           (static_cast<libc_types::u32>(static_cast<unsigned char>(s[1])) << 16) |
           (static_cast<libc_types::u32>(static_cast<unsigned char>(s[2])) << 8) |
           static_cast<libc_types::u32>(static_cast<unsigned char>(s[3]));
}

/*! Tag classes of every game; the value is the fourcc read big-endian */
enum class tag_class_t : libc_types::u32
{
    none      = 0xFFFFFFFF,
    invalid   = none,
    undefined = none,

    actr    = tag_fourcc("actr"), /* Actor */
    actv    = tag_fourcc("actv"), /* Actor variant */
    adlg    = tag_fourcc("adlg"),
    ai__    = tag_fourcc("ai**"),
    ant     = tag_fourcc("ant!"), /* Antenna */
    antr    = tag_fourcc("antr"), /* Animation graph */
    bipd    = tag_fourcc("bipd"), /* Biped */
    bitm    = tag_fourcc("bitm"), /* Bitmap */
    bloc    = tag_fourcc("bloc"),
    bsdt    = tag_fourcc("bsdt"),
    cen_    = tag_fourcc("*cen"),
    cfxs    = tag_fourcc("cfxs"),
    char_   = tag_fourcc("char"),
    cin_    = tag_fourcc("cin*"),
    clu_    = tag_fourcc("clu*"),
    clwd    = tag_fourcc("clwd"),
    coll    = tag_fourcc("coll"), /* Collision mesh */
    colo    = tag_fourcc("colo"), /* Color group */
    comment = tag_fourcc("/**/"),
    cont    = tag_fourcc("cont"), /* Contrail */
    crea    = tag_fourcc("crea"),
    ctrl    = tag_fourcc("ctrl"), /* Control/button/panel */
    dc_s    = tag_fourcc("dc*s"),
    dctr    = tag_fourcc("dctr"),
    dec_    = tag_fourcc("dec*"),
    deca    = tag_fourcc("deca"), /* Decal */
    decp    = tag_fourcc("DECP"),
    decr    = tag_fourcc("DECR"),
    DeLa    = tag_fourcc("DeLa"), /* UI item */
    devc    = tag_fourcc("devc"), /* Device? */
    devi    = tag_fourcc("devi"),
    dgr_    = tag_fourcc("dgr*"),
    dobc    = tag_fourcc("dobc"), /* Detail object collection */
    eap_    = tag_fourcc("*eap"),
    effe    = tag_fourcc("effe"), /* Effect */
    egor    = tag_fourcc("egor"),
    ehi_    = tag_fourcc("*ehi"),
    elec    = tag_fourcc("elec"), /* Lightning */
    eqip    = tag_fourcc("eqip"), /* Equipment */
    flag    = tag_fourcc("flag"), /* Flag of some sort? */
    fog     = tag_fourcc("fog "), /* gof 1718576928 Fog parameters */
    font    = tag_fourcc("font"), /* Font. */
    foot    = tag_fourcc("foot"), /* Material effect? */
    fpch    = tag_fourcc("fpch"),
    fx      = tag_fourcc("<fx>"),
    garb    = tag_fourcc("garb"), /* Garbagio */
    gldf    = tag_fourcc("gldf"),
    glw     = tag_fourcc("glw!"), /* Glow */
    goof    = tag_fourcc("goof"),
    grhi    = tag_fourcc("grhi"), /* Grenade HUD interface */
    hlmt    = tag_fourcc("hlmt"),
    hmt     = tag_fourcc("hmt "), /* tmh 1752003616 HUD message text */
    hsc_    = tag_fourcc("hsc*"),
    hud     = tag_fourcc("hud#"), /* HUD number/element? */
    hudg    = tag_fourcc("hudg"), /* HUD global */
    igh_    = tag_fourcc("*igh"),
    ipd_    = tag_fourcc("*ipd"),
    item    = tag_fourcc("item"), /* Item */
    itmc    = tag_fourcc("itmc"), /* Item collection */
    jmad    = tag_fourcc("jmad"),
    jpt     = tag_fourcc("jpt!"), /* Damage */
    lens    = tag_fourcc("lens"), /* Lens flare */
    lifi    = tag_fourcc("lifi"), /* Light fixture */
    ligh    = tag_fourcc("ligh"), /* Light */
    lsnd    = tag_fourcc("lsnd"), /* Looping sound */
    ltmp    = tag_fourcc("ltmp"),
    mach    = tag_fourcc("mach"), /* Machine */
    matg    = tag_fourcc("matg"), /* Game globals */
    mcsr    = tag_fourcc("mcsr"),
    mdlg    = tag_fourcc("mdlg"),
    metr    = tag_fourcc("metr"), /* Meter, as in weapon meter */
    mgs2    = tag_fourcc("mgs2"), /* Light volume */
    MGS2    = tag_fourcc("MGS2"),
    mod2    = tag_fourcc("mod2"), /* Model */
    mode    = tag_fourcc("mode"), /* Model */
    mply    = tag_fourcc("mply"), /* Multiplayer scenario */
    mulg    = tag_fourcc("mulg"),
    nhdt    = tag_fourcc("nhdt"),
    obje    = tag_fourcc("obje"), /* Object */
    part    = tag_fourcc("part"), /* Particle */
    pctl    = tag_fourcc("pctl"), /* Particle system */
    phmo    = tag_fourcc("phmo"),
    phys    = tag_fourcc("phys"), /* Physics */
    play    = tag_fourcc("play"), /* Resource layout table */
    pmov    = tag_fourcc("pmov"),
    pphy    = tag_fourcc("pphy"), /* Point physics */
    proj    = tag_fourcc("proj"), /* Projectile */
    prt3    = tag_fourcc("prt3"),
    PRTM    = tag_fourcc("PRTM"),
    qip_    = tag_fourcc("*qip"),
    rain    = tag_fourcc("rain"), /* Rain effect */
    rea_    = tag_fourcc("*rea"),
    rm__    = tag_fourcc("rm  "),
    rmdf    = tag_fourcc("rmdf"),
    rmhg    = tag_fourcc("rmhg"),
    rmtr    = tag_fourcc("rmtr"),
    rmw_    = tag_fourcc("rmw "),
    rmt2    = tag_fourcc("rmt2"),
    sbsp    = tag_fourcc("sbsp"), /* Structured BSP */
    sce_    = tag_fourcc("*sce"),
    scen    = tag_fourcc("scen"), /* Scenery item */
    scex    = tag_fourcc("scex"), /* Extended Chicago shader */
    schi    = tag_fourcc("schi"), /* Chicago shader */
    scnr    = tag_fourcc("scnr"), /* Scenario */
    senv    = tag_fourcc("senv"), /* Shader environment */
    sfx     = tag_fourcc("sfx+"),
    sgla    = tag_fourcc("sgla"), /* Glass shader */
    shad    = tag_fourcc("shad"),
    shdr    = tag_fourcc("shdr"), /* Shader */
    sily    = tag_fourcc("sily"),
    skin    = tag_fourcc("skin"),
    sky     = tag_fourcc("sky "), /* yks 1936423200 Skybox */
    skya    = tag_fourcc("skya"),
    sLdT    = tag_fourcc("sLdT"),
    smet    = tag_fourcc("smet"), /* Metal shader */
    sncl    = tag_fourcc("sncl"),
    snd     = tag_fourcc("snd!"), /* Sound */
    snde    = tag_fourcc("snde"), /* Sound environment */
    snmx    = tag_fourcc("snmx"),
    soso    = tag_fourcc("soso"), /* Shader model */
    sotr    = tag_fourcc("sotr"), /* sotr            Shader generic transparency */
    Soul    = tag_fourcc("Soul"), /* UI item collection */
    spas    = tag_fourcc("spas"),
    spk     = tag_fourcc("spk!"),
    spla    = tag_fourcc("spla"), /* Plasma shader */
    ssce    = tag_fourcc("ssce"), /* Sound scenery */
    sslt    = tag_fourcc("sslt"),
    stem    = tag_fourcc("stem"),
    str     = tag_fourcc("str#"), /* String */
    styl    = tag_fourcc("styl"),
    swat    = tag_fourcc("swat"), /* Water shader */
    tagc    = tag_fourcc("tagc"), /* Tag collection? */
    tdtl    = tag_fourcc("tdtl"),
    trak    = tag_fourcc("trak"), /* Camera track */
    trg_    = tag_fourcc("trg*"),
    udlg    = tag_fourcc("udlg"), /* (AI?) Dialog */
    ugh     = tag_fourcc("ugh!"),
    unhi    = tag_fourcc("unhi"), /* Unit HUD interface */
    unic    = tag_fourcc("unic"),
    unit    = tag_fourcc("unit"), /* Unit, possibly a vehi, bipd */
    ustr    = tag_fourcc("ustr"), /* Unicode string */
    vcky    = tag_fourcc("vcky"), /* Virtual keyboard!? */
    vehc    = tag_fourcc("vehc"),
    vehi    = tag_fourcc("vehi"), /* Vehicle */
    vrtx    = tag_fourcc("vrtx"),
    weap    = tag_fourcc("weap"), /* Weapon */
    weat    = tag_fourcc("weat"),
    wgit    = tag_fourcc("wgit"),
    wgtz    = tag_fourcc("wgtz"),
    wigl    = tag_fourcc("wigl"),
    wind    = tag_fourcc("wind"), /* Wind effect */
    wphi    = tag_fourcc("wphi"), /* Weapon HUD interface */
    zone    = tag_fourcc("zone"), /* Resource gestalt */

    /* Long names */
    actor = actr,
    actor_variant = actv,
    antenna = ant,
    animation_trigger = antr,
    biped = bipd,
    bitmap = bitm,
    collision_mesh = coll,
    color_group = colo,
    contrail = cont,
    control_object = ctrl,
    decal = deca,
    ui_element = DeLa,
    device = devc,
    detail_object_collection = dobc,
    effect = effe,
    lightning = elec,
    equipment = eqip,
    flag_object = flag,
    fog_parameters = fog,
    glow = glw,
    grenade_hud_interface = grhi,
    hud_message_text = hmt,
    hud_number = hud,
    hud_global = hudg,
    item_collection = itmc,
    damage_descriptor = jpt,
    lens_flare = lens,
    light_fixture = lifi,
    light = ligh,
    machine = mach,
    meter = metr,
    light_volume = mgs2,
    model2 = mod2,
    model = mode,
    multiplayer_scenario = mply,
    object = obje,
    particle = part,
    particle_system = pctl,
    physics = phys,
    point_physics = pphy,
    projectile = proj,
    rain_effect = rain,
    structured_bsp = sbsp,
    scenery_item = scen,
    shader_chicago_extended = scex,
    shader_chicago = schi,
    scenario = scnr,
    shader_environment = senv,
    shader_glass = sgla,
    shader_generic = shdr,
    skybox = sky,
    shader_meter = smet,
    sound = snd,
    sound_environment = snde,
    shader_model = soso,
    shader_transparent = sotr,
    ui_item_collection = Soul,
    shader_plasma = spla,
    sound_scenery = ssce,
    string = str,
    shader_water = swat,
    tag_collection = tagc,
    camera_track = trak,
    unit_dialog = udlg,
    unit_hud_interface = unhi,
    unicode_string = ustr,
    virtual_keyboard = vcky,
    vehicle = vehi,
    weapon = weap,
    wind_effect = wind,
    weapon_hud_interface = wphi,
};

/*! A fourcc as text */
struct fourcc_t
{
    std::array<char, 4> chars;

    constexpr std::string_view view() const
    {
        return std::string_view(chars.data(), chars.size());
    }

    constexpr bool operator==(std::string_view other) const
    {
        return view() == other;
    }
};

constexpr fourcc_t fourcc_of(tag_class_t cls)
{
    auto v = static_cast<libc_types::u32>(cls);
    return {{static_cast<char>(v >> 24),
             static_cast<char>(v >> 16),
             static_cast<char>(v >> 8),
             static_cast<char>(v)}};
}

inline std::string_view format_as(fourcc_t const& fourcc)
{
    return fourcc.view();
}

/*! A tag class as a file stores it, in the file's byte order */
template<std::endian E>
struct basic_tag_class
{
    std::array<libc_types::u8, 4> raw;

    inline tag_class_t id() const
    {
        libc_types::u32 v;
        std::memcpy(&v, raw.data(), sizeof(v));
        return static_cast<tag_class_t>(from_endian<E>(v));
    }

    inline fourcc_t str() const
    {
        return fourcc_of(id());
    }

    /*! Null classes are all ones; padding entries are zero */
    inline bool valid() const
    {
        auto cls = id();
        return cls != tag_class_t::none && static_cast<libc_types::u32>(cls) != 0;
    }

    inline bool operator==(tag_class_t cls) const
    {
        return id() == cls;
    }

    inline bool operator==(basic_tag_class const& other) const
    {
        return id() == other.id();
    }
};

static_assert(sizeof(basic_tag_class<std::endian::big>) == 4);
static_assert(fourcc_of(tag_class_t::bitm) == "bitm");

} // namespace blam
