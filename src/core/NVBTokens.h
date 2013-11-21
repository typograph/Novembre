//
// Copyright 2006 Timofey <typograph@elec.ru>
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


#ifndef NVBTOKENS_H
#define NVBTOKENS_H

#include <QString>
#include <QPair>
#include <QList>
#include <QMap>
#include <QMetaType>
#include <QSharedData>

namespace NVBTokens {

struct NVBToken {
	enum NVBTokenType {Invalid = 0, Verbatim, FileParam, PageParam, PageComment, Goto} type;
	NVBToken(NVBTokenType t): type(t) {;}
	};

struct NVBVerbatimToken : NVBToken {
	QString sparam;
	NVBVerbatimToken(QString s): NVBToken(Verbatim), sparam(s) {;}
	};

struct NVBPCommentToken : NVBToken {
	QString sparam;
	NVBPCommentToken(QString s): NVBToken(PageComment), sparam(s) {;}
	};

struct NVBGotoToken : NVBToken {
	enum NVBCondType {None = 0, Stop, IsSpec, IsTopo} condition;
	int gototrue;
	int gotofalse;

	NVBGotoToken(NVBCondType c, int t = -1, int f = -1): NVBToken(Goto), condition(c), gototrue(t), gotofalse(f) {;}
	};

struct NVBFileParamToken : NVBToken {
	enum NVBFileParam {Invalid = 0, FileName, FileNames, FileSize, FileATime, FileMTime, FileCTime, NPages} fparam;
	NVBFileParamToken(NVBFileParam f): NVBToken(FileParam), fparam(f) {;}
	};

struct NVBPageParamToken : NVBToken {
	enum NVBPageParam {Invalid = 0, Name, DataSize, DataSpan, IsTopo, IsSpec, XSize, YSize, XSpan, YSpan} pparam;
	NVBPageParamToken(NVBPageParam p): NVBToken(PageParam), pparam(p) {;}
	};

class NVBTokenListData : public QSharedData {
	public:
		QString source;
		QList< NVBToken * > tokens;

		NVBTokenListData(QString s);
		NVBTokenListData(QList< NVBToken * > t);
		Q_DISABLE_COPY(NVBTokenListData);
		~NVBTokenListData() {  while (!tokens.isEmpty()) delete tokens.takeLast(); }

	};

class NVBDescrPair : private QPair<QString, QString> {

	private:

	public:
		NVBDescrPair(): QPair<QString, QString>("BUG", "BUG!!!") {;}
		NVBDescrPair(QString key, QString name): QPair<QString, QString>(key, name) {;}
		NVBDescrPair(const NVBDescrPair & other): QPair<QString, QString>(other.keyName(), other.verbName()) {;}

		QString keyName() const { return first; }
		QString verbName() const { return second; }

		bool operator==(const NVBDescrPair & other) const { return other.keyName() == keyName(); }
		bool operator!=(const NVBDescrPair & other) const { return other.keyName() != keyName(); }

	};

class NVBTokenList {

//   QList< NVBToken * > tokens;
		QSharedDataPointer<NVBTokenListData> data;

	public:

		NVBTokenList(QString s = QString());
		NVBTokenList(QList< NVBToken * >);
		NVBTokenList(const NVBTokenList & other): data(other.data) {;}
		~NVBTokenList() {;}

		QString sourceString() const { return data->source; }
		QString compactTokens() const;
		QString verboseString() const;

		NVBToken* first() const { return data->tokens.first(); }
		NVBToken* last() const { return data->tokens.last(); }
		NVBToken* at(int i) const { return data->tokens.at(i); }
		int size() const { return data->tokens.size(); }


		static QList<NVBToken*> tokenizeString(QString s);
		static QString verboseString(QString s);
		static QString compactTokens(QList<NVBToken*>);

//   void debug();

		static QString nameFileParam(NVBFileParamToken::NVBFileParam);
		static QString namePageParam(NVBPageParamToken::NVBPageParam);


	private:

		static QMap<NVBFileParamToken::NVBFileParam, NVBDescrPair > fileParamNames;
		static QMap<NVBFileParamToken::NVBFileParam, NVBDescrPair > initFileParamNames();
		static NVBFileParamToken::NVBFileParam findFileParam(QString key);
		static QMap<NVBPageParamToken::NVBPageParam, NVBDescrPair > pageParamNames;
		static QMap<NVBPageParamToken::NVBPageParam, NVBDescrPair > initPageParamNames();
		static NVBPageParamToken::NVBPageParam findPageParam(QString key);

		static QString readFromTo(int start, int end, const QList< NVBToken * > & tokens);
		static QList< NVBToken * > tokenizeSubString(QString s, int & pos);
	};

};

Q_DECLARE_METATYPE(NVBTokens::NVBTokenList);

#endif
