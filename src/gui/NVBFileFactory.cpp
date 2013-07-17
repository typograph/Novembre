//
// Copyright 2006 Timofey <typograph@elec.ru>
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
#include "NVBFileFactory.h"
#include "NVBFileBundle.h"
#include "NVBSettings.h"
#include <QStringList>
#include <QRegExp>
#include <QApplication>
#include <QAction>
#include <QPluginLoader>

#include <QDebug>

#ifdef NVB_STATIC
Q_IMPORT_PLUGIN(rhk)
Q_IMPORT_PLUGIN(rhk4)
Q_IMPORT_PLUGIN(createc)
Q_IMPORT_PLUGIN(winspm)
// Q_IMPORT_PLUGIN(textSTM);
Q_IMPORT_PLUGIN(nanonis)
#endif

void NVBFileLoader::run() {
	if (info.generator()) file = info.generator()->loadFile(info);
	}

NVBFileFactory::NVBFileFactory() {
	deadTree = new NVBFileQueue(5); // TODO Make the size of deadTree user-controllable

	confile = qApp->property("NVBSettings").value<QSettings*>();
	
	if (!confile)
		NVBOutputError("FileFactory cannot access the configuration file");
	else
		confile->beginGroup("Plugins");

// Since QSignalMapper needs QObject to map to,
// and NVBFileGenerator is not an object, but
// QPluginLoader::instance is, the actions are
// created on load

	NVBFileBundle * fbPlugin = new NVBFileBundle(this);
	allGenerators << fbPlugin;

	QAction * tAct;
	tAct = new QAction(fbPlugin->moduleName(), this);
	tAct->setToolTip(fbPlugin->moduleDesc());
	tAct->setCheckable(true);
	tAct->setChecked(true);
	actMapper.setMapping(tAct, fbPlugin);
	connect(tAct, SIGNAL(toggled(bool)), &actMapper, SLOT(map()));
	gActions << tAct;

	foreach(QObject * plugin, QPluginLoader::staticInstances()) {
		NVBFileGenerator *generator = qobject_cast<NVBFileGenerator*>(plugin);

		if (generator) {
			NVBOutputPMsg("Static plugin loaded");
			allGenerators << generator;
			generator->useSettings(confile);

			tAct = new QAction(generator->moduleName(), this);
			tAct->setToolTip(generator->moduleDesc());
			tAct->setCheckable(true);
			tAct->setChecked(true);
			actMapper.setMapping(tAct, plugin);
			connect(tAct, SIGNAL(toggled(bool)), &actMapper, SLOT(map()));

			gActions << tAct;

			}
		}

#ifndef NVB_STATIC
#ifdef Q_WS_MAC
	QDir dir = QDir(qApp->libraryPaths().at(0), QString("*.dylib"));
#elif defined Q_WS_WIN
	QDir dir = QDir(qApp->libraryPaths().at(0), QString("*.dll"));
#else
	QDir dir = QDir(qApp->libraryPaths().at(0), QString("*.so"));
#endif

	if (!dir.cd("files"))
		NVBOutputError(QString("File plugin directory %1/files does not exist").arg(dir.absolutePath()));
	else foreach(QString fileName, dir.entryList(QDir::Files)) {
		QPluginLoader loader(dir.absoluteFilePath(fileName));
		NVBOutputPMsg(QString("Loading plugin %1").arg(fileName));
		NVBFileGenerator *generator = qobject_cast<NVBFileGenerator*>(loader.instance());

		if (generator) {
			allGenerators.append(generator);
 			generator->useSettings(confile);
			
			NVBOutputPMsg("Dynamic plugin loaded");
			tAct = new QAction(generator->moduleName(), this);
			tAct->setToolTip(generator->moduleDesc());
			tAct->setCheckable(true);
			tAct->setChecked(true);
			actMapper.setMapping(tAct, loader.instance());
			connect(tAct, SIGNAL(toggled(bool)), &actMapper, SLOT(map()));
			gActions << tAct;
			}
		else NVBOutputError(loader.errorString());
		}

#endif

	if (!confile) {
		generators = allGenerators;
		}
	else {
// 		confile->beginGroup("Plugins");

		for (int i = 0; i < allGenerators.count(); i++) {
			if (confile->value(allGenerators.at(i)->moduleName(), true).toBool())
				generators << allGenerators.at(i);
			else
				gActions[i]->setChecked(false);
			}

		confile->endGroup();
		}

	connect(&actMapper, SIGNAL(mapped(QObject*)), this, SLOT(changeGenerator(QObject*)));

	if (allGenerators.size() < 2)
		NVBCriticalError(QString("No valid plugins found"));
	else
		foreach(const NVBFileGenerator * g, generators)
		commentNames << g->availableInfoFields();

	updateWildcards();

	if (!commentNames.isEmpty()) {
		commentNames.sort();
		QString cache = commentNames.first();

		for (int i = 1; i < commentNames.count();) {
			if (commentNames.at(i) == cache)
				commentNames.removeAt(i);
			else {
				cache = commentNames.at(i);
				i += 1;
				}
			}
		}

	}

