from collections import deque
import math
import sys
import time

class Timer:
    def __init__(self, f=sys.stdout):
        self._start_time = time.monotonic()
        self._time_history = deque([])
        self._file = f

    def _get_current_time(self):
        return time.monotonic() - self._start_time

    def add_received_data(self, data):
        current_time = self._get_current_time()
        current_second = math.floor(current_time)
        while (current_second >= len(self._time_history)):
            if (len(self._time_history) > 0):
                print(f'{len(self._time_history)}, {self._time_history[-1]}', file=self._file)
            else:
                print('Second, Received bits', file=self._file)

            self._time_history.append(0)
        self._time_history.append(self._time_history.pop() + len(data) * 8)
