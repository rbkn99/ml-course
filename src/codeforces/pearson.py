def pearson(x, y):
    n = len(x)
    sum_x = float(sum(x))
    sum_y = float(sum(y))
    sum_x_sqr = sum([xi * xi for xi in x])
    sum_y_sqr = sum([yi * yi for yi in y])
    psum = sum([xi * yi for xi, yi in zip(x, y)])
    cov = psum - (sum_x * sum_y / n)
    sigmas = pow((sum_x_sqr - pow(sum_x, 2) / n) * (sum_y_sqr - pow(sum_y, 2) / n), 0.5)
    if sigmas == 0:
        return 0
    return cov / sigmas


N = int(input())
X, Y = [], []
for _ in range(N):
    _x, _y = list(map(int, input().split()))
    X.append(_x)
    Y.append(_y)
print(pearson(X, Y))
