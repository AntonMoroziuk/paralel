from random import choice

with open('test', 'w') as f:
	for i in range(1048576):
		f.write(''.join(choice('0123456789abcdef') for i in range(64)))
		f.write('\n')
