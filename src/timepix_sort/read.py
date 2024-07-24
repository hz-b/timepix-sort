import itertools

import numpy as np
import logging
from timepix_sort.data_model import Chunk
from timepix_sort.exceptions import NoTPX3Header

logger = logging.getLogger("timepix_sort")


def read_chunks(fp):
    yield from chunks(read(fp))


def read(fp):
    return np.fromfile(fp, dtype="<u8")


def embedded_string(datum: int):
    bytes = [((datum >> 8 * i) & 0xFF) for i in range(4)]
    return  "".join([chr(t) for t in bytes]), bytes


def process_header(datum: int):
    name, bytes = embedded_string(datum)
    if name != "TPX3":
        raise NoTPX3Header(f"datum did not contain TPX3 but {name} {bytes}")
    chip_nr = (int(datum) >> 32) & 0xFF
    if chip_nr not in [0, 1, 2, 3]:
        raise AssertionError(f"chip nr {chip_nr} not 0..3")
    n_entries = ((int(datum) >> 48) & 0xFFFF) // 8
    if n_entries == 0:
        raise AssertionError("header said: no entries")
    return chip_nr, n_entries


def chunks(stream):
    start = 0
    last_start = 0
    loop_count = itertools.count()
    while start < len(stream):
        loop = next(loop_count)
        # two lions in chicago
        n_entries = None
        chip_nr = None
        header = int(stream[start])
        try:
            chip_nr, n_entries = process_header(header)
            valid_header = True
        except NoTPX3Header:
            valid_header = False

        if valid_header:
            assert n_entries is not None
            assert chip_nr is not None
            end = start + n_entries + 1
            event = stream[start + 1: end]
            yield Chunk(chip_nr=chip_nr, payload=event)
            last_start = start
            start = end
            continue

        # no valid header let's try to find one
        # recheck that there was a header at the last start
        _, chk_entries = process_header(int(stream[last_start]))
        logger.warning(f"step {loop_count} searching for new start (header) at file pos {last_start}")
        for cnt, datum in enumerate(stream[last_start+1:]):
            if embedded_string(int(datum))[0] == "TPX3":
                    logger.warning(
                        f" next header was found at offset {cnt} (expected {chk_entries + 1})"
                        f" next header was found with an offset of {cnt} == {last_start + cnt + 1}"
                    )
                    break
        else:
            logger.warning(f"no header found at {cnt}, still had {last_start + 1 + cnt - len(stream)} int64 left")
        end = start + cnt + 1
        last_start = start
        start = end

