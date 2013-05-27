//
// Copyright 2010 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "NVBDirModelHelpers.h"
#include "NVBFileInfo.h"
#include "NVBFileFactory.h"
#include <QtCore/QtAlgorithms>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtCore/QThreadPool>

class NVBFolderListEvent : public QEvent {
	public:
		QList<NVBDirEntry*> deleted, added;

		NVBFolderListEvent(QList<NVBDirEntry*> toadd, QList<NVBDirEntry*> toremove)
			: QEvent((QEvent::Type)(NVBDirEntry::FolderListEvent))
			, deleted(toremove)
			, added(toadd)
			{;}
	};

class NVBFileListEvent : public QEvent {
	public:
		QList<NVBFileInfo*> deleted, added;

		NVBFileListEvent(QList<NVBFileInfo*> toadd, QList<NVBFileInfo*> toremove)
			: QEvent((QEvent::Type)(NVBDirEntry::FileListEvent))
			, deleted(toremove)
			, added(toadd)
			{;}
	};

bool NVBDirModelFileInfoLessThan::operator()(const NVBFileInfo* fi1, const NVBFileInfo* fi2) const {
	if (!fi1 || !fi2) {
		NVBOutputError("Got NULL NVBFileInfo");
		return true;
		}

	switch (sortOrder) {
		case Qt::AscendingOrder :
			return fi1->getInfo(sortKey) < fi2->getInfo(sortKey);

		case Qt::DescendingOrder :
			return fi1->getInfo(sortKey) > fi2->getInfo(sortKey);

		default :
			return false;
		}
	}

bool NVBDirModelFileInfoFilter::operator()(const NVBFileInfo * fi) const {

	if (!fi) {
		NVBOutputError("Got NULL NVBFileInfo");
		return false;
		}

	if (!columns)
		return true;

	if (filters.isEmpty())
		return true;

	bool accept = true;
	foreach (NVBFileFilter f, filters) {
		bool filter_pass;

		switch (f.direction) {
			case NVBFileFilter::Equal:
				filter_pass = fi->getInfoAsString(columns->key(f.column)).contains(f.match);
				break;

			case NVBFileFilter::Less:
				filter_pass = fi->getInfo(columns->key(f.column)) < f.limit;
				break;

			case NVBFileFilter::Greater:
				filter_pass = fi->getInfo(columns->key(f.column)) > f.limit;
				break;
			}

		switch (f.binding) {
			case NVBFileFilter::And : {
				accept &= filter_pass;
				break;
				}

			case NVBFileFilter::AndNot : {
				accept &= !filter_pass;
				break;
				}

			case NVBFileFilter::Or : {
				accept |= filter_pass;
				break;
				}

			case NVBFileFilter::OrNot : {
				accept |= !filter_pass;
				break;
				}
			}
		}
	return accept;
	}

/*
 * We cannot remove folders and/or add files in another thread
 * since otherwise QModelIndex's created by the model may become invalid.
 *
 * Thus we would do better by posting events to NVBDirEntry
 */

NVBDirEntryLoader::NVBDirEntryLoader(NVBDirEntry* entry): QRunnable(), e(entry) {
	setAutoDelete(true);
	fileFactory = qApp->property("filesFactory").value<NVBFileFactory*>();
	}

NVBDirEntryLoader::~NVBDirEntryLoader() {

	}

QList< int > NVBDirEntryLoader::indexesOf(const QStringList& items, const QStringList& list) {
	QList<int> result;
	foreach(QString s, items)
	result << list.indexOf(s);
	return result;
	}


void NVBDirEntryLoader::run() {
	if (!e) return;

	if (e->type == NVBDirEntry::NoContent) {
		QMetaObject::invokeMethod(e, "setLoaded", Qt::AutoConnection);
		return;
		}

	QDir dir = e->dir;

	if (!dir.exists()) {
		NVBOutputError(QString("Directory %1 does not exist").arg(dir.absolutePath()));
		QMetaObject::invokeMethod(e, "errorOnLoad", Qt::AutoConnection);
		return;
		}

	if (e->isRecursive()) {
		QStringList foldernames = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable |  QDir::Executable, QDir::Name);

		QList<NVBDirEntry * > newentries;
		QList<NVBDirEntry * > oldentries;

		foreach(NVBDirEntry * ee, e->folders)

		if (!foldernames.removeOne(ee->dir.dirName()))
			oldentries << ee;

		foreach(QString name, foldernames) {
			QDir rdir(dir.filePath(name));
			newentries << new NVBDirEntry(e, name, rdir, true);
			newentries.last()->moveToThread(QCoreApplication::instance()->thread());
			}

		qApp->postEvent(e, new NVBFolderListEvent(newentries, oldentries));
		}

	if (!fileFactory) {
		NVBOutputError("Cannot access the file factory");
		QMetaObject::invokeMethod(e, "errorOnLoad", Qt::AutoConnection);
		return;
		}

	QStringList filenames = dir.entryList(fileFactory->getDirFilters(), QDir::Files, QDir::Name);

