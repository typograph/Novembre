#include "NVBQuadTree.h"

#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtCore/QVariant>

//#include <math.h>

TreeQuad::TreeQuad(QRectF rect): pts_calculated(false), r(rect), ds(QVariantList()) {
	for(int i = 0; i < 4; i++)
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
	return insert(point,QVariantList() << data);
	}

TreeQuad* TreeQuad::insert(QPointF point, QVariantList data) {
	if (hasChildren()) { // delegate to children
		TreeQuad * q = children[p2q(r,point)]->insert(point,data);
		pts_calculated = false;
		return q;
		}
	if (ds.isEmpty() || p == point) {
		ds.append(data);
		p = point;
		if (pts_calculated)
			insidepts << NVBQuadTree::PointData(point,ds);
		return this;
		}
// Now create children, and push both points;
	children[0] = new TreeQuad(QRectF(r.topLeft(),r.center()));
	children[1] = new TreeQuad(QRectF(r.center().x(),r.top(),r.width()/2,r.height()/2));
	children[2] = new TreeQuad(QRectF(r.left(),r.center().y(),r.width()/2,r.height()/2));
	children[3] = new TreeQuad(QRectF(r.center(),r.bottomRight()));
	children[p2q(r,p)]->insert(p,ds);
	TreeQuad * q = children[p2q(r,point)]->insert(point,data);
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

QVariantList TreeQuad::pointsInRect(const QRectF & rect) const {
			if (!r.intersects(rect)) return QVariantList();
			if (hasChildren())
					return QVariantList() << children[0]->pointsInRect(rect) << children[1]->pointsInRect(rect) << children[2]->pointsInRect(rect) << children[3]->pointsInRect(rect);
			else if (rect.contains(p))
		return ds;
			return QVariantList();
	}

QVariantList TreeQuad::pointsInCircle(const QRectF & rect) const {
	if (!r.intersects(rect)) return QVariantList();
	if (hasChildren())
		return QVariantList() << children[0]->pointsInCircle(rect) << children[1]->pointsInCircle(rect) << children[2]->pointsInCircle(rect) << children[3]->pointsInCircle(rect);
	else if (ds.isEmpty())
		return QVariantList();
	else {
		QPointF d = rect.center()-p;
		qreal rd = 4*d.x()*d.x()/rect.width()/rect.width() + 4*d.y()*d.y()/rect.height()/rect.height();
		if (rd <= 1)
			return ds;
		else
			return QVariantList();
		}
	}

QList<NVBQuadTree::PointData> TreeQuad::points() const {
	if (!pts_calculated) recalcinsidepts();
	return insidepts;
	}

void TreeQuad::recalcinsidepts() const {
	insidepts.clear();
	if (hasChildren())
		insidepts << children[0]->points() << children[1]->points() << children[2]->points() << children[3]->points();
	else if (!ds.isEmpty())
		insidepts << NVBQuadTree::PointData(p,ds);
	pts_calculated = true;
	}

//----------------

NVBQuadTree::~NVBQuadTree() { if (root) delete root; }

void NVBQuadTree::setRect(QRectF r) {
	if (root) delete root;
	root = new TreeQuad(r);
	}

void NVBQuadTree::insert(const QPointF & p, QVariant data) {
	if (!root) return;
	root->insert(p,data);
	}

QVariantList NVBQuadTree::pointsInRect(QRectF rect) const {
	if (!root) return QVariantList();
	return root->pointsInRect(rect);
	}

QVariantList NVBQuadTree::pointsInCircle(QRectF rect) const {
	if (!root) return QVariantList();
	return root->pointsInCircle(rect);
	}

QList<NVBQuadTree::PointData> NVBQuadTree::points() const {
	if (!root) return QList<QPair<QPointF,QVariantList> >();
	return root->points();
	}
