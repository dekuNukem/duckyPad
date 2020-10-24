import time
import pyautogui

time.sleep(2)
# pyautogui.hotkey('ctrl', 'shift', 'left')
# pyautogui.hotkey('shift', 'left')
# pyautogui.hotkey('shift', 'left')
# pyautogui.hotkey('shift', 'left')
# pyautogui.hotkey('shiftleft', 'shiftright', 'left')


for x in range(5):
	pyautogui.hotkey('shiftleft', 'shiftright', 'left')