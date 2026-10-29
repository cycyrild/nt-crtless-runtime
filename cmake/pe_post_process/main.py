import sys
from pathlib import Path

from pe_post_process.map_file import MapFile
from pe_post_process.pe_headers import PeHeaders
from pe_post_process.relative_range import RelativeRange
from pe_post_process.runtime_info import RuntimeInfo
from pe_post_process.verify_relocations import verify_relocations

PAGE_SIZE = 0x1000


def main() -> None:
    if len(sys.argv) != 2:
        print(
            f"Usage: {sys.argv[0]} <map-file>",
            file=sys.stderr,
        )
        raise SystemExit(2)

    map_path = Path(sys.argv[1])

    if not map_path.is_file():
        raise FileNotFoundError(f"Map file not found: {map_path}")

    map_file = MapFile(map_path)

    pe_path = map_file.find_output_path()

    if not pe_path.is_file():
        raise FileNotFoundError(f"PE file not found: {pe_path}")

    image_base = map_file.find_symbol_address("__image_base__")

    section_alignment = map_file.find_symbol_address("__section_alignment__")

    file_alignment = map_file.find_symbol_address("__file_alignment__")

    start_address = map_file.find_symbol_address("__start__")

    reloc = map_file.get_range("__reloc_start__", "__reloc_end__")

    runtime_info_range = map_file.get_range(
        "__runtime_info_start__", "__runtime_info_end__"
    )

    code = map_file.get_range("__code_start__", "__code_end__")

    data = map_file.get_range("__data_start__", "__data_end__")

    if section_alignment != PAGE_SIZE:
        raise ValueError(
            f"SectionAlignment must be 0x{PAGE_SIZE:X}: got 0x{section_alignment:X}"
        )

    if file_alignment != PAGE_SIZE:
        raise ValueError(
            f"FileAlignment must be 0x{PAGE_SIZE:X}: got 0x{file_alignment:X}"
        )

    if runtime_info_range.size != RuntimeInfo.sizeof():
        raise ValueError(
            f"0x{runtime_info_range.va_address:X} has size "
            f"0x{runtime_info_range.size:X}; "
            f"expected 0x{RuntimeInfo.sizeof():X}"
        )

    reloc_file_offset = reloc.va_address - image_base

    headers = PeHeaders(pe_path)

    with pe_path.open("rb") as pe:
        pe.seek(reloc_file_offset)
        reloc_data = pe.read(reloc.size)

    verify_relocations(
        reloc_data=reloc_data,
        image_base=image_base,
        data_address=data.va_address,
        data_size=data.size,
    )

    runtime_info = RuntimeInfo(
        start_rva=start_address - image_base,
        relocations=RelativeRange(
            offset_from_start=reloc.va_address - start_address,
            size=reloc.size,
        ),
        code=RelativeRange(
            offset_from_start=code.va_address - start_address,
            size=code.size,
        ),
        data=RelativeRange(
            offset_from_start=data.va_address - start_address,
            size=data.size,
        ),
    )

    packed_runtime_info = runtime_info.pack()

    runtime_info_file_offset = runtime_info_range.va_address - image_base

    patch_end = runtime_info_file_offset + len(packed_runtime_info)

    pe_size = pe_path.stat().st_size

    if patch_end > pe_size:
        raise ValueError(
            f"Patch range "
            f"[0x{runtime_info_file_offset:X}, "
            f"0x{patch_end:X}) "
            f"is outside the PE "
            f"(size=0x{pe_size:X})"
        )

    with pe_path.open("r+b") as pe:
        pe.seek(runtime_info_file_offset)
        pe.write(packed_runtime_info)

    headers.allow_relocation()


if __name__ == "__main__":
    main()
