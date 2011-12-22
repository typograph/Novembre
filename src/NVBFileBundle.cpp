#include "NVBFileBundle.h"
#include "NVBFileFactory.h"
#include "NVBFileInfo.h"
#include "NVBLogger.h"
#include "NVBforeach.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include "NVBAxisMaps.h"
#include "NVBDataCore.h"
#include <malloc.h>

/**
 * NVBFileBundle file format
 * 
 * The file in question has an *.nbl extension
 * 
 * It is a collection of file names that should be loaded together as a single file.
 * Depending on file contents, the data will be aggregated on one or more axes.
 * 
 * File Format:
 * 
 * First line : number of additional axes (N)
 * Zero means that the datasources will just be all put into a single NVBFile
 * 
 * Next N lines : Axis name
 *       Only allowed for the last axis. The size will be calculated automatically.
 *           OR : Axis name - Axis length
 *       Simple bunching of data
 *           OR : Axis name - Axis length - Start - End - Units
 *       The new axis will have a physmap with specified parameters
 *           OR : Axis name - Axis length - Comment key
 *       The new axis will have a map built from this comment's values.
 *       In case the comment does not exist, a default-constructed value will be taken
 * 
 * In case axis name contains spaces, it has to be enclosed in double quotes.
 * 
 * The rest of all lines : Filename (relative to this file's location)
 *       If the number of files is smaller than implied by the axes, the last
 *       axis will be cut at the last complete slice.
 */

QStringList getFilenamesFromFile(QFile & file) {
	
	QString path = QFileInfo(file).absolutePath() + "/";

	QStringList fnames;
	QByteArray line;
	while (!(line = file.readLine(1024).trimmed()).isNull())
	  fnames << path + line;
	
	return fnames;
}

void skipHeader(QFile & file) {
	file.seek(0);
	int nAxes = file.readLine(100).trimmed().toInt();
	
	for(int i=0; i<nAxes; i++)
		file.readLine(200);
}

QStringList getHeader(QFile & file) {
	file.seek(0);
	int nAxes = file.readLine(100).trimmed().toInt();
	
	QStringList header;
	
	for(int i=0; i<nAxes; i++)
		header << file.readLine(200).trimmed();
	
	return header;
}

NVBAssociatedFilesInfo NVBFileBundle::associatedFiles(QString filename) const {
	QStringList ifnames;

  QFile file(filename);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		NVBOutputFileError(&file);
		return NVBAssociatedFilesInfo();
		}

	skipHeader(file);

	foreach(QString fname, getFilenamesFromFile(file)) {
		if (QFileInfo(fname).exists() && !ifnames.contains(fname)) {
			NVBAssociatedFilesInfo li = fileFactory->associatedFiles(fname);
			if (li.count() == 0)
				NVBOutputError(QString("File %1 did not load").arg(fname));
			else
				ifnames << li;
			}
	  else
			NVBOutputError(QString("File %1 did not exist, but was mentioned").arg(fname));
		}

	// We have to add the name of the bundle file here, to mark that it has been loaded.
	if (ifnames.count()) ifnames.prepend(filename);
	
	return NVBAssociatedFilesInfo(QFileInfo(filename).fileName(), ifnames, this);
}

NVBFile * NVBFileBundle::loadFile(const NVBAssociatedFilesInfo & info) const throw()
{
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() < 2) {
		NVBOutputError("No associated files");
		return 0;
		}

  QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		NVBOutputFileError(&file);
		return 0;
		}

	QStringList axes = getHeader(file);

	QList<NVBFile* > allFiles;
	
	QStringList files = info;
	files.takeFirst(); // this file
	while(!files.isEmpty()) {
		NVBFile * f = fileFactory->getFile(files.first(),false);
		if (!f) {
			NVBOutputError(QString("Loading file %1 failed").arg(files.first()));
			continue;
			}
		foreach(QString fname, f->sources())
			files.removeOne(fname); // One file will not be twice in the list.
		allFiles << f;
		}


	if (axes.isEmpty()) { // No axes specified - just bring all together
		NVBFile * nfile = new NVBFile(info);
		foreach(NVBFile * f, allFiles) {
			NVBDataComments c = f->getAllComments();
			nfile->filterAddComments(c);
			NVB_FOREACH(NVBDataSource * ds, f) {
				NVBConstructableDataSource * cds = qobject_cast< NVBConstructableDataSource* >(ds);
				if (cds) // FIXME should probably just create a copy of the datasource
					cds->filterAddComments(c); // Should consume all comments, as those probably went through filterAdd before
				nfile->append(ds);
				}
			f->clear(); // So that the original doesn't delete datasources
			f->release(); // No error messages, please
			delete f; // Don't touch the list again
			}
		return nfile;
		}

