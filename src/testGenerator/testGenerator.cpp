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
#include <QtGui/QFont>
#include <QtGui/QListView>
#include <QtGui/QCompleter>

#include "../rhk.h"
#include "../NVBFile.h"
#include "../NVBDataSourceModel.h"

int main(int argc, char** argv) {

	NVBTestGenApplication app(argc,argv);
	
	QWidget * mn;
	QVBoxLayout * l;
	
	app.setActiveWindow(mn = new QWidget());
	mn->setLayout(l = new QVBoxLayout());
	l->addWidget(app.files = new QLineEdit(mn));
	QFileSystemModel * model = new QFileSystemModel(app.files);
	model->setFilter(QDir::AllEntries | QDir::AllDirs | QDir::NoDotAndDotDot);
	model->setNameFilters(QStringList(QString("*.sm3")));
	model->setReadOnly(true);
	model->setResolveSymlinks(true);
	model->setRootPath("/home/timoty/programming/Novembre/test_files/");

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

//	app.view->hide();

	mn->show();

	if (argc > 1) {
		app.files->setText(argv[1]);
		app.openFile(argv[1]);
		}
	
	return app.exec();
	
}

NVBTestGenApplication::NVBTestGenApplication( int & argc, char ** argv ):QApplication(argc,argv)
{

#ifdef NVB_ENABLE_LOG
// For threads, we have to do that
	qRegisterMetaType<NVB::LogEntryType>("NVB::LogEntryType");
	NVBLogger * l = new NVBLogger(this);
	setProperty("Logger",QVariant::fromValue(l));
 	connect(l,SIGNAL(message(NVB::LogEntryType, QString, QString, QTime)),this,SLOT(message(NVB::LogEntryType, QString, QString)));
#endif

}

bool NVBTestGenApplication::notify( QObject * receiver, QEvent * event )
{
	try {
		return QApplication::notify(receiver,event);
		}
	catch (int err) {
		NVBOutputError(QString("Uncaught error #%1").arg(err));
		return false;
		}
	catch (...) {
		NVBOutputError("Fatal error");
		return false;
		}
}

#ifdef NVB_ENABLE_LOG
void NVBTestGenApplication::message(NVB::LogEntryType type, QString issuer, QString text)
{
  qDebug() << issuer << "->" << text;
/*
	if (type == NVB::CriticalErrorEntry)
    QMessageBox::critical(0,issuer,text);
  else if (type == NVB::ErrorEntry)
    qDebug() << issuer << "->" << text;
  else if (type == NVB::DebugEntry)
    qDebug() << text;
*/
}
#endif

NVBTestGenApplication::~ NVBTestGenApplication()
{
#ifdef NVB_ENABLE_LOG
  delete property("Logger").value<NVBLogger*>();
#endif
}

void NVBTestGenApplication::openFile(QString name) {

	tree->clear();

	RHKFileGenerator gen;

	//"/home/timoty/programming/Novembre/test_files/rhk/data1850.sm3"
	NVBAssociatedFilesInfo inf = gen.associatedFiles(name.isEmpty() ? files->text() : name/*QFileDialog::getOpenFileName(l,"Select 
file to 
load")*/);
	

	NVBFileInfo * fi = inf.loadFileInfo();
	if (!fi) return;

	QFont font(QApplication::font());
	font.setBold(true);

	tree->setHeaderLabel(fi->files.name());
	
	QTreeWidgetItem * item;
	item = new QTreeWidgetItem(tree,QStringList("Info"));
	item->setFont(0,font);
	QTreeWidgetItem * cmnt = new QTreeWidgetItem(item,QStringList(QString("Comments")));
	NVBDataComments cmnts = fi->getAllComments();
	foreach(QString key, cmnts.keys())
		new QTreeWidgetItem(cmnt,QStringList(QString("%1 : %2").arg(key,cmnts.value(key).toString())));
	
//	tree->addTopLevelItem(item);
	foreach(NVBDataInfo i, *fi) {
		QTreeWidgetItem * di = new QTreeWidgetItem(item,QStringList(QString("%1 [%2]").arg(i.name,i.dimension.baseUnit())));
		foreach(axissize_t sz, i.sizes)
			new QTreeWidgetItem(di,QStringList(QString::number(sz)));
		cmnt = new QTreeWidgetItem(di,QStringList(QString("Comments")));
		foreach(QString key, i.comments.keys())
			new QTreeWidgetItem(cmnt,QStringList(QString("%1 : %2").arg(key,i.comments.value(key).toString())));		
		}
//	l->setText(l->text() + "\n" + fi->files.name());
//	l->setText(l->text() + "\n" + fi->files.name());
	
	NVBFile * fl = gen.loadFile(inf);
	
	item = new QTreeWidgetItem(tree,QStringList("File"));
	item->setFont(0,font);
//	tree->addTopLevelItem(item);

	new QTreeWidgetItem(item,QStringList(fl->name()));
//	new QTreeWidgetItem(item,QString::number(fl->count()));

	cmnt = new QTreeWidgetItem(item,QStringList(QString("Comments")));
	cmnts = fl->getAllComments();
	foreach(QString key, cmnts.keys())
		new QTreeWidgetItem(cmnt,QStringList(QString("%1 : %2").arg(key,cmnts.value(key).toString())));
	
	for(int i=0; i < fl->count(); i++) {
		const NVBDataSource * src = fl->at(i);
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

	QAbstractItemModel * m = view->model();

	view->setModel(new NVBDataSourceListModel(*fl));

	if (m) delete m;


}
