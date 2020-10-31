import os
import sys
import string
import shutil
import random

profile_number = 50

root_name = 'duck_test'

if os.path.isdir(root_name):
	shutil.rmtree(root_name)
os.mkdir(root_name)

color_pool = [8, 16, 32, 64]
word_list = ['indication', 'ambition', 'height', 'pie', 'protection', 'perception', 'selection', 'platform', 'garbage', 'interaction', 'intention', 'organization', 'gate', 'alcohol', 'nature', 'client', 'sample', 'poem', 'player', 'boyfriend', 'injury', 'inflation', 'championship', 'nation', 'wife', 'atmosphere', 'ratio', 'tea', 'mood', 'freedom', 'cousin', 'union', 'climate', 'growth', 'revolution', 'map', 'introduction', 'story', 'percentage', 'obligation', 'debt', 'drawing', 'patience', 'message', 'history', 'driver', 'football', 'cigarette', 'charity', 'cell', 'emphasis', 'idea', 'refrigerator', 'office', 'interaction', 'video', 'apple', 'knowledge', 'complaint', 'teaching', 'recognition', 'philosophy', 'food', 'girlfriend', 'editor', 'history', 'two', 'explanation', 'estate', 'agency', 'media', 'world', 'beer', 'resolution', 'funeral', 'software', 'paper', 'variation', 'river', 'affair', 'thought', 'dad', 'operation', 'climate', 'injury', 'lake', 'sympathy', 'equipment', 'elevator', 'county', 'committee', 'salad', 'drawing', 'nation', 'ambition', 'night', 'meaning', 'foundation', 'law', 'quantity', 'revolution', 'security', 'income', 'director', 'pollution', 'ambition', 'poetry', 'paper', 'strategy', 'setting', 'power', 'awareness', 'attitude', 'departure', 'philosophy', 'painting', 'church', 'sympathy', 'baseball', 'restaurant', 'difference', 'pizza', 'bonus', 'proposal', 'chemistry', 'fact', 'army', 'winner', 'hat', 'aspect', 'alcohol', 'idea', 'recognition', 'writing', 'poet', 'difficulty', 'year', 'television', 'moment', 'stranger', 'ladder', 'candidate', 'effort', 'cookie', 'heart', 'tradition', 'equipment', 'leader', 'drama', 'disaster', 'insurance', 'aspect', 'platform', 'child', 'ratio', 'wealth', 'way', 'family', 'preference', 'replacement', 'instruction', 'exam', 'finding', 'patience', 'country', 'alcohol', 'discussion', 'drawer', 'coffee', 'context', 'excitement', 'people', 'tooth', 'apartment', 'writing', 'chest', 'bonus', 'refrigerator', 'baseball', 'equipment', 'appearance', 'atmosphere', 'literature', 'grandmother', 'two', 'assumption', 'painting', 'performance', 'ambition', 'cancer', 'policy', 'cheek', 'tennis', 'news', 'resolution', 'penalty', 'library', 'drawing', 'analyst', 'employer']
word_list = list(set(word_list))

def get_random_name():
	return random.choice(word_list)

def get_random_colors():
	ret = ''
	for item in random.choices(color_pool, k=3):
		ret += str(item) + ' '
	return ret

root_dir = os.path.join(".", root_name)
for x in range(1, profile_number + 1):
	profile_dir_name = os.path.join(root_dir, 'profile' + str(x) + "_" + get_random_name()[:13])
	os.mkdir(profile_dir_name)
	config_name = os.path.join(profile_dir_name, 'config.txt')
	config_content = ''
	for y in range(1, 16):
		config_content += 'z' + str(y) + ' ' + get_random_name()[:7] + "\n"
	for y in range(1, 16):
		config_content += 'SWCOLOR_' + str(y) + ' ' + get_random_colors() + "\n"
	print(config_content)

	with open(config_name, 'w') as config_file:
		config_file.write(config_content)

	for y in range(1, 16):
		script_name = os.path.join(profile_dir_name, 'key' + str(y) + '.txt')
		script_content = 'STRING ' + get_random_name() + '\nENTER\n'
		with open(script_name, 'w') as script_file:
			script_file.write(script_content)
