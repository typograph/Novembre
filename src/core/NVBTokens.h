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
 * \code
 * <format> ::= <expr-fin> | <expr-cond-fin>
 * 
 * <expr-fin> ::= <expr-token> | <expr-cond>
 * <expr-cond> ::= [ <expr-fin> ] <conditional> [';'*]
 * <expr-token> ::= [ <expr-cond> | <expr-token> ';' ] <token> [';'*]
 * <expr-cond-fin> ::= [ <expr-fin> ] <conditional-fin>
 * 
 * <conditional> ::= '?' <cond_char> [ <expr-fin> ':'] <expr-fin> '|'
 * <conditional-fin> ::= '?' <cond_char> [ <expr-fin> ':'] <expr-cond-fin>]
 * <cond_char> ::= 'S' | 'T' | '=' <number> | '<' <number> | '>' <number> | '!'
 * 
 * <token> ::= <file_param> | <data_param> | <axis_selector_param> | <comment_key> | <user_text>
 * <file_param> ::= 'fileATime' | 'fileMTime' | 'fileCTime' | 'fileName' | 'fileNames' | 'fileSize' | 'nPages'
 * <data_param> ::= 'dataName' | 'dataSize' | 'dataUnits' | 'dataNAxes' | 'dataIsTopo' | 'dataIsSpec'
 * <axis_selector_param> ::= ['(' <axis_name> ')' | '(' <axis_index> ')'] <axis_param>
 * <axis_name> ::= <text-without-closing-brace>
 * <axis_index> ::= <number>
 * <axis_param> ::= 'axisName' | 'axisLength' | 'axisSpan' | 'axisUnits'
 * <comment_key> ::= <text-without-?|;:>
 * <user_text> ::= '"' <text-without-double-quotes> '"'
 * \endcode
 *
 * In simpler terms, the string consists of simple, semicolon-separated parameter
 * tokens and conditional expressions in form of a ternary operator '?:'. 
 * 
 * -# File parameters
 *   - fileATime : File access time
 *   - fileMTime : File modification time
 *   - fileCTime : File creation time
 *   - fileName  : File name
 *   - fileNames : Files on disk
 *   - fileSize  : File size
 *   - nPages    : Number of datasets
 * -# Dataset parameters
 *   - dataName  : Dataset name
 *   - dataSize  : Data dimensions
 *   - dataUnits : Data units
 *   - dataNAxes : Number of axes
 *   - dataIsTopo : Toporaphy data bool
 *   - dataIsSpec : Spectroscopy data bool
 * -# Axis parameters
 *   - axisName   : axis name
 *   - axisLength : axis length
 *   - axisSpan   : axis length in units
 *   - axisUnits  : axis units
 * -# Conditions
 *   - S : True for spectroscopy datasets
 *   - T : True for topography datasets
 *   - =n : True for datasets with n axes
 *   - >n : True for datasets with more than n axes
 *   - \<n : True for datasets with less that n axes
 * 
 * The format string is tokenized by NVBTokenList, which is later processed
 * by NVBFileInfo to produce a NVBVariantList. This list is translated to a string
 * for output in browser.
 */

namespace NVBTokens {

/**
 * \struct NVBToken
 * 
 * A token is a building block of a column format string. 
 * It normaly translates to a NVBVariant in the final output
 * (with the exception of the NVBGotoToken).
 * 
 */
/// A base class for tokens.
struct NVBToken {
	enum NVBTokenType {
		Invalid = 0, ///< Invalid token
		Verbatim,    ///< User string token
		FileParam,   ///< A parameter of the file
		DataParam,   ///< A parameter of the dataset
		AxisParam,   ///< A parameter of a dataset axis
		DataComment, ///< A comment in the dataset/datasource/file 
		Goto         ///< A conditional token
		} type; ///< The type of this token.
	NVBToken(NVBTokenType t): type(t) {;}
	};

/**
 * \struct NVBVerbatimToken
 * 
 * This token translates to a user-defined string in the output.
 * This token is generated for a double-quoted string in the format string.
 * 
 */
/// A token to produce a fixed string
struct NVBVerbatimToken : NVBToken {
	/// The string that is produced.
	QString sparam;
	NVBVerbatimToken(QString s): NVBToken( Verbatim ), sparam(s) {;}
	};

/**
 * \struct NVBPCommentToken
 * 
 * This token translates to the value of the comment with key \a sparam,
 * belonging to the dataset, datasource or the file.
 * 
 * Any non-recognized part of the format string is considered to be a comment key.
 */
/// A token to query a comment value
struct NVBPCommentToken : NVBToken {
	/// The comment key.
	QString sparam;
	NVBPCommentToken(QString s): NVBToken( DataComment ), sparam(s) {;}
	};

/**
 * \struct NVBGotoToken
 * 
 * This token does not produce any output. Rather, it directs the parser
 * to skip parts of the token list depending on the dataset parameters.
 * 
 * A GOTO token is generated when a '?' appears in the format string, if
 * the following character is a legal condition character. Otherwise , an error is generated.
 * 
 * Note that the checks are for more or less than n axes are implemented
 * as checks for at least and at most n axes.
 */
/// A token for conditional output.
struct NVBGotoToken : NVBToken {
	enum NVBCondType {
	  None = 0, ///< 'Always true' condition
	  Stop,     ///< Stop processing the list
	  IsSpec,   ///< True for spectroscopy pages
	  IsTopo,   ///< True for topography pages
	  HasNAxes, ///< True for pages with exactly \a n axes
	  HasAtLeastNAxes, ///< True for pages with at least \a n axes
	  HasAtMostNAxes   ///< True for pages with at most \a n axes
	} condition; ///< The condition governing this GOTO.
	/// The desired number of axes.
	int n;
	/// If \a condition is true, go forward by this amount (default 1)
	unsigned int gototrue;
	/// If \a condition is false, go forward by this amount (default 1)
	unsigned int gotofalse;

