from datetime import datetime
import logging
import numpy as np
from typing import Sequence, Union
from timepix_sort.data_model import PixelEvent, TimeOfFlightEvent

logger = logging.getLogger('timepix_sport')


def sort_events(events: Sequence[Union[PixelEvent,TimeOfFlightEvent]]) -> Sequence[Union[PixelEvent,TimeOfFlightEvent]]:
    def f_sort_val(event: Union[TimeOfFlightEvent,PixelEvent]):
        return event.time_of_arrival

    # numpy again faster?
    events = np.array(events, dtype=object)
    sort_vec = np.array([ev.time_of_arrival for cnt, ev in enumerate(events)])
    start = datetime.now()
    indices = np.argsort(sort_vec)
    end = datetime.now()
    events = events[indices]
    end2 = datetime.now()
    dt = (end - start).total_seconds()
    dt3 = (end2 - end).total_seconds()
    logger.warning(
        f"Sorting {len(events)} events required {dt:.3f} seconds,"
        f" rearranging required {dt3:.3f} seconds"
    )
    return events
