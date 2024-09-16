from typing import Sequence
from timepix_sort._timepix_sort import read_chunks
import numpy as np


def read_file_to_buffer(filename):
    return np.fromfile(filename, dtype="<u8")