//	if (filenames.isEmpty()) {
//		QMetaObject::invokeMethod(e,"setLoaded",Qt::AutoConnection);
//		return;
//		}

	QString dirStr = QString("%1/%2").arg(dir.absolutePath());

	for(QStringList::iterator it = filenames.begin(); it != filenames.end(); it++)
		*it = dirStr.arg(*it);

	// Remove already loaded files from the list
	// We assume the list is OK, i.e. there're no two infos using the same file

	// Also, we should create a local copy of e->files
	// Qt says this is reentrant...
	QList<NVBFileInfo*> efiles = e->files;

	QList<NVBFileInfo*> oldentries;
	QList<NVBFileInfo*>	newentries;

	for (int i = 0; i < efiles.count(); i++) {
		QList<int> rixs = indexesOf(efiles.at(i)->files, filenames);

		if (rixs.count(-1) > 0) {
			NVBFileInfo * nfi = efiles.at(i)->files.loadFileInfo();

			if (!nfi) { // OK, something happened, a file was deleted, we should reverse
				oldentries << efiles.takeAt(i);
				i--; // step back to step forward
				continue; // next fileinfo
				}
			else if (efiles.at(i)->files != nfi->files) {
				oldentries << efiles.takeAt(i);
				newentries << nfi;
				efiles << nfi;
				i--;
				}
			else
				delete nfi;
			}

		qSort(rixs.begin(), rixs.end(), qGreater<int>());
		foreach(int k, rixs)
		filenames.removeAt(k);
		}

	qApp->postEvent(e, new NVBFileListEvent(newentries, oldentries));
	oldentries.clear();
	newentries.clear();

	// Loading NVBFileInfo is faster than loading associatedFilesInfo

	while (filenames.count() > 0) {
		NVBFileInfo * info = fileFactory->getFileInfo(filenames.first());

		if (!info) {
			NVBOutputError(QString("Couldn't load file info for %1").arg(filenames.takeFirst()));
			continue;
			}

		// Remove associated files from list
		QList<int> rixs = indexesOf(info->files, filenames);

		if (rixs.count(-1) > 0) {
			// Somebody may have it already -- cross-check with list
			// If this file is intersecting with another, we can leave the other be,
			// but if the other is contained within, we should remove it
			// The case when the older one is the container should be impossible

			/*
			 * 1) find the file
			 * 2) if exists and is contained within -> remove it completely
			 * 3) if exists and is bigger/comparable/not contained -> leave it be and forget about these files
			 * 4) if exists and contains -> luckily, this is impossible.
			 */

			QStringList inames = info->files; // a copy so that we can forget things

			for(int k = rixs.indexOf(-1); k >= 0; k = rixs.indexOf(-1, k)) {
				int i = locateFile(inames.at(k), efiles);

				if (i >= 0) {
					QList<int> nixs = indexesOf(efiles.at(i)->files, inames);

					if (nixs.count(-1) == 0) // Contained within
						oldentries << efiles.takeAt(i);
					else
						nixs.removeAll(-1);

					qSort(nixs.begin(), nixs.end(), qGreater<int>());
					foreach(int m, nixs) {
						rixs.removeAt(m);
						inames.removeAt(m);

						if (m < k) k -= 1; // so that indexOf in <for> works
						}
					}
				else { // somehow it's not in the list
					rixs.removeAt(k);
					inames.removeAt(k);
					}
				}
			}

		// rixs contains no (-1) now

		qSort(rixs.begin(), rixs.end(), qGreater<int>());
		foreach(int r, rixs)
		filenames.removeAt(r);

		// add
		newentries << info;
		efiles << info;

		if (newentries.count() >= 10) {
			qApp->postEvent(e, new NVBFileListEvent(newentries, oldentries));
			newentries.clear();
			oldentries.clear();
			}

		}

	qApp->postEvent(e, new NVBFileListEvent(newentries, oldentries));
	QMetaObject::invokeMethod(e, "setLoaded", Qt::AutoConnection);
	}

int NVBDirEntryLoader::locateFile(QString file, QList< NVBFileInfo* > list) {
	for(int i = 0; i < list.count(); i++)
		if (list.at(i)->files.contains(file))
			return i;

	return -1;
	}



