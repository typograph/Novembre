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
#include <QStringList>
#include <QRegExp>

#ifdef NVB_STATIC
Q_IMPORT_PLUGIN(rhk);
Q_IMPORT_PLUGIN(createc);
//Q_IMPORT_PLUGIN(winspm);
//Q_IMPORT_PLUGIN(textSTM);
//Q_IMPORT_PLUGIN(nanonis);
#endif

NVBFileFactory::NVBFileFactory():deadTree(new NVBFileQueue(5))
{
	foreach (QObject *plugin, QPluginLoader::staticInstances()) {
		NVBFileGenerator *generator = qobject_cast<NVBFileGenerator*>(plugin);
		if (generator) {
			NVBOutputPMsg("NVBFileFactory::NVBFileFactory","Static plugin loaded");
			generators.append(generator);
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
		NVBOutputError("NVBFileFactory::NVBFileFactory",QString("File plugin directory %1/files does not exist").arg(dir.absolutePath()));
	else foreach (QString fileName, dir.entryList(QDir::Files)) {
		QPluginLoader loader(dir.absoluteFilePath(fileName));
		NVBOutputPMsg("NVBFileFactory::NVBFileFactory",QString("Loading plugin %1").arg(fileName));
		NVBFileGenerator *generator = qobject_cast<NVBFileGenerator*>(loader.instance());
		if (generator) generators.append(generator);
		else NVBOutputError("NVBFileFactory::NVBFileFactory",loader.errorString());
	}
#endif

	if (!generators.size())
		NVBCriticalError("NVBFileFactory::NVBFileFactory",QString("No valid plugins found"));
	else
		foreach (const NVBFileGenerator * g, generators) {
			commentNames << g->availableInfoFields();
			QStringList exts = g->extFilters();
			foreach (QString e, exts)
				wildcards.insert(QRegExp(e,Qt::CaseInsensitive,QRegExp::Wildcard),g);
			}
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

NVBFileFactory::~NVBFileFactory()
{
	while(not files.isEmpty()) delete files.takeFirst();
	if (deadTree) delete deadTree;
	while (not generators.isEmpty()) delete generators.takeFirst();
}


NVBFile * NVBFileFactory::openFile( QString filename )
{
	NVBOutputPMsg("NVBFileFactory::openFile",QString("Requested file %1").arg(filename));

	if (NVBFile * f = retrieveLoadedFile(filename)) {
		NVBOutputVPMsg("NVBFileFactory::openFile",QString("Found already loaded file."));
		return f;
	}
	else if (NVBFile * f = deadTree->retrieve(filename)) {
		NVBOutputVPMsg("NVBFileFactory::openFile",QString("Restored released file."));
		files.append(f);
		return f;
	}
	else
		return loadFile(filename);
}


NVBFileInfo * NVBFileFactory::getFileInfo( QString filename )
{

	NVBOutputVPMsg("NVBFileFactory::getFileInfo",QString("Requested file info for %1").arg(filename));

	if (NVBFile * f = retrieveLoadedFile(filename)) {
		NVBOutputVPMsg("NVBFileFactory::getFileInfo",QString("Constructed file info from loaded file."));
		return new NVBFileInfo(f);
	}
	else if (NVBFile * f = deadTree->consult(filename)) {
		NVBOutputVPMsg("NVBFileFactory::getFileInfo",QString("Constructed file info from released file"));
		return new NVBFileInfo(f);
	}
	else
		return loadFileInfo(filename);

}

QList<const NVBFileGenerator*> NVBFileFactory::getGeneratorsFromFilename(QString filename) const {

	// First find a wildcard that matches (we assume there's only one)

	QList<QRegExp> wcks = wildcards.keys();
	QListIterator<QRegExp> wcki(wcks);
	QList<const NVBFileGenerator*> matches;
	while ( wcki.hasNext() ) {
		if (wcki.next().exactMatch(filename))
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
		NVBFileInfo* fi = wcf.loadFileInfo();
		if (fi) {
			delete fi;
			return wcf;
			}
		}

	// All failed
	return NVBAssociatedFilesInfo(filename);
}

void NVBFileFactory::associatedFiles(const QDir & d, QList<NVBAssociatedFilesInfo> & files) const {
	QStringList filenames = d.entryList(getDirFilters(),QDir::Files,QDir::Name);

	// Remove already loaded files from the list
	// We assume the list is OK, i.e. there're no two infos using the same file
	foreach(NVBAssociatedFilesInfo loaded_info, files)
		foreach(QString filename, loaded_info)
			filenames.removeOne(filename);

	while (filenames.count() > 0) {
		NVBAssociatedFilesInfo info = associatedFiles(filenames.first());
		foreach(QString filename, info) {
			int j = filenames.indexOf(filename);
			if (j >= 0)
				filenames.removeAt(j);
			else // Somebody has it already -- cross-check with list
				for (int i = 0; i < files.count(); i++)
					if (files.at(i).contains(filename)) {
						if (files.at(i).count() >= info.count()) // There's already one good file
							goto skip_file;	
						else {
							files.removeAt(i);
							i -= 1;
							}
						}
			}
		files.append(info);
// Yeah, I know goto's are evil, but how else can I solve this thing with breaking 2 loops?
skip_file: ;
		}
}

NVBFile * NVBFileFactory::loadFile( const NVBAssociatedFilesInfo & info )
{

	if (!info.generator()) return 0;

	NVBFile * file = info.generator()->loadFile(info);

	if (file) {
		files.append(file);
		connect(file,SIGNAL(free(NVBFile*)),SLOT(bury(NVBFile*)));
		return file;
	}

	return 0;

}

QStringList NVBFileFactory::getDirFilters( ) const
{
	QStringList s;
	foreach(const NVBFileGenerator * g, generators)
		s += g->extFilters();
	return s;
}

QString NVBFileFactory::getDialogFilter( )
{
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

NVBFile * NVBFileQueue::consult(QString filename)
{
	for (int i=0; i<size(); i++) {
		if (at(i)->sources().contains(filename)) return operator[](i);
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

