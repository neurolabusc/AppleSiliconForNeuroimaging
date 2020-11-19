#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot geometric mean for 
# load/init/minmax/rgba processing of DigiMorph Desert Iguana

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
#MacBook i5-8259U
#Ubuntu i7-8700K
#Ubuntu 3900X
df = pd.DataFrame({'CPU': ['8259U','8700K','3900X','M1-ARM'],
					'Time': [333.6420985, 191.1947629, 212.9833438, 310.8975668]})


sns.set()
sns.catplot(x="CPU", y="Time", kind="bar", data=df)
plt.savefig('mricrogl.png', dpi=300)