NVBDirEntry::NVBDirEntry( ): QObject(), parent(0), status(NVBDirEntry::Populated), type(NoContent) {;}

NVBDirEntry::NVBDirEntry(NVBDirEntry * _parent, QString _label) :
/*QObject(),*/ parent(_parent), label(_label), status(NVBDirEntry::Populated), type(NoContent) {;}

NVBDirEntry::NVBDirEntry(NVBDirEntry * _parent, QString _label, QDir _dir, bool recursive) :
/*QObject(),*/ parent(_parent), label(_label), dir(_dir), status(NVBDirEntry::Virgin), type(recursive ? AllContent : FileContent) {

//   if (recursive) recurseFolders();
	}

NVBDirEntry::~ NVBDirEntry( ) {
	while (!files.isEmpty()) delete files.takeFirst();

	while (!folders.isEmpty()) delete folders.takeFirst();
	}

bool NVBDirEntry::event(QEvent* e) {
	switch (e->type()) {
		case (QEvent::Type)FileListEvent : {
			NVBFileListEvent * fe = dynamic_cast<NVBFileListEvent*>(e);

			if (fe) {
				fe->accept();
				foreach(NVBFileInfo * i, fe->deleted) {
					int k = files.indexOf(i);

					if (k < 0) {
						NVBOutputError(QString("Trying to remove a non-existing file %1").arg( i ? i->files.name() : "0"));
						continue;
						}

					removeOrigFileAt(k);
					}
				foreach(NVBFileInfo * i, fe->added)
				insertFile(i);
				return true;
				}
			}

		case (QEvent::Type)FolderListEvent : {
			NVBFolderListEvent * fe = dynamic_cast<NVBFolderListEvent*>(e);

			if (fe) {
				foreach(NVBDirEntry * e, fe->deleted) {
					int i = folders.indexOf(e);

					if (i < 0) {
						NVBOutputError(QString("Trying to remove a non-existing folder %1").arg( e ? e->dir.dirName() : "0"));
						continue;
						}

					removeFolderAt(i);
					}
				emit beginOperation(this, folders.count(), fe->added.count(), NVBDirEntry::FolderInsert);
				foreach(NVBDirEntry * e, fe->added) {
					folders << e;
					e->sort(sorter);
					}
				emit endOperation();
				return true;
				}
			}

		default:
			return QObject::event(e);
		}
	}

int NVBDirEntry::folderCount( ) const {
	if (isRecursive() && !isPopulated())
		return estimatedFolderCount();

	return folders.count();
	}

int NVBDirEntry::estimatedFolderCount( ) const {
	if (isRecursive() && !isPopulated())
		return dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable |  QDir::Executable, QDir::Name).count();

	return folders.count();
	}

void NVBDirEntry::recurseFolders() {
	if (!isRecursive()) return;

	QFileInfoList subfolders = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable |  QDir::Executable, QDir::Name);

//   emit beginOperation(this,0,subfolders.size(),FolderInsert);

	foreach(QFileInfo folder, subfolders) {
		QDir rdir(dir);
		rdir.setPath(folder.absoluteFilePath());
		folders << new NVBDirEntry(this, folder.fileName(), rdir, true);
		folders.last()->sort(sorter);
		}

//   emit endOperation();
	}

int NVBDirEntry::estimatedFileCount() const {
	if (type == NoContent)
		return 0;

	if ( isPopulated() )
		return fileCount();

	if ( dir.exists() )
		return dir.count();

	return 0;
	}

/// remove file at mapped index
void NVBDirEntry::removeFileAt(int i) {
	if (i >= fileCount()) {
		NVBOutputError("Index out of bounds");
		return;
		}

	emit beginOperation(this, i, 1, NVBDirEntry::FileRemove);
	delete files.takeAt(indexMap.at(i));

	for (int j = i; j < fileCount() - 1; j++)
		indexMap[j] = indexMap.at(j + 1) - 1;

	indexMap.removeLast();
	emit endOperation();
	}

/// remove file at non-mapped index
void NVBDirEntry::removeOrigFileAt(int i) {
	if (i >= files.count()) {
		NVBOutputError("Index out of bounds");
		return;
		}

	QList<int>::iterator mi = qLowerBound(indexMap.begin(), indexMap.end(), i);

	if ( mi == indexMap.end() || *mi != i ) {
		// FIXME possible situation, when some thread tries
		// to access the files by index during this operation
		delete files.takeAt(i);

		for (; mi != indexMap.end(); mi++)
			*mi -= 1;
		}
	else {
		emit beginOperation(this, mi - indexMap.begin(), 1, NVBDirEntry::FileRemove);
		delete files.takeAt(i);

		for (; mi != indexMap.end(); mi++)
			*mi = *(mi + 1) - 1;

		indexMap.removeLast();
		emit endOperation();
		}
	}

