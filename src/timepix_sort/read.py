import numpy as np
from timepix_sort.data_model import Chunk


def read_chunks(fp):
    yield from chunks(read(fp))


def read(fp):
    return np.fromfile(fp, dtype="<u8")


def process_header(datum: int):
    tmp = [((datum >> 8 * i) & 0xFF) for i in range(4)]
    name = "".join([chr(t) for t in tmp])
    if name != "TPX3":
        raise AssertionError(f"datum did not contain TPX3 but {name} {tmp}")
    chip_nr = (int(datum) >> 32) & 0xFF
    if chip_nr not in [0, 1, 2, 3]:
        raise AssertionError(f"chip nr {chip_nr} not 0..3")
    n_entries = ((int(datum) >> 48) & 0xFFFF) // 8
    if n_entries == 0:
        raise AssertionError("header said: no entries")
    return chip_nr, n_entries


def chunks(stream):
    start = 0
    while start < len(stream):
        header = int(stream[start])
        chip_nr, n_entries = process_header(header)
        if n_entries > 1500:
            raise AssertionError(f"Did not expect {n_entries}")
        end = start + n_entries + 1
        event = stream[start + 1: end]
        yield Chunk(chip_nr=chip_nr, payload=event)
        start = end
