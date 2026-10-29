import struct
from pathlib import Path


class PeHeaders:
    IMAGE_FILE_RELOCS_STRIPPED = 0x0001

    def __init__(self, path: Path) -> None:
        self.path = path

        with path.open("rb") as pe:
            dos_header = pe.read(0x40)
            if len(dos_header) != 0x40 or dos_header[:2] != b"MZ":
                raise ValueError(f"Invalid DOS header in {path}")

            (pe_header_offset,) = struct.unpack_from("<I", dos_header, 0x3C)
            pe.seek(pe_header_offset)
            pe_header = pe.read(24)
            if len(pe_header) != 24 or pe_header[:4] != b"PE\0\0":
                raise ValueError(f"Invalid PE header in {path}")

        self._characteristics_offset = pe_header_offset + 22
        (self._characteristics,) = struct.unpack_from("<H", pe_header, 22)

    def allow_relocation(self) -> None:
        # The bootstrap handles relocations; Windows must be allowed to map
        # the image away from its preferred base.
        self._characteristics &= ~self.IMAGE_FILE_RELOCS_STRIPPED

        with self.path.open("r+b") as pe:
            pe.seek(self._characteristics_offset)
            pe.write(struct.pack("<H", self._characteristics))
