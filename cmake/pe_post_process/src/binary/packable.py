from abc import ABC, abstractmethod


class BinaryPackable(ABC):
    @abstractmethod
    def pack(self) -> bytes: ...

    @classmethod
    @abstractmethod
    def sizeof(cls) -> int: ...
