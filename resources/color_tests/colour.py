import os
import random
import colorsys
# for x in range(1,13):
# 	os.system("echo F" + str(x) + ' > key' + str(x) + '_F' + str(x) + '.txt')

# for x in range(1,13):
# 	os.system('touch key' + str(x) + '_test' + str(x) + '.txt')

# for x in range(128):
# 	print(chr(x), end='')
# ----
# special_keys = ["ESCAPE", "ESC", "ENTER", "UP", "DOWN", "LEFT", "RIGHT", "UPARROW", "DOWNARROW", "LEFTARROW", "RIGHTARROW", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "BACKSPACE", "TAB", "CAPSLOCK", "PRINTSCREEN", "SCROLLLOCK", "PAUSE", "BREAK", "INSERT", "HOME", "PAGEUP", "PAGEDOWN", "DELETE", "END", "SPACE", "SHIFT", "ALT", "GUI", "WINDOWS", "CONTROL", "CTRL"]

# print(special_keys)

# for x in range(1,13):
# 	filename = 'key' + str(x) + '_test' + str(x) + '.txt'
# 	os.system('touch ' + filename)
# 	content = ""
# 	for item in random.sample(special_keys, 4):
# 		content += item + ' '
# 	os.system('echo ' + content.rstrip() + ' > ' + filename)
# -----
# for x in range(1,13):
# 	filename = 'key' + str(x) + '_' + str(x) + '.txt'
# 	os.system('touch ' + filename)
# 	content = "GUI F" + str(x)
# 	os.system('echo ' + content.rstrip() + ' > ' + filename)

# for x in range(13,16):
# 	filename = 'key' + str(x) + '_' + str(x) + '.txt'
# 	os.system('touch ' + filename)
# 	content = "CONTROL F" + str(x-3)
# 	os.system('echo ' + content.rstrip() + ' > ' + filename)
# -----

# colour_values = [16, 127, 255]

# def make_color():
# 	return (random.choice(colour_values), random.choice(colour_values), random.choice(colour_values))

# already_done = set()
# this_color = set((0, 0, 0))

# for x in range(1,16):
# 	while 1:
# 		this_color = make_color()
# 		if this_color not in already_done:
# 			break
# 	print("SWCOLOR_" + str(x) + ' ' + str(this_color[0]) + ' ' + str(this_color[1]) + ' ' + str(this_color[2]))
# 	already_done.add(this_color)

# -----

colour_values = [16, 127, 255]

def make_color():
	rrr = random.choice(colour_values)
	ggg = random.choice(colour_values)
	bbb = random.choice(colour_values)
	hsv = colorsys.rgb_to_hsv(rrr/256, ggg/256, bbb/256)
	return (rrr, ggg, bbb, hsv[0], hsv[1], hsv[2])

already_done = set()
this_color = set((0, 0, 0))
for x in range(1,17):
	while 1:
		this_color = make_color()
		if this_color[0] == this_color[1] == this_color[2]:
			continue
		if this_color not in already_done:
			break
	already_done.add(this_color)

sss = sorted(already_done, key=lambda x: (x[3], x[4]))
for x in range(1,16):
	print("SWCOLOR_" + str(x) + ' ' + str(sss[x][0]) + ' ' + str(sss[x][1]) + ' ' + str(sss[x][2]))
