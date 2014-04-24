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
 * UDF implementation
 * 
 *****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <base64.h>
#include "sprng.h"
#include "udf_helpers.h"
#include "udf_sprng.h"
#include "util.h"

#define MYSQL_SERVER 1

extern "C" {
    
    MYSQL_UDF_REAL_FUNC( sprng_dbl );
    MYSQL_UDF_INT_FUNC( sprng_int );
    MYSQL_UDF_INT_FUNC( sprng_make_seed );
    
}

my_bool sprng_make_seed_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	if(args->arg_count != 0) {
		strcpy(message, "wrong number of arguments: sprng_make_seed required no parameter");
		return 1;
	}

    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;

    return 0;
}

void sprng_make_seed_deinit( UDF_INIT* initid ) {

}

long long sprng_make_seed( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	return (long long)make_sprng_seed();
}

my_bool sprng_dbl_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	int i;
	int length;
	bool found = false;

	switch(args->arg_count) {
		case 2:
			MYSQL_UDF_CHK_PARAM_CHAR(1, "sprng_dbl(seed, type) requires a string for type");

			MYSQL_UDF_PREVENT_DYNAMIC_PARAM( 0, "sprng_dbl(seed, type) only constants allowed, no columns or other dynamic variables" );
			MYSQL_UDF_PREVENT_DYNAMIC_PARAM( 1, "sprng_dbl(seed, type) only constants allowed, no columns or other dynamic variables" );

			//determine type
			length = strlen((char*)args->args[1]);

			//check if value is sane
			if(length < 3 || length > 5) {
				found = false;
			} else {
				//this works as long as LCG comes before LCG64
				for(i = 0; i < sprngModes.count ; i++) {
					if(strncmp((char*)args->args[1], sprngModes.type_names[i], MIN( length, strlen(sprngModes.type_names[i]) )) == 0) {
						found = true;

						//did things change?
						if(i != mysql_sprng_type( current_thd )) {
							mysql_sprng_type_set( current_thd, i );

							//get rid of saved packed version
							mysql_sprng_packed_set( current_thd, NULL );
						}

						break;
					}
				}
			}

			if(found == false) {
				strcpy(message, "sprng_dbl(seed, type) invalid type. Not one of LFG, LCG, LCG64, CMRG, MLFG");
				return 1;
			}
		case 1:
			MYSQL_UDF_CHK_PARAM_INT(0, "sprng_dbl(seed) requires an integer for seed");

			MYSQL_UDF_PREVENT_DYNAMIC_PARAM( 0, "sprng_dbl(seed) only constants allowed, no columns or other dynamic variables" );

			//did things change?
			if((int)*(long long *) args->args[0] != mysql_sprng_seed( current_thd )) {
				mysql_sprng_seed_set( current_thd, (int)*(long long *) args->args[0] );

				//get rid of saved packed version
				mysql_sprng_packed_set( current_thd, NULL );
			}
		case 0:
			break;
		default:
			strcpy(message, "wrong number of arguments: sprng_dbl required up to three parameter");
			return 1;

	}

	Sprng ** sprng;
	if(mysql_sprng_packed( current_thd ) == NULL) {
		//this sprng random number generator has not yet been initialised
		sprng = (Sprng **)malloc(sizeof(Sprng*));
		*sprng = NULL;

		init_rng_simple_mpi(mysql_sprng_seed( current_thd ), SPRNG_DEFAULT, mysql_sprng_type( current_thd ), sprng);
	} else {
		//unpack
		size_t inputLen = strlen( mysql_sprng_packed( current_thd ) );
		size_t decodedLen = base64_needed_decoded_length( inputLen );
		char * decodedString = (char*) malloc(decodedLen);

#if MYSQL_VERSION_ID >= 50601
		if(base64_decode(mysql_sprng_packed( current_thd ), inputLen, decodedString, NULL, NULL) <= 0) {
#else
		if(base64_decode(mysql_sprng_packed( current_thd ), inputLen, decodedString, NULL) <= 0) {
#endif
			strcpy(message, "Error MySQL SPRNG: Could not decode base64 encoded SPRNG stream!\n");
			free(decodedString);
			return 1;
		}

		sprng = (Sprng **)malloc(sizeof(Sprng*));
		*sprng = NULL;

		if(unpack_rng_simple(decodedString, mysql_sprng_type( current_thd ), sprng) == NULL) {
			strcpy(message, "Error MySQL SPRNG: Could not unpack SPRNG stream!\n");
			free(sprng);
			free(decodedString);
			return 1;
		}

		free(decodedString);
	}


    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)sprng;

    return 0;
}

void sprng_dbl_deinit( UDF_INIT* initid ) {
	Sprng ** sprng = (Sprng **)initid->ptr;

	//pack SPRNG object
	char * buffer = NULL;
	int bufLen =  pack_rng_simple(&buffer, sprng);

	//convert to base64
	size_t base64Len = base64_needed_encoded_length(bufLen);
	char * base64Result = (char *)malloc(base64Len);

	if(base64Result == NULL) {
		fprintf(stderr, "Error MySQL SPRNG: Out of memory\n");
	} else {
		if(base64_encode(buffer, bufLen, base64Result) != 0) {
			fprintf(stderr, "Error MySQL SPRNG: Could not convert SPRNG stream to base64!\n");
		} else {
			//save stuff
			mysql_sprng_packed_set( current_thd, base64Result );
		}
	}

	if(buffer != NULL) {
		free(buffer);
	}

	if(sprng != NULL) {
		delete *sprng;
		free(sprng);
	}
}

