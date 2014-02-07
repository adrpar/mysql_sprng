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
 * some macros for making UDF life easier
 * 
 *****************************************************************
 */

#ifndef __MYSQL_SPRNG_UDFHELPERS__
#define __MYSQL_SPRNG_UDFHELPERS__

#define MYSQL_UDF_CHAR_FUNC( NAME ) \
 	my_bool NAME##_init( UDF_INIT* initid, UDF_ARGS* args, char* message ); \
 	void NAME##_deinit( UDF_INIT* initid ); \
 	char *NAME( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error );

#define MYSQL_UDF_REAL_FUNC( NAME ) \
 	my_bool NAME##_init( UDF_INIT* initid, UDF_ARGS* args, char* message ); \
 	void NAME##_deinit( UDF_INIT* initid ); \
 	double NAME( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );

#define MYSQL_UDF_INT_FUNC( NAME ) \
 	my_bool NAME##_init( UDF_INIT* initid, UDF_ARGS* args, char* message ); \
 	void NAME##_deinit( UDF_INIT* initid ); \
 	long long NAME( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );

#define MYSQL_UDF_CHKCONV_PARAM_TOREAL( PARAMNUM, ERRTEXT ) \
		if(args->args[ PARAMNUM ] != NULL) { \
	    	switch (args->arg_type[ PARAMNUM ]) { \
	    		case INT_RESULT: \
	    			args->arg_type[ PARAMNUM ] = REAL_RESULT; \
	    			*(double*)args->args[ PARAMNUM ] = (double)*(long long*)args->args[ PARAMNUM ]; \
	    			break; \
	    		case DECIMAL_RESULT: \
	    			args->arg_type[ PARAMNUM ] = REAL_RESULT; \
	    			*(double*)args->args[ PARAMNUM ] = atof(args->args[ PARAMNUM ]); \
	    			break; \
	    		case REAL_RESULT: \
	    			break; \
	    		default: \
					strcpy(message, ERRTEXT ); \
					return 1; \
	    	} \
	    }

#define MYSQL_UDF_CHK_PARAM_CHAR( PARAMNUM, ERRTEXT ) \
		if(args->args[ PARAMNUM ] != NULL) { \
			switch (args->arg_type[ PARAMNUM ]) { \
				case STRING_RESULT: \
					break; \
				default: \
					strcpy(message, ERRTEXT ); \
					return 1;\
			} \
		}

#define MYSQL_UDF_CHK_PARAM_INT( PARAMNUM, ERRTEXT ) \
		if(args->args[ PARAMNUM ] != NULL) { \
			switch (args->arg_type[ PARAMNUM ]) { \
				case INT_RESULT: \
					break; \
				default: \
					strcpy(message, ERRTEXT ); \
					return 1;\
			} \
		}

#define MYSQL_UDF_DYNCHKCONV_PARAM_TOREAL( PARAMNUM ) \
		if(args->args[ PARAMNUM ] != NULL) { \
	    	switch (args->arg_type[ PARAMNUM ]) { \
	    		case INT_RESULT: \
	    			args->arg_type[ PARAMNUM ] = REAL_RESULT; \
	    			*(double*)args->args[ PARAMNUM ] = (double)*(long long*)args->args[ PARAMNUM ]; \
	    			break; \
	    		case DECIMAL_RESULT: \
	    			args->arg_type[ PARAMNUM ] = REAL_RESULT; \
	    			*(double*)args->args[ PARAMNUM ] = atof(args->args[ PARAMNUM ]); \
	    			break; \
	    		case REAL_RESULT: \
	    			break; \
				default: \
					*is_null = 1; \
					return NULL;\
	    	} \
	    }

#define MYSQL_UDF_DYNCHK_PARAM_CHAR( PARAMNUM ) \
		if(args->args[ PARAMNUM ] != NULL) { \
			switch (args->arg_type[ PARAMNUM ]) { \
				case STRING_RESULT: \
					break; \
				default: \
					*is_null = 1; \
					return NULL;\
			} \
		}

#define MYSQL_UDF_DYNCHK_PARAM_INT( PARAMNUM ) \
		if(args->args[ PARAMNUM ] != NULL) { \
			switch (args->arg_type[ PARAMNUM ]) { \
				case INT_RESULT: \
					break; \
				default: \
					*is_null = 1; \
					return NULL;\
			} \
		}

#endif