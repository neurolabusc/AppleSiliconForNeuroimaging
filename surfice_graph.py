#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot geometric mean for 
# load/init/minmax/rgba processing of DigiMorph Desert Iguana

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
#MacBook i5-8259U
#Ubuntu 3900X
#MacBook M1 (7GPU)
df = pd.DataFrame({'CPU': ['8259U','3900X','M1-ARM'],
					'Time': [4903, 2814, 2013]})


sns.set()
sns.catplot(x="CPU", y="Time", kind="bar", data=df)
plt.savefig('surfice.png', dpi=300)
