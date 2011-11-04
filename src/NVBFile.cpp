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

NVBFile::NVBFile(const NVBFile & other)
 : QObject()
 , QList< NVBDataSource* >(other)
 , files(other.files)
 , comments(other.comments)
	{
	NVB_FOREACH(NVBDataSource * ds, this)
			useDataSource(ds);
	}

NVBFile::NVBFile(const QList<NVBDataSource*> & other)
	: QObject()
	, QList< NVBDataSource* >(other)
	{
	NVB_FOREACH(NVBDataSource * ds, this)
			useDataSource(ds);
	}

NVBFile & NVBFile::operator=(const NVBFile & other)
	{
	if (this != &other) {
		files = other.files;
		QList<NVBDataSource*>::operator =(other);
		NVB_FOREACH(NVBDataSource * ds, this)
			useDataSource(ds);
		}
	return *this;
	}

NVBFile::~NVBFile()
{
  if (refCount)
		NVBOutputError("Non-free file deleted. Possible negative implications for NVBFileFactory");
	NVB_FOREACH(NVBDataSource * d, this) {
		NVBConstructableDataSource * cd = qobject_cast<NVBConstructableDataSource*>(d);
		if (cd) cd->detach();
		releaseDataSource(d);
		}
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
		NVB_FOREACH(NVBDataSource * s, this) {
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
		
	if (comments.isEmpty()) {
		// It can happen, actually - if more than 2 sets have totally different comments
// 		NVBOutputPMsg("Adding first comments to a non-empty file object. This was probably not intended.");
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
