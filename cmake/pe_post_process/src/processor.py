from pathlib import Path

from .binary.sequence import BinarySequence
from .binary.types import RelativeRange, UInt64
from .map_file import MapFile
from .pe.headers import PeHeaders
from .pe.relocations import verify_relocations

PAGE_SIZE = 0x1000


def process(map_path: Path) -> None:
    map_file = MapFile(map_path)
    pe_path = map_file.find_output_path()

    image_base = map_file.find_symbol_address("__image_base__")

    section_alignment = map_file.find_symbol_address("__section_alignment__")

    file_alignment = map_file.find_symbol_address("__file_alignment__")

    start_address = map_file.find_symbol_address("__start__")

    reloc = map_file.get_range("__reloc_start__", "__reloc_end__")

    imports = map_file.get_range("__idata_start__", "__idata_end__")

    runtime_range = map_file.get_range("__runtime_info_start__", "__runtime_info_end__")

    code = map_file.get_range("__code_start__", "__code_end__")

    data = map_file.get_range("__data_start__", "__data_end__")

    if section_alignment != PAGE_SIZE:
        raise ValueError("Invalid section alignment")

    if file_alignment != PAGE_SIZE:
        raise ValueError("Invalid file alignment")

    runtime_info = BinarySequence(
        (
            # Entry point RVA
            UInt64(start_address - image_base),
            # Relocated image base
            UInt64(image_base),
            # Relocations
            RelativeRange(
                offset_from_start=reloc.va_address - start_address,
                size=reloc.size,
            ),
            # Imports
            RelativeRange(
                offset_from_start=imports.va_address - start_address,
                size=imports.size,
            ),
            # Application code
            RelativeRange(
                offset_from_start=code.va_address - start_address,
                size=code.size,
            ),
            # Application data
            RelativeRange(
                offset_from_start=data.va_address - start_address,
                size=data.size,
            ),
        ),
    )

    packed = runtime_info.pack()

    if runtime_range.size != len(packed):
        raise ValueError(
            f"Invalid runtime info size: "
            f"expected 0x{runtime_range.size:X}, "
            f"got 0x{len(packed):X}",
        )

    reloc_offset = reloc.va_address - image_base

    with pe_path.open("rb") as pe:
        pe.seek(reloc_offset)
        reloc_data = pe.read(reloc.size)

    verify_relocations(
        reloc_data,
        image_base,
        data.va_address,
        data.size,
    )

    packed = runtime_info.pack()

    runtime_offset = runtime_range.va_address - image_base

    if runtime_offset < 0:
        raise ValueError("Negative runtime offset")

    if runtime_offset + len(packed) > pe_path.stat().st_size:
        raise ValueError("Runtime info outside PE")

    headers = PeHeaders(pe_path)

    with pe_path.open("r+b") as pe:
        pe.seek(runtime_offset)
        pe.write(packed)

    headers.allow_relocation()
    headers.clear_iat_directory()
