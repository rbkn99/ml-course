import math


class Node:
    def __init__(self, _type, indices, _n=0, _m=0):
        self.indices = indices
        self.type = _type
        self.n = _n
        self.m = _m
        self.data = [[0. for _ in range(_m)] for __ in range(_n)]
        self.diff = [[0. for _ in range(_m)] for __ in range(_n)]


def tanh(indices):
    nd = nodes[indices[0]]
    new_node = Node('tnh', indices, nd.n, nd.m)
    for i in range(nd.n):
        for j in range(nd.m):
            new_node.data[i][j] = math.tanh(nd.data[i][j])
    nodes.append(new_node)


def d_tanh(nd: Node):
    for i in range(nd.n):
        for j in range(nd.m):
            nodes[nd.indices[0]].diff[i][j] += nd.diff[i][j] * (1 - (nd.data[i][j] ** 2))


def rlu(indices, alpha):
    nd = nodes[indices[0]]
    new_node = Node('rlu', indices, nd.n, nd.m)
    for i in range(nd.n):
        for j in range(nd.m):
            new_node.data[i][j] = nd.data[i][j] if nd.data[i][j] >= 0 else nd.data[i][j] / alpha
    nodes.append(new_node)


def d_rlu(nd: Node, alpha):
    for i in range(nd.n):
        for j in range(nd.m):
            nodes[nd.indices[0]].diff[i][j] += nd.diff[i][j] if nodes[nd.indices[0]].data[i][j] >= 0 else \
                nd.diff[i][j] / alpha


def mul(indices):
    x = nodes[indices[0]]
    y = nodes[indices[1]]
    new_node = Node('mul', indices, x.n, y.m)
    for i in range(new_node.n):
        for j in range(new_node.m):
            for z in range(x.m):
                new_node.data[i][j] += x.data[i][z] * y.data[z][j]
    nodes.append(new_node)


def d_mul(nd: Node):
    for i in range(nodes[nd.indices[0]].n):
        for j in range(nodes[nd.indices[0]].m):
            for z in range(nd.m):
                nodes[nd.indices[0]].diff[i][j] += nd.diff[i][z] * nodes[nd.indices[1]].data[j][z]
    for i in range(nodes[nd.indices[1]].n):
        for j in range(nodes[nd.indices[1]].m):
            for z in range(nd.n):
                nodes[nd.indices[1]].diff[i][j] += nd.diff[z][j] * nodes[nd.indices[0]].data[z][i]


def sum_(indices):
    nd = nodes[indices[0]]
    new_node = Node('sum', indices, nd.n, nd.m)
    for ind in indices:
        for i in range(new_node.n):
            for j in range(new_node.m):
                new_node.data[i][j] += nodes[ind].data[i][j]
    nodes.append(new_node)


def d_sum(nd: Node):
    for ind in nd.indices:
        for i in range(nd.n):
            for j in range(nd.m):
                nodes[ind].diff[i][j] += nd.diff[i][j]


def had(indices):
    nd = nodes[indices[0]]
    new_node = Node('had', indices, nd.n, nd.m)
    for i in range(nd.n):
        for j in range(nd.m):
            new_node.data[i][j] = 1
    for ind in indices:
        for i in range(nd.n):
            for j in range(nd.m):
                new_node.data[i][j] *= nodes[ind].data[i][j]
    nodes.append(new_node)


def d_had(nd: Node):
    for i in range(nd.n):
        for j in range(nd.m):
            for z in range(len(nd.indices)):
                cur_val = 1
                for p in range(len(nd.indices)):
                    if p == z:
                        continue
                    cur_val *= nodes[nd.indices[p]].data[i][j]
                nodes[nd.indices[z]].diff[i][j] += cur_val * nd.diff[i][j]


n, m, k = list(map(int, input().split()))
nodes = []
cmds = []
alphas = [0 for i in range(n)]
for _i in range(n):
    cmd, *params = input().split()
    params = [int(x) for x in params]
    cmds.append([cmd, []])
    if cmd == 'var':
        nodes.append(Node(cmd, [], params[0], params[1]))
        cmds[-1][1] = (params[0], params[1])
    elif cmd == 'sum' or cmd == 'had':
        cmds[-1][1] = [x - 1 for x in params[1:]]
    elif cmd == 'mul':
        cmds[-1][1] = [x - 1 for x in params]
    elif cmd == 'rlu':
        alphas[_i] = params[0]
        cmds[-1][1] = [params[1] - 1]
    elif cmd == 'tnh':
        cmds[-1][1] = [params[0] - 1]
for q in range(m):
    for _i in range(nodes[q].n):
        nodes[q].data[_i] = list(map(int, input().split()))
for _i in range(m, n):
    if cmds[_i][0] == 'tnh':
        tanh(cmds[_i][1])
    elif cmds[_i][0] == 'rlu':
        rlu(cmds[_i][1], alphas[_i])
    elif cmds[_i][0] == 'mul':
        mul(cmds[_i][1])
    elif cmds[_i][0] == 'sum':
        sum_(cmds[_i][1])
    elif cmds[_i][0] == 'had':
        had(cmds[_i][1])
for _i in range(n - k, n):
    for ii in range(nodes[_i].n):
        for jj in range(nodes[_i].m):
            print(nodes[_i].data[ii][jj], end=' ')
        print()
for _i in range(n - k, n):
    for ii in range(nodes[_i].n):
        nodes[_i].diff[ii] = list(map(int, input().split()))
for _i in range(n - 1, -1, -1):
    if nodes[_i].type == 'tnh':
        d_tanh(nodes[_i])
    if nodes[_i].type == 'rlu':
        d_rlu(nodes[_i], alphas[_i])
    if nodes[_i].type == 'mul':
        d_mul(nodes[_i])
    if nodes[_i].type == 'sum':
        d_sum(nodes[_i])
    if nodes[_i].type == 'had':
        d_had(nodes[_i])
for _i in range(m):
    for ii in range(nodes[_i].n):
        for jj in range(nodes[_i].m):
            print(nodes[_i].diff[ii][jj], end=' ')
        print()
