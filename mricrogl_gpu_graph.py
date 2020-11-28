#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot time for blur/sobel gradient applied to DigiMorph Desert Iguana

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
#MacBook i5-8259U
#Ubuntu i7-8700K
#Ubuntu 3900X
df = pd.DataFrame({'CPU': ['8259U','8700K','3900X','M1-ARM'],
					'Time': [1261, 246, 163, 534]})


sns.set()
sns.catplot(x="CPU", y="Time", kind="bar", data=df)
plt.savefig('mricrogl_gpu.png', dpi=300)
