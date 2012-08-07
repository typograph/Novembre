#include "testGenerator.h"

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QFileSystemModel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtGui/QFont>
#include <QtGui/QListView>
#include <QtGui/QCompleter>

#include "../NVBFileGenerator.h"
#include "../NVBFile.h"
#include "../NVBDataSourceModel.h"
#include "../NVBforeach.h"
#include "../NVBSingleView.h"

int main(int argc, char** argv) {

	NVBTestGenApplication app(argc,argv);
	
	QWidget * mn;
	QVBoxLayout * l;
	QHBoxLayout * lh;
	
	app.setActiveWindow(mn = new QWidget());
	mn->setLayout(lh = new QHBoxLayout());
	l = new QVBoxLayout();
	lh->addLayout(l);
	l->addWidget(app.files = new QLineEdit(mn));
	QFileSystemModel * model = new QFileSystemModel(app.files);
	model->setFilter(QDir::AllEntries | QDir::AllDirs | QDir::NoDotAndDotDot);
//	model->setNameFilters(QString("*.sm3").split(" "));
	model->setReadOnly(true);
	model->setResolveSymlinks(true);
	model->setRootPath("/");

	QCompleter * cmpl = new QCompleter(model,app.files);
	app.files->setCompleter(cmpl);
	cmpl->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

	app.connect(app.files,SIGNAL(editingFinished()),&app,SLOT(openFile()));

	l->addWidget(app.tree = new QTreeWidget(mn));
	app.tree->setColumnCount(1);
	
	l->addWidget(app.view = new QListView(mn));
	app.view->setViewMode(QListView::IconMode);
	app.view->setFlow(QListView::LeftToRight);
	app.view->setResizeMode(QListView::Adjust);
	app.view->setGridSize(QSize(120,140));
	app.view->setIconSize(QSize(100,100));

	lh->addWidget(app.pview = new NVBSingleView(0,mn));

//	app.view->hide();

	mn->show();

	if (argc > 1) {
		app.files->setText(argv[1]);
		app.openFile(argv[1]);
		}
	
	return app.exec();
	
}

NVBTestGenApplication::NVBTestGenApplication( int & argc, char ** argv )
: NVBCoreApplication(argc,argv)
{
}

NVBTestGenApplication::~ NVBTestGenApplication()
{
}

void NVBTestGenApplication::showPage(const QModelIndex & index) {
	if (index.isValid()) {
		NVBDataSourceListModel * m = qobject_cast<NVBDataSourceListModel*>(view->model());
		if (m) {
			pview->setDataSet(m->dataSetAt(index));
			return;
			}
		else
			NVBOutputError("Couldn't convert model");
		}
	pview->setDataSet(0);
}

void NVBTestGenApplication::openFile(QString name) {

	tree->clear();

	if (name.isEmpty())
		name = files->text();

	if (QDir(name).exists()) {
		openFolder(name);
		return;
		}

	TESTGENERATOR * generator = new TESTGENERATOR();
	
	if (!generator) return;

	NVBAssociatedFilesInfo inf = generator->associatedFiles(name.isEmpty() ? files->text() : name);
	
	NVBFileInfo * fi = inf.loadFileInfo();
	if (!fi) { delete generator; return; }

	tree->setHeaderLabel(fi->files.name());

	addFileInfoToTree(tree->invisibleRootItem(),fi);
	delete fi;
	
	NVBFile * fl = generator->loadFile(inf);
	if (!fl) { delete generator; return; }

	addFileToTree(tree->invisibleRootItem(),fl);

	QAbstractItemModel * m = view->model();

	view->setModel(new NVBDataSourceListModel(fl));
	view->connect(view->selectionModel(),SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),this,SLOT(showPage(const QModelIndex&)));
	

	if (m) delete m;
	delete generator;

}

