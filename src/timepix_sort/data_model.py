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
class TimeOfFlightEvent:
    # typically stored in fs
    time_of_arrival: np.datetime64
    chip_nr: int
    id_: int
    # always this position ?
    pos: PixelPosition(-2, -2)


@dataclass
class PixelEvent:
    chip_nr: int
    # typically stored in fs
    time_of_arrival: np.datetime64
    # typically stored in fs
    time_over_threshold: np.datetime64
    id_: int
    pos: PixelPosition
