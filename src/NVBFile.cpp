//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "NVBFile.h"
#include "NVBforeach.h"

//------------- NVBFile -----------------------

NVBFile::NVBFile(const NVBFile & other)
	: QObject()
	, QList< NVBDataSource* >(other)
	, files(other.files)
	, comments(other.comments) {
	NVB_FOREACH(NVBDataSource * ds, this)
	useDataSource(ds);
	}

NVBFile::NVBFile(const QList<NVBDataSource*> & other)
	: QObject()
	, QList< NVBDataSource* >(other) {
	NVB_FOREACH(NVBDataSource * ds, this)
	useDataSource(ds);
	}

NVBFile & NVBFile::operator=(const NVBFile & other) {
	if (this != &other) {
		files = other.files;
		QList<NVBDataSource*>::operator =(other);
		NVB_FOREACH(NVBDataSource * ds, this)
		useDataSource(ds);
		}

	return *this;
	}

NVBFile::~NVBFile() {
	if (refCount)
		NVBOutputError("Non-free file deleted. Possible negative implications for NVBFileFactory");

	NVB_FOREACH(NVBDataSource * d, this) {
		NVBConstructableDataSource * cd = qobject_cast<NVBConstructableDataSource*>(d);

		if (cd) cd->detach();

		releaseDataSource(d);
		}
	}

void NVBFile::release() {
	refCount--;

	if (!refCount) emit free(this);
	}

void NVBFile::use() {
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

void NVBFile::filterAddComments(NVBDataComments& newComments) {
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
		ds->comments.insert(key, comments.value(key));
		comments.remove(key);
		}

	return;
	}
