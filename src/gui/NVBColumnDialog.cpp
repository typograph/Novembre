//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
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

#include "NVBColumnDialog.h"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QStylePainter>
#include <QtGui/QSpinBox>

#include <QtGui/QApplication>
#include "NVBFileFactory.h"

using namespace NVBTokens;

NVBColumnInputWidget::NVBColumnInputWidget(NVBTokenList l, QWidget * parent): QWidget(parent) {
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//   setMinimumSize(QSize(363, 163));

	QVBoxLayout * mainLayout = new QVBoxLayout(this);

	QButtonGroup * types = new QButtonGroup(this);

	QGridLayout * gridLayout = new QGridLayout();
	fileOption = new QRadioButton("File parameter", this);
	types->addButton(fileOption);
	connect(fileOption, SIGNAL(clicked(bool)), this, SIGNAL(stateChanged()));
//   fileOption->setChecked(true);
	gridLayout->addWidget(fileOption, 0, 0, 1, 1);

	//
	// The main reason why the choices are added manually and not iterating over a map
	// is because this way the order is predictable, and local to NVBColumnInputWidget
	//

	fileChoice = new QComboBox(this);
	fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::FileName), NVBFileParamToken::FileName);
	fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::NPages), NVBFileParamToken::NPages);
	fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::FileSize), NVBFileParamToken::FileSize);
	fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::FileCTime), NVBFileParamToken::FileCTime);
	fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::FileMTime), NVBFileParamToken::FileMTime);
	fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::FileATime), NVBFileParamToken::FileATime);
//  NVBFileParamToken:: fileChoice->addItem(NVBTokenList::nameFileParam(),QVariant());

	fileChoice->setCurrentIndex(0);
	gridLayout->addWidget(fileChoice, 0, 1, 1, 1);
	connect(fileChoice, SIGNAL(currentIndexChanged(int)), SLOT(switchToFile()));

	dataOption = new QRadioButton("Data parameter", this);
	types->addButton(dataOption);
	connect(dataOption, SIGNAL(clicked(bool)), this, SIGNAL(stateChanged()));
	gridLayout->addWidget(dataOption, 1, 0, 1, 1);

	dataChoice = new QComboBox(this);
	dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::Name), NVBDataParamToken::Name);
	dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::Units), NVBDataParamToken::Units);
	dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::DataSize), NVBDataParamToken::DataSize);
	dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::NAxes), NVBDataParamToken::NAxes);
	dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::IsTopo), NVBDataParamToken::IsTopo);
	dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::IsSpec), NVBDataParamToken::IsSpec);
