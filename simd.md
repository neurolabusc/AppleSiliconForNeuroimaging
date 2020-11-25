##  i5-8259U clang 

simd >g++ -O3 -o tst main.cpp -march=native
simd >./tst 5
Reporting minimum time for 5 tests
i16_f32: min/mean	377	545	ms
i16_f32sse: min/mean	383	442	ms
sqrt: min/mean	1366	1744	ms
sqrtSSE: min/mean	1603	1819	ms
sqrtAVX: min/mean	1727	1818	ms
sqrt64: min/mean	5075	5854	ms
sqrt64SSE: min/mean	4765	5227	ms
sqrt64AVX: min/mean	4618	5786	ms
fma: min/mean	1761	2038	ms
fmaSSE: min/mean	1687	1906	ms
fmaAVX: min/mean	1616	1813	ms
fma (memory alignment not forced): min/mean	2675	3246	ms

##  i5-8259U gcc

simd >g++-9 -O3 -o tst main.cpp -march=native
simd >./tst 5
Reporting minimum time for 5 tests
i16_f32: min/mean	382	390	ms
i16_f32sse: min/mean	379	382	ms
sqrt: min/mean	1990	2280	ms
sqrtSSE: min/mean	1617	1719	ms
sqrtAVX: min/mean	1554	1650	ms
sqrt64: min/mean	4885	5573	ms
sqrt64SSE: min/mean	4584	5670	ms
sqrt64AVX: min/mean	4709	5004	ms
fma: min/mean	671	1437	ms
fmaSSE: min/mean	1411	1769	ms
fmaAVX: min/mean	1399	1726	ms
fma (memory alignment not forced): min/mean	3224	3628	ms

## 3900X clang 

simd>clang++ -O3 -o tst main.cpp -march=native
simd>./tst 5
Reporting minimum time for 5 tests
i16_f32: min/mean	234	234	ms
i16_f32sse: min/mean	223	227	ms
sqrt: min/mean	1008	1008	ms
sqrtSSE: min/mean	265	265	ms
sqrtAVX: min/mean	186	186	ms
sqrt64: min/mean	1515	1570	ms
sqrt64SSE: min/mean	762	763	ms
sqrt64AVX: min/mean	341	341	ms
fma: min/mean	179	179	ms
fmaSSE: min/mean	202	202	ms
fmaAVX: min/mean	184	184	ms
fma (memory alignment not forced): min/mean	178	178	ms

## 3900X gcc

g++ -O3 -o tst main.cpp -march=native
make: warning:  Clock skew detected.  Your build may be incomplete.
simd>./tst 5
Reporting minimum time for 5 tests
i16_f32: min/mean	226	226	ms
i16_f32sse: min/mean	227	227	ms
sqrt: min/mean	964	964	ms
sqrtSSE: min/mean	255	255	ms
sqrtAVX: min/mean	180	180	ms
sqrt64: min/mean	1491	1491	ms
sqrt64SSE: min/mean	762	762	ms
sqrt64AVX: min/mean	344	344	ms
fma: min/mean	184	184	ms
fmaSSE: min/mean	206	206	ms
fmaAVX: min/mean	185	185	ms
fma (memory alignment not forced): min/mean	184	185	ms

## M1 clang

> make ARCH=arm
g++ -O3 -o tst main.cpp -march=armv8-a+fp+simd+crypto+crc
simd > ./tst 5      
Reporting minimum time for 5 tests
i16_f32: min/mean	79	91	ms
i16_f32sse: min/mean	79	82	ms
sqrt: min/mean	448	488	ms
sqrtSSE: min/mean	437	478	ms
sqrt64: min/mean	1499	1555	ms
fma: min/mean	441	471	ms
fmaSSE: min/mean	478	509	ms
fma (memory alignment not forced): min/mean	669	709	ms
