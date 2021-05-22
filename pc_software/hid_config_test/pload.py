import pickle

class my_file_obj(object):
	def __init__(self, name, ftype, content):
		super(my_file_obj, self).__init__()
		self.name = name
		self.content = content
		self.type = ftype
	def __str__(self):
		ret = ""
		ret += str('...............') + '\n'
		ret += "name:\t" + str(self.name) + '\n'
		ret += "type:\t" + str(self.type) + '\n'
		ret += "content:\t" + str(self.content) + '\n'
		ret += str('...............') + '\n'
		return ret

file_struct_list = pickle.load(open("save.p", "rb"))

for item in file_struct_list:
	if item.type == 0:
		print(item)
		continue
	if item.content is None:
		continue
	for fff in item.content:
		print(fff)