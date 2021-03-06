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

#ifndef NVBCOLUMNDIALOG_H
#define NVBCOLUMNDIALOG_H

#include <QtGui/QDialog>
#include <QtCore/QStringList>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>

#include "NVBColumnsModel.h"

class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QSpinBox;

/**
 * \class NVBColumnInputWidget
 *
 * Used excludsively by browser, this widget provides a GUI
 * for editing column format strings.
 *
 * For column format specification \see NVBTokenList.
 *
 * The user GUI is limited to a subset of possible formats,
 * with an additional "expert" option for entering arbitrary formats.
 *
 * The available fields are
 * - file parameters
 * - data parameters
 * - axis parameters
 * - comments
 */

class NVBColumnInputWidget : public QWidget {
		Q_OBJECT

		QRadioButton *fileOption;
		QComboBox *fileChoice;
		QRadioButton *dataOption;
		QComboBox *dataChoice;
		QRadioButton *axisOption;
		QLineEdit *axisNameEdit;
		QComboBox *axisChoice;
		QRadioButton *commentOption;
		QComboBox *commentChoice;
		QRadioButton *textOption;
		QLineEdit *textChoice;

		QRadioButton *topoOnlyOption;
		QRadioButton *specOnlyOption;
		QCheckBox *axisNumOption;
		QComboBox *axisLimitChoice;
		QSpinBox *axisNumChoice;

		QRadioButton *expertOption;
		QLineEdit *expertEdit;

	public:
		NVBColumnInputWidget(NVBTokens::NVBTokenList l, QWidget * parent = 0);

		NVBTokens::NVBTokenList getState();

	private:
		void tokenListToLayout(NVBTokens::NVBTokenList l);
	private slots:
		void switchToTopo();
		void switchToSpec();
		void switchToFile();
		void switchToData();
		void switchToAxis();
		void switchToComment();
		void switchToText();
		void switchToExpert();
		void updateExpertText();
	signals:
		void stateChanged();
	};

class NVBColumnInputDialog : public QDialog {
		Q_OBJECT
	private:
		NVBColumnInputWidget * iw;
		QLineEdit * clmnName;

	public:
		NVBColumnInputDialog(QWidget * parent = 0);

		NVBColumnDescriptor getState();
		static QString getColumn();
	public slots:
		void autoName();
	};

class NVBCCDropDown : public QFrame {
		Q_OBJECT

	private:
		NVBColumnInputWidget * widget;

	public:
		NVBCCDropDown(NVBTokens::NVBTokenList l, QWidget * parent = 0);

		NVBColumnInputWidget * input() { return widget; }

	signals:
		void accepted();
		void rejected();

	protected:
//   virtual void leaveEvent ( QEvent * ) { emit choiceDone(); }

	};

class NVBCCCBox : public QComboBox {
		Q_OBJECT
	private:
		NVBTokens::NVBTokenList tokens;
		NVBCCDropDown * dropdown;

	public:

		NVBCCCBox(const NVBTokens::NVBTokenList & l, QWidget * parent = 0):
			QComboBox(parent), tokens(l), dropdown(0) {
			}
		~NVBCCCBox() {;}

		NVBTokens::NVBTokenList getState() { if (dropdown) return dropdown->input()->getState(); else return tokens; }

		virtual void paintEvent(QPaintEvent *);
		virtual void showPopup ();
	public slots:
		virtual void hidePopup ();
	signals:
		void contentChanged();
	private slots:
		void setValue();
	};

class NVBColumnDialogWidget : public QWidget {
		Q_OBJECT

		int index;

		QHBoxLayout *horizontalLayout;
		QLineEdit *clmnName;
		NVBCCCBox *clmnCnt;
//   NVBCCCBox *comboBox;
		QPushButton *pushButton;

	public:

		NVBColumnDialogWidget(int ix, NVBColumnDescriptor clmn, QWidget * parent = 0);

		NVBColumnDescriptor getState();

	public slots:
		void renumber(int xindex);
	private slots:
		void removeColumn() {
			emit removeColumn(index);
			}
		void autoName();
	signals:
		void removeColumn(int);

	};

struct NVBColumnAction {
	enum ActionType {Added, Deleted} action;
	int index;

	NVBColumnAction(ActionType act, int ind): action(act), index(ind) {;}
	};

class NVBColumnDialog : public QDialog {
		Q_OBJECT

	private:
		QVBoxLayout *verticalLayout;
		QHBoxLayout *horizontalLayout;
		QPushButton *addButton;
//   QPushButton *clearButton;
		QPushButton *okButton;
		QPushButton *cancelButton;

		QList<NVBColumnAction> actions;

		void simplifyActions();

	public:

		NVBColumnDialog(const QList< NVBColumnDescriptor > & columns, QWidget * parent = 0);

		QList<NVBColumnAction> getActions();
		QList<NVBColumnDescriptor> getColumns();

		void disableEntry(int entry);

	public slots:
		NVBColumnDialogWidget * addColumn(NVBColumnDescriptor d = NVBColumnDescriptor());
		void removeColumn(int);
		void clearColumns();

	signals:
		void removedColumn(int);

	};


#endif // NVBCOLUMNDIALOG_H
