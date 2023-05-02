import sys
import ast

def is_walkable(node):
	return isinstance(node, ast.BinOp) or isinstance(node, ast.BoolOp) or isinstance(node, ast.Compare)

def get_right(node):
	try:
		return node.right
	except:
		pass
	try:
		if len(node.comparators) > 1:
			return None
		return node.comparators[0]
	except:
		pass
	return None

def is_leaf(node):
	return 'left' not in node.__dict__ and 'right' not in node.__dict__

def postorder_walk(node, action, instruction_list, expr):
	# this generates an error 
	# IF $kc > 12 != 14 THEN
	# c:\users\allen\appdata\roaming\python\python38\scripts\astboom.exe ast "(kc > 12) != 14"
	if node is None:
		raise ValueError(f"Expression parsing fail:\n{expr}\nTry adding parentheses!")
	if isinstance(node, ast.BoolOp):
		for item in node.values:
			postorder_walk(item, action, instruction_list, expr)
	if is_leaf(node):
		action(node, instruction_list)
		return;
	postorder_walk(node.left, action, instruction_list, expr)
	postorder_walk(get_right(node), action, instruction_list, expr)
	action(node, instruction_list)
