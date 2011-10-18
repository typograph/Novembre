
#ifndef NVBTOKENS_H
#define NVBTOKENS_H

#include <QtCore/QString>
#include <QtCore/QPair>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QMetaType>
#include <QtCore/QSharedData>

/**
 *  \namespace NVBTokens
 * 
 * This namespace unites methods and classes for working with
 * column format strings of NVBBrowser.
 * 
 * The string in question has the following format:
 *
 * format ::= expression
 * expression ::= conditional | fileParam | dataParam | axis_param | user_text
 * conditional ::= ? cond_char expression [: expression] '|'
 * cond_char ::= S | T | = number | < number | > number | !
 * user_text ::= '"' any_text '"'
 * axis_param ::= ['(' axis_name ')' | '(' axis_index ')'] axisParam
 *   
 * 1) File parameters
 * 
 * fileATime : File access time
 * fileMTime : File modification time
 * fileCTime : File creation time
 * fileName  : File name
 * fileNames : Files on disk
 * fileSize  : File size
 * nPages    : Number of datasets
 * 
 * 2) Dataset parameters
 * 
 * dataName  : Dataset name
 * dataSize  : Data dimensions
 * dataUnits : Data units
 * dataNAxes : Number of axes
 * dataIsTopo : Toporaphy data bool
 * dataIsSpec : Spectroscopy data bool
 * 
 * 3) Axis parameters
 * 
 * axisName   : axis name
 * axisLength : axis length
 * axisUnits  : axis units
 */


namespace NVBTokens {

struct NVBToken {
  enum NVBTokenType {Invalid = 0, Verbatim, FileParam, DataParam, AxisParam, DataComment, Goto} type;
  NVBToken(NVBTokenType t):type(t) {;}
};

struct NVBVerbatimToken : NVBToken {
  QString sparam;
  NVBVerbatimToken(QString s):NVBToken( Verbatim ),sparam(s) {;}
};
  
struct NVBPCommentToken : NVBToken {
  QString sparam;
  NVBPCommentToken(QString s):NVBToken( DataComment ),sparam(s) {;}
};

struct NVBGotoToken : NVBToken {
  enum NVBCondType {
		None = 0,
		Stop,
		IsSpec,
		IsTopo,
		HasNAxes,
		HasAtLeastNAxes,
		HasAtMostNAxes
	} condition;
	int n;
	/// If \a condition is true, go forward by this amount (default 1)
  unsigned int gototrue;
	/// If \a condition is false, go forward by this amount (default 1)
  unsigned int gotofalse;
  
	/// Constructs a GOTO token. Defaults to a token with no effect
  NVBGotoToken(NVBCondType c, int t = 1, int f = 1):NVBToken(Goto),condition(c),gototrue(t),gotofalse(f) {;}
};

struct NVBFileParamToken : NVBToken {
	enum NVBFileParam {Invalid = 0, FileName, FileNames, FileSize, FileATime, FileMTime, FileCTime, NPages} fparam;
  NVBFileParamToken(NVBFileParam f):NVBToken(FileParam),fparam(f) {;}
};  

struct NVBDataParamToken : NVBToken {
  enum NVBDataParam {Invalid = 0, Name, DataSize, Units, NAxes, IsTopo, IsSpec} pparam;
  NVBDataParamToken(NVBDataParam p):NVBToken(DataParam),pparam(p) {;}
};

struct NVBAxisParamToken : NVBToken {
  enum NVBAxisParam {Invalid = 0, Name, Length, Units} aparam;
	QString nparam;
	int ixparam;
  NVBAxisParamToken(QString name, NVBAxisParam a):NVBToken(AxisParam),aparam(a),nparam(name) {;}
  NVBAxisParamToken(int index, NVBAxisParam a):NVBToken(AxisParam),aparam(a),ixparam(index) {;}
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

class NVBDescrPair : private QPair<QString,QString> {

private:

public:
  NVBDescrPair():QPair<QString,QString>("BUG","BUG!!!") {;}
  NVBDescrPair(QString key, QString name):QPair<QString,QString>(key,name) {;}
  NVBDescrPair(const NVBDescrPair & other):QPair<QString,QString>(other.keyName(),other.verbName()) {;}
  
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
  NVBTokenList(const NVBTokenList & other):data(other.data) {;}
  ~NVBTokenList() {;}

  QString sourceString() const { return data->source; }
  QString compactTokens() const;
  QString verboseString() const;

  NVBToken* first() const { return data->tokens.first(); }
  NVBToken* last() const { return data->tokens.last(); }
  NVBToken* at(int i) const { return data->tokens.at(i); }
  int size() const { return data->tokens.size(); }
  

  static QList<NVBToken*> tokenizeString( QString s );
  static QString verboseString( QString s );
  static QString compactTokens(QList<NVBToken*>);

//   void debug();

  static QString nameFileParam(NVBFileParamToken::NVBFileParam);
  static QString nameDataParam(NVBDataParamToken::NVBDataParam);
  static QString nameAxisParam(NVBAxisParamToken::NVBAxisParam);


private:

  static QMap<NVBFileParamToken::NVBFileParam,NVBDescrPair > fileParamNames;
  static QMap<NVBFileParamToken::NVBFileParam,NVBDescrPair > initFileParamNames();
  static NVBFileParamToken::NVBFileParam findFileParam(QString key);
  static QMap<NVBDataParamToken::NVBDataParam,NVBDescrPair > dataParamNames;
  static QMap<NVBDataParamToken::NVBDataParam,NVBDescrPair > initDataParamNames();
  static NVBDataParamToken::NVBDataParam findDataParam(QString key);
  static QMap<NVBAxisParamToken::NVBAxisParam,NVBDescrPair > axisParamNames;
  static QMap<NVBAxisParamToken::NVBAxisParam,NVBDescrPair > initAxisParamNames();
  static NVBAxisParamToken::NVBAxisParam findAxisParam(QString key);

  static QString readFromTo( int start, int end, const QList< NVBToken * > & tokens);
  static QList< NVBToken * > tokenizeSubString( QString s, int & pos );
	static unsigned int getUInt(QString s, int & pos);
};

};
  
Q_DECLARE_METATYPE(NVBTokens::NVBTokenList);  

#endif
