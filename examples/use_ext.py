"""First steps towards using the extension
"""
import datetime
import numpy as np
import timepix_sort as ts
print(ts)
# from timepix_sort import _timepix_sort as _ts
from timepix_sort.read import read_file_to_buffer, read_chunks
from timepix_sort.process import process_chunks
from timepix_sort.post_process import data_to_points, data_to_volume
from timepix_sort.config import TDC1TriggerMode
import pyvista as pv
import matplotlib

matplotlib.use("qt5agg")

import matplotlib.pyplot as plt


_now = datetime.datetime.now

tic = _now()
data = read_file_to_buffer("tests/data/Co_pos_0000.tpx3")
d1 = read_file_to_buffer("large_data/Fe_pos_0000.tpx3")
d2 = read_file_to_buffer("large_data/Fe_pos_0001.tpx3")
d3 = read_file_to_buffer("large_data/Fe_pos_0002.tpx3")
d4 = read_file_to_buffer("large_data/Fe_pos_0003.tpx3")
# data = np.concatenate([d1, d2, d3, d4])
data = d4
# del d1, d2, d3, d4
print(data.shape)
toc_ff = _now()
chunks = read_chunks(data)
toc_chunks = _now()
last_offset = 0

events, events_statistics = process_chunks(chunks, TDC1TriggerMode.rising_edge, 10)
toc_processed = _now()
print(events_statistics)
diff = events_statistics.n_events - (
    events_statistics.n_timestamps +
    events_statistics.n_pixels +
    events_statistics.n_global_time +
    events_statistics.n_control_indications
)
txt = f"""Event statistics
# of events  {events_statistics.n_events: 8d}

consisting of
pixel        {events_statistics.n_pixels: 8d}
timestamps   {events_statistics.n_timestamps: 8d}
global time  {events_statistics.n_global_time: 8d}
control      {events_statistics.n_control_indications: 8d}

# difference
             {diff: 8d}
"""
print(txt)
si_buf = events.sorted_indices()
sorted_indices = np.array(si_buf, copy=False)

print("sorted indices", sorted_indices[:10], type(sorted_indices), sorted_indices.dtype)
toc_sorted = _now()
pixels_diff = events.pixel_events_with_difference_time(si_buf)
print("len of pixel diff", len(pixels_diff))
toc_pixels = _now()
if not pixels_diff.is_sorted:
    # sorting pixels diff
    pixels_diff.sort()
toc_pixels_sorted = _now()

tmp = np.arange(0, 20)
lut = np.array([tmp, tmp], dtype=np.uint64).T
lut[:, 0] *= int(1.6e6 / 20)
volume = np.zeros([525, 524, len(tmp)], dtype=np.int32)
data_to_volume(pixels_diff, lut, volume)
# print("volume sum", np.sum(np.sum(volume)))

pointsr = data_to_points(pixels_diff)
# = _ts.data_to_volume(pixels_diff)
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
print(txt)
print("creating plots")
if True:
    fig, ax_diff = plt.subplots(1, 1)
    fs = 1e-15
    # ax_overview, ax_diff = axes
    # ax_overview.hist(events.time_of_arrival() * fs, bins=10 * 1000)
    bins = np.arange(1600)
    ax_diff.hist(pixels_diff.time_of_arrival() * fs * 1e9, bins=bins)
    ax_diff.set_label("time of arrival [ns]")
    plt.ioff()
    plt.show()

if True:
    fig, ax = plt.subplots(1, 1)
    tmp = np.sum(volume, axis=-1)
    ref = np.max(tmp.ravel())
    print("ref")
    ax.imshow(tmp, cmap="grey", vmin=0, vmax=ref/3.0)
    # print(tmp.)
    plt.show()

if False:
    # print(len(set(tas)))

    points = pointsr.astype(np.float32)
    points = (points * np.array([1, 1, 1e-6])[np.newaxis, :]).astype(np.float32)
    points = points[:, :]
    print(points.min(axis=0), points.max(axis=0))
    print(pointsr[-2].min(axis=0), pointsr[-2].max(axis=0))
    pl = pv.Plotter(lighting="three lights")
    # pl.add_points(
    #     points,
    #     scalars=pixels_diff.time_over_threshold(),
    #     render_points_as_spheres=True,
    #     point_size=1,
    #     emissive=True,
    #     show_scalar_bar=True,
    # )
    pl.add_volume(volume)
    pl.show_bounds(
        xtitle="pix x",
        ytitle="pix y",
        ztitle="time of arrival [ns]",
        show_zlabels=True,
        color="k",
        font_size=26,
    )
    pl.add_text("Co pos 0000")
    pl.show()
