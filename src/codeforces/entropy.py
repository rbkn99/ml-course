import math

Kx, Ky = list(map(int, input().split()))
N = int(input())
pX, pXY = {}, {}
for _ in range(N):
    _x, _y = list(map(int, input().split()))
    if _x not in pX:
        pX[_x] = 0.
    if (_x, _y) not in pXY:
        pXY[(_x, _y)] = 0.
    pX[_x] += 1. / N
    pXY[(_x, _y)] += 1. / N
entropy = 0.
for x, y in pXY:
    prob = pXY[(x, y)]
    entropy += -prob * (math.log(prob) - math.log(pX[x]))
print(entropy)