void NVBFileFactory::updateWildcards() {
	wildcards.clear();
	foreach(const NVBFileGenerator * g, generators)
	foreach(QString e, g->extFilters())
	wildcards.insertMulti(e, g);
	}

void NVBFileFactory::changeGenerator(QObject * go) {

	NVBFileGenerator * generator = qobject_cast<NVBFileGenerator*>(go);

	if (!generator) {
		NVBOutputError("Object is not a generator");
		return;
		}

	if (!allGenerators.contains(generator)) { // Note - if this check is removed, we could forget about allGenerators
		NVBOutputError(QString("Generator %1 is not a known generator").arg(generator->moduleName()));
		return;
		}

	int gi = generators.indexOf(generator);

	if (gi >= 0)
		generators.removeAt(gi);
	else
		generators << generator;

	if (confile) {
		confile->beginGroup("Plugins");
		confile->setValue(generator->moduleName(), gi < 0);
		confile->endGroup();
		}

	updateWildcards();
	}

NVBFileFactory::~NVBFileFactory() {
	while (!files.isEmpty()) delete files.takeFirst();

	if (deadTree) delete deadTree;

	while (!generators.isEmpty()) delete generators.takeFirst();
	}


NVBFile * NVBFileFactory::openFile(QString filename) {
	NVBOutputPMsg(QString("Requested file %1").arg(filename));

	if (NVBFile * f = retrieveLoadedFile(filename)) {
		NVBOutputVPMsg(QString("Found already loaded file."));
		return f;
		}
	else if (NVBFile * f = deadTree->retrieve(filename)) {
		NVBOutputVPMsg(QString("Restored released file."));
		files.append(f);
		return f;
		}
	else
		return loadFile(filename);
	}

NVBFile* NVBFileFactory::openFile(const NVBAssociatedFilesInfo & info) {
	NVBOutputPMsg(QString("Requested file %1").arg(info.name()));

	if (NVBFile * f = retrieveLoadedFile(info)) {
		NVBOutputVPMsg(QString("Found already loaded file."));
		return f;
		}
	else if (NVBFile * f = deadTree->retrieve(info)) {
		NVBOutputVPMsg(QString("Restored released file."));
		files.append(f);
		return f;
		}
	else
		return loadFile(info);
	}


NVBFileInfo * NVBFileFactory::getFileInfo(QString filename) {

	NVBOutputVPMsg(QString("Requested file info for %1").arg(filename));

	if (NVBFile * f = retrieveLoadedFile(filename)) {
		NVBOutputVPMsg(QString("Constructed file info from loaded file."));
		return new NVBFileInfo(f);
		}
	else if (NVBFile * f = deadTree->consult(filename)) {
		NVBOutputVPMsg(QString("Constructed file info from released file"));
		return new NVBFileInfo(f);
		}
	else
		return loadFileInfo(filename);

	}

NVBFileInfo * NVBFileFactory::getFileInfo(const NVBAssociatedFilesInfo & info) {

	NVBOutputVPMsg(QString("Requested file info %1").arg(info.name()));

	if (NVBFile * f = retrieveLoadedFile(info)) {
		NVBOutputVPMsg(QString("Constructed file info from loaded file."));
		return new NVBFileInfo(f);
		}
	else if (NVBFile * f = deadTree->consult(info)) {
		NVBOutputVPMsg(QString("Constructed file info from released file"));
		return new NVBFileInfo(f);
		}
	else
		return loadFileInfo(info);

	}