// Now there are axes - the interesting part

	NVBFile * nfile = new NVBFile(info);

	QList<NVBConstructableDataSource *> sources;
	NVB_FOREACH(NVBDataSource * ds, allFiles.first()) {
		sources << new NVBConstructableDataSource(nfile);
		if (!sources.last()) {
			NVBOutputError("Allocation of datasources failed");
			delete nfile; // This automatically deletes all allocated datasources
			while (!allFiles.isEmpty()) {
				allFiles.first()->release();
				delete allFiles.takeFirst();
				}
			return 0;
			}
		foreach(NVBAxis a, ds->axes()) {
			sources.last()->addAxis(a.name(),a.length());
			foreach(NVBAxisMapping m, a.maps())
				sources.last()->addAxisMap(m.map->copy(),m.axes);
			}
		nfile->append(sources.last());
		}

	QVector<axissize_t> new_sizes;
	foreach(QString axis, axes) {
		QStringList aprop = axis.split(' ');
		if (aprop.first().at(0) == '\"') {
			QString aname = aprop.takeFirst();
			do {
				if (aprop.isEmpty()) {
					NVBOutputError(QString("Bad axis name format in \'%1\'").arg(axis));
					delete nfile; // This automatically deletes all allocated datasources
					while (!allFiles.isEmpty()) {
						allFiles.first()->release();
						delete allFiles.takeFirst();
						}
					return 0;
					}
				aname += " " + aprop.takeFirst();
				} while (aname.at(aname.length()-1) != '\"');
			aprop.prepend(aname.mid(1,aname.length()-2));
			}
		switch(aprop.length()) {
			case 2: {
				foreach(NVBConstructableDataSource * ds, sources)
					ds->addAxis(aprop.first(),aprop.at(1).toInt());
				break;
				}
			case 3: {
				foreach(NVBConstructableDataSource * ds, sources) {
					ds->addAxis(aprop.first(),aprop.at(1).toInt());
					// FIXME how do we create the map?
					}
				break;
				}
			case 5: {
				foreach(NVBConstructableDataSource * ds, sources) {
					ds->addAxis(aprop.first(),aprop.at(1).toInt());
					ds->addAxisMap(new NVBAxisPhysMap(aprop.at(2).toDouble(),aprop.at(3).toDouble(),NVBUnits(aprop.at(4))));
					}
				break;
				}
			default: {
				NVBOutputError(QString("Bad axis name format in \'%1\'").arg(axis));
				delete nfile; // This automatically deletes all allocated datasources
				while (!allFiles.isEmpty()) {
					allFiles.first()->release();
					delete allFiles.takeFirst();
					}
				return 0;
				break;
				}
			}
		new_sizes << aprop.at(1).toInt();
		}
	
	axissize_t data_size_mult = prod(new_sizes);
	
	for(int i_dsource = 0; i_dsource < nfile->count(); i_dsource++) { // Do every datasource

		NVBDataSource * ds_ref = allFiles.first()->at(i_dsource);
		
		NVBDataComments c = ds_ref->getAllComments();
		
		sources[i_dsource]->filterAddComments(c);
		
		for(int i_dset = 0; i_dset < ds_ref->dataSets().count(); i_dset++ ) {
			
			const NVBDataSet * dset_ref = ds_ref->dataSets().at(i_dset);
			
			axissize_t ex_data_size = prod(dset_ref->sizes());
			axissize_t total_data_size = ex_data_size * data_size_mult;
			
			double * ndata = (double*) malloc(total_data_size);
			double * datafref = ndata;
			
			foreach(NVBFile * ofile, allFiles) {
				if (ofile->count() <= i_dsource || !ofile->at(i_dsource)) {
					NVBOutputError(QString("Not enough datasources in %1").arg(ofile->name()));
					continue;
					}
				if (ofile->at(i_dsource)->dataSets().count() <= i_dset || !ofile->at(i_dsource)->dataSets().at(i_dset)) {
					NVBOutputError(QString("Not enough datasets in %1 [%2]").arg(ofile->name()).arg(i_dsource));
					continue;
					}
				if (!ofile->at(i_dsource)->dataSets().at(i_dset)->data())
					continue;
				memcpy(datafref,ofile->at(i_dsource)->dataSets().at(i_dset)->data(),sizeof(double)*ex_data_size);
				datafref += ex_data_size;
				}
			
			QVector<axisindex_t> new_axes = dset_ref->parentIndexes();
			for(int i = axes.count(); i > 0; i--)
				new_axes << ds_ref->nAxes() - i;
			
			sources[i_dsource]->addDataSet(dset_ref->name(),ndata,dset_ref->dimension(),dset_ref->getAllComments().unite(c),new_axes,dset_ref->type());
			
			}

		}

	while (!allFiles.isEmpty()) {
		allFiles.first()->release();
		delete allFiles.takeFirst(); // We are not reusing any data of the old files, so we can safely delete this.
		}

	return nfile;
	
}

