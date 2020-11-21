#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot geometric mean for 
# load/init/minmax/rgba processing of DigiMorph Desert Iguana

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

#MacBook i5-8259U
#Ubuntu i7-8700K
#Ubuntu 3900X
df = pd.DataFrame({'CPU': ['8259U','3900X','M1-x86'],
					'Time': [11.614, 6.010, 6.68]})
sns.set()
ax = sns.catplot(x="CPU", y="Time", kind="bar", data=df)
plt.savefig('nibabel.png', dpi=300)
