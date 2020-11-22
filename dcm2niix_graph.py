#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot geometric mean for 
# load/init/minmax/rgba processing of DigiMorph Desert Iguana

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

#MacBook i5-8259U
#Ubuntu 3900X
#MacBookAir M1
df = pd.DataFrame({'CPU': ['8259U','3900X','M1-ARM'],
					'Time': [9.824, 1.374, 3.011]})
sns.set()
ax = sns.catplot(x="CPU", y="Time", kind="bar", data=df)
plt.savefig('dcm2niix.png', dpi=300)
