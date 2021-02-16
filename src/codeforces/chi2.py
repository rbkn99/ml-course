K1, K2 = list(map(int, input().split()))
N = int(input())
X = {}
X1_sum, X2_sum = [0 for _ in range(K1 + 1)], [0 for _ in range(K2 + 1)]
for _ in range(N):
    x1, x2 = list(map(int, input().split()))
    if x1 not in X:
        X[x1] = {}
    if x2 not in X[x1]:
        X[x1][x2] = 0
    X[x1][x2] += 1
    X1_sum[x1] += 1
    X2_sum[x2] += 1
chi2 = N
for x1 in X:
    for x2 in X[x1]:
        expected = (X1_sum[x1] * X2_sum[x2]) / N
        chi2 += ((X[x1][x2] - expected) ** 2) / expected - expected
print(chi2)
