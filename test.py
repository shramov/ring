#!/usr/bin/env python

import pyring as ring

r = ring.Ring(1024)

r.write(b'xxx') #, 3)
r.write(b'yyy') #, 3)
r.write(b'zzz') #, 3)

i = ring.RingIter(r)
for _ in range(3):
	print(i.read().tobytes())
	print(i.shift())

for b in r:
	print(b.tobytes())

i = ring.RingIter(r)
assert(i.read().tobytes() == b'xxx')

r = ring.Ring(1024)
br = ring.BufRing(r)

br.write(b'xxx', 1)
br.write(b'yyy', 2)
br.flush()

print("First")
for f in br.read():
	print(f.data.tobytes(), f.flags)
br.shift()

print("Second")
for f in br.read():
	print(f.data.tobytes(), f.flags)
