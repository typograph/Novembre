TEMPLATE = subdirs

# Novembre default visualisers
SUBDIRS += nvbdviz.pro

# Novembre color adjustment
SUBDIRS += nvbcolor.pro

# Novembre spectroscopy tools
SUBDIRS += nvbspec.pro

# Novembre topography tools
SUBDIRS += nvbtopo.pro

# Exports # to QtiPlot
SUBDIRS += nvbexport.pro

# Imports anything as NVBFile
# Currently has only MultiSpecImportView,
# duplicated by Nanonis NNS format
# SUBDIRS += nvbimport.pro