double sprng_dbl( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	Sprng ** sprngObj = (Sprng **)initid->ptr;

	return get_rn_dbl_simple_mpi(sprngObj);
}

my_bool sprng_int_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	int i;
	int length;
	bool found = false;

	switch(args->arg_count) {
		case 2:
			MYSQL_UDF_CHK_PARAM_CHAR(1, "sprng_int(seed, type) requires a string for type");

			MYSQL_UDF_PREVENT_DYNAMIC_PARAM( 0, "sprng_int(seed, type) only constants allowed, no columns or other dynamic variables" );
			MYSQL_UDF_PREVENT_DYNAMIC_PARAM( 1, "sprng_int(seed, type) only constants allowed, no columns or other dynamic variables" );

			//determine type
			length = strlen((char*)args->args[1]);

			//check if value is sane
			if(length < 3 || length > 5) {
				found = false;
			} else {
				//this works as long as LCG comes before LCG64
				for(i = 0; i < sprngModes.count ; i++) {
					if(strncmp((char*)args->args[1], sprngModes.type_names[i], MIN( length, strlen(sprngModes.type_names[i]) )) == 0) {
						found = true;

						//did things change?
						if(i != mysql_sprng_type( current_thd )) {
							mysql_sprng_type_set( current_thd, i );

							//get rid of saved packed version
							mysql_sprng_packed_set( current_thd, NULL );
						}

						break;
					}
				}
			}

			if(found == false) {
				strcpy(message, "sprng_int(seed, type) invalid type. Not one of LFG, LCG, LCG64, CMRG, MLFG");
				return 1;
			}
		case 1:
			MYSQL_UDF_CHK_PARAM_INT(0, "sprng_int(seed) requires an integer for seed");

			MYSQL_UDF_PREVENT_DYNAMIC_PARAM( 0, "sprng_int(seed) only constants allowed, no columns or other dynamic variables" );

			//did things change?
			if((int)*(long long *) args->args[0] != mysql_sprng_seed( current_thd )) {
				mysql_sprng_seed_set( current_thd, (int)*(long long *) args->args[0] );

				//get rid of saved packed version
				mysql_sprng_packed_set( current_thd, NULL );
			}
		case 0:
			break;
		default:
			strcpy(message, "wrong number of arguments: sprng_int required up to three parameter");
			return 1;

	}

	Sprng ** sprng;
	if(mysql_sprng_packed( current_thd ) == NULL) {
		//this sprng random number generator has not yet been initialised
		sprng = (Sprng **)malloc(sizeof(Sprng*));
		*sprng = NULL;

		init_rng_simple_mpi(mysql_sprng_seed( current_thd ), SPRNG_DEFAULT, mysql_sprng_type( current_thd ), sprng);
	} else {
		//unpack
		size_t inputLen = strlen( mysql_sprng_packed( current_thd ) );
		size_t decodedLen = base64_needed_decoded_length( inputLen );
		char * decodedString = (char*) malloc(decodedLen);

#if MYSQL_VERSION_ID >= 50601
		if(base64_decode(mysql_sprng_packed( current_thd ), inputLen, decodedString, NULL, NULL) <= 0) {
#else
		if(base64_decode(mysql_sprng_packed( current_thd ), inputLen, decodedString, NULL) <= 0) {
#endif
			strcpy(message, "Error MySQL SPRNG: Could not decode base64 encoded SPRNG stream!\n");
			free(decodedString);
			return 1;
		}

		sprng = (Sprng **)malloc(sizeof(Sprng*));
		*sprng = NULL;

		if(unpack_rng_simple(decodedString, mysql_sprng_type( current_thd ), sprng) == NULL) {
			strcpy(message, "Error MySQL SPRNG: Could not unpack SPRNG stream!\n");
			free(sprng);
			free(decodedString);
			return 1;
		}

		free(decodedString);
	}


    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)sprng;

    return 0;
}

void sprng_int_deinit( UDF_INIT* initid ) {
	Sprng ** sprng = (Sprng **)initid->ptr;

	//pack SPRNG object
	char * buffer = NULL;
	int bufLen =  pack_rng_simple(&buffer, sprng);

	//convert to base64
	size_t base64Len = base64_needed_encoded_length(bufLen);
	char * base64Result = (char *)malloc(base64Len);

	if(base64Result == NULL) {
		fprintf(stderr, "Error MySQL SPRNG: Out of memory\n");
	} else {
		if(base64_encode(buffer, bufLen, base64Result) != 0) {
			fprintf(stderr, "Error MySQL SPRNG: Could not convert SPRNG stream to base64!\n");
		} else {
			//save stuff
			mysql_sprng_packed_set( current_thd, base64Result );
		}
	}

	if(buffer != NULL) {
		free(buffer);
	}

	if(sprng != NULL) {
		delete *sprng;
		free(sprng);
	}
}

long long sprng_int( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	Sprng ** sprngObj = (Sprng **)initid->ptr;

	return get_rn_int_simple_mpi(sprngObj);
}