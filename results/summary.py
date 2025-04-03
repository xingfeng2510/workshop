#!/usr/bin/env python3

import pathlib
import os

dirname = pathlib.Path(__file__).parent
files = [os.path.join(dirname, f) for f in os.listdir(dirname) if not f.endswith('.py')]
files_with_stats = []

for f in files:
    s = pathlib.Path(f).read_text()
    stats = [float(x.split()[-2]) for x in s.splitlines() if x.strip() != '']
    avg = sum(stats) / len(stats)
    files_with_stats.append([f, avg])

files_with_stats.sort(key=lambda f : f[1])

for f, avg in files_with_stats:
    print(f'File: {f}, average time: {avg:.4f}s')