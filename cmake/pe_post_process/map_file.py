import re
from pathlib import Path
from typing import NamedTuple

RE_HEX = r"0x([0-9A-Fa-f]+)"


class ObjectInfo(NamedTuple):
    va_address: int
    size: int


class MapFile:
    def __init__(self, path: Path) -> None:
        self.path = path
        self.text = path.read_text(
            encoding="utf-8",
            errors="replace",
        )

    def find_symbol_address(
        self,
        symbol_name: str,
    ) -> int:
        pattern = re.compile(
            rf"^\s*{RE_HEX}\s+"
            rf"{re.escape(symbol_name)}"
            rf"(?:\s*=.*)?\s*$",
            re.MULTILINE,
        )

        match = pattern.search(self.text)

        if match is None:
            raise ValueError(f"Symbol {symbol_name!r} not found in {self.path}")

        return int(match.group(1), 16)

    def get_range(
        self,
        start_name: str,
        end_name: str,
    ) -> ObjectInfo:
        start = self.find_symbol_address(
            start_name,
        )

        end = self.find_symbol_address(
            end_name,
        )

        if end < start:
            raise ValueError(
                f"Invalid range: {end_name}=0x{end:X} < {start_name}=0x{start:X}"
            )

        return ObjectInfo(
            va_address=start,
            size=end - start,
        )

    def find_output_path(self) -> Path:
        pattern = re.compile(
            r"^\s*OUTPUT\((.+?)\s+pei-x86-64\)\s*$",
            re.MULTILINE,
        )

        match = pattern.search(self.text)

        if match is None:
            raise ValueError(f"OUTPUT(... pei-x86-64) not found in {self.path}")

        return Path(match.group(1))
