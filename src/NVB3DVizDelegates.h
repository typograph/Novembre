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
#ifndef NVB3DVIZDELEGATES_H
#define NVB3DVIZDELEGATES_H

#include "NVBDataSource.h"
#include <qwt3d_surfaceplot.h>

class NVB3DSurfaceVizDelegate : public Qwt3D::SurfacePlot {
		Q_OBJECT

	private:
		NVB3DDataSource * tttprovider;

	private slots:
		void colorsChanged() { updateData(); updateGL(); }
		void dataChanged();
		void setSource(NVBDataSource * source);

	public:
		NVB3DSurfaceVizDelegate(NVBDataSource * source);
		virtual ~NVB3DSurfaceVizDelegate() {;}

	};

#endif
