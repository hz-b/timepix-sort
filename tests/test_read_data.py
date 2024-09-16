from pathlib import Path
import numpy as np

from timepix_sort.config import TDC1TriggerMode
from timepix_sort.process import process_chunks
from timepix_sort.post_process import data_to_points
from timepix_sort.read import read_file_to_buffer, read_chunks


def test_read():
    data_dir = Path(__file__).parent / "data"
    filename = data_dir / "Co_pos_0000.tpx3"
    buffer = read_file_to_buffer(filename)
    chunks = read_chunks(buffer)
    events, events_statistics = process_chunks(chunks, TDC1TriggerMode.rising_edge, 0)

    si_buf = events.sorted_indices()
    sorted_indices = np.array(si_buf, copy=False)

    pixels_diff = events.pixel_events_with_difference_time(si_buf)
    pixels_diff.sort()
    assert pixels_diff.is_sorted

    points = data_to_points(pixels_diff)