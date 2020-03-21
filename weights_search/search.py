import threading
import subprocess
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
    a = weights.copy()
    for i in range(4, N):
        if np.random.randint(100) == 0:
            a[i] += np.random.randint(-10, 11)
    return a


def combine(weights1, weights2):
    p = np.random.randint(4, N-1)
    a = [0] * N
    a[0] = weights1[0]
    a[1] = weights1[1]
    a[2] = weights1[2]
    a[3] = weights1[3]
    for i in range(4, p+1):
        a[i] = weights1[i]
    for i in range(p+1, N):
        a[i] = weights2[i]
    return a

def worker(tid, scores, indecies):
    for idx in indecies:
        print('[{}] processing {}'.format(tid, idx))
        weights_path = 'weights/{}.w'.format(idx)
        result = subprocess.run(['./search.sh', weights_path], stdout=subprocess.PIPE)
        score = int(result.stdout.decode('utf-8'))
        scores[idx] = score

def run_epoch(population_size, num_threads):
    threads = [None] * num_threads
    scores = [0] * population_size

    p_best = population_size // 10
    p_combine = p_best * 8
    p_mutate = population_size - p_best - p_combine

    part = population_size // num_threads
    rest = population_size - part * num_threads
    current = 0

    for tid in range(num_threads):
        indecies = list(range(current, current + part))
        current += part
        if rest > 0:
            rest -= 1
            indecies.append(current)
            current += 1

        threads[tid] = threading.Thread(target=worker, args=(tid, scores, indecies))
        threads[tid].start()

    for tid in range(num_threads):
        threads[tid].join()

    results = [(scores[i], i) for i in range(population_size)]
    results = sorted(results, reverse=True)
    print([a for a, _ in results])
    results = results[:p_best]
    best = [read('weights/{}.w'.format(i)) for _, i in results]

    population = []

    for _ in range(p_combine):
        n1 = 0
        n2 = 0
        while n1 == n2:
            n1 = np.random.randint(p_best)
            n2 = np.random.randint(p_best)
        population.append(combine(best[n1], best[n2]))
    for _ in range(p_mutate):
        n = np.random.randint(p_mutate)
        population.append(mutate(best[n]))
    population.extend(best)

    for i, weights in enumerate(population):
        write('weights/{}.w'.format(i), weights)

def main(argv):
    population_size = int(argv[1])
    num_threads = int(argv[2])
    num_epochs = int(argv[3])

    for epoch in range(num_epochs):
        print('Epoch {}'.format(epoch))
        run_epoch(population_size, num_threads)

if __name__ == '__main__':
    main(sys.argv)