void NVBDirEntry::removeFolderAt(int i) {
	emit beginOperation(this, i, 1, NVBDirEntry::FolderRemove);
	delete folders.takeAt(i);
	emit endOperation();
	}

void NVBDirEntry::insertFile(NVBFileInfo* file) {
	if (file) {
		QList<NVBFileInfo*>::iterator newpos = qLowerBound(files.begin(), files.end(), file, sorter);
		int ix = newpos - files.begin();
		// We can't use an iterator here, since it doesn't survive appending items
		int im = qLowerBound(indexMap.begin(), indexMap.end(), ix) - indexMap.begin();

		if (accepted(file)) {
			emit beginOperation(this, im, 1, FileInsert);
			files.insert(newpos, file);
			indexMap.append(0);

			for(int jm = indexMap.count() - 1; jm > im; jm--)
				indexMap[jm] = indexMap.at(jm - 1) + 1;

			indexMap[im] = ix;
			emit endOperation();
			}
		else {
			files.insert(newpos, file);

			for(; im < indexMap.count(); im++)
				indexMap[im] += 1;
			}
		}
	}


void NVBDirEntry::populate() {
	if (status != Loading) {
		status = NVBDirEntry::Loading;
//		QApplication::setOverrideCursor(Qt::BusyCursor);
		QThreadPool::globalInstance()->start(new NVBDirEntryLoader(this));
		}
	else
		NVBOutputError("populate called while populating");
	}

bool NVBDirEntry::refresh() {
	if (status != Loading) {
//		QApplication::setOverrideCursor(Qt::BusyCursor);
		status = NVBDirEntry::Loading;
		QThreadPool::globalInstance()->start(new NVBDirEntryLoader(this));
		return true;
		}
	else {
		NVBOutputError("refresh called while populating");
		return false;
		}
	}

void NVBDirEntry::sort(const NVBDirModelFileInfoLessThan & lessThan) {
	sorter = lessThan;

	if (isPopulated()) {
		foreach(NVBDirEntry * e, folders)
		e->sort(lessThan);
		qSort(files.begin(), files.end(), lessThan);
		refilter();
		}
	}

void NVBDirEntry::filter(const NVBDirModelFileInfoFilter & acceptFile) {
	accepted = acceptFile;

	// Check if we have subfolders

	if (!isPopulated())	return;

	foreach(NVBDirEntry * e, folders)
	e->filter(acceptFile);

	// Check if we have files to filter

	if (files.isEmpty()) return;

	// Clear filtered indexes

	emit beginOperation(this, 0, indexMap.count(), NVBDirEntry::FileRemove);
	indexMap.clear();
	emit endOperation();

	// Put new indexes in

	for(int i = 0; i < files.count(); i++)
		if (accepted(files.at(i))) {
			emit beginOperation(this, indexMap.count(), 1, NVBDirEntry::FileInsert);
			indexMap << i;
			emit endOperation();
			}
	}

void NVBDirEntry::refilter() {
	if (isPopulated() && files.count() > 0) {
		indexMap.clear();

		for(int i = 0; i < files.count(); i++)
			if (accepted(files.at(i))) {
				indexMap << i;
				}
		}
	}

int NVBDirEntry::indexOf( QString name ) {
	for (int i = 0; i < folders.count(); i++)
		if (folders[i]->label == name)
			return i;

	name = QDir::cleanPath(name);

	for (int i = 0; i < files.count(); i++)
		if (files[i]->files.name() == name || files[i]->files.contains(name) )
			return i;

	return -1;
	}

void NVBDirEntry::addFolder(NVBDirEntry * folder) {
	if (folders.indexOf(folder) == -1 ) {
		emit beginOperation(this, folders.size(), 1, FolderInsert);
		folders.append(folder);
		folder->sort(sorter);
		emit endOperation();
		}
	}

void NVBDirEntry::insertFolder(int index, NVBDirEntry *folder) {
	if (folders.indexOf(folder) == -1 ) {
		emit beginOperation(this, index, 1, FolderInsert);
		folders.insert(index, folder);
		folder->sort(sorter);
		emit endOperation();
		}
	}

void NVBDirEntry::refreshSubfolders() {
	foreach (NVBDirEntry * e, folders)

	if (e->isPopulated()) {
		e->refresh();
		e->refreshSubfolders();
		}
	}

