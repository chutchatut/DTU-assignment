from collections import Counter
from math import comb

data = input().strip()
c = Counter(data)
au_count = min(c['A'], c['U'])
cg_count = min(c['C'], c['G'])

print(
    comb(c['A'], au_count)
    * comb(c['U'], au_count)
    * comb(c['C'], cg_count)
    * comb(c['G'], cg_count)
)
