import datetime
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt
from timepix_sort.config import TDC2TriggerMode, TDC1TriggerMode
from timepix_sort.read import read_chunks, read_file_to_buffer
from timepix_sort.process import process_chunks
from timepix_sort.post_process import data_to_volume

data_dir = Path(__file__).parent.parent / "tests" / "data"
filename = data_dir / "Co_pos_0000.tpx3"
# filename = data_dir / "Co_test_0000.tpx3"


start = datetime.datetime.now()
chunks = read_chunks(read_file_to_buffer(filename))
events, event_statistics = process_chunks(chunks, TDC1TriggerMode.rising_edge, 6)
end = datetime.datetime.now()
dt = (end - start).total_seconds()
print(f"Processing {len(events)} events required {dt:.3f} seconds")

si_buf = events.sorted_indices()
sorted_indices = np.array(si_buf, copy=False)

pixels_diff = events.pixel_events_with_difference_time(si_buf)
pixels_diff.sort()
assert pixels_diff.is_sorted

tmp = np.arange(0, 20)
lut = np.array([tmp, tmp]).T
lut[:, 0] *= int(1.6e6 / 20)
volume = np.zeros([525, 524, len(tmp)], dtype=np.int16)
data_to_volume(pixels_diff, lut, volume)
result = np.sum(volume, axis=-1)
largest_hit = np.max(result.ravel())
lowest_hit = np.min(result.ravel())
np.save("image_data.npy", result)
print(f"hit range {lowest_hit} {largest_hit}")
plt.imshow(result[100:350, :400], vmax=largest_hit/3)
plt.axis("equal")
plt.savefig("rough_data_result.png")
