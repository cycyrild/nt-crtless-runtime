import struct

from dataclasses import dataclass
from typing import ClassVar


IMAGE_REL_BASED_ABSOLUTE = 0
IMAGE_REL_BASED_DIR64 = 10

DIR64_SIZE = 8


@dataclass(frozen=True)
class ImageRelocationRecord:
    offset: int
    type: int

    STRUCT: ClassVar[struct.Struct] = struct.Struct("<H")

    @classmethod
    def unpack_from(
        cls,
        buffer: bytes,
        offset: int = 0,
    ) -> "ImageRelocationRecord":
        (value,) = cls.STRUCT.unpack_from(buffer, offset)

        return cls(
            offset=value & 0x0FFF,
            type=value >> 12,
        )

    @classmethod
    def sizeof(cls) -> int:
        return cls.STRUCT.size


@dataclass(frozen=True)
class ImageBaseRelocation:
    virtual_address: int
    size_of_block: int

    STRUCT: ClassVar[struct.Struct] = struct.Struct("<II")

    @classmethod
    def unpack_from(
        cls,
        buffer: bytes,
        offset: int = 0,
    ) -> "ImageBaseRelocation":
        return cls(*cls.STRUCT.unpack_from(buffer, offset))

    @classmethod
    def sizeof(cls) -> int:
        return cls.STRUCT.size


def verify_relocations(
    reloc_data: bytes,
    image_base: int,
    data_address: int,
    data_size: int,
) -> None:
    data_end = data_address + data_size

    block_offset = 0

    while block_offset < len(reloc_data):
        remaining = len(reloc_data) - block_offset

        if remaining < ImageBaseRelocation.sizeof():
            raise ValueError(
                f"Truncated IMAGE_BASE_RELOCATION at offset 0x{block_offset:X}"
            )

        block = ImageBaseRelocation.unpack_from(
            reloc_data,
            block_offset,
        )

        if block.size_of_block < ImageBaseRelocation.sizeof():
            raise ValueError(
                f"Invalid relocation block size "
                f"0x{block.size_of_block:X} at "
                f"offset 0x{block_offset:X}"
            )

        block_end = block_offset + block.size_of_block

        if block_end > len(reloc_data):
            raise ValueError(
                f"Relocation block at offset "
                f"0x{block_offset:X} extends beyond "
                f"the relocation table"
            )

        records_size = block.size_of_block - ImageBaseRelocation.sizeof()

        if records_size % ImageRelocationRecord.sizeof() != 0:
            raise ValueError(
                f"Invalid relocation block size "
                f"0x{block.size_of_block:X} at "
                f"offset 0x{block_offset:X}"
            )

        record_offset = block_offset + ImageBaseRelocation.sizeof()

        while record_offset < block_end:
            record = ImageRelocationRecord.unpack_from(
                reloc_data,
                record_offset,
            )

            record_offset += ImageRelocationRecord.sizeof()

            if record.type == IMAGE_REL_BASED_ABSOLUTE:
                continue

            if record.type != IMAGE_REL_BASED_DIR64:
                raise ValueError(
                    f"Unsupported relocation type "
                    f"{record.type} at relocation table "
                    f"offset 0x{record_offset - ImageRelocationRecord.sizeof():X}"
                )

            target_address = image_base + block.virtual_address + record.offset

            target_end = target_address + DIR64_SIZE

            if target_address < data_address or target_end > data_end:
                raise ValueError(
                    f"DIR64 relocation target "
                    f"[0x{target_address:X}, "
                    f"0x{target_end:X}) is outside "
                    f"data range "
                    f"[0x{data_address:X}, "
                    f"0x{data_end:X})"
                )

        block_offset = block_end
