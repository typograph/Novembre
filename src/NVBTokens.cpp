
#include "NVBTokens.h"
#include "NVBLogger.h"
#include <QRegExp>

using namespace NVBTokens;

QMap<NVBFileParamToken::NVBFileParam,NVBDescrPair > NVBTokenList::fileParamNames = NVBTokenList::initFileParamNames();

QMap<NVBFileParamToken::NVBFileParam,NVBDescrPair > NVBTokenList::initFileParamNames( )
{
  
  QMap<NVBFileParamToken::NVBFileParam,NVBDescrPair > pnames;
  
  pnames.insert(NVBFileParamToken::FileATime,NVBDescrPair("fileATime","File access time"));
  pnames.insert(NVBFileParamToken::FileMTime,NVBDescrPair("fileMTime","File modification time"));
  pnames.insert(NVBFileParamToken::FileCTime,NVBDescrPair("fileCTime","File creation time"));
  pnames.insert(NVBFileParamToken::FileName,NVBDescrPair("fileName","File name"));
	pnames.insert(NVBFileParamToken::FileNames,NVBDescrPair("fileNames","Files on disk"));
	pnames.insert(NVBFileParamToken::FileSize,NVBDescrPair("fileSize","File size"));
  pnames.insert(NVBFileParamToken::NPages,NVBDescrPair("nPages","Number of pages"));
   
//   pnames.insert(NVBFileParamToken::,NVBDescrPair("",""));

  return pnames;

}

QString NVBTokenList::nameFileParam( NVBFileParamToken::NVBFileParam p )
{
  return fileParamNames.value(p).verbName();
}

NVBFileParamToken::NVBFileParam NVBTokenList::findFileParam( QString key )
{
  return fileParamNames.key(NVBDescrPair(key,QString()));
}

QMap<NVBPageParamToken::NVBPageParam,NVBDescrPair > NVBTokenList::pageParamNames = NVBTokenList::initPageParamNames();

QMap<NVBPageParamToken::NVBPageParam,NVBDescrPair > NVBTokenList::initPageParamNames( )
{
  QMap<NVBPageParamToken::NVBPageParam,NVBDescrPair > fnames;
  
  fnames.insert(NVBPageParamToken::Name,NVBDescrPair("pageName","Page name"));
  fnames.insert(NVBPageParamToken::DataSize,NVBDescrPair("pageDataSize","Page dimensions"));
  fnames.insert(NVBPageParamToken::IsSpec,NVBDescrPair("isSpec","Page is a spectroscopy page"));
  fnames.insert(NVBPageParamToken::IsTopo,NVBDescrPair("isTopo","Page is a topography page"));
  fnames.insert(NVBPageParamToken::XSize,NVBDescrPair("XSize","X resolution"));
  fnames.insert(NVBPageParamToken::YSize,NVBDescrPair("YSize","Y resolution"));

//  fnames.insert(NVBPageParamToken::,NVBDescrPair("",""));

  return fnames;
}

NVBPageParamToken::NVBPageParam NVBTokenList::findPageParam( QString key )
{
  return pageParamNames.key(NVBDescrPair(key,QString()));
}

QString NVBTokenList::namePageParam( NVBPageParamToken::NVBPageParam p )
{
  return pageParamNames.value(p).verbName();
}

