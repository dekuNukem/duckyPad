name_list = [("KEY_UP_ARROW", 0xDA), ("KEY_DOWN_ARROW", 0xD9), ("KEY_LEFT_ARROW", 0xD8), ("KEY_RIGHT_ARROW", 0xD7), ("KEY_BACKSPACE", 0xB2), ("KEY_TAB", 0xB3), ("KEY_RETURN", 0xB0), ("KEY_ESC", 0xB1), ("KEY_INSERT", 0xD1), ("KEY_DELETE", 0xD4), ("KEY_PAGE_UP", 0xD3), ("KEY_PAGE_DOWN", 0xD6), ("KEY_HOME", 0xD2), ("KEY_END", 0xD5), ("KEY_CAPS_LOCK", 0xC1), ("KEY_F1", 0xC2), ("KEY_F2", 0xC3), ("KEY_F3", 0xC4), ("KEY_F4", 0xC5), ("KEY_F5", 0xC6), ("KEY_F6", 0xC7), ("KEY_F7", 0xC8), ("KEY_F8", 0xC9), ("KEY_F9", 0xCA), ("KEY_F10", 0xCB), ("KEY_F11", 0xCC), ("KEY_F12", 0xCD)]
name_list = sorted(name_list, key=lambda x: x[1])

for item in name_list:
	print("#define " + item[0] + " (" + str(hex(item[1]-0x88)).upper() + "+0x88)")