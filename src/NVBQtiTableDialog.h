#ifndef NVBQTITABLEDIALOG_H
#define NVBQTITABLEDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QMap>
#include <QColor>
#include <QSignalMapper>

class QGridLayout;

typedef QMap<QRgb,QString> QtiColorMap;

class NVBQtiCurveNameWidget : public QWidget
{
		Q_OBJECT

		QtiColorMap colors;
		QList<QColor> asked;
		QtiColorMap uniasked;
		QSignalMapper mapper;

		QGridLayout * gl;
private:
		void updateColorsFromList();
		void updateUI();

public:
		explicit NVBQtiCurveNameWidget(QWidget *parent = 0);
		virtual ~NVBQtiCurveNameWidget() {;}

		void setAvailableColors(const QList<QColor> & list);
		void setKnownColors(const QtiColorMap & map);
		QtiColorMap knownColors() const { return colors; }
		QString colorNameAt(int i) const { return colors.value(asked.at(i).rgba()); }

signals:

public slots:
		void updateColorNameAt(int i);

};

class QLineEdit;
class QComboBox;

class NVBQtiTableDialog : public QDialog
{
	Q_OBJECT
	QLineEdit *table_name_edit;
	QLineEdit *table_comment_edit;
	QComboBox *column_comments_combo;
	NVBQtiCurveNameWidget *color_table;
public:
	explicit NVBQtiTableDialog(QWidget *parent = 0);
	virtual ~NVBQtiTableDialog() {;}

	QString tableName() const;
	QString tableComment() const;

	int columnComment() const;

	void setValues(QString tname, QString tcomment = QString(), int ccomment = -1);

	NVBQtiCurveNameWidget * colorTable() const { return color_table; }

signals:

public slots:

};

#endif // NVBQTICURVENAMEDIALOG_H