	/// Constructs a GOTO token. Defaults to a token with no effect
	NVBGotoToken(NVBCondType c, int t = 1, int f = 1): NVBToken(Goto), condition(c), gototrue(t), gotofalse(f) {;}
	};

/**
 * \struct NVBFileParamToken
 * 
 * This token produces a NVBVariant corresponding to the designated file parameter.
 * 
 * It is generated by a recognized file parameter string.
 */
/// A token for a file parameter
struct NVBFileParamToken : NVBToken {
	enum NVBFileParam {
		Invalid = 0, ///< Invalid parameter
		FileName,    ///< Processed name of the file (as defined by the NVBFileGenerator) (QString)
		FileNames,   ///< Names of the files on disk that define this file object (QString)
		FileSize,    ///< Size of the files on disk (int)
		FileATime,   ///< Access time for the files on disk (the last acccess) (QDateTime)
		FileMTime,   ///< Modification time for the files on disk (the last modification) (QDateTime)
		FileCTime,   ///< Creation time for the files on disk (the earliest acccess) (QDateTime)
		NPages       ///< Number of datasources in file (int)
	} fparam; ///< Parameter type
	/// Constructs a fileParam token of type \a f
	NVBFileParamToken(NVBFileParam f): NVBToken(FileParam), fparam(f) {;}
	};

/**
 * \struct NVBDataParamToken
 * 
 * This token produces a NVBVariant corresponding to the designated dataset parameter.
 * 
 * It is generated by a recognized dataset parameter string.
 */
/// A token for a dataset parameter
struct NVBDataParamToken : NVBToken {
	enum NVBDataParam {
		Invalid = 0, ///< Invalid parameter
		Name,        ///< Dataset name (QString)
		DataSize,    ///< Dataset data size (size_t)
		Units,       ///< Data units (NVBUnits)
		NAxes,       ///< Number of axes (axisindex_t)
		IsTopo,      ///< type == NVBDataSet::Topography (bool)
		IsSpec       ///< type == NVBDataSet::Spectroscopy (bool)
		} pparam; ///< Parameter type
	/// Constructs a dataParam token of type \a p
	NVBDataParamToken(NVBDataParam p): NVBToken(DataParam), pparam(p) {;}
	};

/**
 * \struct NVBAxisParamToken
 * 
 * This token produces a NVBVariant corresponding to the designated axis parameter
 * for a specific axis
 * 
 * It is generated by a recognized dataset parameter string, preceded with the 
 * axis name/index in round brackets.
 */
/// A token for an axis parameter
struct NVBAxisParamToken : NVBToken {
	enum NVBAxisParam {
		Invalid = 0, ///< Invalid parameter
		Exists,      ///< Axis exists (boolean)
		Name,        ///< Name of the axis (QString)
		Length,      ///< Length of the axis (axissize_t)
		Units,       ///< Associated units, if any (NVBUnits)
		Span         ///< Length of the axis (NVBPhysValue)
		} aparam; ///< Parameter type
	/// Name of the axis, if specified
	QString nparam;
	/// Axis index in dataset if specified
	int ixparam;
	/// Constructs a token type \a a for an axis with name \a name
	NVBAxisParamToken(QString name, NVBAxisParam a): NVBToken(AxisParam), aparam(a), nparam(name) {;}
	/// Constructs a token type \a a for an axis at index \a index
	NVBAxisParamToken(int index, NVBAxisParam a): NVBToken(AxisParam), aparam(a), ixparam(index) {;}
	};

/**
 * \class NVBTokenListData
 * 
 * This class keeps a string together with the associated list of tokens.
 * It is used for shallow copying of NVBTokenList.
 * 
 */
/// Internal data of NVBTokenList
class NVBTokenListData : public QSharedData {
	public:
		/// The format string
		QString source;
		/// The list of tokens as parsed from the format string
		QList< NVBToken * > tokens;

		/// Parses the string and constructs an NVBTokenListData.
		NVBTokenListData(QString s);
		/// Translates the list of tokens into a format string and constructs an NVBTokenListData
		NVBTokenListData(QList< NVBToken * > t);
		Q_DISABLE_COPY(NVBTokenListData)