//  NVBDataParamToken:: dataChoice->addItem(NVBTokenList::nameDataParam(),QVariant());
	dataChoice->setCurrentIndex(0);
	gridLayout->addWidget(dataChoice, 1, 1, 1, 1);
	connect(dataChoice, SIGNAL(currentIndexChanged(int)), SLOT(switchToData()));

	QHBoxLayout *horizontalLayout = new QHBoxLayout();
	axisOption = new QRadioButton("Axis", this);
	types->addButton(axisOption);
	connect(axisOption, SIGNAL(clicked(bool)), this, SIGNAL(stateChanged()));
	horizontalLayout->addWidget(axisOption);

	axisNameEdit = new QLineEdit(this);
	horizontalLayout->addWidget(axisNameEdit);
	connect(axisNameEdit, SIGNAL(textEdited(QString)), SLOT(switchToAxis()));

	gridLayout->addLayout(horizontalLayout, 2, 0, 1, 1);

	axisChoice = new QComboBox(this);
	axisChoice->addItem(NVBTokenList::nameAxisParam(NVBAxisParamToken::Exists), NVBAxisParamToken::Exists);
	axisChoice->addItem(NVBTokenList::nameAxisParam(NVBAxisParamToken::Name), NVBAxisParamToken::Name);
	axisChoice->addItem(NVBTokenList::nameAxisParam(NVBAxisParamToken::Length), NVBAxisParamToken::Length);
	axisChoice->addItem(NVBTokenList::nameAxisParam(NVBAxisParamToken::Units), NVBAxisParamToken::Units);
	axisChoice->addItem(NVBTokenList::nameAxisParam(NVBAxisParamToken::Span), NVBAxisParamToken::Span);

	axisChoice->setCurrentIndex(0);
	gridLayout->addWidget(axisChoice, 2, 1, 1, 1);
	connect(axisChoice, SIGNAL(currentIndexChanged(int)), SLOT(switchToAxis()));

	commentOption = new QRadioButton("Parameter", this);
	types->addButton(commentOption);
	connect(commentOption, SIGNAL(clicked(bool)), this, SIGNAL(stateChanged()));
	gridLayout->addWidget(commentOption, 3, 0, 1, 1);

	commentChoice = new QComboBox(this);
	commentChoice->setEditable(true);
	commentChoice->addItems(qApp->property("filesFactory").value<NVBFileFactory*>()->availableInfoFields());
	commentChoice->setAutoCompletion(true);
	gridLayout->addWidget(commentChoice, 3, 1, 1, 1);
	connect(commentChoice, SIGNAL(currentIndexChanged(int)), SLOT(switchToComment()));

	textOption = new QRadioButton("Text", this);
	types->addButton(textOption);
	connect(textOption, SIGNAL(clicked(bool)), this, SIGNAL(stateChanged()));
	gridLayout->addWidget(textOption, 4, 0, 1, 1);

	textChoice = new QLineEdit(this);
	connect(textChoice, SIGNAL(textEdited(QString)), SLOT(switchToText()));
	gridLayout->addWidget(textChoice, 4, 1, 1, 1);

	mainLayout->addLayout(gridLayout);

	QFrame * limitFrame = new QFrame(this);
	mainLayout->addWidget(limitFrame);

	limitFrame->setLineWidth(1);
	limitFrame->setFrameShape(QFrame::Panel);
	limitFrame->setFrameShadow(QFrame::Sunken);

	QVBoxLayout * vertLayout = new QVBoxLayout(limitFrame);

	topoOnlyOption = new QRadioButton("Limit to topographic data", limitFrame);
	topoOnlyOption->setAutoExclusive(false);
	connect(topoOnlyOption, SIGNAL(clicked(bool)), this, SLOT(switchToTopo()));
	vertLayout->addWidget(topoOnlyOption);
	specOnlyOption = new QRadioButton("Limit to spectroscopic data", limitFrame);
	specOnlyOption->setAutoExclusive(false);
	connect(specOnlyOption, SIGNAL(clicked(bool)), this, SLOT(switchToSpec()));
	vertLayout->addWidget(specOnlyOption);
	horizontalLayout = new QHBoxLayout();
	axisNumOption = new QCheckBox("Limit to data with", limitFrame);
	connect(axisNumOption, SIGNAL(clicked(bool)), this, SIGNAL(stateChanged()));
	horizontalLayout->addWidget(axisNumOption);
	axisLimitChoice = new QComboBox(limitFrame);
	axisLimitChoice->addItem("exactly", NVBGotoToken::HasNAxes);
	axisLimitChoice->addItem("at least", NVBGotoToken::HasAtLeastNAxes);
	axisLimitChoice->addItem("at most", NVBGotoToken::HasAtMostNAxes);
	connect(axisLimitChoice, SIGNAL(currentIndexChanged(int)), this, SIGNAL(stateChanged()));
	horizontalLayout->addWidget(axisLimitChoice);
	axisNumChoice = new QSpinBox(limitFrame);
	axisNumChoice->setMinimum(1);
	axisNumChoice->setValue(2);
	axisNumChoice->setSuffix(" axes");
	connect(axisNumChoice, SIGNAL(valueChanged(int)), this, SIGNAL(stateChanged()));
	horizontalLayout->addWidget(axisNumChoice);
	horizontalLayout->addStretch(1);

	vertLayout->addLayout(horizontalLayout);

	horizontalLayout = new QHBoxLayout();
	expertOption = new QRadioButton("Expert", this);
	types->addButton(expertOption);
	connect(expertOption, SIGNAL(clicked(bool)), this, SIGNAL(stateChanged()));
	connect(expertOption, SIGNAL(toggled(bool)), limitFrame, SLOT(setDisabled(bool)));
	horizontalLayout->addWidget(expertOption);

	expertEdit = new QLineEdit(this);
	horizontalLayout->addWidget(expertEdit);
	connect(expertEdit, SIGNAL(textEdited(QString)), SLOT(switchToExpert()));

	mainLayout->addLayout(horizontalLayout);

	tokenListToLayout(l);

	connect(this, SIGNAL(stateChanged()), this, SLOT(updateExpertText()));
	}

void NVBColumnInputWidget::switchToSpec() {
	topoOnlyOption->setChecked(false);
	emit stateChanged();
	}

