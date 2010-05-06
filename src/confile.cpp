/***************************************************************************
 *   Copyright (C) 2006 by Timofey Balashov                                *
 *   Timofey.Balashov@pi.uka.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/
#include "confile.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>

char * readSStop( FILE* rf, char stopchar) {
	char * str;
	char c;
	int slen = -1, nblocks = 0;
	
	str = (char *)malloc(BLOCKL*++nblocks);
	
	do {
		fread(&c,1,1,rf);
		str[++slen] = c;
		if (slen+SICHERL > BLOCKL*nblocks) {
			str = (char *)realloc(str,BLOCKL*++nblocks);
			}
		}
	while (c != stopchar);

	str[slen] = '\0';
	return str;
}


void skipline( FILE* rf) {
	char c;
	do
		fread(&c,1,1,rf);
	while (c != '\n');
}

/* --------------- TConSect --------------- */

TConSect::TConSect(const char * aname) {
	
 #ifdef DEBUG_SHOW_CONFILEMESSAGES 
	NVBOutputPMsg("TConSect::TConSect","New section %s added",aname);
 #endif 

	this->name = strdup(aname);
	next = NULL;
	hTree = NULL;
	tTree = &hTree;
}

TConSect::TConSect(QString aname) {
  
 #ifdef DEBUG_SHOW_CONFILEMESSAGES 
  NVBOutputPMsg("TConSect::TConSect","New section %s added",aname);
 #endif 

  this->name = strdup(aname.toLatin1().data());
  next = NULL;
  hTree = NULL;
  tTree = &hTree;
}

TConSect::~TConSect() {

	PNVPair cTree;

	free(name);
	while ((cTree = hTree)) {
		hTree = hTree->next;
		free(cTree);
		}
	// free everything;
}

void TConSect::add( const char * name, const char * value)
{
//	fprintf(stderr,"Section %s: Adding %s with value %s\n",this->name,name,value);

	PNVPair cPair = getPair(name);

	if (cPair) {
		free(cPair->value);
		cPair->value = strdup(value);
		}
	else {
		(*tTree) = (PNVPair)malloc(sizeof(TNVPair));

		if (!(*tTree)) {
			NVBOutputError("TConSect::add","Not enough memory");
			return;
			}

		(*tTree)->name = strdup(name);
		(*tTree)->value = strdup(value);
    (*tTree)->next = NULL;  

		tTree = &((*tTree)->next);
		}
}

void TConSect::add(const char * str) {
	int i = 0;
	char * string = strdup(str);

 #ifdef DEBUG_SHOW_CONFILEMESSAGES 
	NVBOutputPMsg("TConSect : add", "adding string %s", string);
 #endif 

	while ( string[i] && string[i] != ' ' && string[i] != '=') i++;
	if (string[i] == '\0') {
		NVBOutputError("TConSect : add", "No spaces or equal signs in option string" );
		return;
		}
	string[i++] = '\0';

	PNVPair cPair = getPair(string);

	if (!cPair) {
		(*tTree) = (PNVPair)malloc(sizeof(TNVPair));
		(*tTree)->name = NULL;
		(*tTree)->value = NULL;
		(*tTree)->name = strdup(string);
    (*tTree)->next = NULL;

		cPair = (*tTree);
		tTree = &((*tTree)->next);
		}
	else
		free(cPair->value);

	while ( string[i] == ' ' || string[i] == '=') i++;
	if (string[i] == '\"') {
		int j = ++i;
		while (string[i] != '\"' && string[i] != '\0') i++;
		if (string[i]) {
			string[i] = '\0';
			cPair->value = strdup(string + j);
			}
		else
			NVBOutputError("TConSect : add","Unexpected end of string");
		}
	else {
		int j = i;
		while (string[i] != ' ' && string[i] != '\r' && string[i] != '\n' && string[i] != '\0') i++;
		string[i] = '\0';
		cPair->value = strdup(string + j);
		}
	
	free(string);
}

bool TConSect::hasPair( const char * sname )
{
	return (bool)getPair(sname);
/*
	PNVPair cPair = hTree;
	while (cPair) {
		if (strcmp(sname,cPair->name) == 0)
			return true;
		cPair = cPair->next;
		}
	return false;	
*/
}

PNVPair TConSect::getPair( const char * sname )
{
	PNVPair cPair = hTree;
	while (cPair) {
		if (strcmp(sname,cPair->name) == 0)
			return cPair;
		cPair = cPair->next;
		}
	return NULL;	
}

void TConSect::add( const char * name, int svalue) {
  char c[20];
  sprintf(c,"%d",svalue);
  add(name,c);
}

