from pathlib import Path

import timepix_sort
import numpy as np

from timepix_sort.config import TDC2TriggerMode
from timepix_sort.data_model import PixelEvent
from timepix_sort.read import chunks, process_chunks, read


def test_package_content():

    dir(timepix_sort)

def test_data_read():
    data_dir = Path(__file__).parent / "data"
    filename = data_dir / "Co_pos_0000.tpx3"
    # filename = data_dir / "Co_test_0000.tpx3"
    stream = read(filename, dtype='<u8')

    t_chunks = [ch for ch in chunks(stream)]
    assert len(t_chunks) == 178693
    events = [
        ev for ev in process_chunks(t_chunks, trigger_mode=TDC2TriggerMode.rising_edge, tot_min=1)
        if ev is not None
    ]

    assert len(events) > len(t_chunks)
    result = np.zeros([1024, 1024])
    for ev in events:
        if isinstance(ev, PixelEvent):
            x, y = ev.pos.x, ev.pos.y
            assert x >= 0
            assert y >= 0
            x = int(round(x))
            y = int(round(y))
            result[x, y] += 1
    np.save("image_data.npy", result)
    return
