#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot geometric mean for nibabel tests (see nibabel.tsv)

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

#MacBook i5-8259U
#Ubuntu i7-8700K
#Ubuntu 3900X
df = pd.DataFrame({'CPU': ['8259U','3900X','M1-x86', 'M1-ARM', 'M1-ARM-1.23',],
					'Time': [26.5, 12.418, 14.08, 12.934, 10.563]})
sns.set()
ax = sns.catplot(x="CPU", y="Time", kind="bar", data=df)
plt.savefig('nibabel.png', dpi=300)