NVBFileInfo * NVBFileBundle::loadFileInfo( const NVBAssociatedFilesInfo & info ) const throw()
{
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() < 2) {
		NVBOutputError("No associated files");
		return 0;
		}

  QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		NVBOutputFileError(&file);
		return 0;
		}

	QStringList axes = getHeader(file);

	if (axes.isEmpty()) { // No axes specified - just bring all together
		NVBFileInfo * fi = new NVBFileInfo(info);
		QStringList files = info;
		files.takeFirst(); // this file
		while(!files.isEmpty()) {
			NVBFileInfo * finfo = fileFactory->getFileInfo(files.first());
			foreach(QString fname, finfo->files)
				files.removeOne(fname); // One file will not be twice in the list.
			NVBDataComments c = finfo->getAllComments();
			fi->filterAddComments(c);
			NVB_FOREACH(NVBDataInfo i, finfo)
				fi->append(NVBDataInfo(i.name,i.dimension,i.axes,i.comments.unite(c),i.type));
			delete finfo;
			}
		return fi;
		}

// Now there are axes
// To simplify and speed up loading we assume that the shape of the data is the same for all files

	QList<NVBAxisInfo> new_axes;
	foreach(QString axis, axes) {
		QStringList aprop = axis.split(' ');
		if (aprop.first().at(0) == '\"') {
			QString aname = aprop.takeFirst();
			do {
				if (aprop.isEmpty()) {
					NVBOutputError(QString("Bad axis name format in \'%1\'").arg(axis));
					return 0;
					}
				aname += " " + aprop.takeFirst();
				} while (aname.at(aname.length()-1) != '\"');
			aprop.prepend(aname.mid(1,aname.length()-2));
			}
		switch(aprop.length()) {
			case 2: {
				new_axes << NVBAxisInfo(aprop.at(0),aprop.at(1).toInt());
				break;
				}
			case 3: {
				new_axes << NVBAxisInfo(aprop.at(0),aprop.at(1).toInt()); // FIXME axis units not empty
				break;
				}
			case 5: {
				new_axes << NVBAxisInfo(aprop.at(0),aprop.at(1).toInt(),aprop.at(4));
				break;
				}
			default: {
				NVBOutputError(QString("Bad axis name format in \'%1\'").arg(axis));
				return 0;
				break;
				}
			}
		}
	
	NVBFileInfo * finfo = fileFactory->getFileInfo(info.last());
	
	if (!finfo) {
		NVBOutputError("Couldn't load file info for component files");
		return 0;
		}
	
	NVBFileInfo * fi = new NVBFileInfo(info);
	
	fi->filterAddComments(finfo->comments);
	
	NVB_FOREACH(NVBDataInfo i, finfo)
		fi->append(NVBDataInfo(i.name,i.dimension,QList<NVBAxisInfo>() << i.axes << new_axes,i.comments,i.type));
	delete finfo;
	
	return fi;
}
