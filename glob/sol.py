import numpy as np

GAP_PENALTY = 5

alphas = ['A', 'C', 'D', 'E', 'F', 'G', 'H',
          'I', 'K', 'L', 'M', 'N', 'P', 'Q',
          'R', 'S', 'T', 'V', 'W', 'Y']
alpha_to_idx = {a: i for i, a in enumerate(alphas)}

with open('blosum62.txt', 'r') as f:
    blosum_raw = [int(i) for i in f.read().split()]


def get_dist(c1, c2):
    i1, i2 = alpha_to_idx[c1], alpha_to_idx[c2]
    return blosum_raw[i1*len(alphas) + i2]


data1 = input().strip()
data2 = input().strip()

if len(data1) > len(data2):
    data1, data2 = data2, data1

dp = np.zeros((len(data1), len(data2)), dtype=int)
dp[0][0] = get_dist(data1[0], data2[0])

for j in range(1, len(data2)):
    dp[0][j] = max(dp[0][j-1], get_dist(data1[0], data2[j]))

for i in range(1, len(data1)):
    dp[i][0] = get_dist(data1[i], data2[0])

for i in range(1, len(data1)):
    for j in range(1, len(data2)):
        dp[i][j] = max(dp[i][j-1],
                       dp[i-1][j-1] + get_dist(data1[i], data2[j]))

print(dp[len(data1)-1, len(data2)-1]
      - GAP_PENALTY * (len(data2) - len(data1)))
