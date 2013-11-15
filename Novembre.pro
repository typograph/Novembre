TEMPLATE = subdirs

nvblib.file = nvblib.pro

fileplg.file = fileplg.pro
fileplg.depends = nvblib

tools.file = tools.pro
tools.depends = nvblib

nvbmain.file = nvbmain.pro
nvbmain.depends = nvblib tools

SUBDIRS = \
          nvblib \
          fileplg \
          tools \
          nvbmain
