//
// C++ Implementation: NVBFile
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBFile.h"

//------------- NVBFile -----------------------

NVBFile::~NVBFile() {
  if (refCount)
		NVBOutputError("Non-free file deleted. Possible negative implications for NVBFileFactory");
	emit free(sourceInfo.name());
	}

void NVBFile::addSource(NVBDataSource * page) {
	if (page != 0) {
		// NVBFile is appending pages, instead of prepending them
		NVBPageViewModel::addSource(page,rowCount());
		page->owner = this;
		}
	else
		NVBOutputError("Can't add a NULL page");
	}

void NVBFile::addSources(QList<NVBDataSource *> pages) {
	foreach (NVBDataSource * s, pages) {
		addSource(s);
		}
	}

QRectF NVBFile::fullarea() {
  QRectF rect;
  
  for (int i = pages.size()-1;i>=0;i--) {
    NVBDataSource * fPage = pages.at(i);
    switch (fPage->type()) {
			case NVB::TopoPage : {
				rect |= ((NVB3DPage *)fPage)->position();
				break;
				}
			case NVB::SpecPage : {
				QListIterator<QPointF> ppi(((NVBSpecPage*)fPage)->positions());
				ppi.toFront();
				while(ppi.hasNext()) {
					if (!rect.contains(ppi.next()))
#if QT_VERSION >= 0x040300
						rect |= QRectF(ppi.peekPrevious(),rect.topLeft());
#else
						rect |= QRectF(ppi.peekPrevious().x(),ppi.peekPrevious().y(),rect.x()-ppi.peekPrevious().x(),rect.y()-ppi.peekPrevious().y());
#endif			    
					}
				break;
			}
			default : {
				break;
				}
      }

    }
  return rect;
	}

void NVBFile::release() {
  refCount--;
  if (!refCount) emit free(this);
	}

void NVBFile::use()	{
  refCount++;
	}


/*
void NVBFile::setVisualizer(NVBVizUnion visualizer)
{
  // This function will be called after a query for a new icon.
  // The pages are appended, so we change the bottom.
  icons->setVisualizer(visualizer,rowCount()-1);
}
*/

NVBFile::NVBFile(NVBAssociatedFilesInfo sources, QList< NVBDataSource * > pages)
	:	NVBPageViewModel() \
	,	sourceInfo(sources)
	,	refCount(0) \
{
	foreach(NVBDataSource * s, pages) {
		addSource(s);
		}
	}
