#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot geometric mean for 
# fslmaths 4drest  -Tmean -mul -1 -add 4drest temp
# fslmaths 4drest  -bptf 77 8.68 temp
# fslmaths 4drest -s 2.548 temp
# fslmaths 3dt1  -kernel boxv 7 -dilM temp

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
df = pd.DataFrame({'CPU': ['8259U','8259U','8259U','8259U','8259U','8259U','8259U','8259U','8700K','8700K','8700K','8700K','8700K','8700K','8700K','8700K','3900X','3900X','3900X','3900X','3900X','3900X','3900X','3900X','M1','M1','M1','M1','M1','M1','M1','M1'],
					'Mode': ['pigzCF','pigzCF','pigzCF','pigzCF','pigzSys','pigzSys','pigzSys','pigzSys','pigzCF','pigzCF','pigzCF','pigzCF','pigzSys','pigzSys','pigzSys','pigzSys','pigzCF','pigzCF','pigzCF','pigzCF','pigzSys','pigzSys','pigzSys','pigzSys','pigzCF','pigzCF','pigzCF','pigzCF','pigzSys','pigzSys','pigzSys','pigzSys'],
					'Threads': [1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5,1,2,4,5],
					'mbs': [50,97,174,196.3333333,28,53.66666667,96.33333333,136,72.66666667,126.6666667,232.6666667,408,39.33333333,71.66666667,134,263.6666667,67.66666667,125,231,568.6666667,38.66666667,71.66666667,134.3333333,446.3333333,71.33333333,138.3333333,256,343.6666667,45.33333333,87,163,221.6666667]})

sns.set()
ax = sns.lineplot(x='Threads', y='mbs', hue='CPU', data=df, marker='o')
#plt.show()
plt.savefig('pigz.png', dpi=300)
