MEMORY {
    foreman_ram (rwx)  : ORIGIN = 0x00000000, LENGTH = 16M
    wrench_l2 (rwx)    : ORIGIN = 0x20000000, LENGTH = 2M
    key_l2 (rwx)       : ORIGIN = 0x30000000, LENGTH = 2M
    noc_if (rw)        : ORIGIN = 0x40000000, LENGTH = 64K
    global_dram (rwx)  : ORIGIN = 0x80000000, LENGTH = 16384M
}

SECTIONS {
    .text : {
        *(.text.entry)
        *(.text*)
        *(.text.startup)
    } > foreman_ram

    .rodata : {
        *(.rodata*)
        *(.rodata.cst*)
    } > foreman_ram

    .data : {
        *(.data*)
        *(.data.rel*)
    } > foreman_ram

    .bss : {
        *(.bss*)
        *(COMMON)
    } > foreman_ram

    .bees_models (NOLOAD) : {
        __bees_start = .;
        . += 242M;
        __bees_end = .;
    } > global_dram

    .wrench_sram : {
        *(.wrench*)
    } > wrench_l2

    .key_sram : {
        *(.key*)
    } > key_l2

    .heap (NOLOAD) : {
        __heap_start = .;
        . += 4096M;
        __heap_end = .;
    } > global_dram

    .stack (NOLOAD) : {
        __stack_start = .;
        . += 2M;
        __stack_end = .;
    } > global_dram
}

__foreman_ram_start = ORIGIN(foreman_ram);
__foreman_ram_end = ORIGIN(foreman_ram) + LENGTH(foreman_ram);
__wrench_l2_start = ORIGIN(wrench_l2);
__wrench_l2_end = ORIGIN(wrench_l2) + LENGTH(wrench_l2);
__key_l2_start = ORIGIN(key_l2);
__key_l2_end = ORIGIN(key_l2) + LENGTH(key_l2);
__global_dram_start = ORIGIN(global_dram);
__global_dram_end = ORIGIN(global_dram) + LENGTH(global_dram);
