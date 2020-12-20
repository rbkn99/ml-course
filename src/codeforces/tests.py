import os
import sys
import random

for it in range(1000):
    with open('input.txt', 'w') as f:
        n = random.randint(1, 10)
        m = random.randint(1, min(n, 4))
        f.write(str(n) + ' ' + str(m) + '\n')
        for i in range(n):
            for j in range(m + 1):
                f.write(str(random.randint(0, 100)) + ' ')
            f.write('\n')

    os.system("./cmake-build-debug/cf < input.txt > output.txt")

    with open('output.txt', 'r') as f:
        eval_result = float(f.read())
        if eval_result < 0:
            print('wrong')
            sys.exit(1)
        elif eval_result < 20:
            print('partial')
            sys.exit(1)
        else:
            print('ok')
