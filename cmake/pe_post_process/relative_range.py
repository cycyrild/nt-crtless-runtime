import struct
from dataclasses import dataclass


@dataclass(frozen=True)
class RelativeRange:
    offset_from_start: int
    size: int

    STRUCT = struct.Struct("<II")

    def pack(self) -> bytes:
        return self.STRUCT.pack(
            self.offset_from_start,
            self.size,
        )

    @classmethod
    def sizeof(cls) -> int:
        return cls.STRUCT.size
