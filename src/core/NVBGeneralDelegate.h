//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef NVBGENERALDELEGATE_H
#define NVBGENERALDELEGATE_H

#include "NVBDataSource.h"
/*
#include "QDimension.h"
*/
#ifdef WITH_2DVIEW
#include <QGraphicsItem>
#endif
#ifdef WITH_GRAPHVIEW
#include <qwt_text.h>
#include <qwt_plot.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_item.h>
#include <qwt_plot_picker.h>
#endif
#ifdef WITH_3DVIEW
#include "../qwtplot3d/qwt3d_plot.h"
#endif
#include <QIcon>

namespace NVB {

enum ViewType {
  NoView = InvalidPage,
  DefaultTopoView = TopoPage,
  DefaultSpecView = SpecPage,
  ListView = '1',
#ifdef WITH_2DVIEW
  TwoDView = '2',
#endif
#ifdef WITH_3DVIEW
  ThreeDView = '3',
#endif
#ifdef WITH_GRAPHVIEW
  GraphView = 'g',
#endif
  IconView = 'i',
  DefaultView = 'd',
  AnyView = '*',
  AllWindows = '.'

               /*
                 NoView      = 0x0000,
                 TwoDView    = 0x0001,
                 ThreeDView  = 0x0002,
                 GraphView   = 0x0004,
                 IconView    = 0x0100,
                 AnyView     = TwoDView | ThreeDView | GraphView,

                 AllWindows  = 0xffff
               */

};

}

#ifdef WITH_2DVIEW
class NVBGraphicsItem : public QGraphicsItem {
	public:
		NVBGraphicsItem(QGraphicsItem * parent = 0): QGraphicsItem(parent) { setFlag(QGraphicsItem::ItemIsSelectable); }
		virtual ~NVBGraphicsItem() {;}
	protected :
		virtual bool sceneEvent(QEvent * event) {
			if (isSelected())
				return QGraphicsItem::sceneEvent(event);
			else
				return false;
			}
	};
#endif

#ifdef WITH_GRAPHVIEW
class NVBPlotPicker : public QwtPlotPicker {
	public:
		NVBPlotPicker(QwtPlotCanvas * canvas): QwtPlotPicker(canvas) {;}
		NVBPlotPicker(int xAxis, int yAxis, QwtPlotCanvas *canvas): QwtPlotPicker(xAxis, yAxis, canvas) {;}
		NVBPlotPicker(int xAxis, int yAxis, int selectionFlags,  QwtPicker::RubberBand rubberBand,  QwtPicker::DisplayMode trackerMode, QwtPlotCanvas * canvas): QwtPlotPicker(xAxis, yAxis, selectionFlags, rubberBand, trackerMode, canvas) {;}
		virtual QwtText trackerText(const QwtDoublePoint & pos) const {
			if (const QwtPlot * p = plot()) {
				switch (rubberBand()) {
					case HLineRubberBand:
						return p->axisScaleDraw(yAxis())->label(pos.y());

					case VLineRubberBand:
						return p->axisScaleDraw(xAxis())->label(pos.x());

					default:
						return QwtText(QString("%1, %2").arg(p->axisScaleDraw(xAxis())->label(pos.x()).text(), p->axisScaleDraw(yAxis())->label(pos.y()).text()));
					}
				}

			return QwtText();
			}
	};
#endif

struct NVBVizUnion {

	NVB::PageType ptype;
	NVB::ViewType vtype;
	union {
		int valid;
		void * p;
#ifdef WITH_2DVIEW
		QGraphicsItem* TwoDViz;
#endif
#ifdef WITH_GRAPHVIEW
		QwtPlotItem * GraphViz;
		QwtPlotPicker * GraphInteractiveViz;
#endif
		QIcon * IconViz;
#ifdef WITH_3DVIEW
		Qwt3D::Plot3D * ThreeDViz;
#endif
		};
	QObject * filter;

	NVBVizUnion(): ptype(NVB::InvalidPage), vtype(NVB::NoView), p(0), filter(0) {;}
#ifdef WITH_3DVIEW
	NVBVizUnion(NVB::PageType _ptype, Qwt3D::Plot3D * item): ptype(_ptype), vtype(NVB::ThreeDView), ThreeDViz(item), filter(0) {;}
#endif
#ifdef WITH_2DVIEW
	NVBVizUnion(NVB::PageType _ptype, QGraphicsItem* item): ptype(_ptype), vtype(NVB::TwoDView), TwoDViz(item), filter(0) {;}
#endif
#ifdef WITH_GRAPHVIEW
	NVBVizUnion(NVB::PageType _ptype, QwtPlotItem * item): ptype(_ptype), vtype(NVB::GraphView), GraphViz(item), filter(0) {;}
#endif
	NVBVizUnion(NVB::PageType _ptype, QIcon * item): ptype(_ptype), vtype(NVB::IconView), IconViz(item), filter(0) {;}

	inline bool isValid() {
		return valid || filter;
		}

	void clear() {
		if (valid) {
			switch (vtype) {
#ifdef WITH_2DVIEW

				case NVB::TwoDView : {
					delete TwoDViz;
					break;
					}

#endif
#ifdef WITH_3DVIEW

				case NVB::ThreeDView : {
					delete ThreeDViz;
					break;
					}

#endif
#ifdef WITH_GRAPHVIEW

				case NVB::GraphView : {
					delete GraphViz;
					break;
					}

#endif

				case NVB::IconView : {
					delete IconViz;
					break;
					}

				default : {
					//        delete u.p;
					NVBOutputError(QString("Unknown viz type %1").arg(vtype));
					break;
					}
				}
			}

		valid = false;
		vtype = NVB::NoView;
		ptype = NVB::InvalidPage;

		if (filter) {
			delete filter;
			filter = 0;
			}
		}
	};

Q_DECLARE_METATYPE(NVBVizUnion);
#ifdef WITH_GRAPHVIEW
Q_DECLARE_METATYPE(QwtPlotItem*);
#endif

#endif