void NVBColumnInputWidget::switchToTopo() {
	specOnlyOption->setChecked(false);
	emit stateChanged();
	}

void NVBColumnInputWidget::switchToFile() {
	fileOption->setChecked(true);
	topoOnlyOption->parentWidget()->setEnabled(true);
	emit stateChanged();
	}

void NVBColumnInputWidget::switchToData() {
	dataOption->setChecked(true);
	topoOnlyOption->parentWidget()->setEnabled(true);
	emit stateChanged();
	}

void NVBColumnInputWidget::switchToAxis() {
	axisOption->setChecked(true);
	topoOnlyOption->parentWidget()->setEnabled(true);
	emit stateChanged();
	}

void NVBColumnInputWidget::switchToComment() {
	commentOption->setChecked(true);
	topoOnlyOption->parentWidget()->setEnabled(true);
	emit stateChanged();
	}

void NVBColumnInputWidget::switchToText() {
	textOption->setChecked(true);
	topoOnlyOption->parentWidget()->setEnabled(true);
	emit stateChanged();
	}

void NVBColumnInputWidget::switchToExpert() {
	expertOption->setChecked(true);
	topoOnlyOption->parentWidget()->setEnabled(false);
	emit stateChanged();
	}

void NVBColumnInputWidget::updateExpertText() {
	if (!expertOption->isChecked()) {
		expertEdit->setText(getState().sourceString());
		}
	}

NVBTokenList NVBColumnInputWidget::getState() {
	if (expertOption->isChecked())
		return NVBTokenList(expertEdit->text());

	QList<NVBToken*> state_tokens;

	if (topoOnlyOption->isChecked()) {
		state_tokens << new NVBGotoToken( NVBGotoToken::IsTopo, 1, axisNumOption->isChecked() ? 3 : 2 );
		}
	else if (specOnlyOption->isChecked()) {
		state_tokens << new NVBGotoToken( NVBGotoToken::IsSpec, 1, axisNumOption->isChecked() ? 3 : 2  );
		}

	if (axisNumOption->isChecked()) {
		NVBGotoToken * gt;
		gt = new NVBGotoToken( (NVBGotoToken::NVBCondType) axisLimitChoice->itemData(axisLimitChoice->currentIndex()).toInt(), 1, 2 );
		gt->n = axisNumChoice->value();
		state_tokens << gt;
		}

	if (fileOption->isChecked()) {
		state_tokens << new NVBFileParamToken(
		               (NVBFileParamToken::NVBFileParam) fileChoice->itemData(fileChoice->currentIndex()).toInt() );
		}
	else if (dataOption->isChecked()) {
		state_tokens << new NVBDataParamToken(
		               (NVBDataParamToken::NVBDataParam) dataChoice->itemData(dataChoice->currentIndex()).toInt() );
		}
	else if (axisOption->isChecked()) {
		if (axisNameEdit->text().isEmpty())
			axisNameEdit->setText("0");

		bool ok;
		int ix = axisNameEdit->text().toInt(&ok);

		if (ok)
			state_tokens << new NVBAxisParamToken(ix, (NVBAxisParamToken::NVBAxisParam) axisChoice->itemData(axisChoice->currentIndex()).toInt() );
		else
			state_tokens << new NVBAxisParamToken(axisNameEdit->text(), (NVBAxisParamToken::NVBAxisParam) axisChoice->itemData(axisChoice->currentIndex()).toInt() );
		}
	else if (commentOption->isChecked()) {
		state_tokens << new NVBPCommentToken( commentChoice->currentText() );
		}
	else if (textOption->isChecked()) {
		state_tokens << new NVBVerbatimToken( textChoice->text() );
		}
	else { // s is null;
		return NVBTokenList();
		}

	return NVBTokenList(state_tokens);;

	}

NVBColumnInputDialog::NVBColumnInputDialog(QWidget * parent): QDialog(parent) {
	setWindowTitle("New column");

	QFormLayout * formLayout = new QFormLayout(this);
	formLayout->addRow("Name", clmnName = new QLineEdit(this));
	formLayout->addRow("Contents", iw = new NVBColumnInputWidget(NVBTokenList(QString()), this));
	QDialogButtonBox * buttonBox;
	formLayout->addRow(buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this));
//	connect(iw,SIGNAL())

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	connect(iw, SIGNAL(stateChanged()), this, SLOT(autoName()));
	}

void NVBColumnInputDialog::autoName() {
	if (clmnName->text().isEmpty())
		clmnName->setText(iw->getState().verboseString());
	}

