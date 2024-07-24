from itertools import cycle


def distribute_chunks_for_multiprocessing(events, n_cores):
    """assume that events with much work are at the start
    """
    work_loads = [list() for x in range(n_cores)]
    foreseen = cycle(work_loads)
    for ev in events:
        to_add = next(foreseen)
        to_add.append(ev)
    return work_loads
