#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot mean time to calculate matrix multiplication
# main_mmul.cpp

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
#MacBook Air M1
#Ubuntu 3900X
df = pd.DataFrame({'CPU': ['3900X','3900X', 'M1-ARM','M1-ARM'],
					'Mode': ['scalar','blas', 'scalar','blas'],
					'Time': [279, 86, 530, 19]})


sns.set()
sns.catplot(x="CPU", y="Time", hue="Mode", kind="bar", data=df)
plt.savefig('simd_mmul.png', dpi=300)
