import sys
from collections import Counter

text_file = open(sys.argv[1], encoding="utf8")

sssss = []

for line in text_file:
	if not line.startswith('0x'):
		continue
	if "0x0000" in line:
		continue
	sssss.append(line.split(' ')[1].lower())

text_file.close()

cccc = Counter(sssss).most_common()

for item in cccc:
	if(item[1] != 1):
		print(item)
print("done")