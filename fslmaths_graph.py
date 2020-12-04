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
#Ubuntu i7-8700K
#Ubuntu 3900X
df = pd.DataFrame({'CPU': ['8259U', '8259U', '8259U', '8700K','8700K','8700K','3900X','3900X','3900X','M1x86','M1x86','M1x86','M1-ARM','M1-ARM','M1-ARM'],
					'Mode': ['FSL','Serial', 'Parallel', 'FSL','Serial', 'Parallel', 'FSL','Serial', 'Parallel', 'FSL','Serial', 'Parallel', 'FSL','Serial', 'Parallel'],
					'Time': [267.232034,107.6176803, 36.78517745, 225.1618024, 52.98525377, 21.28169104, 166.2675524, 47.24493726, 12.39050298, 194.0339967, 72.6238476, 13.77630938, 0, 47.16417738, 15.9435970478868]})


sns.set()
sns.catplot(x="CPU", y="Time", hue="Mode", kind="bar", data=df)
plt.savefig('fslmaths.png', dpi=300)
