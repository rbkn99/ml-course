import math


class Vertex:
    pass


class Node(Vertex):
    def __init__(self, split_index, split_const, left_index, right_index):
        self.split_index = split_index
        self.split_const = split_const
        self.left_index = left_index
        self.right_index = right_index


class Leaf(Vertex):
    def __init__(self, leaf_class):
        self.leaf_class = leaf_class


def test_split(index, value, dataset):
    left, right = list(), list()
    for row in dataset:
        if row[index] < value:
            left.append(row)
        else:
            right.append(row)
    return left, right


def calc_entropy(left_count, right_count):
    total_size = sum(left_count) + sum(right_count)
    entropy = 0.0
    for group in [left_count, right_count]:
        size = sum(group)
        if size == 0:
            continue
        score = 0.0
        for class_count in group:
            p = class_count / size
            if p > 0:
                score += p * math.log2(p)
        entropy += -score * (size / total_size)
    return entropy


def get_split(dataset, use_entropy):
    y = [row[-1] for row in dataset]
    total_size = len(y)
    b_index, b_value, b_score, b_groups = None, None, None, None
    for index in range(len(dataset[0]) - 1):
        sorted_indices = sorted(zip([row[index] for row in dataset], range(len(dataset))))
        left_classes_count = [0 for _ in range(k + 1)]
        right_classes_count = [0 for _ in range(k + 1)]
        for cls in y:
            right_classes_count[cls] += 1
        left_size, right_size = 0, total_size
        left_score, right_score = 0., sum([p * p for p in right_classes_count])
        si = 0
        while si < total_size:
            start_i, end_i = si, si
            while end_i < total_size and sorted_indices[start_i][0] == sorted_indices[end_i][0]:
                end_i += 1
            for q in range(start_i, end_i):
                value, j = sorted_indices[q]
                cls = y[j]
                left_score += 2 * left_classes_count[cls] + 1
                right_score += -2 * right_classes_count[cls] + 1
                left_classes_count[cls] += 1
                right_classes_count[cls] -= 1
                left_size += 1
                right_size -= 1

            if use_entropy:
                measure = calc_entropy(left_classes_count, right_classes_count)
            else:
                measure = (left_size - (left_score / left_size)) / total_size
                if right_size > 0:
                    measure += (right_size - (right_score / right_size)) / total_size
            if not b_score or measure < b_score:
                if end_i == total_size:
                    aligned_value = sorted_indices[start_i][0] + 0.5
                else:
                    aligned_value = (sorted_indices[start_i][0] + sorted_indices[end_i][0]) / 2
                b_index, b_value, b_score = index, aligned_value, measure
            si = end_i
    b_groups = test_split(b_index, b_value, dataset)
    return {'index': b_index, 'value': b_value, 'groups': b_groups}


def ID3(dataset, height, use_entropy=False):
    y = [row[-1] for row in dataset]
    major_label = max(map(lambda val: (y.count(val), val), set(y)))
    if height == h or major_label[0] == len(y):
        tree_nodes.append(Leaf(major_label[1]))
        return len(tree_nodes) - 1
    best_split = get_split(dataset, use_entropy)
    if len(best_split['groups'][0]) == 0 or \
            len(best_split['groups'][1]) == 0:
        tree_nodes.append(Leaf(major_label[1]))
        return len(tree_nodes) - 1
    new_node = Node(best_split['index'], best_split['value'], 0, 0)
    tree_nodes.append(new_node)
    new_node_index = len(tree_nodes) - 1
    left_index = ID3(best_split['groups'][0], height + 1)
    right_index = ID3(best_split['groups'][1], height + 1)
    tree_nodes[new_node_index].left_index = left_index
    tree_nodes[new_node_index].right_index = right_index
    return new_node_index


m, k, h = list(map(int, input().split()))
n = int(input())
data = [[0 for j in range(m + 1)] for i in range(n)]
for i in range(n):
    data[i] = list(map(int, input().split()))
tree_nodes = []
ID3(data, 0, n < 1000)
print(len(tree_nodes))
for node in tree_nodes:
    if isinstance(node, Leaf):
        print('C', int(node.leaf_class))
    else:
        print('Q', node.split_index + 1, node.split_const, node.left_index + 1, node.right_index + 1)
