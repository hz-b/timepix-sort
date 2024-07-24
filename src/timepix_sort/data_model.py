from dataclasses import dataclass
from typing import Sequence
import numpy as np


@dataclass
class Chunk:
    chip_nr: int
    payload: Sequence[np.uint64]


@dataclass
class PixelPosition:
    x: float
    y: float


@dataclass
class ReferenceEvent:
    time_of_arrival: float
    chip_nr: int
    id_: int
    # always this position ?
    pos: PixelPosition(-2, -2)


@dataclass
class PixelEvent:
    chip_nr: int
    time_of_arrival: float
    time_over_threshold: float
    id_: int
    pos: PixelPosition
