//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "NVBTokens.h"
#include "NVBLogger.h"
#include <QtCore/QRegExp>

using namespace NVBTokens;

QMap<NVBFileParamToken::NVBFileParam, NVBDescrPair > NVBTokenList::fileParamNames = NVBTokenList::initFileParamNames();

QMap<NVBFileParamToken::NVBFileParam, NVBDescrPair > NVBTokenList::initFileParamNames( ) {

	QMap<NVBFileParamToken::NVBFileParam, NVBDescrPair > pnames;

	pnames.insert(NVBFileParamToken::FileATime, NVBDescrPair("fileATime", "Time of last access"));
	pnames.insert(NVBFileParamToken::FileMTime, NVBDescrPair("fileMTime", "Time of last modification"));
	pnames.insert(NVBFileParamToken::FileCTime, NVBDescrPair("fileCTime", "Time of creation"));
	pnames.insert(NVBFileParamToken::FileName, NVBDescrPair("fileName", "Name"));
	pnames.insert(NVBFileParamToken::FileNames, NVBDescrPair("fileNames", "All included files"));
	pnames.insert(NVBFileParamToken::FileSize, NVBDescrPair("fileSize", "Size"));
	pnames.insert(NVBFileParamToken::NPages, NVBDescrPair("nPages", "Number of datasets"));

//   pnames.insert(NVBFileParamToken::,NVBDescrPair("",""));

	return pnames;

	}

QString NVBTokenList::nameFileParam( NVBFileParamToken::NVBFileParam p ) {
	return fileParamNames.value(p,NVBDescrPair("-- BUG --", "-- BUG --")).verbName();
	}

NVBFileParamToken::NVBFileParam NVBTokenList::findFileParam( QString key ) {
	return fileParamNames.key(NVBDescrPair(key, QString()));
	}

QMap<NVBDataParamToken::NVBDataParam, NVBDescrPair > NVBTokenList::dataParamNames = NVBTokenList::initDataParamNames();

QMap<NVBDataParamToken::NVBDataParam, NVBDescrPair > NVBTokenList::initDataParamNames( ) {
	QMap<NVBDataParamToken::NVBDataParam, NVBDescrPair > dnames;

	dnames.insert(NVBDataParamToken::Name, NVBDescrPair("dataName", "Name"));
	dnames.insert(NVBDataParamToken::DataSize, NVBDescrPair("dataSize", "Dimensions"));
	dnames.insert(NVBDataParamToken::Units, NVBDescrPair("dataUnits", "Units"));
	dnames.insert(NVBDataParamToken::NAxes, NVBDescrPair("dataNAxes", "Number of axes"));
	dnames.insert(NVBDataParamToken::IsTopo, NVBDescrPair("dataIsTopo", "Topography type"));
	dnames.insert(NVBDataParamToken::IsSpec, NVBDescrPair("dataIsSpec", "Spectroscopic type"));

//  fnames.insert(NVBDataParamToken::,NVBDescrPair("",""));

	return dnames;
	}

QString NVBTokenList::nameDataParam( NVBDataParamToken::NVBDataParam p ) {
	return dataParamNames.value(p,NVBDescrPair("-- BUG --", "-- BUG --")).verbName();
	}

NVBDataParamToken::NVBDataParam NVBTokenList::findDataParam( QString key ) {
	return dataParamNames.key(NVBDescrPair(key, QString()));
	}

QMap<NVBAxisParamToken::NVBAxisParam, NVBDescrPair > NVBTokenList::axisParamNames = NVBTokenList::initAxisParamNames();

QMap<NVBAxisParamToken::NVBAxisParam, NVBDescrPair > NVBTokenList::initAxisParamNames( ) {
	QMap<NVBAxisParamToken::NVBAxisParam, NVBDescrPair > anames;

	anames.insert(NVBAxisParamToken::Exists, NVBDescrPair("axisExists", "Exists"));
	anames.insert(NVBAxisParamToken::Name, NVBDescrPair("axisName", "Name"));
	anames.insert(NVBAxisParamToken::Length, NVBDescrPair("axisLength", "Length"));
	anames.insert(NVBAxisParamToken::Units, NVBDescrPair("axisUnits", "Units"));
	anames.insert(NVBAxisParamToken::Span, NVBDescrPair("axisSpan", "Span"));

//  fnames.insert(NVBAxisParamToken::,NVBDescrPair("",""));

	return anames;
	}

