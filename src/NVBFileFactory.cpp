//
// C++ Implementation: NVBFileFactory
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBFileFactory.h"
#include "NVBFileBundle.h"
#include <QtCore/QStringList>
#include <QtCore/QRegExp>
#include <QtCore/QPluginLoader>

#include <QtGui/QApplication>
#include <QtGui/QAction>

#ifdef NVB_STATIC
Q_IMPORT_PLUGIN(rhk);
Q_IMPORT_PLUGIN(createc);
Q_IMPORT_PLUGIN(winspm);
// Q_IMPORT_PLUGIN(textSTM);
Q_IMPORT_PLUGIN(nanonis);
#endif

/*
void NVBFileLoader::run() {
	if (info.generator()) file = info.generator()->loadFile(info);
}
*/
NVBFileFactory::NVBFileFactory()
{
	deadTree = new NVBFileQueue(5); // TODO Make the size of deadTree user-controllable
	
	NVBFileBundle * fbPlugin = new NVBFileBundle(this);
	allGenerators << fbPlugin;
	
	QAction * tAct;
	tAct = new QAction(fbPlugin->moduleName(),this);
	tAct->setToolTip(fbPlugin->moduleDesc());
	tAct->setCheckable(true);
	tAct->setChecked(true);
	actMapper.setMapping(tAct,fbPlugin);
	connect(tAct,SIGNAL(triggered()),&actMapper,SLOT(map()));
	gActions << tAct;
  
	foreach (QObject *plugin, QPluginLoader::staticInstances()) {
		NVBFileGenerator *generator = qobject_cast<NVBFileGenerator*>(plugin);
		if (generator) {
			NVBOutputPMsg("Static plugin loaded");
			allGenerators << generator;

			tAct = new QAction(generator->moduleName(),this);
			tAct->setToolTip(generator->moduleDesc());
			tAct->setCheckable(true);
			tAct->setChecked(true);
			actMapper.setMapping(tAct,plugin);
			connect(tAct,SIGNAL(triggered()),&actMapper,SLOT(map()));
			
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
	if (!dir.cd ("files"))
		NVBOutputError(QString("File plugin directory %1/files does not exist").arg(dir.absolutePath()));
	else foreach (QString fileName, dir.entryList(QDir::Files)) {
		QPluginLoader loader(dir.absoluteFilePath(fileName));
		NVBOutputPMsg(QString("Loading plugin %1").arg(fileName));
		NVBFileGenerator *generator = qobject_cast<NVBFileGenerator*>(loader.instance());
		if (generator) {
			allGenerators.append(generator);
			NVBOutputPMsg("Dynamic plugin loaded");
			tAct = new QAction(generator->moduleName(),this);
			tAct->setToolTip(generator->moduleDesc());
			tAct->setCheckable(true);
			tAct->setChecked(true);
			actMapper.setMapping(tAct,loader.instance());
			connect(tAct,SIGNAL(triggered()),&actMapper,SLOT(map()));
			gActions << tAct;
			}
		else NVBOutputError(loader.errorString());
	}
#endif

	generators = allGenerators;
	// TODO Disable generators as saved in confile
	connect(&actMapper,SIGNAL(mapped(QObject*)),this,SLOT(changeGenerator(QObject*)));

	if (generators.size() < 2)
		NVBCriticalError(QString("No valid plugins found"));
	else
		foreach (const NVBFileGenerator * g, generators)
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
	foreach (const NVBFileGenerator * g, generators)
		foreach (QString e, g->extFilters())
			wildcards.insertMulti(e,g);
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
	
	updateWildcards();
}

NVBFileFactory::~NVBFileFactory()
{
	while(not files.isEmpty()) delete files.takeFirst();
	if (deadTree) delete deadTree;
	while (not generators.isEmpty()) delete generators.takeFirst();
}

QList<const NVBFileGenerator*> NVBFileFactory::getGeneratorsFromFilename(QString filename) const {

	// TODO 0.1 :: Wildcards should be in an additional list to avoid creating them every time 

	QList<QString> wcks = wildcards.keys();
	QListIterator<QString> wcki(wcks);
	QList<const NVBFileGenerator*> matches;
	while ( wcki.hasNext() ) {
		if (QRegExp( QString("*/%1").arg(wcki.next()),Qt::CaseInsensitive,QRegExp::Wildcard).exactMatch(filename))
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
	foreach (const NVBFileGenerator * wcg, wcgs)
		wcfs << wcg->associatedFiles(filename);

	// Return the first generator that can load the info.

	foreach (NVBAssociatedFilesInfo wcf, wcfs) {
		if (wcf.isEmpty()) continue;
		NVBFileInfo* fi = wcf.loadFileInfo();
		if (fi) {
			delete fi;
			return wcf;
			}
		}

	// All failed
	return NVBAssociatedFilesInfo(filename);
}

NVBFile * NVBFileFactory::getFile( QString filename, bool track )
{
	NVBOutputPMsg(QString("Requested file %1").arg(filename));

	if (!track) return openFile(filename,track);
	
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
		return openFile(filename);
}

NVBFile* NVBFileFactory::getFile( const NVBAssociatedFilesInfo& info, bool track )
{
	NVBOutputPMsg(QString("Requested file %1").arg(info.name()));

	if (!track) return openFile(info,track);
	
	if (NVBFile * f = retrieveLoadedFile(info)) {
		NVBOutputVPMsg(QString("Found already loaded file."));
		return f;
	}
	
	if (NVBFile * f = deadTree->retrieve(info)) {
		NVBOutputVPMsg(QString("Restored released file."));
		files.append(f);
		return f;
	}
	
	return openFile(info);
}


NVBFileInfo * NVBFileFactory::getFileInfo( QString filename )
{

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
		return openFileInfo(filename);

}

NVBFileInfo * NVBFileFactory::getFileInfo( const NVBAssociatedFilesInfo & info)
{

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
		return openFileInfo(info);
}

NVBFileInfo* NVBFileFactory::openFileInfo(QString filename) const
{
	// Get generators, and if there's only one, rely on it

	QList<const NVBFileGenerator*> wcgs = getGeneratorsFromFilename(filename);
	if (wcgs.count() == 0) return 0;
	if (wcgs.count() == 1) return wcgs.first()->associatedFiles(filename).loadFileInfo();

	// Compare generators for the case there's more than one

	QList<NVBAssociatedFilesInfo> wcfs;
	foreach (const NVBFileGenerator * wcg, wcgs)
		wcfs << wcg->associatedFiles(filename);

	// Pick the first generator that can load the info.

	foreach (NVBAssociatedFilesInfo wcf, wcfs) {
		if (wcf.isEmpty()) continue;
		NVBFileInfo* fi = wcf.loadFileInfo();
		if (fi)
			return fi;
		}
		
	return 0;
}


NVBFile * NVBFileFactory::openFile( const NVBAssociatedFilesInfo& info, bool track )
{

	if (!info.generator()) return 0;

	NVBFile * file = info.generator()->loadFile(info);

	if (file) {
		if (track) {
			files.append(file);
			connect(file,SIGNAL(free(NVBFile*)),SLOT(bury(NVBFile*)));
			}
		return file;
	}

	return 0;

}

QStringList NVBFileFactory::getDirFilters( ) const
{
	// TODO 0.1 :: This should possibly be cached
	QStringList s;
	foreach(const NVBFileGenerator * g, allGenerators)
		s += g->extFilters();
	return s;
}

QString NVBFileFactory::getDialogFilter( )
{
	// TODO 0.1 :: make it a class mutable variable and clear on adding more generators;
	static QString filter;
	if (filter.isNull()) {
		QString s;
		foreach(const NVBFileGenerator * g, allGenerators) {
			s += ";;" + g->nameFilter();
		}
		filter = QString("All supported formats (%1);;All files (*.*)").arg(getDirFilters().join(" ")) + s;
	}
	return filter;
}


NVBFileQueue::~ NVBFileQueue()
{
	while(!isEmpty()) delete takeFirst();
}

NVBFileQueue::NVBFileQueue(int queue_depth):QList<NVBFile*>(),depth(queue_depth)
{
}

void NVBFileQueue::add(NVBFile * file)
{
	append(file);
	if (size() > depth) delete takeFirst();
}

NVBFile * NVBFileQueue::retrieve(QString filename)
{
	for (int i=0; i<size(); i++) {
		if (at(i)->sources().contains(filename)) return takeAt(i);
	}
	return 0;
}

NVBFile * NVBFileQueue::retrieve(const NVBAssociatedFilesInfo & info)
{
	for (int i=0; i<size(); i++) {
		if (at(i)->sources() == info) return takeAt(i);
	}
	return 0;
}

NVBFile * NVBFileQueue::consult(QString filename)
{
	for (int i=0; i<size(); i++) {
		if (at(i)->sources().contains(filename)) return operator[](i);
	}
	return NULL;
}

NVBFile * NVBFileQueue::consult(const NVBAssociatedFilesInfo & info)
{
	for (int i=0; i<size(); i++) {
		if (at(i)->sources() == info) return operator[](i);
	}
	return NULL;
}

void NVBFileFactory::bury(NVBFile * f)
{
	if (f) {
		files.removeOne(f);
		deadTree->add(f);
	}
}

void NVBFileFactory::release(QString filename)
{
	int i = loadedFileIndex(filename);
	if (i >= 0)
		files.takeAt(i);
	else
		deadTree->retrieve(filename);
}

int NVBFileFactory::loadedFileIndex(QString filename)
{
	for (int i=0;i<files.count();i++) {
		if (files.at(i)->sources().contains(filename)) return i;
		}

	return -1;
}

NVBFile * NVBFileFactory::retrieveLoadedFile(QString filename)
{
	foreach (NVBFile * f, files) {
		if (f->sources().contains(filename)) return f;
	}
	return 0;
}

NVBFile * NVBFileFactory::retrieveLoadedFile(const NVBAssociatedFilesInfo & info)
{
	foreach (NVBFile * f, files) {
		if (f->sources() == info) return f;
	}
	return 0;
}

