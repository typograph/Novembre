//
// C++ Implementation: NVBColumnDialog
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "NVBColumnDialog.h"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QStylePainter>

#include <QtGui/QApplication>
#include "NVBFileFactory.h"

using namespace NVBTokens;

NVBColumnInputWidget::NVBColumnInputWidget(NVBTokenList l, QWidget * parent):QWidget(parent)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//   setMinimumSize(QSize(363, 163));

  QGridLayout * gridLayout = new QGridLayout(this);
  fileOption = new QRadioButton("File parameter",this);
//   fileOption->setChecked(true);
  gridLayout->addWidget(fileOption, 0, 0, 1, 1);

  fileChoice = new QComboBox(this);
  fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::FileName),NVBFileParamToken::FileName);
  fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::NPages),NVBFileParamToken::NPages);
  fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::FileSize),NVBFileParamToken::FileSize);
  fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::FileCTime),NVBFileParamToken::FileCTime);
  fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::FileMTime),NVBFileParamToken::FileMTime);
  fileChoice->addItem(NVBTokenList::nameFileParam(NVBFileParamToken::FileATime),NVBFileParamToken::FileATime);
//  NVBFileParamToken:: fileChoice->addItem(NVBTokenList::nameFileParam(),QVariant());
  
  fileChoice->setCurrentIndex(0); 
  gridLayout->addWidget(fileChoice, 0, 1, 1, 1);
	connect(fileChoice,SIGNAL(currentIndexChanged(int)),SLOT(switchToFile()));

  dataOption = new QRadioButton("Data parameter",this);
  gridLayout->addWidget(dataOption, 1, 0, 1, 1);

  dataChoice = new QComboBox(this);
  dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::Name),NVBDataParamToken::Name);
  dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::DataSize),NVBDataParamToken::DataSize);
  dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::IsTopo),NVBDataParamToken::IsTopo);
  dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::IsSpec),NVBDataParamToken::IsSpec);
//  dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::XSize),NVBDataParamToken::XSize);
//  dataChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::YSize),NVBDataParamToken::YSize);
//  NVBDataParamToken:: dataChoice->addItem(NVBTokenList::nameDataParam(),QVariant());
  dataChoice->setCurrentIndex(0);
  gridLayout->addWidget(dataChoice, 1, 1, 1, 1);  
	connect(dataChoice,SIGNAL(currentIndexChanged(int)),SLOT(switchToData()));

  topoOption = new QRadioButton("Topography parameter",this);
  gridLayout->addWidget(topoOption, 2, 0, 1, 1);

  topoChoice = new QComboBox(this);
  topoChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::Name),NVBDataParamToken::Name);
  topoChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::DataSize),NVBDataParamToken::DataSize);
//  topoChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::IsTopo),NVBDataParamToken::IsTopo);
//  topoChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::IsSpec),NVBDataParamToken::IsSpec);
//  topoChoice->addItem("Points in X",NVBDataParamToken::XSize);
//  topoChoice->addItem("Points in Y",NVBDataParamToken::YSize);
//  NVBDataParamToken:: topoChoice->addItem(NVBTokenList::nameDataParam(),QVariant());
  topoChoice->setCurrentIndex(0);
  gridLayout->addWidget(topoChoice, 2, 1, 1, 1);
	connect(topoChoice,SIGNAL(currentIndexChanged(int)),SLOT(switchToTopo()));

  specOption = new QRadioButton("Spectroscopy parameter",this);
  gridLayout->addWidget(specOption, 3, 0, 1, 1);

  specChoice = new QComboBox(this);
  specChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::Name),NVBDataParamToken::Name);
  specChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::DataSize),NVBDataParamToken::DataSize);