NVBAxisParamToken::NVBAxisParam NVBTokenList::findAxisParam( QString key ) {
	return axisParamNames.key(NVBDescrPair(key, QString()));
	}

QString NVBTokenList::nameAxisParam(NVBAxisParamToken::NVBAxisParam p) {
	return axisParamNames.value(p,NVBDescrPair("-- BUG --", "-- BUG --")).verbName();
	}

QString NVBTokenList::readFromTo( int start, int end, const QList< NVBToken * > & tokens ) {
	int i = start;
	QString s, iftext, elsetext;
	bool needsemicolon = false;

	while (i < end) {
		switch (tokens.at(i)->type) {
			case NVBToken::Verbatim : {
				needsemicolon = false;
				s += QString("\"%1\"").arg(static_cast<NVBVerbatimToken*>(tokens.at(i++))->sparam);
				break;
				}

			case NVBToken::DataComment : {
				if (needsemicolon) s += ';';

				s += static_cast<NVBPCommentToken*>(tokens.at(i++))->sparam;
				needsemicolon = true;
				break;
				}

			case NVBToken::DataParam : {
				if (needsemicolon) s += ';';

				s += dataParamNames.value(static_cast<NVBDataParamToken*>(tokens.at(i++))->pparam,
				                          NVBDescrPair("-- BUG --", "-- BUG --")).keyName();
				needsemicolon = true;
				break;
				}

			case NVBToken::AxisParam : {
				if (needsemicolon) s += ';';

				NVBAxisParamToken * at = static_cast<NVBAxisParamToken*>(tokens.at(i++));

				if (at->nparam.isEmpty())
					s += QString("(%1)%2").arg(at->ixparam).arg(axisParamNames.value(at->aparam,
				                                                NVBDescrPair("-- BUG --", "-- BUG --")).keyName());
				else
					s += QString("(%1)%2").arg(at->nparam).arg(axisParamNames.value(at->aparam,
				                                               NVBDescrPair("-- BUG --", "-- BUG --")).keyName());

				needsemicolon = true;
				break;
				}

			case NVBToken::FileParam : {
				if (needsemicolon) s += ';';

				s += fileParamNames.value(static_cast<NVBFileParamToken*>(tokens.at(i++))->fparam,
				                          NVBDescrPair("-- BUG --", "-- BUG --")).keyName();
				needsemicolon = true;
				break;
				}

			case NVBToken::Goto : {

				needsemicolon = false;

				switch (static_cast<NVBGotoToken*>(tokens.at(i))->condition) {
					case NVBGotoToken::None : {
						s += "?!";
						break;
						}

					case NVBGotoToken::Stop : {
						return s;
						break;
						}

					case NVBGotoToken::IsTopo : {
						s += "?T";
						break;
						}

					case NVBGotoToken::IsSpec : {
						s += "?S";
						break;
						}

					case NVBGotoToken::HasNAxes : {
						s += QString("?=%1").arg(static_cast<NVBGotoToken*>(tokens.at(i))->n);
						break;
						}

					case NVBGotoToken::HasAtLeastNAxes : {
						s += QString("?>%1").arg(static_cast<NVBGotoToken*>(tokens.at(i))->n - 1);
						break;
						}

					case NVBGotoToken::HasAtMostNAxes : {
						s += QString("?<%1").arg(static_cast<NVBGotoToken*>(tokens.at(i))->n + 1);
						break;
						}
					}

				int gt = static_cast<NVBGotoToken*>(tokens.at(i))->gototrue;
				int gf = static_cast<NVBGotoToken*>(tokens.at(i))->gotofalse;

				if (gt == gf) {
					i += gt;
					break;
					}
				else if (gt > gf) {
					if (tokens.at(i + gt - 1)->type == NVBToken::Goto) {
						iftext = readFromTo( i + gt, i + gt + static_cast<NVBGotoToken*>(tokens.at(i + gt - 1))->gototrue - 1, tokens);
						elsetext = readFromTo( i + gf, i + gt - 1, tokens);
						i += gt + static_cast<NVBGotoToken*>(tokens.at(i + gt - 1))->gototrue - 1;
						}
					else {
						iftext = "";
						elsetext = readFromTo( i + gf, i + gt, tokens);
						i += gt;
						}
					}
				else {
					if (tokens.at(i + gf - 1)->type == NVBToken::Goto) {
						iftext = readFromTo( i + gt, i + gf - 1, tokens);
						elsetext = readFromTo( i + gf, i + gf + static_cast<NVBGotoToken*>(tokens.at(i + gf - 1))->gototrue - 1, tokens);
						i += gf + static_cast<NVBGotoToken*>(tokens.at(i + gf - 1))->gototrue - 1;
						}
					else {
						iftext = readFromTo(i + gt, i + gf, tokens);
						elsetext = "";
						i += gf;
						}
					}

				if (elsetext.isEmpty())
					s += iftext + "|";
				else
					s += iftext + ":" + elsetext + "|";

				break;
				}

			default : {
				break;
				}
			}
		}

	return s;
	}

