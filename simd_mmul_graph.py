#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#plot mean time to calculate matrix multiplication
# main_mmul.cpp

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
#MacBook Air M1
#Ubuntu 3900X
df = pd.DataFrame({'CPU': ['3900X','3900X','3900X','3900X','3900X','3900X','3900X','3900X','3900X','3900X','M1-ARM','M1-ARM','M1-ARM','M1-ARM','M1-ARM','M1-ARM'],
					'Mode': ['scalar32','blas32','accel32','mkl32','cuda32','scalar64','blas64','accel64','mkl64','cuda64','scalar32','blas32','accel32','scalar64','blas64','accel64'],
					'Time': [152, 61, 0, 28, 22, 279, 86, 0, 55, 60, 530, 50, 11, 530, 83, 16]})

sns.set()
sns.catplot(x="CPU", y="Time", hue="Mode", kind="bar", data=df)
plt.savefig('simd_mmul.png', dpi=300)