//  specChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::IsTopo),NVBDataParamToken::IsTopo);
//  specChoice->addItem(NVBTokenList::nameDataParam(NVBDataParamToken::IsSpec),NVBDataParamToken::IsSpec);
//  specChoice->addItem("Points per curve",NVBDataParamToken::XSize);
//  specChoice->addItem("Number of curves",NVBDataParamToken::YSize);
//  NVBDataParamToken:: specChoice->addItem(NVBTokenList::nameDataParam(),QVariant());
  specChoice->setCurrentIndex(0);
  gridLayout->addWidget(specChoice, 3, 1, 1, 1);
	connect(specChoice,SIGNAL(currentIndexChanged(int)),SLOT(switchToSpec()));

  QHBoxLayout *horizontalLayout = new QHBoxLayout();

  commentOption = new QRadioButton("Custom",this);
  horizontalLayout->addWidget(commentOption);

  horizontalLayout->addStretch(1);

  topoCustomCheck = new QCheckBox("T",this);
  topoCustomCheck->setChecked(true);
  horizontalLayout->addWidget(topoCustomCheck);

  specCustomCheck = new QCheckBox("S",this);
  specCustomCheck->setChecked(true);
  horizontalLayout->addWidget(specCustomCheck);

  gridLayout->addLayout(horizontalLayout, 4, 0, 1, 1);

  commentChoice = new QComboBox(this);
//	commentChoice->setEditable(true);
  commentChoice->addItems(qApp->property("filesFactory").value<NVBFileFactory*>()->availableInfoFields());
	commentChoice->setAutoCompletion(true);
  gridLayout->addWidget(commentChoice, 4, 1, 1, 1);
	connect(commentChoice,SIGNAL(currentIndexChanged(int)),SLOT(switchToCustom()));

  expertOption = new QRadioButton("Expert",this);
  gridLayout->addWidget(expertOption, 5, 0, 1, 1);

  expertEdit = new QLineEdit(this);
  gridLayout->addWidget(expertEdit, 5, 1, 1, 1);
	connect(expertEdit,SIGNAL(textEdited(QString)),SLOT(switchToExpert()));

  tokenListToLayout(l);

}

void NVBColumnInputWidget::switchToFile() {
	fileOption->setChecked(true);
}

void NVBColumnInputWidget::switchToData() {
	dataOption->setChecked(true);
}

void NVBColumnInputWidget::switchToTopo() {
	topoOption->setChecked(true);
}

void NVBColumnInputWidget::switchToSpec() {
	specOption->setChecked(true);
}

void NVBColumnInputWidget::switchToCustom() {
	commentOption->setChecked(true);
}

void NVBColumnInputWidget::switchToExpert() {
	expertOption->setChecked(true);
}

NVBTokenList NVBColumnInputWidget::getState()
{
  QList<NVBToken*> state_tokens;
  
  if (fileOption->isChecked()) {
    state_tokens << new NVBFileParamToken(
      (NVBFileParamToken::NVBFileParam) fileChoice->itemData(fileChoice->currentIndex()).toInt() );
    }
  else if (dataOption->isChecked()) {
    state_tokens << new NVBDataParamToken(
      (NVBDataParamToken::NVBDataParam) dataChoice->itemData(dataChoice->currentIndex()).toInt() );
    }
  else if (topoOption->isChecked()) {
    state_tokens << new NVBGotoToken( NVBGotoToken::IsTopo, 1, 2 );
    state_tokens << new NVBDataParamToken(
      (NVBDataParamToken::NVBDataParam) topoChoice->itemData(topoChoice->currentIndex()).toInt() );
    }
  else if (specOption->isChecked()) {
    state_tokens << new NVBGotoToken( NVBGotoToken::IsSpec, 1, 2 );
    state_tokens << new NVBDataParamToken(
      (NVBDataParamToken::NVBDataParam) specChoice->itemData(specChoice->currentIndex()).toInt() );
    }
  else if (commentOption->isChecked()) {
    if (topoCustomCheck->isChecked() && !specCustomCheck->isChecked()) {
      state_tokens << new NVBGotoToken( NVBGotoToken::IsTopo, 1, 2 );
      }
    else if (!topoCustomCheck->isChecked() && specCustomCheck->isChecked()) {
      state_tokens << new NVBGotoToken( NVBGotoToken::IsSpec, 1, 2 );
      }
      
    if (topoCustomCheck->isChecked() || specCustomCheck->isChecked()) {
      state_tokens << new NVBPCommentToken( commentChoice->currentText() );
      }    
    }
  else if (expertOption->isChecked()) {
    return NVBTokenList(expertEdit->text());
    }
  else { // s is null;
    return NVBTokenList();
    }

  NVBTokenList result(state_tokens);
  
  expertEdit->setText(result.sourceString());
  
  return result;

}

