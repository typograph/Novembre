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

NVBFile::~NVBFile()
{
  if (refCount)
		NVBOutputError("Non-free file deleted. Possible negative implications for NVBFileFactory");
	foreach(NVBDataSource * d, *this)
		delete d;
}

void NVBFile::release()
{
  refCount--;
  if (!refCount) emit free(this);
}

void NVBFile::use()
{
  refCount++;
}

NVBVariant NVBFile::getComment(const QString& key) const {
	if (comments.contains(key))
		return comments.value(key);
	else {
		NVBVariantList l;
		foreach(NVBDataSource * s, *this) {
			NVBVariant v = s->getComment(key);
			if (v.isValid()) {
				if (v.isAList())
					l.append(v.toList());
				else
					l << v;
				}
			}
		if (l.isEmpty())
			return NVBVariant();
		else if (l.count() == 1)
			return l.first();
		else
			return NVBVariant(l);
		}
	}

