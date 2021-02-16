K = int(input())
N = int(input())
X, classX = [], {}
for _ in range(N):
    _x, _y = list(map(int, input().split()))
    X.append(_x)
    if _y not in classX:
        classX[_y] = []
    classX[_y].append(_x)
X.sort(reverse=True)
intraclass_distance = 0
interclass_distance = 0
for i in range(N):
    intraclass_distance += -X[i] * i + X[i] * (N - i - 1)
intraclass_distance *= 2
for y in classX:
    classX[y].sort(reverse=True)
    dist_y = 0
    for i in range(len(classX[y])):
        dist_y += -classX[y][i] * i + classX[y][i] * (len(classX[y]) - i - 1)
    interclass_distance += dist_y * 2
intraclass_distance -= interclass_distance
print(interclass_distance)
print(intraclass_distance)