QString NVBTokenList::compactTokens() const {
	return readFromTo(0, data->tokens.size(), data->tokens);
	}

QList< NVBToken * > NVBTokenList::tokenizeSubString( QString s, int & pos) {
	QList< NVBToken * > result;
	QList< NVBToken * > trueclause;
	QList< NVBToken * > falseclause;

	static QRegExp param("[^?|;:\"]+");
	static QRegExp axis("\\(([1-9][0-9]*|[^\\)]+)\\)([^?|;:\"]+)");
	static QRegExp user("\"([^\"]*)\"");

	bool inif = false;
	bool inelse = false;

	while (pos != s.length()) {
		switch (s[pos].toLatin1()) {
			case ';' : {
				pos += 1;
				break;
				}

			case '?' : {
				inif = true;
				inelse = false;

				switch (s[pos + 1].toLatin1()) {
					case 'S' : {
						result << new NVBGotoToken(NVBGotoToken::IsSpec);
						break;
						}

					case 'T' : {
						result << new NVBGotoToken(NVBGotoToken::IsTopo);
						break;
						}

					case '=' : {
						result << new NVBGotoToken(NVBGotoToken::HasNAxes);
						pos += 2;
						static_cast<NVBGotoToken*>(result.last())->n = getUInt(s, pos);
						pos -= 2;
						break;
						}

					case '>' : {
						result << new NVBGotoToken(NVBGotoToken::HasAtLeastNAxes);
						pos += 2;
						static_cast<NVBGotoToken*>(result.last())->n = getUInt(s, pos) + 1;
						pos -= 2;
						break;
						}

					case '<' : {
						result << new NVBGotoToken(NVBGotoToken::HasAtMostNAxes);
						pos += 2;
						static_cast<NVBGotoToken*>(result.last())->n = getUInt(s, pos) - 1;
						pos -= 2;
						break;
						}

					case '!' : {
						result << new NVBGotoToken(NVBGotoToken::None);
						break;
						}

					default : {
						NVBOutputError(QString("Error in string %1 : invalid conditional %2 at %3").arg(s).arg(s[pos + 1]).arg(pos + 1));
						return QList< NVBToken * >();
						}
					}

				pos += 2;
				trueclause = tokenizeSubString(s, pos);
				break;
				}

			case ':' : {
				if (inelse) {
					NVBOutputError(QString("Error in string %1 : double else at %2").arg(s).arg(pos));
					return QList< NVBToken * >();
					}
				else if (inif) {
					inelse = true;
					pos += 1;
					falseclause = tokenizeSubString(s, pos);
					}
				else {
					return result;
					}

				break;
				}

			case '|' : {
				if (!inif) return result;

				inif = false;
				pos += 1;

				if (!inelse || falseclause.isEmpty()) {
					if (trueclause.isEmpty()) {
						result.removeLast();
						}
					else {
						static_cast<NVBGotoToken*>(result.last())->gototrue = 1;
						static_cast<NVBGotoToken*>(result.last())->gotofalse = trueclause.size() + 1;
						result << trueclause;
						}
					}
				else {
					if (trueclause.isEmpty()) {
						static_cast<NVBGotoToken*>(result.last())->gototrue = falseclause.size() + 1;
						static_cast<NVBGotoToken*>(result.last())->gotofalse = 1;
						result << falseclause;
						}
					else {
						static_cast<NVBGotoToken*>(result.last())->gototrue = 1;
						static_cast<NVBGotoToken*>(result.last())->gotofalse = trueclause.size() + 2;
						result << trueclause << new NVBGotoToken(NVBGotoToken::None, falseclause.size() + 1);
						result << falseclause;
						}
					}

				inelse = false;
				break;
				}

			case '(' : {
				if (axis.indexIn(s, pos) == pos) {
					NVBAxisParamToken::NVBAxisParam k;

					if ((k = findAxisParam(axis.cap(2))) != NVBAxisParamToken::Invalid) {
						bool ok;
						int ix = axis.cap(1).toInt(&ok);

						if (ok)
							result << new NVBAxisParamToken(ix, findAxisParam(axis.cap(2)));
						else
							result << new NVBAxisParamToken(axis.cap(1), findAxisParam(axis.cap(2)));

						pos += axis.matchedLength();
						break;
						}
					}
				}

			default : {
				if (user.indexIn(s, pos) == pos) {
					result << new NVBVerbatimToken(user.cap(1));
					pos += user.matchedLength();
					}
				else if (param.indexIn(s, pos) == pos) {
					NVBDataParamToken::NVBDataParam k;
					NVBFileParamToken::NVBFileParam l;

					if ((k = findDataParam(param.cap(0))) != NVBDataParamToken::Invalid)
						result << new NVBDataParamToken(k);
					else if (( l = findFileParam(param.cap(0))) != NVBFileParamToken::Invalid)
						result << new NVBFileParamToken(l);
					else
						result << new NVBPCommentToken(param.cap(0));

					pos += param.matchedLength();
					}
				else {
					NVBOutputError(QString("Unrecognized token in string %1 at %2").arg(s).arg(pos));
					return result;
					}

				break;
				}
			}
		}

	if (inif) {
		if (!inelse || falseclause.isEmpty()) {
			if (trueclause.isEmpty()) {
				delete result.takeLast();
				}
			else {
				static_cast<NVBGotoToken*>(result.last())->gototrue = 1;
				static_cast<NVBGotoToken*>(result.last())->gotofalse = trueclause.size() + 1;
				result << trueclause;
				}
			}
		else {
			if (trueclause.isEmpty()) {
				static_cast<NVBGotoToken*>(result.last())->gototrue = falseclause.size() + 1;
				static_cast<NVBGotoToken*>(result.last())->gotofalse = 1;
				result << falseclause;
				}
			else {
				static_cast<NVBGotoToken*>(result.last())->gototrue = 1;
				static_cast<NVBGotoToken*>(result.last())->gotofalse = trueclause.size() + 2;
				result << trueclause << new NVBGotoToken(NVBGotoToken::None, falseclause.size() + 1);
				result << falseclause;
				}
			}
		}

	return result;

	}

