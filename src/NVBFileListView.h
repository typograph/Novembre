#ifndef NVBFILELISTVIEW_H
#define NVBFILELISTVIEW_H

#include <QtGui/QTreeView>
#include <QtGui/QHeaderView>

class NVBFileListView : public QTreeView {
Q_OBJECT
public:
	NVBFileListView(QWidget * parent = 0);
	virtual ~NVBFileListView() {;}
protected:
	virtual void mousePressEvent(QMouseEvent *event);
signals:
	void rightPressed(const QModelIndex &);
};

class NVBFileListHeaderView : public QHeaderView {
Q_OBJECT
public:
	NVBFileListHeaderView(Qt::Orientation orientation, QWidget * parent = 0):QHeaderView(orientation,parent) {;}
	virtual ~NVBFileListHeaderView() {;}
protected:
	virtual void mousePressEvent(QMouseEvent *event);
signals:
	void sectionRightPressed(int);
};

#endif // NVBFILELISTVIEW_H
