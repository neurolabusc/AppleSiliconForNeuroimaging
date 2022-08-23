#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot geometric mean for nibabel tests (see nibabel.tsv)
# can be generated online:
#  https://www.codabrainy.com/en/python-compiler/

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

#MacBook i5-8259U
#Ubuntu i7-8700K
#Ubuntu 3900X
df = pd.DataFrame({'CPU': ['8259U','3900X', 'M1-x86', 'M1-ARM','A1','N1'],
					'Time': [26.5, 12.418, 14.08, 10.563, 20.6, 23.0]})
sns.set()
ax = sns.catplot(x="CPU", y="Time", kind="bar", data=df)
plt.savefig('nibabel.png', dpi=300)
