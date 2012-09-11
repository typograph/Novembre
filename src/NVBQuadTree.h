/**
 * Adapted for Qt from lib2geom
 *
 * This library is free software; you can redistribute it and/or
 * modify it either under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * (the "LGPL") or, at your option, under the terms of the Mozilla
 * Public License Version 1.1 (the "MPL"). If you do not alter this
 * notice, a recipient may use your version of this file under either
 * the MPL or the LGPL.
 *
 * You should have received a copy of the LGPL along with this library
 * in the file COPYING-LGPL-2.1; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * You should have received a copy of the MPL along with this library
 * in the file COPYING-MPL-1.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY
 * OF ANY KIND, either express or implied. See the LGPL or the MPL for
 * the specific language governing rights and limitations.
 *
 */

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtCore/QVariant>

class TreeQuad;

class NVBQuadTree {
public:
	typedef QPair<QPointF,QVariantList> PointData;
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
	TreeQuad(const TreeQuad & ) {;}

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

