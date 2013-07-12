// 
// Adapted for Qt from lib2geom by Timofey <typograph@elec.ru>
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

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtCore/QVariant>

class TreeQuad;

class NVBQuadTree {
	public:
		typedef QPair<QPointF, QVariantList> PointData;
		typedef QList<PointData> PointDataList;

		TreeQuad* root;

		NVBQuadTree() : root(0) {;}
		NVBQuadTree(QRectF r): root(0) { setRect(r); }
		~NVBQuadTree();

		QRectF rect() const;
		void setRect(QRectF r);

		void insert(const QPointF & p, QVariant data);
		void insert(const PointData & data);

		//	QVariant closestPoint(QPointF point);
		PointDataList pointsInRect(QRectF rect) const;
		PointDataList pointsInCircle(QRectF rect) const;

		PointDataList points() const;
	};

class TreeQuad {
	private:
		TreeQuad() {;}
		TreeQuad(const TreeQuad &) {;}

		mutable bool pts_calculated;
		mutable NVBQuadTree::PointDataList insidepts;
		void recalcinsidepts() const ;

	public:

		TreeQuad* children[4];

		QRectF r;

		QVariantList ds;
		QPointF p;

		TreeQuad(QRectF rect);
		~TreeQuad();

		bool hasChildren() const { return children[0] != 0; }
		bool isEmpty() const;

		TreeQuad* insert(QPointF point, QVariant data);
		TreeQuad* insert(QPointF point, QVariantList data);
		TreeQuad* insert(NVBQuadTree::PointData data);

		static unsigned char p2q(const QRectF & r, const QPointF & p);

		NVBQuadTree::PointDataList pointsInRect(const QRectF & rect) const;
		NVBQuadTree::PointDataList pointsInCircle(const QRectF & rect) const;

		NVBQuadTree::PointDataList points() const;

	};

