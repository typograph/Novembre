import sys
from date_hg import allfiles

files = allfiles()

cfname = sys.argv[1]
with open(cfname,'r') as f:
	copyright = f.readlines()
for filename in sys.argv[2:]:
	text = None
	try:
		with open(filename,'r') as f:
			text = f.readlines()
		if text is not None:
			with open(filename,'w') as f:
				if filename in files:
					dates = files[filename]
					if dates[0] == dates[1]:
						years = str(dates[0])
					else:
						years = "{} - {}".format(*dates)
					for line in copyright:
						f.write(line.format(years))					
				else:
					for line in copyright:
						f.write(line)
				for line in text:
					f.write(line)
	except Exception:
		pass
