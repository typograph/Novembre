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
#include "NVB3DPageView.h"
#include <QIcon>
#include <QFileDialog>
#include <QImageWriter>
#include <QToolBar>

#include "../../icons/icons_3Dview.xpm"

NVB3DPageView::NVB3DPageView(NVBVizModel* model, QWidget * parent): QStackedWidget(parent), vizmodel(model) {

	if (vizmodel->rowCount() > 1)
		rowsInserted(QModelIndex(), 1, vizmodel->rowCount() - 1);

	connect(vizmodel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), SLOT(rowsRemoved(const QModelIndex&, int, int)));
//   connect(vizmodel,SIGNAL(rowsAboutToBeRemoved(const QModelIndex&,int,int)),SLOT(rowsAboutToBeRemoved(const QModelIndex&,int,int)));
	connect(vizmodel, SIGNAL(rowsInserted(const QModelIndex&, int, int)), SLOT(rowsInserted(const QModelIndex&, int, int)));
	connect(vizmodel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), SLOT(updateVizs(const QModelIndex&, const QModelIndex&)));


	}

NVB3DPageView::~ NVB3DPageView() {
	}

QToolBar * NVB3DPageView::generateToolbar(QWidget * parent) const {
	QToolBar * tBar = new QToolBar(parent);
	tBar->addAction(QIcon(_3Dview_export), "Export view", this, SLOT(exportImage()));
	return tBar;
	}

void NVB3DPageView::select(const QModelIndex & index) {
	if (index.isValid())
		setCurrentIndex(index.row());
	}

void NVB3DPageView::exportImage() {
	QStringList sl;
	foreach(QByteArray b, QImageWriter::supportedImageFormats()) {
		QString a(b);
		sl << a.toUpper() + " files (*." + a + ")";
		};

	QString filename = QFileDialog::getSaveFileName(0, "Export image", QString(), sl.join(";;"));

	if (filename.isEmpty()) return;

	currentPlot()->renderPixmap(1024, 1024).save(filename);

	}

Qwt3D::Plot3D * NVB3DPageView::currentPlot() {
	return qobject_cast<Qwt3D::Plot3D*>(currentWidget());
	}

void NVB3DPageView::rowsInserted(const QModelIndex & parent, int start, int end) {
	Q_UNUSED(parent);

	for (int i = start; i <= end; i++) {
		NVBVizUnion tmp = vizmodel->index(i).data(PageVizItemRole).value<NVBVizUnion>();

		if (tmp.valid && tmp.vtype == NVB::ThreeDView)
			insertWidget(i, tmp.ThreeDViz);
		else {
			NVBOutputError("Unsuitable viz type");
			insertWidget(i, new QWidget(this));
			}
		}
	}

void NVB3DPageView::rowsRemoved(const QModelIndex & parent, int start, int end) {
	Q_UNUSED(parent);

	for (int i = end; i >= start; i--)
		removeWidget(widget(i));
	}

void NVB3DPageView::updateVizs(const QModelIndex & start, const QModelIndex & end) {
	rowsRemoved(QModelIndex(), start.row(), end.row());
	rowsInserted(QModelIndex(), start.row(), end.row());
	}

bool NVB3DPageView::acceptsPage(NVBDataSource *) const {
	return true;
	}