NVBColumnDescriptor NVBColumnInputDialog::getState() {
	return NVBColumnDescriptor(clmnName->text(), iw->getState());
	}

QString NVBColumnInputDialog::getColumn() {
	NVBColumnInputDialog dialog;
	int result = dialog.exec();

	if (result == QDialog::Accepted) {
		NVBColumnDescriptor cd = dialog.getState();
		return QString("%1/%2").arg(cd.name).arg(cd.contents.compactTokens());
		}

	return QString();

	}

// NVBColumnDescriptor NVBColumnInputDialog::getColumnDescriptor()
// {
//   NVBColumnInputDialog dialog;
//   int result = dialog.exec();
//
//   if (result == QDialog::Accepted)
//     return dialog.getState();
//
//   return NVBColumnDescriptor();
//
// }

NVBColumnDialogWidget::NVBColumnDialogWidget(int ix, NVBColumnDescriptor clmn, QWidget * parent): QWidget(parent), index(ix) {
	resize(554, 45);

	horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setContentsMargins(0, 0, 0, 0);

	clmnName = new QLineEdit(clmn.name, this);
	horizontalLayout->addWidget(clmnName, 2);

	clmnCnt = new NVBCCCBox(clmn.contents, this);
	connect(clmnCnt, SIGNAL(contentChanged()), SLOT(autoName()));
	horizontalLayout->addWidget(clmnCnt, 3);


	/*
	  comboBox = new QComboBox(this);
	  comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	  comboBox->setEditable(false);

	  horizontalLayout->addWidget(comboBox);
	*/

	pushButton = new QPushButton("-", this);
//   pushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	horizontalLayout->addWidget(pushButton);
	connect(pushButton, SIGNAL(clicked()), this, SLOT(removeColumn()));
	}

void NVBColumnDialogWidget::autoName() {
	if (clmnName->text().isEmpty())
		clmnName->setText(clmnCnt->getState().verboseString());
	}

void NVBColumnDialogWidget::renumber(int xindex) {
	if (index > xindex) index -= 1;
	}

NVBColumnDialog::NVBColumnDialog(const QList< NVBColumnDescriptor > & columns, QWidget * parent): QDialog(parent) {
	resize(504, 317);
	setSizeGripEnabled(true);
	setWindowTitle("Columns");

	verticalLayout = new QVBoxLayout(this);
	verticalLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

	horizontalLayout = new QHBoxLayout();
	addButton = new QPushButton("Add column", this);
	horizontalLayout->addWidget(addButton);
	connect(addButton, SIGNAL(clicked()), this, SLOT(addColumn()));

	/*
	  clearButton = new QPushButton("Clear",this);
	  horizontalLayout->addWidget(clearButton);
	  connect(clearButton,SIGNAL(clicked()),this,SLOT(clearColumns()));
	*/

	horizontalLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	okButton = new QPushButton("OK", this);
	okButton->setDefault(true);
	horizontalLayout->addWidget(okButton);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

	cancelButton = new QPushButton("Cancel", this);
	cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
	horizontalLayout->addWidget(cancelButton);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	verticalLayout->addLayout(horizontalLayout);

	foreach (NVBColumnDescriptor d, columns) {
		addColumn(d);
		}

	actions.clear();

	}

NVBColumnDialogWidget * NVBColumnDialog::addColumn(NVBColumnDescriptor d) {
	NVBColumnDialogWidget * widget = new NVBColumnDialogWidget(verticalLayout->count() - 2, d, this);
	actions << NVBColumnAction(NVBColumnAction::Added, verticalLayout->count() - 2);
	verticalLayout->insertWidget(verticalLayout->count() - 2, widget);
	connect(widget, SIGNAL(removeColumn(int)), this, SLOT(removeColumn(int)));
	connect(this, SIGNAL(removedColumn(int)), widget, SLOT(renumber(int)));
//   clearButton->setEnabled(true);
	return widget;
	}

void NVBColumnDialog::removeColumn(int index) {
	QWidget * w = verticalLayout->itemAt(index)->widget();

	if (w) {
		verticalLayout->removeWidget(w);
		delete w;
		emit removedColumn(index);
		}

	actions << NVBColumnAction(NVBColumnAction::Deleted, index);
//   clearButton->setEnabled(verticalLayout->count() > 2);
	}

void NVBColumnDialog::clearColumns() {
	for (int i = verticalLayout->count(); i > 2; i--) {
		QLayoutItem * item = verticalLayout->takeAt(1);
		delete item->widget();
		delete item;
//     delete ;
		}
	}

