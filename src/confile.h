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


#ifndef CONFILE_H
#define CONFILE_H

#include <stdio.h>
#include "NVBLogger.h"
#include <QString>

#define BLOCKL 32
#define SICHERL 8

using namespace NVBErrorCodes;

typedef struct TNVPair {
	char * name;
	char * value;
	TNVPair * next;
} TNVpair, *PNVPair;

//typedef TNVPair* PNVPair;

//class TConSect;

class TConSect {
private:
	PNVPair hTree, *tTree;
public:
	TConSect * next;
	char*	name;
		TConSect(const char *);
    TConSect(QString);
		~TConSect();
	bool setValue( const char * , int );
	bool setValue( const char * , double );
	bool setValue( const char * , bool );
	bool setValue( const char * , const char * );

	bool hasPair(const char *);
	PNVPair getPair(const char * );
	char*	getValue(const char *);
	const char*	getStrValue(const char *);
	int		getIntValue(const char *);
	double	getDoubleValue(const char *);
	bool	getBoolValue(const char *);
	void	add(const char *);
	void	add(const char*, const char*);
  void  add( const char * , int );
  void  add( const char * , double );
  void  add( const char * , bool );
	void	save(FILE*);
};

class TConFile {
private:
	TConSect *hSect;
  TConSect **tSect;
  
  void load(FILE *);
public:
		char * filename;
		TConFile(const char *);
    TConFile(QString);
    TConFile(FILE *);
		~TConFile();
		TConSect * getSect(const char *);
		TConSect * addSect(const char *);
    void remSect(const char *);  
		void save();
    void saveas(FILE * , bool = false);  
		void saveas(const char*, bool = true);
};

char* readSStop( FILE*, char);
void skipline( FILE* );

#endif
