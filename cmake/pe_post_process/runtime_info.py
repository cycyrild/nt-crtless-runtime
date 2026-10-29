import struct
from dataclasses import dataclass

from pe_post_process.relative_range import RelativeRange


@dataclass(frozen=True)
class RuntimeInfo:
    start_rva: int
    relocations: RelativeRange
    code: RelativeRange
    data: RelativeRange

    START_RVA_STRUCT = struct.Struct("<I")

    def pack(self) -> bytes:
        return b"".join(
            (
                self.START_RVA_STRUCT.pack(self.start_rva),
                self.relocations.pack(),
                self.code.pack(),
                self.data.pack(),
            )
        )

    @classmethod
    def sizeof(cls) -> int:
        return cls.START_RVA_STRUCT.size + 3 * RelativeRange.sizeof()
