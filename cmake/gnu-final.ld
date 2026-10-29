OUTPUT_FORMAT(pei-x86-64)

PAGE_SIZE = __section_alignment__;

SECTIONS
{
  . = SIZEOF_HEADERS;
  . = ALIGN(PAGE_SIZE);

  .text __image_base__ + PAGE_SIZE :
  {
      __start__ = .;
      *(.text$runtime_start)
      *(SORT_BY_ALIGNMENT(.text$runtime*))


      /* Application code, excluding the runtime bootstrap */
      __code_start__ = .;
      *(SORT_BY_ALIGNMENT(.text*))
      __code_end__ = .;
  }

  .data BLOCK(PAGE_SIZE) :
  {
    __runtime_info_start__ = .;
    KEEP(*(.runtime_info*))
    __runtime_info_end__ = .;
      
    . = ALIGN(8);
    
    /* Application data, excluding relocation metadata */
    __data_start__ = .;
  
    *(SORT_BY_ALIGNMENT(.rdata*))

    *(SORT_BY_ALIGNMENT(.data*))

    *(SORT_BY_ALIGNMENT(.bss*))

    . = ALIGN(PAGE_SIZE);

    __data_end__ = .;

    . = ALIGN(8);

    __reloc_start__ = .;
    *(.reloc)
    __reloc_end__ = .;
  }

  .idata BLOCK(PAGE_SIZE) :
  {
    KEEP(SORT(*)(.idata$2))
    KEEP(SORT(*)(.idata$3))

    LONG(0); LONG(0); LONG(0); LONG(0); LONG(0);

    . = ALIGN(8);

    KEEP(SORT(*)(.idata$4))

    __IAT_start__ = .;

    SORT(*)(.idata$5)

    __IAT_end__ = .;

    KEEP(SORT(*)(.idata$6))
    KEEP(SORT(*)(.idata$7))
  }
}
