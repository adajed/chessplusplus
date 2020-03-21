#!/usr/bin/python3.6

import sys
import numpy as np

N = 4 + 2 * (5 * 65 + 64 + 6 + 7)

def read(path):
    a = []
    with open(path, 'rb') as f:
        for _ in range(N):
            i = int.from_bytes(f.read(2), byteorder='little', signed=True)
            a.append(i)
    return a

def write(path, a):
    with open(path, 'wb') as f:
        for i in a:
            f.write((i).to_bytes(2, byteorder='little', signed=True))

def mutate(weights):
    for i in range(4, N):
        if np.random.randint(100) == 0:
            weights[i] += np.random.randint(-10, 11)
    return weights

if __name__ == '__main__':
    path_in = sys.argv[1]
    path_out = sys.argv[2]

    weights = read(path_in)
    weights = mutate(weights)
    write(path_out, weights)
