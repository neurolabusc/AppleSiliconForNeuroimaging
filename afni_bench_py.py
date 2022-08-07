#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot geometric mean for 
# bigger speed test
#  https://sscc.nimh.nih.gov/afni/doc/misc/afni_speed/index_html

import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
df = pd.DataFrame({'CPU': ['8259U','8259U','8259U','8259U','8259U','8259U','3900X','3900X','3900X','3900X','3900X','3900X','M1','M1','M1','M1','M1','M1','5950X','5950X','5950X','5950X','5950X','5950X','Altra','Altra','Altra','Altra','Altra','Altra','7551','7551','7551','7551','7551','7551','8358','8358','8358','8358','8358','8358'],
					'Threads': [1,2,4,8,12,24,1,2,4,8,12,24,1,2,4,8,12,24,1,2,4,8,12,24,1,2,4,8,12,24,1,2,4,8,12,24,1,2,4,8,12,24],
					'Time': [124,80,67,60,np.nan,np.nan, 101,74,71,53,45,36, 67,57,48,47,np.nan,np.nan,71, 50, 40, 33, 32, 30,119.2,100.6,63.7,54.2,50.9,55.2,140,99.0,90.8,97.3, 89.4, 85.6,106.9,76.2,59.4,49.8,47.0,50.0]})
sns.set()
ax = sns.lineplot(x='Threads', y='Time', hue='CPU', data=df, marker='o')
#plt.show()
plt.savefig('afni_bench.png', dpi=300)
