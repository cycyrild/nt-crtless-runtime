import struct
from dataclasses import dataclass
from typing import ClassVar

from .packable import BinaryPackable


@dataclass(frozen=True)
class UInt32(BinaryPackable):
    value: int

    STRUCT: ClassVar[struct.Struct] = struct.Struct("<I")

    def pack(self) -> bytes:
        return self.STRUCT.pack(self.value)

    @classmethod
    def sizeof(cls) -> int:
        return cls.STRUCT.size


@dataclass(frozen=True)
class RelativeRange(BinaryPackable):
    offset_from_start: int
    size: int

    STRUCT: ClassVar[struct.Struct] = struct.Struct("<II")

    def pack(self) -> bytes:
        return self.STRUCT.pack(
            self.offset_from_start,
            self.size,
        )

    @classmethod
    def sizeof(cls) -> int:
        return cls.STRUCT.size
