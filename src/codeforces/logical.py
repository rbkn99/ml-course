m = int(input())
max_mask = (1 << m)

fs = list(filter(lambda x: x[1] != 0, [(i, int(input())) for i in range(max_mask)]))
if len(fs) == 0:
    print(1)
    print(1)
    for _ in range(m + 1):
        print(-1.0, end=' ')
    exit(0)
max_mask = min(512, len(fs))
print(2)
print(max_mask, 1)
for i in range(max_mask):
    z = fs[i][0]
    for pos in range(m):
        if z & (1 << pos):
            print(1.0, end=' ')
        else:
            print(-10000000.0, end=' ')
    ones = str(bin(z)).count('1')
    print(0.5 - ones)
for i in range(max_mask):
    print(1.0, end=' ')
print(-0.5)
