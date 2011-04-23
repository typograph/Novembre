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
#include "NVBforeach.h"

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

NVBVariant NVBFile::collectComments(const QString& key) const {
	if (comments.contains(key))
		return comments.value(key);
	else {
		NVBVariantList l;
		foreach(NVBDataSource * s, *this) {
			NVBVariant v = s->collectComments(key);
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

void NVBFile::filterAddComments(NVBDataComments& newComments)
{
	if (newComments.isEmpty())
		return;

	if (count() == 0) {
		comments.unite(newComments);
		newComments.clear();
		return;
		}
		
	foreach (QString key, comments.keys())
		if (newComments.contains(key) && newComments.value(key) == comments.value(key))
			newComments.remove(key);
		else {
			NVB_FOREACH(NVBDataSource * ds, this)
				ds->comments.insert(key,comments.value(key));
			comments.remove(key);
			}
			
	return;
}
