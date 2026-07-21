/* Dolphin's BS2 HLE always DMAs the apploader body to a fixed RAM address (see
 * RunApploader: `DVDRead(system, volume, offset + 0x20, 0x01200000, ...)`), so this
 * has to link at exactly that address regardless of how it's booted. */
OUTPUT_FORMAT("elf32-powerpc")
OUTPUT_ARCH(powerpc:common)
ENTRY(apploader_entry)

MEMORY
{
    ram (rwx) : ORIGIN = 0x81200000, LENGTH = 0x100000
}

SECTIONS
{
    . = 0x81200000;

    .text : { *(.text*) } > ram
    .rodata : { *(.rodata*) *(.rodata.*) } > ram
    .data : { *(.data*) } > ram

    /* No crt0 zeroes this, and nothing here needs a load image for it --
     * apploader_entry() writes every field before first use. Keeping it out of
     * the output binary keeps the DMA'd (and disc-stored) apploader image small. */
    .bss (NOLOAD) : { *(.bss*) *(COMMON) } > ram

    /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) *(.gnu*) }
}
