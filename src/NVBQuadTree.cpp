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

#include "NVBQuadTree.h"

#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtCore/QVariant>

//#include <math.h>

TreeQuad::TreeQuad(QRectF rect): pts_calculated(false), r(rect), ds(QVariantList()) {
	for (int i = 0; i < 4; i++)
		children[i] = 0;
	}

TreeQuad::~TreeQuad() {
	if (hasChildren()) {
		delete children[0];
		delete children[1];
		delete children[2];
		delete children[3];
		}
	}

bool TreeQuad::isEmpty() const {
	if (hasChildren())
		return children[0]->isEmpty() && children[1]->isEmpty() && children[2]->isEmpty() && children[3]->isEmpty();
	else
		return ds.isEmpty();
	}

TreeQuad* TreeQuad::insert(QPointF point, QVariant data) {
	return insert(point, QVariantList() << data);
	}

TreeQuad* TreeQuad::insert(NVBQuadTree::PointData data) {
	return insert(data.first, data.second);
	}

TreeQuad* TreeQuad::insert(QPointF point, QVariantList data) {
	if (hasChildren()) { // delegate to children
		TreeQuad * q = children[p2q(r, point)]->insert(point, data);
		pts_calculated = false;
		return q;
		}

	if (ds.isEmpty() || p == point) {
		ds.append(data);
		p = point;

		if (pts_calculated)
			insidepts << NVBQuadTree::PointData(point, ds);

		return this;
		}

// Now create children, and push both points;
	children[0] = new TreeQuad(QRectF(r.topLeft(), r.center()));
	children[1] = new TreeQuad(QRectF(r.center().x(), r.top(), r.width() / 2, r.height() / 2));
	children[2] = new TreeQuad(QRectF(r.left(), r.center().y(), r.width() / 2, r.height() / 2));
	children[3] = new TreeQuad(QRectF(r.center(), r.bottomRight()));
	children[p2q(r, p)]->insert(p, ds);
	TreeQuad * q = children[p2q(r, point)]->insert(point, data);
	pts_calculated = false;
	return q;
	}

unsigned char TreeQuad::p2q(const QRectF & r, const QPointF & p) {
	char i = 0;
	QPointF c = r.center();

	if (p.x() > c.x()) i += 1;

	if (p.y() > c.y()) i += 2;

	return i;
	}

NVBQuadTree::PointDataList TreeQuad::pointsInRect(const QRectF & rect) const {
	NVBQuadTree::PointDataList res;

	if (hasChildren()) {
		bool nw = true;
		bool ne = true;
		bool sw = true;
		bool se = true;

		QPointF c = r.center();

		if (rect.bottom() <= c.y()) { sw = false; se = false; }

		if (rect.top() >= c.y()) { nw = false; ne = false; }

		if (rect.left() >= c.x()) { nw = false; sw = false; }

		if (rect.right() <= c.x()) { ne = false; se = false; }

		if (nw) res << children[0]->pointsInRect(rect);

		if (ne) res << children[1]->pointsInRect(rect);

		if (sw) res << children[2]->pointsInRect(rect);

		if (se) res << children[3]->pointsInRect(rect);

		return res;
		}
	else if (rect.contains(p))
		res << NVBQuadTree::PointData(p, ds);

	return res;
	}

NVBQuadTree::PointDataList TreeQuad::pointsInCircle(const QRectF & rect) const {
	NVBQuadTree::PointDataList res;

	if (hasChildren()) {
		bool nw = true;
		bool ne = true;
		bool sw = true;
		bool se = true;

		QPointF c = r.center();

		if (rect.bottom() <= c.y()) { sw = false; se = false; }

		if (rect.top() >= c.y()) { nw = false; ne = false; }

		if (rect.left() >= c.x()) { nw = false; sw = false; }

		if (rect.right() <= c.x()) { ne = false; se = false; }

		if (nw) res << children[0]->pointsInCircle(rect);

		if (ne) res << children[1]->pointsInCircle(rect);

		if (sw) res << children[2]->pointsInCircle(rect);

		if (se) res << children[3]->pointsInCircle(rect);

		return res;
		}
	else if (!ds.isEmpty()) {
		QPointF d = rect.center() - p;
		qreal rd = 4 * d.x() * d.x() / rect.width() / rect.width() + 4 * d.y() * d.y() / rect.height() / rect.height();

		if (rd <= 1)
			res << NVBQuadTree::PointData(p, ds);
		}

	return res;
	}

NVBQuadTree::PointDataList TreeQuad::points() const {
	if (!pts_calculated) recalcinsidepts();

	return insidepts;
	}

void TreeQuad::recalcinsidepts() const {
	insidepts.clear();

	if (hasChildren())
		insidepts << children[0]->points() << children[1]->points() << children[2]->points() << children[3]->points();
	else if (!ds.isEmpty())
		insidepts << NVBQuadTree::PointData(p, ds);

	pts_calculated = true;
	}

//----------------

NVBQuadTree::~NVBQuadTree() { if (root) delete root; }

void NVBQuadTree::setRect(QRectF r) {
	if (root) delete root;

	root = new TreeQuad(r);
	}

QRectF NVBQuadTree::rect() const {
	if (!root) return QRectF();

	return root->r;
	}

void NVBQuadTree::insert(const QPointF & p, QVariant data) {
	if (!root) return;

	root->insert(p, data);
	}

NVBQuadTree::PointDataList NVBQuadTree::pointsInRect(QRectF rect) const {
	if (!root) return NVBQuadTree::PointDataList();

	return root->pointsInRect(rect);
	}

NVBQuadTree::PointDataList NVBQuadTree::pointsInCircle(QRectF rect) const {
	if (!root) return NVBQuadTree::PointDataList();

	return root->pointsInCircle(rect);
	}

NVBQuadTree::PointDataList NVBQuadTree::points() const {
	if (!root) return NVBQuadTree::PointDataList();

	return root->points();
	}
