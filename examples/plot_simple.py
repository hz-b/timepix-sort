from datetime import datetime
from pathlib import Path

import numpy as np
import matplotlib.pyplot as plt
from timepix_sort.config import TDC2TriggerMode
from timepix_sort.data_model import PixelEvent, TimeOfFlightEvent
from timepix_sort.read import read_chunks
from timepix_sort.process_chunks import process_chunks
from timepix_sort.sort import sort_events

data_dir = Path(__file__).parent.parent / "tests" / "data"
filename = data_dir / "Co_pos_0000.tpx3"

# data_dir = Path(__file__).parent.parent / "large_data"
# filename = data_dir / "Fe_pos_0001.tpx3"

start = datetime.now()
chunks = list(read_chunks(filename))
end = datetime.now()
dt = (end - start).total_seconds()
print(f"Reading {len(chunks)} chunks  required {dt:.3f} seconds")

start = datetime.now()
events = [
    ev
    for ev in process_chunks(
        chunks, trigger_mode=TDC2TriggerMode.rising_edge, tot_min=6
    )
    if ev is not None
]
end = datetime.now()
dt = (end - start).total_seconds()
print(f"Perocessing {len(events)} events required {dt:.3f} seconds")
events = sort_events(events)

result = np.zeros([515, 514])
for ev in events:
    if isinstance(ev, PixelEvent):
        x, y = ev.pos.x, ev.pos.y
        if x < 0 or y < 0:
            continue
        x = int(round(x))
        y = int(round(y))
        result[x, y] += 1

np.save("image_data.npy", result)
plt.imshow(result)
plt.axis("equal")
plt.savefig("rough_data_result.png")
