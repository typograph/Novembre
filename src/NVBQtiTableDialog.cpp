#include "NVBQtiTableDialog.h"

#include <QGridLayout>
#include <QPalette>
#include <QLineEdit>
#include "NVBDiscrColorModel.h"
#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QScrollArea>

class NVBColorWidget : public QWidget {

private :
	QColor d_color;

public :
	NVBColorWidget(QColor color, QWidget *parent = 0):QWidget(parent) {
		setAutoFillBackground(true);
		setMinimumSize(10,10);
		setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
		setColor(color);
		}
	~NVBColorWidget() {;}

	QColor color() { return d_color; }
	void setColor(QColor color) {
		d_color = color;
		QPalette p = palette();
		p.setColor(QPalette::Window,color);
		setPalette(p);
		}
};


NVBQtiCurveNameWidget::NVBQtiCurveNameWidget(QWidget *parent):QWidget(parent)
{
	gl = new QGridLayout(this);
	connect(&mapper,SIGNAL(mapped(int)),this,SLOT(updateColorNameAt(int)));
}

void NVBQtiCurveNameWidget::setKnownColors(const QtiColorMap & map)
{
	colors = map;
	updateColorsFromList();
}

void NVBQtiCurveNameWidget::setAvailableColors(const QList<QColor> & list)
{
	asked = list;
	updateColorsFromList();
}

void NVBQtiCurveNameWidget::updateColorsFromList()
{
	uniasked.clear();
	foreach(QColor c, asked) {
		if (!colors.contains(c.rgba()))
			colors.insert(c.rgba(),c.name());
		if (!uniasked.contains(c.rgba()))
			uniasked.insert(c.rgba(),colors.value(c.rgba()));
		}

	updateUI();
}

void NVBQtiCurveNameWidget::updateUI()
{
	if ( layout()) {
			QLayoutItem* item;
			while ( ( item = layout()->takeAt(0) ) != 0 ) {
					delete item->widget();
					delete item;
				}
		}
	int i;
	for (i=0; i<uniasked.size(); i++) {
		gl->addWidget(new NVBColorWidget(QColor(uniasked.keys().at(i)),this),i,0);
		QLineEdit * le = new QLineEdit(uniasked.values().at(i),this);
		gl->addWidget(le,i,1);
		mapper.setMapping(le,i);
		connect(le,SIGNAL(editingFinished()),&mapper,SLOT(map()));
		}
}

void NVBQtiCurveNameWidget::updateColorNameAt(int i)
{
	QLineEdit * le = qobject_cast<QLineEdit*>(gl->itemAtPosition(i,1)->widget());
	if (!le) return;

	uniasked[uniasked.keys().at(i)] = le->text();
	colors[uniasked.keys().at(i)] = le->text();
}

NVBQtiTableDialog::NVBQtiTableDialog(QWidget *parent) :
		QDialog(parent)
{
		QVBoxLayout *verticalLayout;
		QHBoxLayout *horizontalLayout;

		QScrollArea *scrollArea;
		QDialogButtonBox *buttonBox;

		resize(272, 422);

		verticalLayout = new QVBoxLayout(this);

		// ---

		horizontalLayout = new QHBoxLayout();
		horizontalLayout->addWidget(new QLabel("Table name:",this));

		table_name_edit = new QLineEdit(this);
		horizontalLayout->addWidget(table_name_edit);

		verticalLayout->addLayout(horizontalLayout);

		// ---

		horizontalLayout = new QHBoxLayout();
		horizontalLayout->addWidget(new QLabel("Table comment:",this));

		table_comment_edit = new QLineEdit(this);
		horizontalLayout->addWidget(table_comment_edit);

		verticalLayout->addLayout(horizontalLayout);

		// ---

		horizontalLayout = new QHBoxLayout();
		horizontalLayout->addWidget(new QLabel("Column comments:",this));

		column_comments_combo = new QComboBox(this);
		column_comments_combo->insertItems(0, QStringList() << "No comments" << "Name" << "Units");
		column_comments_combo->setCurrentIndex(2);
		horizontalLayout->addWidget(column_comments_combo);

		verticalLayout->addLayout(horizontalLayout);

		// ---

		scrollArea = new QScrollArea(this);
		scrollArea->setWidgetResizable(true);
		color_table = new NVBQtiCurveNameWidget();
		color_table->setGeometry(QRect(0, 0, 250, 265));
		scrollArea->setWidget(color_table);

		verticalLayout->addWidget(scrollArea);

		// ---

		buttonBox = new QDialogButtonBox(this);
		buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
		connect(buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
		connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));

		verticalLayout->addWidget(buttonBox);

}

QString NVBQtiTableDialog::tableName() const
{
	return table_name_edit ? table_name_edit->text() : QString();
}

QString NVBQtiTableDialog::tableComment() const
{
	return table_comment_edit ? table_comment_edit->text() : QString();
}

int NVBQtiTableDialog::columnComment() const
{
	return column_comments_combo ? column_comments_combo->currentIndex() : 0;
}

void NVBQtiTableDialog::setValues(QString tname, QString tcomment, int ccomment)
{
	if (table_name_edit) table_name_edit->setText(tname);
	if (table_comment_edit)
		table_comment_edit->setText(tcomment);
	if (ccomment >= 0 && column_comments_combo)
		column_comments_combo->setCurrentIndex(ccomment);
}

