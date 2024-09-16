"""

Warning:
    check that the numbers are correct
"""
from enum import IntEnum


class TDC1TriggerMode(IntEnum):
    rising_edge = 0x6F
    falling_edge = 0x6A


class TDC2TriggerMode(IntEnum):
    rising_edge = 0x6E
    falling_edge = 0x6B


class TDCEventType(IntEnum):
    timestamp = 0x6
    pixel = 0xB
