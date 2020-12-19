#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
from numpy.testing import measure
#example where translated code (Python 3.8.3, NumPy version 1.19.4) is x10 faster than native code (Python 3.9.1rc1, NumPy version 1.19.4)
rng = np.random.RandomState(20111001)
img_shape = (128, 128, 64, 10)
repeat = 100
arr = rng.normal(size=img_shape)
mtime = measure('np.max(arr)', repeat)
print('%30s %6.2f' % ('max all finite', mtime))
mtime = measure('np.min(arr)', repeat)
print('%30s %6.2f' % ('min all finite', mtime))
arr[:, :, :, 1] = np.nan
mtime = measure('np.max(arr)', repeat)
print('%30s %6.2f' % ('max all nan', mtime))
mtime = measure('np.min(arr)', repeat)
print('%30s %6.2f' % ('min all nan', mtime))