void NVBTestGenApplication::openFolder(QString dirname)
{
	qDebug() << "open dir " << dirname;
	tree->clear();
	QAbstractItemModel * m = view->model();
	view->setModel(0);	
	if (m) delete m;

	if (dirname.isEmpty()) { qDebug() << "no name : empty"; return; }

	TESTGENERATOR * generator = new TESTGENERATOR();

	if (!generator) { qDebug() << "memalloc failed"; return; }
	
	QDir dir(dirname,generator->extFilters().join(";"));
	if (!dir.exists()) { qDebug() << "no folder"; delete generator; return; }
	
	QStringList fnames = dir.entryList();
	
	while(!fnames.isEmpty()) {
		qDebug() << fnames.first();
	
		NVBAssociatedFilesInfo inf = generator->associatedFiles(dir.absoluteFilePath(fnames.takeFirst()));

		foreach(QString fname, inf)
			fnames.removeOne(dir.relativeFilePath(fname));

		NVBFileInfo * fi = inf.loadFileInfo();
		if (!fi) continue;

		QTreeWidgetItem * fitem;
		fitem = new QTreeWidgetItem(tree);
		fitem->setText(0,fi->files.name());
		
		addFileInfoToTree(fitem,fi);
		delete fi;
		
		NVBFile * fl = generator->loadFile(inf);
		if (!fl) continue;

		addFileToTree(fitem,fl);
		delete fl;
	}
	
	delete generator;
}

void NVBTestGenApplication::addFileInfoToTree(QTreeWidgetItem* parent, NVBFileInfo* info)
{
	QFont font(QApplication::font());
	font.setBold(true);

	QTreeWidgetItem * item;
	item = new QTreeWidgetItem(parent,QStringList("Info"));
	item->setFont(0,font);
	QTreeWidgetItem * cmnt = new QTreeWidgetItem(item,QStringList(QString("Comments")));
	NVBDataComments cmnts = info->getAllComments();
	foreach(QString key, cmnts.keys())
		new QTreeWidgetItem(cmnt,QStringList(QString("%1 : %2").arg(key,cmnts.value(key).toString())));
	
//	tree->addTopLevelItem(item);
	NVB_FOREACH(NVBDataInfo i, info) {
		QTreeWidgetItem * di = new QTreeWidgetItem(item,QStringList(QString("%1 [%2]").arg(i.name,i.dimension.baseUnit())));
		foreach(NVBAxisInfo ai, i.axes)
			new QTreeWidgetItem(di,QStringList(QString("%1 [%2] x%3").arg(ai.name).arg(ai.units.toStr()).arg(ai.length)));
		cmnt = new QTreeWidgetItem(di,QStringList(QString("Comments")));
		foreach(QString key, i.comments.keys())
			new QTreeWidgetItem(cmnt,QStringList(QString("%1 : %2").arg(key,i.comments.value(key).toString())));		
		}
//	l->setText(l->text() + "\n" + fi->files.name());
//	l->setText(l->text() + "\n" + fi->files.name());

}

void NVBTestGenApplication::addFileToTree(QTreeWidgetItem* parent, NVBFile* file)
{
	QFont font(QApplication::font());
	font.setBold(true);

	QTreeWidgetItem * item;
	
	item = new QTreeWidgetItem(parent,QStringList("File"));
	item->setFont(0,font);
//	tree->addTopLevelItem(item);

	new QTreeWidgetItem(item,QStringList(file->name()));
//	new QTreeWidgetItem(item,QString::number(fl->count()));

	QTreeWidgetItem * cmnt = new QTreeWidgetItem(item,QStringList(QString("Comments")));
	NVBDataComments cmnts = file->getAllComments();
	foreach(QString key, cmnts.keys())
		new QTreeWidgetItem(cmnt,QStringList(QString("%1 : %2").arg(key,cmnts.value(key).toString())));
	
	for(int i=0; i < file->count(); i++) {
		const NVBDataSource * src = file->at(i);
		QTreeWidgetItem * dsi = new QTreeWidgetItem(item,QStringList(QString("Source")));

		QTreeWidgetItem * dsa = new QTreeWidgetItem(dsi,QStringList(QString("Axes")));
		foreach(const NVBAxis & a, src->axes())
			new QTreeWidgetItem(dsa,QStringList(QString("%1 *%2").arg(a.name()).arg(a.length())));

		dsa = new QTreeWidgetItem(dsi,QStringList(QString("Data")));
		foreach(NVBDataSet * ds, src->dataSets()) {
			cmnt = new QTreeWidgetItem(dsa,QStringList(QString("%1 [%2]").arg(ds->name(),ds->dimension().baseUnit())));
			cmnts = ds->getAllComments();
			foreach(QString key, cmnts.keys())
				new QTreeWidgetItem(cmnt,QStringList(QString("%1 : %2").arg(key,cmnts.value(key).toString())));
			
			}

		cmnt = new QTreeWidgetItem(dsi,QStringList(QString("Comments")));
		cmnts = src->getAllComments();
		foreach(QString key, cmnts.keys())
			new QTreeWidgetItem(cmnt,QStringList(QString("%1 : %2").arg(key,cmnts.value(key).toString())));
		
	}

}
