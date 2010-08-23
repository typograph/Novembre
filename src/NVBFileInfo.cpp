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
      return pages.size();
      }
    default :
      return NVBVariant();
    }

}

NVBVariant NVBFileInfo::pageParam(NVBDataInfo pi, NVBPageParamToken::NVBPageParam p) const {

  switch (p) {
    case NVBPageParamToken::Name : {
      return pi.name;
      }
/*
		case NVBPageParamToken::IsTopo : {
      return pi.type == NVB::TopoPage;
      }
    case NVBPageParamToken::IsSpec : {
      return pi.type == NVB::SpecPage;;
      }
*/
		case NVBPageParamToken::DataSize : {
			return NVBVariantList() << pi.sizes;
      }
/*
		case NVBPageParamToken::XSize : {
      return pi.datasize.width();
      }
    case NVBPageParamToken::YSize : {
      return pi.datasize.height();
      }
*/
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
			
			foreach(NVBDataInfo pi, pages) {
				
				pans.clear();
				
				for (int i = 0; i < list.size(); ) {
					switch (list.at(i)->type) {
						case NVBToken::Verbatim : {
							pans << static_cast<NVBVerbatimToken*>(list.at(i++))->sparam;
							break;
							}
						case NVBToken::PageComment : {
							pans << pi.comments.value(static_cast<NVBVerbatimToken*>(list.at(i++))->sparam,QVariant());
							break;
							}
						case NVBToken::FileParam : {
							pans << fileParam(static_cast<NVBFileParamToken*>(list.at(i++))->fparam);
							break;
							}
						case NVBToken::PageParam : {
							pans << pageParam(pi,static_cast<NVBPageParamToken*>(list.at(i++))->pparam);
							break;
							}
						case NVBToken::Goto : {
							switch (static_cast<NVBGotoToken*>(list.at(i))->condition) {
								case NVBGotoToken::None : {
									i += static_cast<NVBGotoToken*>(list.at(i))->gototrue;
									break;
									}
/*
								case NVBGotoToken::IsSpec : {
									if (pi.type == NVB::SpecPage)
										i += static_cast<NVBGotoToken*>(list.at(i))->gototrue;
									else
										i += static_cast<NVBGotoToken*>(list.at(i))->gotofalse;
									break;
									}
								case NVBGotoToken::IsTopo : {
									if (pi.type == NVB::TopoPage)
										i += static_cast<NVBGotoToken*>(list.at(i))->gototrue;
									else
										i += static_cast<NVBGotoToken*>(list.at(i))->gotofalse;
									break;
									}
*/
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
  return getInfo(list).toString(" : ");
}

NVBFileInfo::NVBFileInfo(const NVBFile * const file)
{
	files = file->sources();
	comments = files->getAllComments();
	foreach(NVBDataSource * source, file)
		foreach(NVBDataSet set, &source)
			dataInfos.append(NVBPageInfo(set));
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
