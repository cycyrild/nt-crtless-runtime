from dataclasses import dataclass

from .packable import BinaryPackable


@dataclass(frozen=True)
class BinarySequence:
    items: tuple[BinaryPackable, ...]

    def pack(self) -> bytes:
        return b"".join(item.pack() for item in self.items)

    def size(self) -> int:
        return sum(type(item).sizeof() for item in self.items)
