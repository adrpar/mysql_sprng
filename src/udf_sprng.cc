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
 * file handling registration of UDF function and global variables 
 * with MySQL
 * 
 *****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>

#include <mysql/plugin.h>
#include <mysql_version.h>

#include "globals.h"
#include "util.h"
#include "udf_sprng.h"

#define MYSQL_SERVER 1

/* *** GLOBAL SERVER VARIABLES LINKAGE *** */

// defined in globals.h:
unsigned int numNodes;
unsigned int myId;

/* *** VARIABLE SETTING/CHECKING FUNCTIONS *** */
int type_check(MYSQL_THD thd, struct st_mysql_sys_var *var, void *save, struct st_mysql_value *value) {
	const char * buf;
	char * bufTmp;
	int length;
	int i;
	bool found = false;
	
	buf = value->val_str(value, bufTmp, &length);

	//check if value is sane
	if(length < 3 || length > 5) {
		return 1;
	}

	//this works as long as LCG comes before LCG64
	for(i = 0; i < sprngModes.count ; i++) {
		if(strncmp(buf, sprngModes.type_names[i], MAX( length, strlen(sprngModes.type_names[i]) )) == 0) {
			found = true;
			*(unsigned long *) save = i;
			break;
		}
	}
	
	if(found == true) {
		return 0;
	} else {
		return 1;
	}
}

void type_update(MYSQL_THD thd, struct st_mysql_sys_var *var, void *var_ptr, const void *save) {
	if(*(unsigned int *)save != mysql_sprng_type( thd )) {
		mysql_sprng_type_set( thd, *(unsigned int *)save );

		mysql_sprng_packed_set( thd, NULL );
	}
}

void seed_update(MYSQL_THD thd, struct st_mysql_sys_var *var, void *var_ptr, const void *save) {
	if(*(int *)save != mysql_sprng_seed( thd )) {
		mysql_sprng_seed_set( thd, *(int *)save );

		mysql_sprng_packed_set( thd, NULL );
	}
}

MYSQL_SYSVAR_UINT(numNodes, numNodes, PLUGIN_VAR_READONLY | PLUGIN_VAR_RQCMDARG,
				  "Number of SPRNG nodes in the MySQL cluster.", NULL, NULL, 1, 1, INT_MAX, 0);
MYSQL_SYSVAR_UINT(myId, myId, PLUGIN_VAR_READONLY,
				  "Current SPRNG node id.", NULL, NULL, 0, 0, INT_MAX, 0);
MYSQL_THDVAR_ENUM(type, 0,
				  "SPRNG generator type", type_check, type_update, 4, &sprngModes);
MYSQL_THDVAR_INT(seed, 0,
				  "SPRNG generator seed", NULL, seed_update, 1639287621, INT_MIN, INT_MAX, 0);
MYSQL_THDVAR_STR(packed, PLUGIN_VAR_READONLY | PLUGIN_VAR_NOCMDOPT,
				  "Packed SPRNG object - don't touch!", NULL, NULL, 0);

struct st_mysql_sys_var *vars_system[] = {
	MYSQL_SYSVAR(numNodes),
	MYSQL_SYSVAR(myId),
	MYSQL_SYSVAR(type),
	MYSQL_SYSVAR(seed),
	MYSQL_SYSVAR(packed),
	NULL
};

/* *** THDVAR ACCESSORS *** */

int mysql_sprng_seed( MYSQL_THD thd ) {
	return THDVAR( thd, seed );
}

void mysql_sprng_seed_set( MYSQL_THD thd, int newSeed ) {
	THDVAR( thd, seed ) = newSeed;
}

unsigned long mysql_sprng_type( MYSQL_THD thd ) {
	return THDVAR( thd, type );
}

void mysql_sprng_type_set( MYSQL_THD thd, unsigned long newType ) {
	THDVAR( thd, type ) = newType;
}

char * mysql_sprng_packed( MYSQL_THD thd ) {
	return THDVAR( thd, packed );
}

void mysql_sprng_packed_set( MYSQL_THD thd, char * newPacked ) {
	if(THDVAR( thd, packed ) != NULL) {
		free(THDVAR( thd, packed ));
	}

	THDVAR( thd, packed ) = newPacked;
}

/* *** PLUGIN DEFINITION *** */

struct st_mysql_daemon vars_plugin_info = {MYSQL_DAEMON_INTERFACE_VERSION};

mysql_declare_plugin(vars) {
	MYSQL_DAEMON_PLUGIN,
	&vars_plugin_info,
	"SPRNG",
	"Adrian M. Partl",
	"SPRGN random numbers for MySQL",
	PLUGIN_LICENSE_GPL,
	NULL,
	NULL,
	0x0100,
	NULL,
	vars_system,
	NULL
}
mysql_declare_plugin_end;