NVBColumnDescriptor NVBColumnDialogWidget::getState() {
	return NVBColumnDescriptor(clmnName->text(), clmnCnt->getState());
	}

QList< NVBColumnDescriptor > NVBColumnDialog::getColumns() {
	QList< NVBColumnDescriptor > list;

	for (int i = 0; i < verticalLayout->count() - 2; i++) {
		QLayoutItem * item = verticalLayout->itemAt(i);

		if (item && item->widget())
			list << qobject_cast<NVBColumnDialogWidget*>(item->widget())->getState();
		}

	return list;
	}

void NVBColumnDialog::simplifyActions() {
	if (actions.size() < 2) return;

	int tr = 0;
	int z = 0;
	int ba = actions.size() - 1;

	while (tr < ba) {
		while (tr < ba && actions.at(tr).action == NVBColumnAction::Deleted) tr++;

		while (tr < ba && actions.at(ba).action == NVBColumnAction::Added) ba--;

		if (!(tr < ba)) return;

		z = tr + 1;

		while (z <= ba && actions.at(z).action == NVBColumnAction::Added) z++;

		while (z != tr && z <= ba) {
			if (actions.at(z).index == actions.at(z - 1).index) {
				actions.removeAt(z);
				actions.removeAt(z - 1);
				ba -= 2;
				z -= 1;

				while (z <= ba && actions.at(z).action == NVBColumnAction::Added) z++;
				}
			else {
				actions.swap(z, z - 1);
				actions[z].index -= 1;
				z -= 1;
				}
			}

		if (z > ba) break;

		}

	}

QList< NVBColumnAction > NVBColumnDialog::getActions() {
	simplifyActions();
	return actions;
	}

void NVBColumnDialog::disableEntry(int entry) {
	QWidget * w = verticalLayout->itemAt(entry)->widget();

	if (w) w->setEnabled(false);
	}

void NVBCCCBox::paintEvent( QPaintEvent * e) {
	QStylePainter painter(this);
	painter.setPen(palette().color(QPalette::Text));

	// draw the combobox frame, focusrect and selected etc.
	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	opt.currentText = tokens.verboseString();
	painter.drawComplexControl(QStyle::CC_ComboBox, opt);

	// draw the icon and text
	painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
	Q_UNUSED(e);
	}

void NVBCCCBox::showPopup( ) {
//   if (!dropdown) {
	dropdown = new NVBCCDropDown(tokens, this);
//     dropdown->layout()->activate();
	connect(dropdown, SIGNAL(rejected()), this, SLOT(hidePopup()));
	connect(dropdown, SIGNAL(accepted()), this, SLOT(setValue()));
//     }

//   dropdown->move(mapToGlobal(rect().bottomRight())-QPoint(dropdown->width(),-2));
	dropdown->move(mapToGlobal(rect().bottomLeft()) - QPoint(1, -2));
	dropdown->show();

	}

void NVBCCCBox::hidePopup( ) {
	if (dropdown) {
		dropdown->hide();
		delete dropdown;
		dropdown = 0;

		}
	}

void NVBCCCBox::setValue( ) {
	tokens = dropdown->input()->getState();
	update();
	hidePopup();
	emit contentChanged();
	}

