"""First steps towards using the extension
"""
import datetime
import numpy as np
from timepix_sort import _timepix_sort as _ts

_now = datetime.datetime.now

tic = _now()
data = np.fromfile("tests/data/Co_pos_0000.tpx3", dtype="<u8")
d1 = np.fromfile("large_data/Fe_pos_0000.tpx3", dtype="<u8")
d2 = np.fromfile("large_data/Fe_pos_0001.tpx3", dtype="<u8")
d3 = np.fromfile("large_data/Fe_pos_0002.tpx3", dtype="<u8")
d4 = np.fromfile("large_data/Fe_pos_0003.tpx3", dtype="<u8")
data = np.concatenate([d1,d2,d3,d4])
print(data.shape)
toc_ff = _now()

chunks = _ts.read_chunks(data)
toc_chunks = _now()

rising_edge = 0x6E;
# correct ?
falling_edge = 0x6f;
events = _ts.process(chunks, falling_edge, 6)
toc_processed = _now()

si_buf = events.sorted_indices()
sorted_indices = np.array(si_buf)

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
tas = pixels_diff.time_of_arrival()
txt = f"""Time required for processing steps
number of chunks     : { len(chunks) }
number of events     : { len(events) }

loading              : { toc_ff            - tic           }
reading chunks       : { toc_chunks        - toc_ff        }
processing events    : { toc_processed     - toc_chunks    }
sorting indices      : { toc_sorted        - toc_processed }
pixel events         : { toc_pixels        - toc_sorted    }
sorting  pixel event : { toc_pixels_sorted - toc_pixels    }

total                : { toc_pixels_sorted - tic           }
"""
print(txt)