void TConSect::add( const char * name, bool svalue) {
  add(name,svalue?"yes":"no");
}

void TConSect::add( const char * name, double svalue) {
  char c[100];
  add(name,sprintf(c,"%f",svalue));
}

bool TConSect::setValue( const char * sname , const char * svalue)
{
	PNVPair cPair = getPair(sname);
	if (cPair) {
		free(cPair->value);
		cPair->value = strdup(svalue);
		return true;
		}
	else
		return false;
}

bool TConSect::setValue( const char * sname , int svalue)
{
	char c[20];
  sprintf(c,"%d",svalue);
	return setValue(sname,c);
}

bool TConSect::setValue( const char * sname , bool svalue)
{
	return setValue(sname,svalue?"yes":"no");
}

bool TConSect::setValue( const char * sname , double svalue)
{
	char c[100];
	return setValue(sname,sprintf(c,"%f",svalue));
}

char * TConSect::getValue( const char * sname )
{
	PNVPair cPair = getPair(sname);
	if (cPair)
		return strdup(cPair->value);
	else
		return NULL;
}

const char * TConSect::getStrValue( const char * sname )
{
	PNVPair cPair = getPair(sname);
	if (cPair)
		return cPair->value;
	else
		return NULL;
}

int TConSect::getIntValue( const char * sname )
{
	char * c = getValue(sname);
	if (c) {
		errno = 0;
		int i = strtol(c,(char **)NULL,0);
		if (errno) 
			NVBOutputError("TConSect : getIntValue", "Conversion to int failed : %s is not a valid integer", c);
		free(c);
		return i;
		}
	else {
		NVBOutputError("TConSect : getIntValue", "The parameter %s does not exist in section %s",sname,this->name);
		return 0;
		}
}

double TConSect::getDoubleValue( const char * sname )
{
	char * c = getValue(sname);
	if (c) {
		errno = 0;
		double i = strtod(c,(char **)NULL);
		if (errno) 
			NVBOutputError("TConSect : getDoubleValue", "Conversion to double failed : %s is not a valid floting-point number",c);
		free(c);
		return i;
		}
	else {
    NVBOutputError("TConSect : getDoubleValue", "The parameter %s does not exist in section %s",sname,this->name);
		return 0;
		}
}

bool TConSect::getBoolValue( const char * sname )
{
	char * c = getValue(sname);
	if (c) {
		switch (strlen(c)) {
			case 1: {
				switch (c[0]) {
					case '0':
					case 'n':
					case 'N':
					case 'f':
					case 'F': {
						free(c);
						return false;
						}
					case '1':
					case 'y':
					case 'Y':
					case 't':
					case 'T': {
						free(c);
						return true;
						}
					} 
				}
			case 2: {
				if (strcasecmp(c,"no")==0) { free(c); return false; }
				}
			case 3: {
				if (strcasecmp(c,"yes")==0) { free(c); return true; }
				}
			case 4: {
				if (strcasecmp(c,"true")==0) { free(c); return true; }
				}
			case 5: {
				if (strcasecmp(c,"false")==0) { free(c); return false; }
				}
			default: {
				NVBOutputError("TConSect : getBoolValue","Conversion to bool failed : %s is not a valid boolean",c);
				free (c);
				return false;
				}
			}
		}
	else {
    NVBOutputError("TConSect : getBoolValue", "The parameter %s does not exist in section %s",sname,this->name);
		return false;
		}
}

void TConSect::save( FILE * wf)
{
	if (wf) {
			PNVPair cTree = hTree;
			fprintf(wf,"[%s]\n",name);
			while (cTree) {
				if (memchr(cTree->value,' ',strlen(cTree->value)) != 0)
					fprintf(wf,"%s \"%s\"\n",cTree->name,cTree->value);
				else
					fprintf(wf,"%s %s\n",cTree->name,cTree->value);
				cTree = cTree->next;
				}
			fprintf(wf,"\n");
		}
}

/* ------------- TConFile ---------------*/

TConFile::TConFile(const char * _filename) {

	FILE* rf;
	hSect = NULL;
	tSect = &hSect;

	filename = strdup(_filename);

 #ifdef DEBUG_SHOW_CONFILEMESSAGES 
	NVBOutputPMsg("TConFile::TConFile","Opening file %s\n",filename);
 #endif 

if ((rf = fopen(filename,"r")))
  load(rf);
else {
	NVBOutputFileError("TConFile::TConFile", _filename);
  NVBOutputError("TConFile::TConFile", "Opening configuration file failed\n");
  throw nvberr_file_error;
  }
fclose(rf);
}

