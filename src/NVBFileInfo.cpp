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

NVBVariant NVBFileInfo::pageParam(NVBPageInfo pi, NVBPageParamToken::NVBPageParam p) const {

  switch (p) {
    case NVBPageParamToken::Name : {
      return pi.name;
      }
    case NVBPageParamToken::IsTopo : {
      return pi.type == NVB::TopoPage;
      }
    case NVBPageParamToken::IsSpec : {
      return pi.type == NVB::SpecPage;;
      }
    case NVBPageParamToken::DataSize : {
      return NVBVariantList() << pi.datasize.width() << QString("x") << pi.datasize.height();
			}
		case NVBPageParamToken::DataSpan : {
			switch(pi.type) {
				case NVB::TopoPage:
					return NVBVariantList() << pi.xSpan << QString("x") << pi.ySpan;
				case NVB::SpecPage:
					return pi.xSpan;
				default:
					return NVBVariant();
				}
			}
    case NVBPageParamToken::XSize : {
      return pi.datasize.width();
      }
    case NVBPageParamToken::YSize : {
      return pi.datasize.height();
      }
		case NVBPageParamToken::XSpan : {
			return pi.xSpan;
			}
		case NVBPageParamToken::YSpan : {
			return pi.ySpan;
			}
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
			
			foreach(NVBPageInfo pi, pages) {
				
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

/*
NVBFileInfo::NVBFileInfo(NVBFile & file)
{
	fileInfo = file.info;
	for (int j = 0; j< file.rowCount(); j++) {
		pages.append(NVBPageInfo(file.index(j).data(PageRole).value<NVBDataSource*>()));
	}
}
*/

NVBFileInfo::NVBFileInfo(const NVBFile * const file)
{
	files = file->sources();
	for (int j = 0; j < file->rowCount(); j++) {
		pages.append(NVBPageInfo(file->index(j).data(PageRole).value<NVBDataSource*>()));
	}
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
/*
	 There's no real need for exceptions here, if the errors are reported from the generator
	 and for outside it is coded in the return value.

	try {
		return generator->loadFileInfo(this);
		}
	catch (int err) {
		switch (err) {
			case (nvberr_invalid_input) : {
				NVBOutputError(QString("Loader %1 returned \"Bad Input\"").arg(generator->moduleName()));
				break;
				}
			case (nvberr_plugin_failure) : {
				NVBOutputError("File failed to load");
				break;
				}
			case (nvberr_unexpected_value) : {
				NVBOutputError(QString("Loader %1 returned \"Unexpected value\"").arg(generator->moduleName()));
				break;
				}
			case (nvberr_not_enough_memory) : {
				NVBOutputError("Not enough memory to load file");
				return NULL;
				}
			default : {
				NVBOutputError(QString("Unknown error #%1 in module %2").arg(err).arg(generator->moduleName()));
				break;
				}
			}
		}
	return 0;
*/
}
