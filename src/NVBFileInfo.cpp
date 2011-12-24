//
// C++ Implementation: NVBFileWrappers
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBFileInfo.h"
#include "NVBFileGenerator.h"
#include "NVBFile.h"
#include "NVBforeach.h"
#include <QtCore/QDateTime>

using namespace NVBTokens;

// NVBFileInfo

NVBVariant NVBFileInfo::fileParam(NVBFileParamToken::NVBFileParam p) const {

  switch (p) {
    case NVBFileParamToken::FileName : {
			return files.name();
      }
		case NVBFileParamToken::FileNames : {
			return files.join(" ");
			}
		case NVBFileParamToken::FileSize : {
			int size = 0;
			foreach(QString file, files) {
				size += QFileInfo(file).size();
				}
      return size;
      }
    case NVBFileParamToken::FileATime : {
			QDateTime access = QDateTime::currentDateTime().addYears(-1000);
			foreach(QString file, files) {
				QDateTime t = QFileInfo(file).lastRead();
				if (access < t) access = t;
				}
      return access;
      }
    case NVBFileParamToken::FileMTime : {
			QDateTime modified = QDateTime::currentDateTime().addYears(-1000);
			foreach(QString file, files) {
				QDateTime t = QFileInfo(file).lastModified();
				if (modified < t) modified = t;
				}
			return modified;
      }
    case NVBFileParamToken::FileCTime : {
			QDateTime created = QDateTime::currentDateTime();
			foreach(QString file, files) {
				QDateTime t = QFileInfo(file).created();
				if (created > t) created = t;
				}
			return created;
      }
    case NVBFileParamToken::NPages : {
      return count();
      }
    default :
      return NVBVariant();
    }

}

NVBVariant NVBFileInfo::dataParam(const NVBDataInfo& pi, NVBDataParamToken::NVBDataParam p) const {

  switch (p) {
    case NVBDataParamToken::Name : {
      return pi.name;
      }
		case NVBDataParamToken::DataSize : {
			NVBVariantList l;
			foreach(NVBAxisInfo s, pi.axes)
				l << s.length;
			l.setSeparator(" x ");
			return l;
      }
		case NVBDataParamToken::Units : {
			return pi.dimension;
      }
		case NVBDataParamToken::NAxes : {
      return pi.axes.count();
      }
		case NVBDataParamToken::IsTopo : {
      return pi.type == NVBDataSet::Topography;
      }
		case NVBDataParamToken::IsSpec : {
      return pi.type == NVBDataSet::Spectroscopy;
      }
    default :
      return NVBVariant();
    }

}

NVBVariant NVBFileInfo::axisParam(const NVBDataInfo & pi, QString name, NVBAxisParamToken::NVBAxisParam p) const
{
	for(int i=0; i<pi.axes.count(); i++)
		if (pi.axes.at(i).name == name)
			return p == NVBAxisParamToken::Exists ? true : axisParam(pi,i,p);
	return p == NVBAxisParamToken::Exists ? false : NVBVariant();
}

NVBVariant NVBFileInfo::axisParam(const NVBDataInfo& pi, int index, NVBAxisParamToken::NVBAxisParam p) const
{	
	if (index < 0 || index > pi.axes.count())
		return p == NVBAxisParamToken::Exists ? false : NVBVariant();
	
  switch (p) {
		case NVBAxisParamToken::Exists:
			return true;
		case NVBAxisParamToken::Name :
      return pi.axes.at(index).name;
		case NVBAxisParamToken::Length :
			return pi.axes.at(index).length;
		case NVBAxisParamToken::Units :
			return pi.axes.at(index).units;
    default :
      return NVBVariant();
    }
}