TConFile::TConFile(QString _filename) {

  FILE* rf;
  hSect = NULL;
  tSect = &hSect;

  filename = strdup(_filename.toLatin1().data());

 #ifdef DEBUG_SHOW_CONFILEMESSAGES 
  NVBOutputPMsg("TConFile::TConFile","Opening file %s\n",filename);
 #endif

if ((rf = fopen(filename,"r")))
  load(rf);
else {
  NVBOutputFileError("TConFile::TConFile", filename);
  NVBOutputError("TConFile::TConFile", "Opening configuration file failed\n");
  throw nvberr_file_error;
  }
fclose(rf);
}

TConSect * TConFile::getSect(const char * ssecname) {
	TConSect* cSect = hSect;
	while (cSect) {
		if (strcmp(ssecname,cSect->name) == 0)
			return cSect;
		cSect = cSect->next;
		}
	return NULL;	
}

TConFile::TConFile(FILE * rf) {
  
  hSect = NULL;
  tSect = &hSect;
  filename = NULL;
  
  if (!rf) throw nvberr_invalid_input;
  load(rf);
}

void TConFile::load(FILE * rf) {
  char c;
  while (!feof(rf)) {
    fread(&c,1,1,rf);
    switch (c) {
      case '[' : {
        if (hSect) tSect = &((*tSect)->next);
        char* c = readSStop(rf,']');
        (*tSect) = new TConSect(c);
        free(c);
        skipline(rf);
        break;
        }
      case '#' : {
        skipline(rf);
        break;
        }
      case ' ' : 
      case '\r' :
      case '\n' : {
        break;
        }
      default : {
        if (!hSect) {
          hSect = new TConSect("__blank__");
//          tSect = &(hSect->next);
          }
        fseek(rf,-1,SEEK_CUR);
        char * c = readSStop(rf,'\n');
        (*tSect)->add(c);
        free(c);
        fseek(rf,-1,SEEK_CUR);
        break;
        }
      }
    }
  if (hSect) tSect = &((*tSect)->next);

}

TConFile::~TConFile() {

  if (filename) free(filename);

	TConSect* cSect;
	while ((cSect = hSect)) {
		hSect = hSect->next;
		delete cSect;
		}
}

void TConFile::save( )
{

  if (!filename) {
    NVBOutputError("TConFile::save","This class was constructed without passing in an explicit filename. Cannot save.");
    return;
    }

	FILE* wf;

	if ((wf = fopen(filename,"w"))) {
		TConSect * cSect = hSect;
		while (cSect) {
			cSect->save(wf);
			cSect = cSect->next;
			}
		fclose(wf);
		}
	else 
		NVBOutputError("TConFile::save", "Opening configuration file for rewriting failed");

}

void TConFile::saveas( FILE * wf, bool redirect) // redirect defines changing internal filename
{
  if (!wf) throw nvberr_invalid_input;
  if (redirect && filename) {
    free(filename);
    filename = NULL;
    }
  TConSect * cSect = hSect;
  while (cSect) {
    cSect->save(wf);
    cSect = cSect->next;
    }
#ifdef DEBUG_SHOW_CONFILEMESSAGES 
  NVBOutputPMsg("TConFile::saveas","Configuration file saved");
#endif  
}

void TConFile::saveas( const char * newname, bool redirect) // redirect defines changing internal filename
{
	FILE* wf;

	if (redirect) {
		if (filename) free(filename);
		filename = strdup(newname);
		}

	if ((wf = fopen(newname,"w"))) {
		saveas(wf);
		fclose(wf);
  #ifdef DEBUG_SHOW_CONFILEMESSAGES 
		NVBOutputPMsg("TConFile::saveas","Configuration file saved");
  #endif  
		}
	else 
		NVBOutputError("TConFile::saveas","Opening configuration file %s for writing failed", newname);
	
}

TConSect * TConFile::addSect( const char * secname)
{
	TConSect * cSect = getSect(secname);
	if (!cSect) {
		cSect = ((*tSect) = new TConSect(secname));
		tSect = &((*tSect)->next);
		}
	return cSect;
}


void TConFile::remSect( const char * secname)
{
  TConSect * cSect = getSect(secname);
  if (cSect) {
    if (cSect != hSect) {
      TConSect * ttSect;
      for(ttSect = hSect; ttSect->next != cSect; ttSect = ttSect->next) {};
        ttSect->next = cSect->next;
      if (tSect == &(cSect->next))
        tSect = &(ttSect->next);
      }
    else {
      hSect = cSect->next;
      if (tSect == &(cSect->next))
        tSect = &hSect;
      }
    cSect->next = NULL;
    delete cSect;
    }
  else
    NVBOutputPMsg("TConFile::remSect","Section %s doesn't exist",secname);      
}