NVBTokenList::NVBTokenList( QString s ) { data = new NVBTokenListData(s); }

QString NVBTokenList::verboseString() const {
	if (data->tokens.isEmpty()) return QString("");

	if (data->tokens.size() > 3 || (data->tokens.first()->type != NVBToken::Goto && data->tokens.size() > 1) )
		return QString("Expert: \"%1\"").arg(data->source);

	QString prefix;

	if (data->tokens.first()->type == NVBToken::Goto)
		switch (static_cast<NVBGotoToken*>(data->tokens.first())->condition) {
			case NVBGotoToken::None : {
				return QString("Nothing");
				}

			case NVBGotoToken::Stop : {
				return QString("Empty");
				}

			case NVBGotoToken::IsSpec : {
				prefix = "Spectroscopy: ";
				break;
				}

			case NVBGotoToken::IsTopo : {
				prefix = "Topography: ";
				break;
				}

			case NVBGotoToken::HasAtLeastNAxes : {
				prefix = QString("Has >= %1 axes : ").arg(static_cast<NVBGotoToken*>(data->tokens.first())->n);
				break;
				}

			case NVBGotoToken::HasAtMostNAxes : {
				prefix = QString("Has <= %1 axes : ").arg(static_cast<NVBGotoToken*>(data->tokens.first())->n);
				break;
				}

			case NVBGotoToken::HasNAxes : {
				prefix = QString("Has %1 axes : ").arg(static_cast<NVBGotoToken*>(data->tokens.first())->n);
				break;
				}

			default : {
				prefix = "(): ";
				break;
				}
			}

	if (data->tokens.size() == 3) {
		if (data->tokens.at(1)->type != NVBToken::Goto)
			return QString("Expert: \"%1\"").arg(data->source);

		switch (static_cast<NVBGotoToken*>(data->tokens.at(1))->condition) {
			case NVBGotoToken::None : {
				return QString("Nothing");
				}

			case NVBGotoToken::Stop : {
				return QString("Empty");
				}

			case NVBGotoToken::IsSpec : {
				prefix += "Spectroscopy: ";
				break;
				}

			case NVBGotoToken::IsTopo : {
				prefix += "Topography: ";
				break;
				}

			case NVBGotoToken::HasAtLeastNAxes : {
				prefix += QString("If >= %1 axes : ").arg(static_cast<NVBGotoToken*>(data->tokens.at(1))->n);
				break;
				}

			case NVBGotoToken::HasAtMostNAxes : {
				prefix += QString("If <= %1 axes : ").arg(static_cast<NVBGotoToken*>(data->tokens.at(1))->n);
				break;
				}

			case NVBGotoToken::HasNAxes : {
				prefix += QString("If %1 axes : ").arg(static_cast<NVBGotoToken*>(data->tokens.at(1))->n);
				break;
				}

			default : {
				prefix += "(): ";
				break;
				}
			}
		}

	switch (data->tokens.last()->type) {
		case NVBToken::Verbatim :
			return prefix + QString("Text: \"%1\"").arg(static_cast<NVBVerbatimToken*>(data->tokens.last())->sparam);

		case NVBToken::DataComment :
			return prefix + static_cast<NVBPCommentToken*>(data->tokens.last())->sparam;

		case NVBToken::FileParam :
			return prefix + fileParamNames.value((static_cast<NVBFileParamToken*>(last())->fparam), NVBDescrPair("", "Bug!!!")).verbName() + " of the file";

		case NVBToken::DataParam :
			return prefix +  dataParamNames.value((static_cast<NVBDataParamToken*>(last())->pparam), NVBDescrPair("", "Bug!!!")).verbName() + " of dataset";

		case NVBToken::AxisParam : {
			NVBAxisParamToken * at = static_cast<NVBAxisParamToken*>(last());
			QString axisname = QString("axis %1").arg(at->nparam.isEmpty() ? ("#" + QString::number(at->ixparam)) : at->nparam);

			if (at->aparam == NVBAxisParamToken::Exists) // grammar...
				return prefix + "Has " + axisname;

			return prefix +  axisParamNames.value(at->aparam, NVBDescrPair("", "Bug!!!")).verbName() + " of " + axisname;
			}

		case NVBToken::Goto : {
			return QString();
			}

		default : return QString();
		}
	}

