from mercurial import ui, hg
from datetime import date

def allfiles():
	repo = hg.repository(ui.ui(),'.')

	files = {}
	for file in repo[0]:
		files[file] = (2006,2006)

	for changeset in repo:
		ctx = repo[changeset]
		year = date.fromtimestamp(ctx.date()[0]).year
		for file in ctx:
			if file not in files:
				files[file] = (year,year)
			else:
				y_start, y_end = files[file]
				y_start = min(year,y_start)
				y_end = max(year,y_end)
				files[file] = y_start, y_end

	return files

if __name__ == "__main__":
	files = allfiles()
	for file in files:
		print "{}-{} {}".format(files[file][0],files[file][1],file)
