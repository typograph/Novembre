#include "testGenerator.h"

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QComboBox>
#include <QtGui/QDirModel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QFont>
#include <QtGui/QListView>

#include "../rhk.h"
#include "../NVBFile.h"
#include "../NVBDataSourceModel.h"

int main(int argc, char** argv) {

	NVBTestGenApplication app(argc,argv);
	
	QWidget * mn;
	QVBoxLayout * l;
	QComboBox * files;
	QTreeWidget * tree;
	
	app.setActiveWindow(mn = new QWidget());
	mn->setLayout(l = new QVBoxLayout());
	l->addWidget(files = new QComboBox(mn));
	files->setModel(new QDirModel(QStringList(QString("*.sm3")),QDir::AllEntries | QDir::Readable,QDir::Name | QDir::DirsFirst,files));
	files->setAutoCompletion(true);
	files->setEditable(true);
	l->addWidget(tree = new QTreeWidget(mn));
	tree->setColumnCount(1);
	
	RHKFileGenerator gen;
	//"/home/timoty/programming/Novembre/test_files/rhk/data1850.sm3"
	NVBAssociatedFilesInfo inf = gen.associatedFiles(argv[1]/*QFileDialog::getOpenFileName(l,"Select file to load")*/);
	
	NVBFileInfo * fi = gen.loadFileInfo(inf);
	QFont font(QApplication::font());
	font.setBold(true);

	tree->addTopLevelItem(new QTreeWidgetItem(tree,QStringList(fi->files.name())));
	
	QTreeWidgetItem * item;
	item = new QTreeWidgetItem(tree,QStringList("Info"));
	item->setFont(0,font);
//	tree->addTopLevelItem(item);
	foreach(NVBDataInfo i, *fi) {
		QTreeWidgetItem * di = new QTreeWidgetItem(item,QStringList(QString("%1 [%2]").arg(i.name,i.dimension.baseUnit())));
		foreach(axissize_t sz, i.sizes)
			new QTreeWidgetItem(di,QStringList(QString::number(sz)));
		}
//	l->setText(l->text() + "\n" + fi->files.name());
//	l->setText(l->text() + "\n" + fi->files.name());
	
	NVBFile * fl = gen.loadFile(inf);
	
	item = new QTreeWidgetItem(tree,QStringList("File"));
	item->setFont(0,font);
//	tree->addTopLevelItem(item);

	new QTreeWidgetItem(item,QStringList(fl->name()));
//	new QTreeWidgetItem(item,QString::number(fl->count()));

	for(int i=0; i < fl->count(); i++) {
		const NVBDataSource * src = fl->at(i);
		QTreeWidgetItem * dsi = new QTreeWidgetItem(item,QStringList(QString("Source")));
		QTreeWidgetItem * dsa = new QTreeWidgetItem(dsi,QStringList(QString("Axes")));
		foreach(const NVBAxis & a, src->axes())
			new QTreeWidgetItem(dsa,QStringList(QString("%1 *%2").arg(a.name()).arg(a.length())));
		dsa = new QTreeWidgetItem(dsi,QStringList(QString("Data")));
		foreach(NVBDataSet * ds, src->dataSets())
			new QTreeWidgetItem(dsa,QStringList(QString("%1 [%2]").arg(ds->name(),ds->dimension().baseUnit())));
	}

	QListView * view;
	l->addWidget(view = new QListView(mn));
	view->setModel(new NVBDataSourceModel(fl->first()));
	view->setViewMode(QListView::IconMode);
	view->setFlow(QListView::LeftToRight);
	view->setResizeMode(QListView::Adjust);
	view->setGridSize(QSize(120,140));
	view->setIconSize(QSize(100,100));


	mn->show();
	
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
  if (type == NVB::CriticalErrorEntry)
    QMessageBox::critical(0,issuer,text);
  else if (type == NVB::ErrorEntry)
    qDebug() << issuer << "->" << text;
  else if (type == NVB::DebugEntry)
    qDebug() << text;
}
#endif

NVBTestGenApplication::~ NVBTestGenApplication()
{
#ifdef NVB_ENABLE_LOG
  delete property("Logger").value<NVBLogger*>();
#endif
}
