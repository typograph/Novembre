//
// C++ Interface: QCollapsibleBox
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QCOLLAPSINGBOX_H
#define QCOLLAPSINGBOX_H

#include <QString>
#include <QWidget>
#include <QMouseEvent>
#include <QResizeEvent>
// #include <QGroupBox>
#include <QStyleOptionGroupBox>

/*class QStyleOptionCollapsibleBox : public QStyleOptionGroupBox {
public:
  bool 

  QPixmap indicator_expanded;
  QPixmap indicator_collapsed;


  QStyleOptionCollapsibleBox();
  QStyleOptionCollapsibleBox(const QStyleOptionCollapsibleBox &other) : QStyleOptionGroupBox() { *this = other; }
protected:
  QStyleOptionCollapsibleBox(int version):QStyleOptionGroupBox(version) {;}
};
*/
class QCollapsibleBox : public QWidget {
Q_OBJECT
public:
  enum IndicatorPosition { Left, Right, LeftWithText, RightWithText };
  enum HeaderStyle { None, Flat, Horizontal, Button };
private:
  QWidget * cwidget;
  int marg_l,marg_r,marg_t,marg_b;
  int imarg_l,imarg_r,imarg_t,imarg_b;
  int marg_f;

  QString t_text;
  Qt::Alignment t_alignment;

  IndicatorPosition ind_at;
  HeaderStyle h_style;
  bool checked;
  bool flat;
  bool owner;

  int pixsize;

  QPixmap * expanded;
  QPixmap * collapsed;

  int sqsize() const;
  QRect titleRect() const;
  QRect widgetRect() const;
  QRect centralRect() const;

  QSize sizeHintFromWidgetSize(QSize size, bool statesensitive = true) const;

public :
  QCollapsibleBox(QString title, QWidget * parent = 0);
  virtual ~QCollapsibleBox();

  void setCentralWidget(QWidget * w);

  inline bool isFlat() const { return flat;}
  void setFlat(bool flat);

  inline bool isExclusiveOwner() { return owner; }
  inline void setExclusiveOwner(bool exclusiveOwner) { owner = exclusiveOwner; }

  inline IndicatorPosition indicatorPosition() const { return ind_at; }
  void setIndicatorPosition(IndicatorPosition pos);

  inline HeaderStyle headerStyle() const { return h_style;}
  void setHeaderStyle(HeaderStyle style);

  inline QString title() const { return t_text;}
  void setTitle(const QString &title);

  inline Qt::Alignment alignment() const { return t_alignment;}
  void setAlignment(Qt::Alignment alignment);

  inline bool isExpanded() const { return checked;}

  void setMargins(int ml, int mt, int mr, int mb);

  void setExpandedStatePixmap(QPixmap * pixmap);
  void setCollapsedStatePixmap(QPixmap * pixmap);

  void setArrowIndicators();
  void setPlusMinusIndicators();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

  virtual int heightForWidth ( int w ) const;

protected:
  virtual void paintEvent(QPaintEvent * event);
//   void drawTitle();

  void resizeEvent(QResizeEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);  

public slots:
  void collapse(bool = true);
  void expand(bool = true);

signals:
  void clicked(bool checked = false);
  void toggled(bool);
};

#endif
