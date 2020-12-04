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
#MacBook i5-8259U
#Ubuntu 3900X
#MackBook Air M1
df = pd.DataFrame({'CPU': ['8259U', '8259U', '3900X','3900X','M1-ARM','M1-ARM'],
					'Mode': ['Serial', 'Parallel', 'Serial', 'Parallel','Serial', 'Parallel'],
					'Time': [1088, 236, 658, 51, 576, 128]})


sns.set()
sns.catplot(x="CPU", y="Time", hue="Mode", kind="bar", data=df)
plt.savefig('openmp.png', dpi=300)