QString NVBTokenList::readFromTo( int start, int end, const QList< NVBToken * > & tokens )
{
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
      case NVBToken::PageComment : {
        if (needsemicolon) s += ';';
        s += static_cast<NVBPCommentToken*>(tokens.at(i++))->sparam;
        needsemicolon = true;
        break;
        }
      case NVBToken::PageParam : {
        if (needsemicolon) s += ';';
        s += pageParamNames.value(static_cast<NVBPageParamToken*>(tokens.at(i++))->pparam).keyName();
        needsemicolon = true;
        break;        
        }
      case NVBToken::FileParam : {
        if (needsemicolon) s += ';';
        s += fileParamNames.value(static_cast<NVBFileParamToken*>(tokens.at(i++))->fparam).keyName();
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
          }
      
        int gt = static_cast<NVBGotoToken*>(tokens.at(i))->gototrue;
        int gf = static_cast<NVBGotoToken*>(tokens.at(i))->gotofalse;     
      
        if (gt == gf) {
          i += gt;
          break;
          }
        else if (gt > gf) {
          if (tokens.at(i+gt-1)->type == NVBToken::Goto) {
            iftext = readFromTo( i + gt, i + gt + static_cast<NVBGotoToken*>(tokens.at(i+gt-1))->gototrue-1,tokens);
            elsetext = readFromTo( i+gf, i+gt-1,tokens);
            i += gt + static_cast<NVBGotoToken*>(tokens.at(i+gt-1))->gototrue-1;
            }
          else {
            iftext = "";
            elsetext = readFromTo( i+gf, i+gt,tokens);
            i += gt;
            }
          }
        else {
          if (tokens.at(i+gf-1)->type == NVBToken::Goto) {
            iftext = readFromTo( i + gt, i + gf - 1,tokens);
            elsetext = readFromTo( i + gf, i + gf + static_cast<NVBGotoToken*>(tokens.at(i+gf-1))->gototrue - 1, tokens);
            i += gf + static_cast<NVBGotoToken*>(tokens.at(i+gf-1))->gototrue-1;
            }
          else {
            iftext = readFromTo(i+gt, i+gf, tokens);
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

QString NVBTokenList::compactTokens() const
{
  return readFromTo(0,data->tokens.size(),data->tokens);
}

QList< NVBToken * > NVBTokenList::tokenizeSubString( QString s, int & pos)
{
  QList< NVBToken * > result;
  QList< NVBToken * > trueclause;
  QList< NVBToken * > falseclause;

  static QRegExp param("[^?|;:\"]+");
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
        switch (s[pos+1].toLatin1()) {
          case 'S' : {
            result << new NVBGotoToken(NVBGotoToken::IsSpec);
            break;
            }
          case 'T' : {
            result << new NVBGotoToken(NVBGotoToken::IsTopo);
            break;
            }
          case '!' : {
            result << new NVBGotoToken(NVBGotoToken::None);
            break;
            }
          default : {
						NVBOutputError(QString("Error in string %1 : invalid conditional %2 at %3").arg(s).arg(s[pos+1]).arg(pos+1));
            return QList< NVBToken * >();
            }
          }
        pos += 2;
        trueclause = tokenizeSubString(s,pos);
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
          falseclause = tokenizeSubString(s,pos);
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
            result << trueclause << new NVBGotoToken(NVBGotoToken::None,falseclause.size()+1);
            result << falseclause;
            }
          }
        inelse = false;
        break;
        }
      default : {
        if (user.indexIn(s,pos) == pos) {
          result << new NVBVerbatimToken(user.cap(1));
          pos += user.matchedLength();
          }
        else if (param.indexIn(s,pos) == pos) {
          NVBPageParamToken::NVBPageParam k;
          NVBFileParamToken::NVBFileParam l;
          if ((k = findPageParam(param.cap(0))) != NVBPageParamToken::Invalid)
            result << new NVBPageParamToken(k);
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
        result << trueclause << new NVBGotoToken(NVBGotoToken::None,falseclause.size()+1);
        result << falseclause;
        }
      }
    }
  
  return result;
  
}

NVBTokenList::NVBTokenList( QString s ) { data = new NVBTokenListData(s); }

QString NVBTokenList::verboseString() const
{
  if (data->tokens.isEmpty()) return QString("");
  
  if (data->tokens.size() > 2 || (data->tokens.first()->type != NVBToken::Goto && data->tokens.size() > 1) ) return QString("Expert: \"%1\"").arg(data->source); 
  
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
      default : {
        prefix = "(): ";
        break;
        }
      }
  
  
  switch (data->tokens.last()->type) {
    case NVBToken::Verbatim :
      return prefix + QString("Text: \"%1\"").arg(static_cast<NVBVerbatimToken*>(data->tokens.last())->sparam);
    case NVBToken::PageComment :
      return prefix + static_cast<NVBPCommentToken*>(data->tokens.last())->sparam;
    case NVBToken::FileParam :
      return prefix + fileParamNames.value((static_cast<NVBFileParamToken*>(last())->fparam),NVBDescrPair("","Bug!!!")).verbName();
    case NVBToken::PageParam :
      return prefix +  pageParamNames.value((static_cast<NVBPageParamToken*>(last())->pparam),NVBDescrPair("","Bug!!!")).verbName();
    case NVBToken::Goto : {
      return QString();
      }
    default : return QString();
    }
}

QList< NVBToken * > NVBTokenList::tokenizeString( QString s )
{
  if (s.isEmpty()) return QList< NVBToken * >();
  
  int a = 0;
  return tokenizeSubString(s,a); // << new NVBGotoToken(NVBGotoToken::Stop);
}

QString NVBTokenList::verboseString( QString s )
{
  NVBTokenList l(s);
  return l.verboseString();
}

// The constructor has to be here to resolve circular dependence
NVBTokenListData::NVBTokenListData( QString s ):source(s),tokens(NVBTokenList::tokenizeString(s)) {}

NVBTokenListData::NVBTokenListData( QList< NVBToken * > t ):source(NVBTokenList::compactTokens(t)),tokens(t) {}


NVBTokenList::NVBTokenList( QList< NVBToken * > new_tokens) {
// TODO sanity check on new_tokens
 data = new NVBTokenListData(new_tokens); 
}

QString NVBTokens::NVBTokenList::compactTokens( QList< NVBToken * > t)
{
  return readFromTo(0, t.size(), t);
}



