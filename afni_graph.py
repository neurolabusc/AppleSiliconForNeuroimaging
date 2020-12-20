#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot total wall clock time to complete AFNI_data6 https://afni.nimh.nih.gov/pub/dist/doc/htmldoc/background_install/unix_tutorial/misc/install.data.html
# s01.ap.simple  

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
#MacBook i5-8259U
#Ubuntu 3900X
#MacBook Air M1
df = pd.DataFrame({'CPU': ['8259U','3900X','M1-ARM'],
					'Time': [440, 269, 174]})


sns.set()
sns.catplot(x="CPU", y="Time", kind="bar", data=df)
plt.savefig('afni.png', dpi=300)
