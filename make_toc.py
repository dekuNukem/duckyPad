import sys

if len(sys.argv) <= 1:
	print(__file__, 'text_file')
	exit()

depth = 3
while 1:
	try:
		depth = int(input("how many levels? (1 to 4, default 3): "))
	except Exception as e:
		depth = 3
		break
	if 1 <= depth <= 4:
		break
		
def make_section(text):
	level = text.split(' ')[0].count('#')
	if level > depth:
		return
	text = text.lstrip("#").replace('\r', '').replace('\n', '').strip()
	link = text.lower().replace('.', '')
	result = ''
	for letter in link:
		if letter.isalnum():
			result += letter
		else:
			result += '-'
	result = result.strip('-')
	print('    '*(level-2) + f'- [{text}](#{result})\n')


text_file = open(sys.argv[1],   encoding="utf8")

print("## Table of Contents\n")

for line in text_file:
	if 'table of content' in line.lower():
		continue
	if line.startswith("# "):
		continue
	if line.startswith("#"):
		make_section(line)

text_file.close()