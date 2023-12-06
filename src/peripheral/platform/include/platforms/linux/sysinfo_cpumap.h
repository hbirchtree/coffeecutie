#pragma once

#include <optional>
#include <string>
#include <stdint.h>

namespace platform::info::proc::linux_::detail {

inline std::optional<std::string_view> map_cpu_implementer(
    std::string_view const& imp)
{
    if(imp == "0x41")
        return "ARM";
    if(imp == "0x42")
        return "Broadcom";
    if(imp == "0x4E")
        return "NVIDIA";
    if(imp == "0x51")
        return "Qualcomm";
    if(imp == "0x53")
        return "Samsung";

    return std::nullopt;
}

inline std::optional<std::string_view> map_cpu_part(
    std::string_view const& imp, std::string_view const& part)
{
    /* Sourced from
     * - gcc/config/aarch64/aarch64-cores.def
     * - gcc/config/arm/arm-cpus.in
     */

    // clang-format off
    if(imp == "0x41")
    {
        /* ARMv5-TEJ */
        if(part == "0x926") return "ARM926EJ-S";
        if(part == "0xa26") return "ARM1026EJ-S";

        /* ARMv6 */
        if(part == "0xb02") return "ARM11-MPCore";
        if(part == "0xb36") return "ARM1136JF-S";
        if(part == "0xb56") return "ARM1156T2S";
        if(part == "0xb76") return "ARM1176JZF-S";

        /* ARMv6-M */
        if(part == "0xc20") return "Cortex-M0(+)";

        /* ARMv7-A */
        if(part == "0xc05") return "Cortex-A5";
        if(part == "0xc06") return "Cortex-A6";
        if(part == "0xc07") return "Cortex-A7";
        if(part == "0xc08") return "Cortex-A8";
        if(part == "0xc09") return "Cortex-A9";
        if(part == "0xc0d") return "Cortex-A12";
        if(part == "0xc0f") return "Cortex-A15";
        if(part == "0xc0e") return "Cortex-A17";

        /* ARMv7-M */
        if(part == "0xc21") return "Cortex-M1";
        if(part == "0xc23") return "Cortex-M3";
        if(part == "0xc24") return "Cortex-M4(F)";

        /* ARMv8.0-A */
        if(part == "0xd02") return "Cortex-A34";
        if(part == "0xd04") return "Cortex-A35";
        if(part == "0xd03") return "Cortex-A53";
        if(part == "0xd07") return "Cortex-A57";
        if(part == "0xd08") return "Cortex-A72";
        if(part == "0xd09") return "Cortex-A73";

        /* ARMv8.1-A */
        /* None? */

        /* ARMv8.2-A */
        if(part == "0xd05") return "Cortex-A55";
        if(part == "0xd06") return "Cortex-A65";
        if(part == "0xd0a") return "Cortex-A75";
        if(part == "0xd0b") return "Cortex-A76";
        if(part == "0xd0e") return "Cortex-A76AE";
        if(part == "0xd0d") return "Cortex-A77";
        if(part == "0xd41") return "Cortex-A78";

        if(part == "0xd44") return "Cortex-X1";
        if(part == "0xd4c") return "Cortex-X1C";

        if(part == "0xd0c") return "Neoverse-N1";
        if(part == "0xd4a") return "Neoverse-E1";

        /* ARMv8.4-A */
        if(part == "0xd40") return "Neoverse-V1";

        /* ARMv8-R */
        if(part == "0xd15") return "Cortex-R82";

        /* ARMv9-A */
        if(part == "0xd48") return "Cortex-X2";
        if(part == "0xd4e") return "Cortex-X3";

        if(part == "0xd46") return "Cortex-A510";
        if(part == "0xd47") return "Cortex-A710";

        if(part == "0xd4d") return "Cortex-A715";

        if(part == "0xd49") return "Neoverse-N2";
        if(part == "0xd4f") return "Neoverse-V2";

        /* ARMv9.2-A */
        if(part == "0xd80") return "Cortex-A520";
        if(part == "0xd81") return "Cortex-X4";
        if(part == "0xd81") return "Cortex-A720";
    } else if(imp == "0x4E") // NVIDIA
    {
        if(part == "0x000") return "Denver";
        if(part == "0x003") return "Denver2";
        if(part == "0x004") return "Carmel";
    } else if(imp == "0x51") // QCOM
    {
        /* From pytorch/cpuinfo/src/arm/uarch.c */
        /* ARMv7-A */
        if(part == "0x00f") return "Scorpion";
        if(part == "0x02d") return "Scorpion (Dual-core)";
        if(part == "0x04d") return "Krait 200/300 (Dual-core)";
        if(part == "0x06f") return "Krait 200/300/400 (Quad-core)";

        /* ARMv8.0-A */
        if(part == "0x201") return "Kryo Silver";
        if(part == "0x205") return "Kryo Gold";
        if(part == "0x211") return "Kryo Silver (Low-power)";
        if(part == "0xc00") return "Falkor";
        if(part == "0xc01") return "Saphira";
        if(part == "0x800") return "Kryo 260/280 Gold";
        if(part == "0x801") return "Kryo 260/280 Silver";

        /* ARMv8.2-A */
        if(part == "0x802") return "Kryo 385 Gold";
        if(part == "0x803") return "Kryo 385 Silver";
        if(part == "0x804") return "Kryo 485 Gold";
        if(part == "0x805") return "Kryo 485 Silver";

    } else if(imp == "0x53") // Samsung
    {
        if(part == "0x001") return "Exynos-M1";
    }
    return std::nullopt;
    // clang-format on
}

} // namespace platform::info::proc::linux_::detail