QList<const NVBFileGenerator*> NVBFileFactory::getGeneratorsFromFilename(QString filename) const {

	// First find a wildcard that matches (we assume there's only one)

	QList<QString> wcks = wildcards.keys();
	QListIterator<QString> wcki(wcks);
	QList<const NVBFileGenerator*> matches;

	while (wcki.hasNext()) {
		if (QRegExp(QString("*/") + wcki.next(), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(filename))
			matches.append(wildcards.values(wcki.peekPrevious()));
		}

	return matches;
	}

NVBAssociatedFilesInfo NVBFileFactory::associatedFiles(QString filename) const {

	// Get generators, and if there's only one, rely on it

	QList<const NVBFileGenerator*> wcgs = getGeneratorsFromFilename(filename);

	if (wcgs.count() == 0) return NVBAssociatedFilesInfo(filename);

	if (wcgs.count() == 1) return wcgs.first()->associatedFiles(filename);

	// Compare generators for the case there's more than one

	QList<NVBAssociatedFilesInfo> wcfs;
	foreach(const NVBFileGenerator * wcg, wcgs)
	wcfs << wcg->associatedFiles(filename);

	// Return the first generator that can load the info.

	foreach(NVBAssociatedFilesInfo wcf, wcfs) {
		NVBFileInfo* fi = wcf.loadFileInfo();

		if (fi) {
			delete fi;
			return wcf;
			}
		}

	// All failed
	return NVBAssociatedFilesInfo(filename);
	}

QList<NVBAssociatedFilesInfo> NVBFileFactory::associatedFilesFromDir(const QDir & d, QList<NVBAssociatedFilesInfo> * old_files, QList<int> * deleted) const {
	QStringList filenames = d.entryList(getDirFilters(), QDir::Files, QDir::Name);

	for (QStringList::iterator it = filenames.begin(); it != filenames.end(); it++)
		*it = d.absolutePath() + "/" + *it;

	return associatedFilesFromList(filenames, old_files, deleted);
	}

QList<NVBAssociatedFilesInfo> NVBFileFactory::associatedFilesFromList(QStringList filenames, QList<NVBAssociatedFilesInfo> * old_files, QList<int> * deleted) const {

	qApp->setOverrideCursor(Qt::BusyCursor);

	// Remove already loaded files from the list
	// We assume the list is OK, i.e. there're no two infos using the same file
	if (old_files)
		for (QList<NVBAssociatedFilesInfo>::const_iterator it = old_files->begin(); it != old_files->end(); it++)
			for (int ni = 0; ni < it->count(); ni += 1)
				if (!filenames.removeOne(it->at(ni))) { // backtrace
					for (ni -= 1; ni >= 0; ni -= 1) {
						filenames.append(it->at(ni));
						}

					if (deleted) deleted->append(it - old_files->begin());

					break;
					}

	QList<NVBAssociatedFilesInfo>	files;

	while (filenames.count() > 0) {
		NVBAssociatedFilesInfo info = associatedFiles(filenames.first());
		foreach(QString filename, info) {
//			QStringList::const_iterator j = qBinaryFind(filenames,filename);
//			if (j != filenames.end())
//				filenames.removeAt(j - filenames.begin());
			int j = filenames.indexOf(filename);

			if (j != -1)
				filenames.removeAt(j);
			else if (old_files) // Somebody has it already -- cross-check with list
				for (int i = 0; i < old_files->count(); i++)
					if (old_files->at(i).contains(filename) && (!deleted || deleted->contains(i))) {
						if (old_files->at(i).count() >= info.count()) // There's already one good file
							goto skip_file;
						else {
							foreach(QString fname, old_files->at(i))
							filenames << fname;
							filenames.removeOne(filename);

							if (deleted) deleted->append(i);
							}
						}
			}

		if (info.count() == 0)
			NVBOutputError(QString("Couldn't load file %1").arg(filenames.takeFirst()));
		else
			files.append(info);

// Yeah, I know goto's are evil, but how else can I solve this thing with breaking 2 loops?
skip_file:
		;
		}

	qApp->restoreOverrideCursor();

	return files;
	}

NVBFile * NVBFileFactory::loadFile(const NVBAssociatedFilesInfo & info) {

	if (!info.generator()) return 0;

	NVBFile * file = info.generator()->loadFile(info);

	if (file) {
		files.append(file);
		connect(file, SIGNAL(free(NVBFile*)), SLOT(bury(NVBFile*)));
		return file;
		}

	return 0;

	}

void NVBFileFactory::openFile(const NVBAssociatedFilesInfo & info, const QObject * receiver) {
	NVBOutputPMsg(QString("Requested file %1").arg(info.name()));

	if (NVBFileLoader * l = getLoaderInProgress(info)) {
		if (!l->customers.contains(const_cast<QObject*>(receiver)))
			l->customers << const_cast<QObject*>(receiver);
		}
	else if (NVBFile * f = retrieveLoadedFile(info)) {
		NVBOutputVPMsg(QString("Found already loaded file."));
		NVBFileLoadEvent event(info.name(), f);
		qApp->sendEvent(const_cast<QObject*>(receiver), &event);
		}
	else if (NVBFile * f = deadTree->retrieve(info)) {
		NVBOutputVPMsg(QString("Restored released file."));
		files.append(f);
		NVBFileLoadEvent event(info.name(), f);
		qApp->sendEvent(const_cast<QObject*>(receiver), &event);
		}
	else if (!info.generator()) {
		NVBFileLoadEvent event(info.name(), 0);
		qApp->sendEvent(const_cast<QObject*>(receiver), &event);
		}
	else {
		NVBFileLoader * l = new NVBFileLoader(info, const_cast<QObject*>(receiver));
		loaders << l;
		connect(l, SIGNAL(finished()), this, SLOT(removeLoader()));
		l->start();
		}
	}


NVBFileLoader * NVBFileFactory::getLoaderInProgress(const NVBAssociatedFilesInfo & info) {
	foreach(NVBFileLoader * l, loaders)

	if (l->info == info)
		return l;

	return 0;
	}


void NVBFileFactory::removeLoader() {
	NVBFileLoader * l = qobject_cast<NVBFileLoader*>(sender());
	loaders.removeOne(l);

	if (l->file) {
		files.append(l->file);
		connect(l->file, SIGNAL(free(NVBFile*)), SLOT(bury(NVBFile*)));
		}

	NVBFileLoadEvent event(l->info.name(), l->file);
	foreach(QObject * receiver, l->customers)
	qApp->sendEvent(receiver, &event);
	delete l;
	}

QStringList NVBFileFactory::getDirFilters() const {
	QStringList s;
	foreach(const NVBFileGenerator * g, generators)
	s += g->extFilters();
	return s;
	}

QString NVBFileFactory::getDialogFilter() {
	static QString filter; // TODO make it a class mutable variable and clear on adding more generators;

	if (filter.isNull()) {
		QString s;
		foreach(const NVBFileGenerator * g, generators) {
			s += ";;" + g->nameFilter();
			}
		filter = QString("All supported formats (%1);;All files (*.*)").arg(getDirFilters().join(" ")) + s;
		}

	return filter;
	}


NVBFileQueue::~ NVBFileQueue() {
	while (!isEmpty()) delete takeFirst();
	}

NVBFileQueue::NVBFileQueue(int queue_depth): QList<NVBFile*>(), depth(queue_depth) {
	}

void NVBFileQueue::add(NVBFile * file) {
	append(file);

	if (size() > depth) delete takeFirst();
	}

NVBFile * NVBFileQueue::retrieve(QString filename) {
	for (int i = 0; i < size(); i++) {
		if (at(i)->sources().contains(filename)) return takeAt(i);
		}

	return 0;
	}

NVBFile * NVBFileQueue::retrieve(const NVBAssociatedFilesInfo & info) {
	for (int i = 0; i < size(); i++) {
		if (at(i)->sources() == info) return takeAt(i);
		}

	return 0;
	}

NVBFile * NVBFileQueue::consult(QString filename) {
	for (int i = 0; i < size(); i++) {
		if (at(i)->sources().contains(filename)) return operator[](i);
		}

	return NULL;
	}

NVBFile * NVBFileQueue::consult(const NVBAssociatedFilesInfo & info) {
	for (int i = 0; i < size(); i++) {
		if (at(i)->sources() == info) return operator[](i);
		}

	return NULL;
	}

void NVBFileFactory::bury(NVBFile * f) {
	if (f) {
		files.removeOne(f);
		deadTree->add(f);
		}
	}

void NVBFileFactory::release(QString filename) {
	int i = loadedFileIndex(filename);

	if (i >= 0)
		files.takeAt(i);
	else
		deadTree->retrieve(filename);
	}

int NVBFileFactory::loadedFileIndex(QString filename) {
	for (int i = 0; i < files.count(); i++) {
		if (files.at(i)->sources().contains(filename)) return i;
		}

	return -1;
	}

NVBFile * NVBFileFactory::retrieveLoadedFile(QString filename) {
	foreach(NVBFile * f, files) {
		if (f->sources().contains(filename)) return f;
		}
	return 0;
	}

NVBFile * NVBFileFactory::retrieveLoadedFile(const NVBAssociatedFilesInfo & info) {
	foreach(NVBFile * f, files) {
		if (f->sources() == info) return f;
		}
	return 0;
	}
