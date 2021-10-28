#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot for compression of Silesia corpus at compression level 6 (b_speed_threads.py) 
# https://github.com/neurolabusc/pigz-bench-python

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
df = pd.DataFrame({'CPU': ['M1pro','M1pro','M1pro','M1pro','M1pro','M1pro','M1pro','M1pro','8259U','8259U','8259U','8259U','8259U','8259U','8259U','8259U','8700K','8700K','8700K','8700K','8700K','8700K','8700K','8700K','3900X','3900X','3900X','3900X','3900X','3900X','3900X','3900X','M1','M1','M1','M1','M1','M1','M1','M1', '5950X','5950X','5950X','5950X','5950X','5950X','5950X','5950X'],
					'Mode': ['pigzCF','pigzCF','pigzCF','pigzCF','pigzSys','pigzSys','pigzSys','pigzSys',   'pigzCF','pigzCF','pigzCF','pigzCF','pigzSys','pigzSys','pigzSys','pigzSys','pigzCF','pigzCF','pigzCF','pigzCF','pigzSys','pigzSys','pigzSys','pigzSys','pigzCF','pigzCF','pigzCF','pigzCF','pigzSys','pigzSys','pigzSys','pigzSys','pigzCF','pigzCF','pigzCF','pigzCF','pigzSys','pigzSys','pigzSys','pigzSys','pigzCF','pigzCF','pigzCF','pigzCF','pigzSys','pigzSys','pigzSys','pigzSys'],
					'Threads': [1,2,4,5,1,2,4,5,    1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5],
					'mbs': [72,140,266,407,39,73,140,226,   50,97,174,196.3333333,28,53.66666667,96.33333333,136,72.66666667,126.6666667,232.6666667,408,39.33333333,71.66666667,134,263.6666667,67.66666667,125,231,568.6666667,38.66666667,71.66666667,134.3333333,446.3333333,71.33333333,138.3333333,256,343.6666667,45.33333333,87,163,221.6666667, 75, 142, 272, 730, 38, 74, 140, 523]})

sns.set()
ax = sns.lineplot(x='Threads', y='mbs', hue='CPU', data=df, marker='o')
#plt.show()
plt.savefig('pigz.png', dpi=300)