void NVBColumnInputWidget::tokenListToLayout( NVBTokenList l ) {
	expertEdit->setText(l.sourceString()); // For curious people (like me)

	if (l.size() == 0 || l.size() > 3) {
		expertOption->setChecked(true);
		return;
		}

	if (l.size() == 3) {
		if (!l.first()->type == NVBToken::Goto || !l.at(1)->type == NVBToken::Goto) {
			expertOption->setChecked(true);
			return;
			}

		NVBGotoToken* gt1 = static_cast<NVBGotoToken*>(l.at(0));
		NVBGotoToken* gt2 = static_cast<NVBGotoToken*>(l.at(1));

		if (gt1->condition == gt2->condition) {
			expertOption->setChecked(true);
			return;
			}

		switch (gt1->condition) {
			case NVBGotoToken::IsSpec : {
				if (gt2->condition == NVBGotoToken::IsTopo) {
					expertOption->setChecked(true);
					return;
					}

				specOnlyOption->setChecked(true);
				break;
				}

			case NVBGotoToken::IsTopo : {
				if (gt2->condition == NVBGotoToken::IsSpec) {
					expertOption->setChecked(true);
					return;
					}

				topoOnlyOption->setChecked(true);
				break;
				}

			case NVBGotoToken::HasNAxes :
			case NVBGotoToken::HasAtLeastNAxes :
			case NVBGotoToken::HasAtMostNAxes : {
				axisNumOption->setChecked(true);
				axisNumChoice->setValue(gt1->n);
				axisLimitChoice->setCurrentIndex(axisLimitChoice->findData(gt1->condition));
				break;
				}

			case NVBGotoToken::None :
			case NVBGotoToken::Stop :
			default : {
				expertOption->setChecked(true);
				return;
				}
			}

		switch (gt2->condition) {
			case NVBGotoToken::IsSpec : {
				specOnlyOption->setChecked(true);
				break;
				}

			case NVBGotoToken::IsTopo : {
				topoOnlyOption->setChecked(true);
				break;
				}

			case NVBGotoToken::HasNAxes :
			case NVBGotoToken::HasAtLeastNAxes :
			case NVBGotoToken::HasAtMostNAxes : {
				axisNumOption->setChecked(true);
				axisNumChoice->setValue(gt2->n);
				axisLimitChoice->setCurrentIndex(axisLimitChoice->findData(gt2->condition));
				break;
				}

			case NVBGotoToken::None :
			case NVBGotoToken::Stop :
			default : {
				expertOption->setChecked(true);
				return;
				}
			}
		}
	else if (l.size() == 2) {
		NVBGotoToken* gt = static_cast<NVBGotoToken*>(l.first());

		if (!gt) {NVBOutputError("Impossible cast error"); setEnabled(false); return;}

		switch (gt->condition) {
			case NVBGotoToken::IsSpec : {
				specOnlyOption->setChecked(true);
				break;
				}

			case NVBGotoToken::IsTopo : {
				topoOnlyOption->setChecked(true);
				break;
				}

			case NVBGotoToken::HasNAxes :
			case NVBGotoToken::HasAtLeastNAxes :
			case NVBGotoToken::HasAtMostNAxes : {
				axisNumOption->setChecked(true);
				axisNumChoice->setValue(gt->n);
				axisLimitChoice->setCurrentIndex(axisLimitChoice->findData(gt->condition));
				break;
				}

			case NVBGotoToken::None :
			case NVBGotoToken::Stop :
			default : {
				expertOption->setChecked(true);
				return;
				}
			}
		}

	switch (l.last()->type) {
		case NVBToken::FileParam : {
			fileOption->setChecked(true);
			fileChoice->setCurrentIndex(fileChoice->findData(static_cast<NVBFileParamToken*>(l.last())->fparam));
			return;
			}

		case NVBToken::DataParam : {
			dataOption->setChecked(true);
			dataChoice->setCurrentIndex(dataChoice->findData(static_cast<NVBDataParamToken*>(l.last())->pparam));
			return;
			}

		case NVBToken::AxisParam : {
			NVBAxisParamToken * at = static_cast<NVBAxisParamToken*>(l.last());
			axisOption->setChecked(true);
			axisNameEdit->setText(at->nparam.isEmpty() ? QString::number(at->ixparam) : at->nparam);
			axisChoice->setCurrentIndex(axisChoice->findData(at->aparam));
			return;
			}

		case NVBToken::DataComment : {
			int ix = commentChoice->findText(static_cast<NVBPCommentToken*>(l.last())->sparam);

			if (ix > -1) {
				commentOption->setChecked(true);
				commentChoice->setCurrentIndex(ix);
				return;
				}

			// Else fall through
			}

		case NVBToken::Verbatim : {
			textOption->setChecked(true);
			textChoice->setText(static_cast<NVBVerbatimToken*>(l.last())->sparam);
			return;
			}

		case NVBToken::Goto :
		case NVBToken::Invalid : {
			expertOption->setChecked(true);
			return;
			}
		}

	}

NVBCCDropDown::NVBCCDropDown( NVBTokens::NVBTokenList l, QWidget * parent ):
	QFrame(parent, Qt::Popup), widget(new NVBColumnInputWidget(l, this)) {
	setLayout(new QVBoxLayout(this));
	layout()->addWidget(widget);
	QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect(btns, SIGNAL(accepted()), this, SIGNAL(accepted()));
	connect(btns, SIGNAL(rejected()), this, SIGNAL(rejected()));
	layout()->addWidget(btns);

	setFrameStyle(QFrame::StyledPanel);
	}