NVBVariant NVBFileInfo::getInfo(const NVBTokenList & list) const {
  
		if (list.size() == 1 && list.first()->type == NVBToken::FileParam ) {
			return fileParam(static_cast<NVBFileParamToken*>(list.first())->fparam);
			}
		else {   
			NVBVariantList ans, pans;
			ans.setSeparator(" : ");
			pans.setSeparator("");
			foreach(NVBDataInfo pi, *this) {
				
				pans.clear();
				
				for (int i = 0; i < list.size(); ) {
					switch (list.at(i)->type) {
						case NVBToken::Verbatim : {
							pans << static_cast<NVBVerbatimToken*>(list.at(i++))->sparam;
							break;
							}
						case NVBToken::DataComment : {
							NVBVariant tv = pi.comments.value(static_cast<NVBVerbatimToken*>(list.at(i++))->sparam,QVariant());
							if (tv.isValid())
								pans << tv;
							else
								pans << comments.value(static_cast<NVBVerbatimToken*>(list.at(i-1))->sparam,QVariant());
							break;
							}
						case NVBToken::FileParam : {
							pans << fileParam(static_cast<NVBFileParamToken*>(list.at(i++))->fparam);
							break;
							}
						case NVBToken::DataParam : {
							pans << dataParam(pi,static_cast<NVBDataParamToken*>(list.at(i++))->pparam);
							break;
							}
						case NVBToken::AxisParam : {
							NVBAxisParamToken * at = static_cast<NVBAxisParamToken*>(list.at(i++));
							pans << axisParam(pi,at->nparam,at->aparam);
							break;
							}
						case NVBToken::Goto : {
							switch (static_cast<NVBGotoToken*>(list.at(i))->condition) {
								case NVBGotoToken::None : {
									i += static_cast<NVBGotoToken*>(list.at(i))->gototrue;
									break;
									}
								case NVBGotoToken::IsSpec : {
									if (pi.type == NVBDataSet::Spectroscopy)
										i += static_cast<NVBGotoToken*>(list.at(i))->gototrue;
									else
										i += static_cast<NVBGotoToken*>(list.at(i))->gotofalse;
									break;
									}
								case NVBGotoToken::IsTopo : {
									if (pi.type == NVBDataSet::Topography)
										i += static_cast<NVBGotoToken*>(list.at(i))->gototrue;
									else
										i += static_cast<NVBGotoToken*>(list.at(i))->gotofalse;
									break;
									}
								case NVBGotoToken::HasNAxes : {
									if (pi.axes.count() == static_cast<NVBGotoToken*>(list.at(i))->n)
										i += static_cast<NVBGotoToken*>(list.at(i))->gototrue;
									else
										i += static_cast<NVBGotoToken*>(list.at(i))->gotofalse;
									break;
									}
								case NVBGotoToken::HasAtLeastNAxes : {
									if (pi.axes.count() >= static_cast<NVBGotoToken*>(list.at(i))->n)
										i += static_cast<NVBGotoToken*>(list.at(i))->gototrue;
									else
										i += static_cast<NVBGotoToken*>(list.at(i))->gotofalse;
									break;
									}
								case NVBGotoToken::HasAtMostNAxes : {
									if (pi.axes.count() <= static_cast<NVBGotoToken*>(list.at(i))->n)
										i += static_cast<NVBGotoToken*>(list.at(i))->gototrue;
									else
										i += static_cast<NVBGotoToken*>(list.at(i))->gotofalse;
									break;
									}
/*								case NVBGotoToken:: : {
									break;
									} */
								case NVBGotoToken::Stop : {
									continue;
									}
								default : {
									i += 1;
									}
								}
							break;
							}
						default : return NVBVariant();
						}
					}
				if (ans.isEmpty() || ans.last() != pans)
					ans << NVBVariant(pans);
				}
			return ans;
			}
}

QString NVBFileInfo::getInfoAsString(const NVBTokenList & list) const
{
  return getInfo(list).toString();
}

NVBFileInfo::NVBFileInfo(const NVBFile * const file)
{
	files = file->sources();
	comments = file->getAllComments();
	NVB_FOREACH(NVBDataSource * source, file)
		foreach(const NVBDataSet * set, source->dataSets())
			append(NVBDataInfo(set));
}

bool NVBAssociatedFilesInfo::operator==(const NVBAssociatedFilesInfo & other) const {
	if (_generator != other._generator) return false;
	if (_name != other._name) return false;
	return QStringList::operator ==(other);
}

NVBFileInfo * NVBAssociatedFilesInfo::loadFileInfo() const
{
	if (!generator()) return 0;
	return generator()->loadFileInfo(*this);
}

NVBVariant NVBFileInfo::getComment(const QString& key) {
	if (comments.contains(key))
		return comments.value(key);
	else {
		NVBVariantList l;
		foreach(const NVBDataInfo & i, *this) {
			NVBVariant v = i.comments.value(key);
			if (v.isValid())
				l << v;
			}
		if (l.isEmpty())
			return NVBVariant();
		else if (l.count() == 1)
			return l.first();
		else
			return NVBVariant(l);
		}
}

void NVBFileInfo::filterAddComments(NVBDataComments& newComments)
{
	if (newComments.isEmpty())
		return;

	if (count() == 0) {
		comments.unite(newComments);
		newComments.clear();
		return;
		}
		
	foreach (QString key, comments.keys())
		if (newComments.contains(key) && newComments.value(key) == comments.value(key))
			newComments.remove(key);
		else {
			for(int i = 0; i<count(); i++)
				operator[](i).comments.insert(key,comments.value(key));
			comments.remove(key);
			}
			
	return;

}
