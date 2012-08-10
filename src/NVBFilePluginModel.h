#ifndef NVBFILEPLUGINMODEL_H
#define NVBFILEPLUGINMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QSignalMapper>

class QAction;
class NVBFileGenerator;

class NVBFilePluginModel : public QAbstractTableModel
{
Q_OBJECT
private:
	QList<NVBFileGenerator*> generators;
	QList<QAction*> gactions;
	QList<QString> gsource;
	QSignalMapper actMapper;
public:
	NVBFilePluginModel();
	~NVBFilePluginModel();

	virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
	virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
	virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

	bool addGenerator(NVBFileGenerator* generator, QString filename="Built-in");

	QList<NVBFileGenerator*> activeGenerators() const;
	const QList<NVBFileGenerator*> & availableGenerators() const { return generators; }

	const QList<QAction*> & actions() const { return gactions; }

	bool isGeneratorActive(NVBFileGenerator* generator);
	void setGeneratorActive(NVBFileGenerator* generator, bool active = true);

	bool isGeneratorActive(int index);
	void setGeneratorActive(int index, bool active = true);

public slots:
	void toggleGenerator(QObject* generator);
	void toggleGenerator(NVBFileGenerator* generator);
	void toggleGenerator(int index);

};

#endif // NVBFILEPLUGINMODEL_H