		/// Deconstructs the NVBTokenListData and deletes the list.
		~NVBTokenListData() { while (!tokens.isEmpty()) delete tokens.takeLast(); }

	};

/**
 * \class NVBDescrPair
 * 
 * This class is used for associating token type values with the string representation
 * and description.
 * 
 */
/// A pair of QString-QString values
class NVBDescrPair : private QPair<QString, QString> {

	private:
		NVBDescrPair();
	public:
		/// Constructs a key-value pair
		NVBDescrPair(QString key, QString name): QPair<QString, QString>(key, name) {;}

		/// Returns the key
		QString keyName() const { return first; }
		/// Returns the value
		QString verbName() const { return second; }

		/// Compares two NVBDescrPair - only the key has to match
		bool operator==(const NVBDescrPair & other) const { return other.keyName() == keyName(); }
		/// Compares two NVBDescrPair - only the key has to match
		bool operator!=(const NVBDescrPair & other) const { return other.keyName() != keyName(); }

	};

/**
 * \class NVBTokenList
 * 
 * The NVBTokenList class contains the list of NVBToken generated
 * from a parsed format string. It can be constructed either from a format string
 * or a given list of tokens. Its data is accessible either as the format string
 * (sourceString()), a recompiled format string (compactTokens())
 * or as a verbose representation of the string (verboseString()).
 * The list itself can be accessed on a per-element basis using first(),
 * at() and last().
 */
/// A list of tokens parsed from a format string
class NVBTokenList {

//   QList< NVBToken * > tokens;
	/// The internal data
		QSharedDataPointer<NVBTokenListData> data;

	public:

		/// Parses the string \a s and constructs the token list
		NVBTokenList(QString s = QString());
		/// Constructs the token list from the list of tokens
		NVBTokenList(QList< NVBToken * >);
		~NVBTokenList() {;}

		/// Returns the original format string
		QString sourceString() const { return data->source; }
		/// Converts the list to a format string
		QString compactTokens() const;
		/// Converts the list to its verbose description
		QString verboseString() const;

		/// Returns the first token
		NVBToken* first() const { return data->tokens.first(); }
		/// Returns the last token
		NVBToken* last() const { return data->tokens.last(); }
		/// Returns the token at the index \a i
		NVBToken* at(int i) const { return data->tokens.at(i); }
		/// Returns the length of the list
		int size() const { return data->tokens.size(); }

		/// Parses the string \a s and returns a list of tokens
		static QList<NVBToken*> tokenizeString( QString s );
		/// Parses the string \a s and returns a verbose representation of it
		static QString verboseString( QString s );
		/// Converts a list of tokens to corresponding format string
		static QString compactTokens(QList<NVBToken*>);

//   void debug();

		/// Converts the file parameter enum value to string representation
		static QString nameFileParam(NVBFileParamToken::NVBFileParam);
		/// Converts the data parameter enum value to string representation
		static QString nameDataParam(NVBDataParamToken::NVBDataParam);
		/// Converts the axis parameter enum value to string representation
		static QString nameAxisParam(NVBAxisParamToken::NVBAxisParam);


	private:

		/// Mapping of file parameter enum values to key-description representation
		static QMap<NVBFileParamToken::NVBFileParam, NVBDescrPair > fileParamNames;
		/// Initializes the mapping of file parameter enum values to key-description representation
		static QMap<NVBFileParamToken::NVBFileParam, NVBDescrPair > initFileParamNames();
		/// Converts the file parameter string representation to enum value
		static NVBFileParamToken::NVBFileParam findFileParam(QString key);
		/// Mapping of data parameter enum values to key-description representation
		static QMap<NVBDataParamToken::NVBDataParam, NVBDescrPair > dataParamNames;
		/// Initializes the mapping of data parameter enum values to key-description representation
		static QMap<NVBDataParamToken::NVBDataParam, NVBDescrPair > initDataParamNames();
		/// Converts the data parameter string representation to enum value
		static NVBDataParamToken::NVBDataParam findDataParam(QString key);
		/// Mapping of axis parameter enum values to key-description representation
		static QMap<NVBAxisParamToken::NVBAxisParam, NVBDescrPair > axisParamNames;
		/// Initializes the mapping of axis parameter enum values to key-description representation
		static QMap<NVBAxisParamToken::NVBAxisParam, NVBDescrPair > initAxisParamNames();
		/// Converts the axis parameter string representation to enum value
		static NVBAxisParamToken::NVBAxisParam findAxisParam(QString key);

		/// Converts a part of a list of tokens to a format string
		static QString readFromTo( int start, int end, const QList< NVBToken * > & tokens);
		/// Converts a format substring to a list of tokens
		static QList< NVBToken * > tokenizeSubString( QString s, int & pos );
		/// Parses an unsigned integer from string \a s at position \s pos and advances the position
		static unsigned int getUInt(QString s, int & pos);
	};

};

Q_DECLARE_METATYPE(NVBTokens::NVBTokenList);

#endif
