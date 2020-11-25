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
df = pd.DataFrame({'CPU': ['8259U', '8259U', '8259U', '8259U', '8259U', '8259U', '3900X','3900X','3900X','3900X','3900X','3900X', 'M1-ARM','M1-ARM','M1-ARM', 'M1-ARM','M1-ARM','M1-ARM'],
					'Mode': ['1x32','4x32', '8x32','1x64','2x64', '4x64','1x32','4x32', '8x32','1x64','2x64', '4x64','1x32','4x32', '8x32','1x64','2x64', '4x64'],
					'Time': [1366, 1603, 1727, 5075, 4765, 4618, 1008, 265, 186, 1515, 762, 341, 448, 437, 0, 1499, 0, 0]})


sns.set()
sns.catplot(x="CPU", y="Time", hue="Mode", kind="bar", data=df)
plt.savefig('simd.png', dpi=300)
