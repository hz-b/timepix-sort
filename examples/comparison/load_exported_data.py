import pandas as pd
import numpy as np
from pathlib import Path


t_dir = Path("/home/mfp/Devel/github/hz-b/timepix-sort-cpp/test_data")
filename = "Co_pos_0000._chunks_event_datum.txt"
path = t_dir / filename
# path_h5 = (path.parent.str()+ ".h5")
if True:
    df = pd.read_csv(path, sep='\t', comment='#', header=None)
    df.columns = ["offset", "chunk_n", "event_n", "event_type",  "timestamp", "chip_nr", "x_pix", "y_pix"]
    df.loc[:, "event_type"] = pd.Categorical(df.event_type.apply(lambda x: x.strip()))
    df.loc[:, "chip_nr"] = df.chip_nr.astype(int)
    # df.to_hdf(path_h5, key="data")
else:
    # df = pd.read_h5(path_h5, key="data")
    pass

ps_max = 26.8435456


sel = df.loc[df.event_type == "trigger", "timestamp"]
timestamp_range = np.array([sel.min() % ps_max, sel.max() % ps_max])
sel = df.loc[df.event_type == "pixel", "timestamp"]
pixel_range = np.array([sel.min(), sel.max()])

diff_range =  timestamp_range - pixel_range
txt = f"""
timestamp {timestamp_range[0]} -- {timestamp_range[-1]}, {timestamp_range[-1] - timestamp_range[0]}
pixel     {pixel_range[0]} -- {pixel_range[-1]} {pixel_range[-1] - pixel_range[0]}
diff      {diff_range} -- {diff_range} {diff_range - diff_range}
"""
print(txt)