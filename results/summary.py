#!/usr/bin/env python3

import pathlib
import os

dirname = pathlib.Path(__file__).parent
files = [os.path.join(dirname, f) for f in os.listdir(dirname) if f.endswith('.txt')]
files_with_stats = []

for f in files:
    s = pathlib.Path(f).read_text()
    stats = [float(x.split()[-2]) for x in s.splitlines() if x.strip() != '']
    avg = sum(stats) / len(stats)
    files_with_stats.append({ 'file': f, 'average_time': avg })

files_with_stats.sort(key=lambda f : f['average_time'])
baseline_time = files_with_stats[-1]['average_time']

for f in files_with_stats:
    f, avg = f['file'], f['average_time']
    flops = 1024**3 * 2
    mflops = flops / 1e6 / avg
    # Forgot to print mflops in the original program and now mflops are approximate.
    line = f'File: {f}, average time: {avg:.4f}s'
    if baseline_time != avg:
        line += f' ({(baseline_time / avg):.2f}x)'
    line += f' , mflops: ~{mflops:.2f}'
    print(line)