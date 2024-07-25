from typing import Sequence
import numpy as np

from timepix_sort.config import TDCEventType
from timepix_sort.data_model import Chunk, TimeOfFlightEvent, PixelEvent, PixelPosition


def t_in_seconds_to_fs(t: float) -> np.datetime64:
    """ready to be converted to `numpy.datetime64`
    """
    #: seconds to femto seconds
    s2fs = 1e12
    return np.datetime64(round(t * s2fs), 'fs')


def tdc_time_stamp(datum: int, index: int, chip_nr: int):

    coarsetime = (datum >> 9) & 0x1FFFFFFFF
    coarsetimef = coarsetime * (1 / 320e6)
    tmpfine = (datum >> 5) & 0xF

    if tmpfine > 0:
        TDC_timestamp = coarsetimef + (tmpfine - 1) * 260e-12
    else:
        raise AssertionError("TDC timestamp unknown!")
    time_of_arrival = TDC_timestamp
    return TimeOfFlightEvent(
        time_of_arrival=t_in_seconds_to_fs(time_of_arrival), chip_nr=chip_nr, id_=index
    )


def correct_time_of_arrival(xx, yy, TOA_s, TOT, w_Corr):
    return (
        TOA_s
        - (w_Corr[xx, yy, 0] + w_Corr[xx, yy, 1] * np.exp(-w_Corr[xx, yy, 2] * TOT))
    ) / 1e9  # / xy


def pixel_event(pkg: int, TOT_min, index: int, chip_nr: int):
    """ """
    # can I assume that the event is only of length ?
    spidrTime = pkg & 0xFFFF
    dcol = (pkg & 0x0FE0000000000000) >> 52  # (pkg >> 52) & 0xfe
    spix = (pkg & 0x001F800000000000) >> 45  # (pkg >> 45) & 0x1f8 //
    pix = (pkg & 0x0000700000000000) >> 44  # (pkg >> 44) & 0x7 //

    xx = dcol + pix / 4
    yy = spix + (pix & 0x3)

    # time of arrival
    TOA = (pkg >> (16 + 14)) & 0x3FFF
    # time over threshold
    TOT = (pkg >> (16 + 4)) & 0x3FF
    # fine time of arrival
    FTOA = (pkg >> 16) & 0xF
    # coarse time of arrival ?
    CTOA = (TOA << 4) | (~FTOA & 0xF)
    # time over treshshold

    if xx > 193 and xx < 204:  # "Tram line" correction
        CTOA = CTOA - 8
    else:
        CTOA = CTOA + 8

    if xx in [0, 255] or yy in [0, 255]:
        # mark every 3rd count at edge to equalize intensity
        centerpixel = index % 3
    else:
        centerpixel = 1

    spidrTime = spidrTime * 25 * 16384

    # todo: float or int division ?
    TOA_s = spidrTime + CTOA * (25 / 16)
    if chip_nr == 3:
        # correct for chip dependent TOT shift
        TOT_check = TOT_min - 1
    else:
        TOT_check = TOT_min

    if TOT > TOT_check and centerpixel != 0:
        # Remove events with lower TOT to improve time resolution
        # ignore  the switch for start
        if chip_nr == 0:
            xx = 255 - xx
            yy = 255 - yy + 258
        elif chip_nr == 1:
            xx = xx
            yy = yy
        elif chip_nr == 2:
            xx = xx + 258
            yy = yy
        elif chip_nr == 3:
            xx = 255 - xx + 258
            yy = 255 - yy + 258
        else:
            raise AssertionError(f"unknown chip number {chip_nr}")

        # todo: find out why offset is calculated before the next
        # pixel
        # Should the whole here be rather a filter
        # if xx == 255:
        #     xx += offset1
        #     offset1 = abs(mod(round(enoise(4)), 2))
        #
        # if xx == 258:
        #     xx -= offset2
        #     offset2 = abs(mod(round(enoise(4)), 2))
        #
        # if yy == 255:
        #     yy += offset1
        #     offset1 = abs(mod(round(enoise(4)), 2))
        #
        # if yy == 258:
        #     yy -= offset2
        #     offset2 = abs(mod(round(enoise(4)), 2))

        # can be dropped for start
        # time_of_arrival = correct_time_of_arrival(xx, yy, TOA_s, TOT)
        return PixelEvent(
            chip_nr=chip_nr,
            time_of_arrival=t_in_seconds_to_fs(TOA),
            time_over_threshold=t_in_seconds_to_fs(TOT),
            id_=index,
            pos=PixelPosition(x=xx, y=yy),
        )


def process_chunk(pkg: int, trigger_mode, tot_min, index: int, chip_nr: int):
    # todo: should there not be some mask?
    pkg_header = int(pkg) >> 60
    pkg_header2 = int(pkg) >> 56

    # event_type = TDCEventType(pkg_header)
    event_type = pkg_header
    if event_type == TDCEventType.timestamp:
        if pkg_header2 == trigger_mode:
            return tdc_time_stamp(int(pkg), index, chip_nr)
        else:
            pass
    elif event_type == TDCEventType.pixel:
        return pixel_event(pkg, tot_min, index, chip_nr)
    else:
        return None
        # raise AssertionError("How could I end up here?")


def process_chunks(events: Sequence[Chunk], trigger_mode, tot_min):
    for ev in events:
        for idx, datum in enumerate(ev.payload):
            yield process_chunk(int(datum), trigger_mode, tot_min, idx, ev.chip_nr)
