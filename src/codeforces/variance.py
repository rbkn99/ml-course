K = int(input())
N = int(input())
X = [[] for _ in range(K)]
for _ in range(N):
    _x, _y = list(map(int, input().split()))
    X[_x - 1].append(_y)
var = 0.
for i in range(K):
    m = len(X[i])
    if m == 0:
        continue
    e, e2 = 0., 0.
    for y in X[i]:
        e += y
        e2 += y * y
    e /= m
    e2 /= m
    var += (e2 - (e * e)) * m / N
print(var)
