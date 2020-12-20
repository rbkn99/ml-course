# import sys
import math

# sys.stdin = open("input.txt", "r")
# sys.stdout = open("output.txt", "w")

n_classes = int(input())
penalties = list(map(lambda p: float(p), input().split()))
alpha = float(input())
n_train = int(input())
vocab = {}
docs_number = [0] * n_classes
for _ in range(n_train):
    split_line = input().split()
    C = int(split_line[0]) - 1
    doc = set(split_line[2:])
    for word in doc:
        if word not in vocab:
            vocab[word] = [0] * n_classes
        vocab[word][C] += 1
    docs_number[C] += 1

for word, classes in vocab.items():
    prob = [0.] * n_classes
    for c in range(n_classes):
        prob[c] = (vocab[word][c] + alpha) / (docs_number[c] + 2 * alpha)
    vocab[word] = prob

n_test = int(input())
test_messages = []
for _ in range(n_test):
    split_line = input().split()
    doc = set(split_line[1:])
    scores = [0.] * n_classes
    for c in range(n_classes):
        if docs_number[c] == 0:
            scores[c] = 0.
            continue
        pr = math.log(docs_number[c] / n_train)
        for word in vocab:
            if word in doc:
                pr += math.log(vocab[word][c])
            else:
                pr += math.log(1 - vocab[word][c])
        scores[c] = pr
    _max = max(scores)
    for i in range(n_classes):
        if scores[i] != 0.:
            scores[i] = math.exp(scores[i] - _max) * penalties[i]
    pr_sum = sum(scores)
    for score in scores:
        print(score / pr_sum, end=' ')
    print()
