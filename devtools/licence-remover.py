import sys
for filename in sys.argv[1:]:
	text = None
	try:
		with open(filename,'r') as f:
			text = f.readlines()
		if text is not None:
			c_l = len(text)
			while text[0].startswith('//'):
				text.pop(0)
			if text[0].startswith('/*'):
				while text[0].find('*/') < 0:
					text.pop(0)
				text.pop(0)
			while len(text[0]) < 5:
				text.pop(0)
		
			if c_l != len(text):
				with open(filename,'w') as f:
					for line in text:
						f.write(line)
	except Exception:
		pass
