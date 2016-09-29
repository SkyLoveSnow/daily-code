import argparse
import sys
# 通过继承argparse.Action可以定制需要的action
# 每次指定--name参数后都会触发对应的action，能够多次指定
# 
if __name__ == '__main__':
	test = []
	class NameAction(argparse.Action):
		def __call__(self,parser,namespaces,values,option_string=None):
			test.append(values)
	parser = argparse.ArgumentParser()
	parser.add_argument('--name',
		metavar = 'NAME',
		action = NameAction,
		help="test name")
	print parser.parse_args(sys.argv[1:])

	print ' '.join(test)