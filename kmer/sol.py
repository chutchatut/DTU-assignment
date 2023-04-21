k = 4

answer = [0] * (4**k)

alpha = ['A', 'T', 'C', 'G']
alpha_to_pos = {a: i for i, a in enumerate(sorted(alpha))}

data = input().strip()

index = 0
for i in range(k-1):
    index = (k * index) + alpha_to_pos[data[i]]

for i in range(k-1, len(data)):
    index = (k * index) + alpha_to_pos[data[i]]
    index %= len(answer)
    answer[index] += 1

print(' '.join(map(str, answer)))