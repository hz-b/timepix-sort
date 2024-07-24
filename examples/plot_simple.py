import datetime
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt
from timepix_sort.config import TDC2TriggerMode
from timepix_sort.data_model import PixelEvent
from timepix_sort.read import read_chunks
from timepix_sort.process_chunks import process_chunks

data_dir = Path(__file__).parent.parent / "tests" / "data"
filename = data_dir / "Co_pos_0000.tpx3"
# filename = data_dir / "Co_test_0000.tpx3"


start = datetime.datetime.now()
events = [
    ev
    for ev in process_chunks(
        read_chunks(filename), trigger_mode=TDC2TriggerMode.rising_edge, tot_min=1
    )
    if ev is not None
]
end = datetime.datetime.now()
dt = (end - start).total_seconds()
print(f"Processing {len(events)} events required {dt:.3f} seconds")

result = np.zeros([515, 514])
for ev in events:
    if isinstance(ev, PixelEvent):
        x, y = ev.pos.x, ev.pos.y
        assert x >= 0
        assert y >= 0
        x = int(round(x))
        y = int(round(y))
        result[x, y] += 1

np.save("image_data.npy", result)
plt.imshow(result[100:350, :400])
plt.axis("equal")
plt.savefig("rough_data_result.png")
