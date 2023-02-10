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

def postorder_walk(node, action, instruction_list):
	if node is None:
		print("postorder_walk error: Get child node failed")
		exit()
	if isinstance(node, ast.BoolOp):
		for item in node.values:
			postorder_walk(item, action, instruction_list)
	if is_leaf(node):
		# print("[LEAF] ", end='')
		action(node, instruction_list)
		return;
	postorder_walk(node.left, action, instruction_list)
	postorder_walk(get_right(node), action, instruction_list)
	action(node, instruction_list)
