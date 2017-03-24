#!/usr/bin/env python3

import sys
import statistics
import collections

measurements = collections.OrderedDict()

with open(sys.argv[1], 'r') as f:
    for line in f:
        if line.startswith("TIMER"):
            fields = line.split(',')
            name = fields[1]
            percentiles = fields[0:]
            if name not in measurements:
                measurements[name] = [percentiles]
            else:
                measurements[name].append(percentiles)

for stat in measurements:
    print("%s: count: %d" % (stat, len(measurements[stat])))

    def summarize(percentile, field):
        col = [ float(x[field].strip()) for x in measurements[stat] ]
        print('\t%.1f percentile:\tmean: %f\tstdev: %f' \
            % (percentile, statistics.mean(col), statistics.pstdev(col)));

    summarize(25, 2)
    summarize(50, 3)
    summarize(75, 4)
    summarize(95, 5)
    summarize(99, 6)
    summarize(99.9, 7)
    print("\n");
