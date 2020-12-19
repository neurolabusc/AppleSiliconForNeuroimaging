#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot geometric mean for 
# fslmaths 4drest  -Tmean -mul -1 -add 4drest temp
# fslmaths 4drest  -bptf 77 8.68 temp
# fslmaths 4drest -s 2.548 temp
# fslmaths 3dt1  -kernel boxv 7 -dilM temp

import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
df = pd.DataFrame({'CPU': ['8259U','8259U','8259U','8259U','8259U','8259U','3900X','3900X','3900X','3900X','3900X','3900X','M1','M1','M1','M1','M1','M1'],
					'Threads': [1,2,4,8,12,24,1,2,4,8,12,24,1,2,4,8,12,24],
					'Time': [124,80,67,60,np.nan,np.nan, 101,74,71,53,45,36, 67,57,48,47,np.nan,np.nan]})

sns.set()
ax = sns.lineplot(x='Threads', y='Time', hue='CPU', data=df, marker='o')
#plt.show()
plt.savefig('afni_bench.png', dpi=300)
