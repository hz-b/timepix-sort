"""First steps towards using the extension
"""
import datetime
import numpy as np
from timepix_sort import _timepix_sort as _ts

import pyvista as pv
import matplotlib
import pandas as pd

matplotlib.use("qt5agg")
# matplotlib.use("gtk4agg")
import matplotlib.pyplot as plt

print(_ts)


_now = datetime.datetime.now

tic = _now()
data = np.fromfile("tests/data/Co_pos_0000.tpx3", dtype="<u8")
d1 = np.fromfile("large_data/Fe_pos_0000.tpx3", dtype="<u8")
# d2 = np.fromfile("large_data/Fe_pos_0001.tpx3", dtype="<u8")
# d3 = np.fromfile("large_data/Fe_pos_0002.tpx3", dtype="<u8")
# d4 = np.fromfile("large_data/Fe_pos_0003.tpx3", dtype="<u8")
# data = np.fromfile("tests/data/Co_test_0000.tpx3", dtype="<u8")
# data = np.concatenate([d1, d2, d3, d4])
data = d1
print(data.shape)
toc_ff = _now()
chunks = _ts.read_chunks(data)
toc_chunks = _now()
last_offset = 0

rising_edge = 0x6E
# correct ?
falling_edge = 0x6F
events, events_statistics = _ts.process(chunks, falling_edge, 10)
toc_processed = _now()

si_buf = events.sorted_indices()
sorted_indices = np.array(si_buf, copy=False)

if False:
    cnt = 0
    for idx in sorted_indices:
        event = events[idx]
        if event.time_of_arrival == 0:
            continue
        cnt += 1
        print(cnt, event)
        if cnt > 10:
            break

print("sorted indices", sorted_indices[:10], type(sorted_indices), sorted_indices.dtype)
toc_sorted = _now()
pixels_diff = events.pixel_events_with_difference_time(si_buf)
toc_pixels = _now()
if not pixels_diff.is_sorted:
    # sorting pixels diff
    pixels_diff.sort()
toc_pixels_sorted = _now()

# print("pixel_diff")
# cnt = 0
# for event in pixels_diff:
#     if event.time_of_arrival in [0, 4, 2500, 2504]:
#         continue
#     cnt += 1
#     print(cnt, event)
#     if cnt > 20:
#         break
#
#
tmp = np.arange(0, 2000)
lut = np.array([tmp, tmp]).T
lut[:, 0] *= int(2e9 / 2000)
# volume = np.zeros([525, 524, len(tmp)], dtype=np.uint16)
# _ts.data_to_volume(pixels_diff, lut, volume)
# print("volume sum", np.sum(np.sum(volume)))

pointsr = _ts.data_to_points(pixels_diff)
tas = pixels_diff.time_of_arrival()
toc_volume = _now()
#
txt = f"""Time required for processing steps
number of chunks     : { len(chunks) }
number of events     : { len(events) }

loading              : { toc_ff            - tic                }
reading chunks       : { toc_chunks        - toc_ff             }
processing events    : { toc_processed     - toc_chunks         }
sorting indices      : { toc_sorted        - toc_processed      }
pixel events         : { toc_pixels        - toc_sorted         }
sorting  pixel event : { toc_pixels_sorted - toc_pixels         }
producing volume     : { toc_volume        - toc_pixels_sorted  }

total                : { toc_volume        - tic                }
"""
print("creating plots")
fig, axes = plt.subplots(2, 1)
fs = 1e-15
ax_overview, ax_diff = axes
# ax_overview.hist(events.time_of_arrival() * fs, bins=10 * 1000)
ax_diff.hist(pixels_diff.time_of_arrival() * fs * 1e9, bins=1000)
ax_diff.set_label("time of arrival [ns]")
print(txt)
plt.ioff()
plt.show()

if True:
    # print(len(set(tas)))
    points = pointsr.astype(np.float32)
    points = (points * np.array([1, 1, 1e-6])[np.newaxis, :]).astype(np.float32)
    # points = points[::10, :]
    print(points.min(axis=0), points.max(axis=0))
    print(pointsr[-2].min(axis=0), pointsr[-2].max(axis=0))
    pv.plot(
        points,
        scalars=pixels_diff.time_over_threshold(),
        render_points_as_spheres=True,
        point_size=1,
        show_scalar_bar=True,
    )
