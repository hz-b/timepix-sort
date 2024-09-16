from pathlib import Path

import timepix_sort
import numpy as np
from timepix_sort import _timepix_sort as _ts




def test_data_read():
    data_dir = Path(__file__).parent / "data"
    filename = data_dir / "Co_pos_0000.tpx3"
    data = np.fromfile(filename, dtype="<u8")
    chunks = _ts.read_chunks(data)
    falling_edge = 0x6F
    events, events_statistics = _ts.process(chunks, falling_edge, 10)

    si_buf = events.sorted_indices()
    sorted_indices = np.array(si_buf, copy=False)

    pixels_diff = events.pixel_events_with_difference_time(si_buf)

    points = _ts.data_to_points(pixels_diff)

    tmp = np.arange(0, 20)
    lut = np.array([tmp, tmp]).T
    lut[:, 0] *= int(1.6e6 / 20)
    volume = np.zeros([525, 524, len(tmp)], dtype=np.uint16)
    _ts.data_to_volume(pixels_diff, lut, volume)
