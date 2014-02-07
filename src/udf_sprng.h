/*****************************************************************
 * *******                   UDF_SPRNG                     *******
 *****************************************************************
 * (C) 2014 A. Partl, eScience Group AIP - Distributed under GPL
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****************************************************************
 *  file handling registration of UDF function and global variables 
 * with MySQL
 * 
 *****************************************************************
 */

#include <mysql/plugin.h>

#ifndef __MYSQL_SPRNG_H__
#define __MYSQL_SPRNG_H__

#define MYSQL_SERVER 1
#include <sql/mysqld.h>

static const char *sprngTypes[] = { "LFG", "LCG", "LCG64", "CMRG", "MLFG" };
static TYPELIB sprngModes = { 5, NULL, sprngTypes, NULL };

int mysql_sprng_seed( MYSQL_THD thd );
void mysql_sprng_seed_set( MYSQL_THD thd, int newSeed );

unsigned long mysql_sprng_type( MYSQL_THD thd );
void mysql_sprng_type_set( MYSQL_THD thd, unsigned long newType );

char * mysql_sprng_packed( MYSQL_THD thd );
void mysql_sprng_packed_set( MYSQL_THD thd, char * newPacked );

#endif