/* Minimal GameCube apploader, targeting Dolphin's BS2 HLE (Core/Boot/Boot_BS2Emu.cpp,
 * CBoot::RunApploader). It implements only the entry/init/main/close call sequence that
 * RunApploader drives: main() repeatedly publishes a (dest, len, disc_offset) descriptor
 * and the host performs that disc read before calling main() again. Every read this
 * apploader itself needs -- including discovering the DOL's own header -- goes through
 * that same loop; there is no direct DI/hardware access anywhere in this file.
 *
 * Loaded verbatim at 0x81200000 (see link.ld) and entered with the PC set straight to
 * apploader_entry and LR forced to 0 -- there is no crt0, so nothing has zeroed .bss.
 * apploader_entry() is guaranteed to run first and explicitly initializes all state below
 * before it is read.
 */

#include <stdint.h>

enum
{
    DOL_SECTION_COUNT = 18, /* 7 text + 11 data */
};

struct section_job
{
    uint32_t disc_off;
    uint32_t ram_addr;
    uint32_t len;
};

enum
{
    STATE_WANT_BOOT_HEADER,
    STATE_WANT_DOL_HEADER,
    STATE_PARSE_DOL_HEADER,
    STATE_COPY_SECTIONS,
    STATE_DONE,
};

static struct
{
    uint32_t            dol_offset;
    uint32_t            entry_point;
    struct section_job  jobs[DOL_SECTION_COUNT];
    uint32_t            job_count;
    uint32_t            job_next;
    int                 state;
} g;

/* Disc data is big-endian and so is this CPU, but read byte-wise anyway rather than
 * type-punning through a cast -- these buffers are filled by the host copying raw disc
 * bytes in, not by anything this translation unit wrote, so there's no aliasing benefit
 * to lose. */
static uint32_t rd_be32(const uint8_t* p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) |
           (uint32_t)p[3];
}

static uint8_t g_boot_hdr[0x440];
static uint8_t g_dol_hdr[0x100];

static void apploader_init(void (*report)(const char* fmt, ...))
{
    (void)report;
}

static int dispatch_next(uint32_t* out_addr, uint32_t* out_len, uint32_t* out_off)
{
    if(g.job_next >= g.job_count)
    {
        g.state = STATE_DONE;
        return 0;
    }
    struct section_job* j = &g.jobs[g.job_next++];
    *out_addr             = j->ram_addr;
    *out_len              = j->len;
    *out_off              = j->disc_off;
    return 1;
}

static int apploader_main(uint32_t* out_addr, uint32_t* out_len, uint32_t* out_off)
{
    switch(g.state)
    {
    case STATE_WANT_BOOT_HEADER:
        *out_addr = (uint32_t)(uintptr_t)g_boot_hdr;
        *out_len  = sizeof(g_boot_hdr);
        *out_off  = 0;
        g.state   = STATE_WANT_DOL_HEADER;
        return 1;

    case STATE_WANT_DOL_HEADER:
        /* boot.bin: dol_offset is a big-endian u32 at 0x420 */
        g.dol_offset = rd_be32(g_boot_hdr + 0x420);
        *out_addr    = (uint32_t)(uintptr_t)g_dol_hdr;
        *out_len     = sizeof(g_dol_hdr);
        *out_off     = g.dol_offset;
        g.state      = STATE_PARSE_DOL_HEADER;
        return 1;

    case STATE_PARSE_DOL_HEADER:
    {
        /* DOL header: 18 section file-offsets @0x00, 18 load addresses @0x48,
         * 18 sizes @0x90 (7 text + 11 data, back to back in each table), entry
         * point @0xE0. */
        g.job_count = 0;
        for(uint32_t i = 0; i < DOL_SECTION_COUNT; i++)
        {
            uint32_t off  = rd_be32(g_dol_hdr + 0x00 + i * 4);
            uint32_t addr = rd_be32(g_dol_hdr + 0x48 + i * 4);
            uint32_t size = rd_be32(g_dol_hdr + 0x90 + i * 4);
            if(size == 0)
                continue;
            g.jobs[g.job_count].disc_off = g.dol_offset + off;
            g.jobs[g.job_count].ram_addr = addr;
            g.jobs[g.job_count].len      = size;
            g.job_count++;
        }
        g.entry_point = rd_be32(g_dol_hdr + 0xE0);
        g.job_next    = 0;
        g.state       = STATE_COPY_SECTIONS;
        return dispatch_next(out_addr, out_len, out_off);
    }

    case STATE_COPY_SECTIONS:
        return dispatch_next(out_addr, out_len, out_off);

    default:
        return 0;
    }
}

static uint32_t apploader_close(void)
{
    return g.entry_point;
}

void apploader_entry(uint32_t* init_slot, uint32_t* main_slot, uint32_t* close_slot)
{
    g.state       = STATE_WANT_BOOT_HEADER;
    g.job_count   = 0;
    g.job_next    = 0;
    g.dol_offset  = 0;
    g.entry_point = 0;

    init_slot[0]  = (uint32_t)(uintptr_t)apploader_init;
    main_slot[0]  = (uint32_t)(uintptr_t)apploader_main;
    close_slot[0] = (uint32_t)(uintptr_t)apploader_close;
}
