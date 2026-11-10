from pathlib import Path

import pefile


class PeHeaders:
    def __init__(self, path: Path) -> None:
        self.path = path
        self.pe = pefile.PE(str(path), fast_load=True)

        if (
            self.pe.FILE_HEADER.Machine
            != pefile.MACHINE_TYPE["IMAGE_FILE_MACHINE_AMD64"]
        ):
            raise ValueError("Only AMD64 PE files are supported")

        if self.pe.OPTIONAL_HEADER.Magic != pefile.OPTIONAL_HEADER_MAGIC_PE_PLUS:
            raise ValueError("Expected PE32+ optional header")

        if len(self.pe.OPTIONAL_HEADER.DATA_DIRECTORY) <= 12:
            raise ValueError("IAT data directory is missing")

    def allow_relocation(self) -> None:
        # The bootstrap handles relocations; Windows must be allowed to map
        # the image away from its preferred base.
        self.pe.FILE_HEADER.Characteristics &= ~pefile.IMAGE_CHARACTERISTICS[
            "IMAGE_FILE_RELOCS_STRIPPED"
        ]

        self.pe.write(filename=str(self.path))

    def clear_iat_directory(self) -> None:
        # Clear the IAT data directory (RVA and Size).
        iat = self.pe.OPTIONAL_HEADER.DATA_DIRECTORY[
            pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_IAT"]
        ]

        iat.VirtualAddress = 0
        iat.Size = 0

        self.pe.write(filename=str(self.path))