QList< NVBToken * > NVBTokenList::tokenizeString( QString s ) {
	if (s.isEmpty()) return QList< NVBToken * >();

	int a = 0;
	return tokenizeSubString(s, a); // << new NVBGotoToken(NVBGotoToken::Stop);
	}

QString NVBTokenList::verboseString( QString s ) {
	NVBTokenList l(s);
	return l.verboseString();
	}

// The constructor has to be here to resolve circular dependence
NVBTokenListData::NVBTokenListData( QString s ): source(s), tokens(NVBTokenList::tokenizeString(s)) {}

NVBTokenListData::NVBTokenListData( QList< NVBToken * > t ): source(NVBTokenList::compactTokens(t)), tokens(t) {}


NVBTokenList::NVBTokenList( QList< NVBToken * > new_tokens) {
// TODO sanity check on new_tokens
	data = new NVBTokenListData(new_tokens);
	}

QString NVBTokens::NVBTokenList::compactTokens( QList< NVBToken * > t) {
	return readFromTo(0, t.size(), t);
	}

unsigned int NVBTokens::NVBTokenList::getUInt(QString s, int & pos) {
	int k = pos;

	for(; pos < s.length() && s[pos].isDigit(); pos += 1) {;}

	return s.mid(k, pos - k).toUInt();
	}




