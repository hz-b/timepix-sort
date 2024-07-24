import datetime
from functools import partial
from itertools import chain, cycle
import multiprocessing as mp
from pathlib import Path

import pickle
import time
import sys

stream = sys.stderr

import numpy as np
import matplotlib.pyplot as plt

from timepix_sort.config import TDC2TriggerMode
from timepix_sort.data_model import PixelEvent
from timepix_sort.read import read_chunks
from timepix_sort.process_chunks import process_chunks
from timepix_sort.utils import distribute_chunks_for_multiprocessing

data_dir = Path(__file__).parent.parent / "tests" / "data"
filename = data_dir / "Co_pos_0000.tpx3"


def prepare_chunks(filename):
    start = datetime.datetime.now()
    chunks = list(read_chunks(filename))
    # process large chunks first
    chunks.sort(key=lambda ch: len(ch.payload), reverse=True)
    print("chunk size varies from {}..{}".format(len(chunks[0].payload), len(chunks[-1].payload)))
    end = datetime.datetime.now()
    dt = (end - start).total_seconds()
    n_events = np.sum([len(ch.payload) for ch in chunks])
    print(f"Reading {len(chunks)} chunks required {dt:.3f} seconds: total number events {n_events}")
    return chunks


def process_one(chunk):
    n_events = len(chunk.payload)
    print(f"processing {n_events}")
    r = [ev for ev in process_chunks([chunk], trigger_mode=TDC2TriggerMode.rising_edge, tot_min=6) if ev is not None]
    print(f"processing {n_events} done")
    return r


def process(chunks):
    n_events = np.sum([len(ch.payload) for ch in chunks])
    # n_events = len(chunk.payload)
    stream.write(f"processing {n_events}\n")
    stream.flush()
    r = [ev for ev in process_chunks(chunks, trigger_mode=TDC2TriggerMode.rising_edge, tot_min=6) if ev is not None]
    stream.write(f"processing {n_events} done\n")
    stream.flush()
    return r


def process_file(filename):
    print(f"Processing file {filename}")

    chunks = prepare_chunks(filename)
    work_loads = distribute_chunks_for_multiprocessing(chunks, mp.cpu_count())

    start = datetime.datetime.now()

    pit =  len(chunks)//(mp.cpu_count() * 8)
    print(f"chunk iters size {pit}, for  {mp.cpu_count()} cpu's")
    with mp.Pool(mp.cpu_count()) as p:
        # events = p.imap_unordered(process_one, chunks, pit)
        events = p.imap_unordered(process, work_loads)
    end = datetime.datetime.now()
    dt = (end - start).total_seconds()
    events = list(events)
    print(
        f"Preparing {len(events)} events required {dt:.3f} seconds"
    )
    events = list(chain(*events))
    end2 = datetime.datetime.now()
    dt2 = (end2- end).total_seconds()
    print(
        f"Preparing {len(events)} events required {dt:.3f} seconds, chaining together {dt2:.3f} seconds "
    )

    start = datetime.datetime.now()
    def f(ev):
        return ev.time_of_arrival
    events.sort(key=f)
    end = datetime.datetime.now()
    print(f"Sorting {len(events)} events required {dt:.3f} seconds")
    with open(str(filename) + '.pk', 'wb') as fp:
        pickle.dump(events, fp)


data_dir = Path(__file__).parent.parent / "large_data"

filenames = [
    "Fe_pos_0000.tpx3",
    "Fe_pos_0001.tpx3",
    "Fe_pos_0002.tpx3",
    "Fe_pos_0003.tpx3",
    "Fe_pos_0004.tpx3"
    ]

for filename in filenames:
    process_file(data_dir / filename)
    break

# exit()

# print("Making plot")
# result = np.zeros([515, 514])
# for ev in events:
#     if isinstance(ev, PixelEvent):
#        x, y = ev.pos.x, ev.pos.y
#        # are these all -2
#        if x < 0 or y < 0:
#            continue
#        assert x >= 0
#        assert y >= 0
#        x = int(round(x))
#        y = int(round(y))
#        result[x, y] += 1
#
#np.save("image_data.npy", result)
#plt.imshow(result[100:350, :400])
#plt.axis("equal")
#plt.savefig("rough_data_result.png")
# plt.show()