NVBColumnInputDialog::NVBColumnInputDialog(QWidget * parent):QDialog(parent)
{
  setWindowTitle("New column");

#if QT_VERSION >= 0x040400
  QFormLayout * formLayout = new QFormLayout(this);
  formLayout->addRow("Name",clmnName = new QLineEdit(this));
  formLayout->addRow("Contents",iw = new NVBColumnInputWidget(NVBTokenList(QString()),this));
  QDialogButtonBox * buttonBox;
  formLayout->addRow(buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,Qt::Horizontal,this));
#else
  QGridLayout * gridLayout = new QGridLayout(this);
  gridLayout->addWidget(new QLabel("Name",this),0,0);
  gridLayout->addWidget(clmnName = new QLineEdit(this),0,1);
  gridLayout->addWidget(new QLabel("Contents",this),1,0);
  gridLayout->addWidget(iw = new NVBColumnInputWidget(NVBTokenList(QString()),this));
  QDialogButtonBox * buttonBox;
  gridLayout->addWidget(buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,Qt::Horizontal,this),2,0,1,2);
#endif
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

NVBColumnDescriptor NVBColumnInputDialog::getState()
{
  return NVBColumnDescriptor(clmnName->text(),iw->getState());
}

QString NVBColumnInputDialog::getColumn()
{
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

NVBColumnDialogWidget::NVBColumnDialogWidget(int ix, NVBColumnDescriptor clmn, QWidget * parent):QWidget(parent),index(ix)
{
  resize(554, 45);

  horizontalLayout = new QHBoxLayout(this);
#if QT_VERSION >= 0x040300
  horizontalLayout->setContentsMargins(0,0,0,0);
#else
  horizontalLayout->setMargin(0);
#endif

  clmnName = new QLineEdit(clmn.name,this);
  horizontalLayout->addWidget(clmnName,2);

  clmnCnt = new NVBCCCBox(clmn.contents,this);
	connect(clmnCnt,SIGNAL(contentChanged()),SLOT(autoName()));
  horizontalLayout->addWidget(clmnCnt,3);


/*
  comboBox = new QComboBox(this);
  comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  comboBox->setEditable(false);

  horizontalLayout->addWidget(comboBox);
*/

  pushButton = new QPushButton("-",this);
//   pushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  horizontalLayout->addWidget(pushButton);
  connect(pushButton,SIGNAL(clicked()),this,SLOT(removeColumn()));
}

void NVBColumnDialogWidget::autoName() {
	if (clmnName->text().isEmpty())
		clmnName->setText(clmnCnt->getState().verboseString());
}

void NVBColumnDialogWidget::renumber(int xindex)
{
  if (index > xindex) index -= 1;
}

NVBColumnDialog::NVBColumnDialog(const QList< NVBColumnDescriptor > & columns, QWidget * parent):QDialog(parent)
{
  resize(504, 317);
  setSizeGripEnabled(true);
  setWindowTitle("Columns");

  verticalLayout = new QVBoxLayout(this);
  verticalLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

  horizontalLayout = new QHBoxLayout();
  addButton = new QPushButton("Add column",this);
  horizontalLayout->addWidget(addButton);
  connect(addButton,SIGNAL(clicked()),this,SLOT(addColumn()));

/*
  clearButton = new QPushButton("Clear",this);
  horizontalLayout->addWidget(clearButton);
  connect(clearButton,SIGNAL(clicked()),this,SLOT(clearColumns()));
*/

  horizontalLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

  okButton = new QPushButton("OK",this);
  okButton->setDefault(true);
  horizontalLayout->addWidget(okButton);
  connect(okButton,SIGNAL(clicked()),this,SLOT(accept()));

  cancelButton = new QPushButton("Cancel",this);
  cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
  horizontalLayout->addWidget(cancelButton);
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

  verticalLayout->addLayout(horizontalLayout);

  foreach (NVBColumnDescriptor d, columns) {
    addColumn(d);
    }

  actions.clear();

}

NVBColumnDialogWidget * NVBColumnDialog::addColumn(NVBColumnDescriptor d)
{
  NVBColumnDialogWidget * widget = new NVBColumnDialogWidget(verticalLayout->count()-2,d,this);
  actions << NVBColumnAction(NVBColumnAction::Added,verticalLayout->count()-2);
  verticalLayout->insertWidget(verticalLayout->count()-2,widget);
  connect(widget,SIGNAL(removeColumn(int)),this,SLOT(removeColumn(int)));
  connect(this,SIGNAL(removedColumn(int)),widget,SLOT(renumber(int)));
//   clearButton->setEnabled(true);
  return widget;
}

void NVBColumnDialog::removeColumn(int index)
{
  QWidget * w = verticalLayout->itemAt(index)->widget();
  if (w) {
    verticalLayout->removeWidget(w);
    delete w;
    emit removedColumn(index);
    }
  actions << NVBColumnAction(NVBColumnAction::Deleted,index);
//   clearButton->setEnabled(verticalLayout->count() > 2);
}

void NVBColumnDialog::clearColumns()
{
  for (int i = verticalLayout->count(); i>2; i--) {
    QLayoutItem * item = verticalLayout->takeAt(1);
    delete item->widget();
    delete item;
//     delete ;
    }
}

NVBColumnDescriptor NVBColumnDialogWidget::getState()
{
  return NVBColumnDescriptor(clmnName->text(),clmnCnt->getState());
}

QList< NVBColumnDescriptor > NVBColumnDialog::getColumns() {
  QList< NVBColumnDescriptor > list;

  for (int i = 0; i < verticalLayout->count()-2; i++) {
    QLayoutItem * item = verticalLayout->itemAt(i);
    if (item && item->widget())
      list << qobject_cast<NVBColumnDialogWidget*>(item->widget())->getState();
    }

  return list;
}

void NVBColumnDialog::simplifyActions()
{
  if (actions.size() < 2) return;
  int tr = 0;
  int z = 0;
  int ba = actions.size()-1;

  while (tr < ba) {
    while (tr < ba && actions.at(tr).action == NVBColumnAction::Deleted) tr++;
    while (tr < ba && actions.at(ba).action == NVBColumnAction::Added) ba--;
    if (!(tr < ba)) return;

    z = tr + 1;

    while (z <= ba && actions.at(z).action == NVBColumnAction::Added) z++;

    while (z != tr && z <= ba) {
      if (actions.at(z).index == actions.at(z-1).index) {
        actions.removeAt(z);
        actions.removeAt(z-1);
        ba -= 2;
        z -= 1;
        while (z <= ba && actions.at(z).action == NVBColumnAction::Added) z++;
        }
      else {
        actions.swap(z,z-1);
        actions[z].index -= 1;
        z -= 1;
        }
      }
  
    if (z > ba) break;

    }

}

QList< NVBColumnAction > NVBColumnDialog::getActions()
{
  simplifyActions();
  return actions;
}

void NVBColumnDialog::disableEntry(int entry)
{
	QWidget * w = verticalLayout->itemAt(entry)->widget();
	if (w) w->setEnabled(false);
}

void NVBCCCBox::paintEvent( QPaintEvent * e)
{
#if QT_VERSION >= 0x040300
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
#else
  QComboBox::paintEvent(e);
#endif
}

void NVBCCCBox::showPopup( )
{
//   if (!dropdown) {
    dropdown = new NVBCCDropDown(tokens,this);
//     dropdown->layout()->activate();
		connect(dropdown,SIGNAL(rejected()),this,SLOT(hidePopup()));
    connect(dropdown,SIGNAL(accepted()),this,SLOT(setValue()));
//     }
  
//   dropdown->move(mapToGlobal(rect().bottomRight())-QPoint(dropdown->width(),-2)); 
  dropdown->move(mapToGlobal(rect().bottomLeft())-QPoint(1,-2)); 
  dropdown->show();
  
}

void NVBCCCBox::hidePopup( )
{
  if (dropdown) {
    dropdown->hide();
    delete dropdown;
    dropdown = 0;

    }
}

void NVBCCCBox::setValue( )
{
  tokens = dropdown->input()->getState();
#if QT_VERSION < 0x040300
  QComboBox::setItemText(0,tokens.verboseString());
#endif
  update();
  hidePopup();
	emit contentChanged();
}

void NVBColumnInputWidget::tokenListToLayout( NVBTokenList l )
{
  expertEdit->setText(l.sourceString()); // For curious people
  
  if (l.size() == 0 || l.size() > 2 || (l.size() == 2 && l.first()->type != NVBToken::Goto)) {
    expertOption->setChecked(true);    
    return;
    }
    
  if (l.size() == 2)
    switch (static_cast<NVBGotoToken*>(l.last())->type) {
      case NVBToken::FileParam :
      case NVBToken::Goto :
      case NVBToken::Verbatim :
      case NVBToken::Invalid :
        {
        expertOption->setChecked(true);    
        return;
        }
      case NVBToken::DataComment : {
        commentOption->setChecked(true);
        
        switch (static_cast<NVBGotoToken*>(l.first())->condition) {
          case NVBGotoToken::IsSpec : {
            specCustomCheck->setChecked(true);
            topoCustomCheck->setChecked(false);
            break;
            }
          case NVBGotoToken::IsTopo : {
            topoCustomCheck->setChecked(true);
            specCustomCheck->setChecked(false);
            break;
            }
          case NVBGotoToken::None : 
          case NVBGotoToken::Stop :
          default : {
            expertOption->setChecked(true);    
            return;
            }
          }
          
        commentChoice->lineEdit()->setText(static_cast<NVBPCommentToken*>(l.last())->sparam);          
        return;
        }
      case NVBToken::DataParam : {
        switch (static_cast<NVBGotoToken*>(l.first())->condition) {
          case NVBGotoToken::IsSpec : {
            specOption->setChecked(true);
            specChoice->setCurrentIndex(specChoice->findData(static_cast<NVBDataParamToken*>(l.last())->pparam));
            return;
            }
          case NVBGotoToken::IsTopo : {
            topoOption->setChecked(true);
            topoChoice->setCurrentIndex(topoChoice->findData(static_cast<NVBDataParamToken*>(l.last())->pparam));
            return;
            }
          case NVBGotoToken::None : 
          case NVBGotoToken::Stop :
          default : {
            expertOption->setChecked(true);    
            return;
            }
          }
        return; // Unreachable
        }
      }
      
    switch (l.first()->type) {
      case NVBToken::DataComment : {
        commentOption->setChecked(true);
        topoCustomCheck->setChecked(true);
        specCustomCheck->setChecked(true);
				int ci = commentChoice->findText(static_cast<NVBPCommentToken*>(l.first())->sparam);
				if (ci > -1)
					commentChoice->setCurrentIndex(ci);
				else {
					commentChoice->setEditable(true);
					commentChoice->lineEdit()->setText(static_cast<NVBPCommentToken*>(l.first())->sparam);
					}
        break;
        }
      case NVBToken::FileParam : {
        fileOption->setChecked(true);
        fileChoice->setCurrentIndex(fileChoice->findData(static_cast<NVBFileParamToken*>(l.first())->fparam));
        break;
        }
      case NVBToken::DataParam : {
        dataOption->setChecked(true);
        dataChoice->setCurrentIndex(dataChoice->findData(static_cast<NVBDataParamToken*>(l.first())->pparam));
        break;
        }
      case NVBToken::Verbatim :
      case NVBToken::Goto : 
      default :
        {
        expertOption->setChecked(true);  
        return;
        }
      }

}

NVBCCDropDown::NVBCCDropDown( NVBTokens::NVBTokenList l, QWidget * parent ):
  QFrame(parent, Qt::Popup),widget(new NVBColumnInputWidget(l,this))
{
  setLayout(new QVBoxLayout(this));
  layout()->addWidget(widget);
  QDialogButtonBox * btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
  connect(btns,SIGNAL(accepted()),this,SIGNAL(accepted()));
  connect(btns,SIGNAL(rejected()),this,SIGNAL(rejected()));
  layout()->addWidget(btns);
  
  setFrameStyle(QFrame::StyledPanel);
}

