/*
 * Copyright (C) 2016 by Joseph A. Marrero. http://www.manvscode.com/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <libcollections/vector.h>
#include <libutility/utility.h>
#include "libobj.h"

typedef enum obj_elem {
    ELM_UNKNOWN = 0,
    ELM_VERTEX,
    ELM_TEXTURECOORD,
    ELM_NORMAL,
    ELM_FACE,
    ELM_GROUP,
    ELM_OTHER
} obj_elem_t;


struct obj_face {
    size_t* v_indices;
    size_t* t_indices;
    size_t* n_indices;
};

struct obj_group {
    char* name;
    obj_face_t* faces;
};

struct obj_loader {
	bool verbose;
    obj_group_t* groups;
    obj_vertex_t* vertices;
    obj_texture_coord_t* texture_coords;
    obj_normal_t* normals;
};

static obj_elem_t obj_loader_element( const char *token );

#define DELIMETERS  " \n"
#define WHITESPACE  " \t\r\n"

static void obj_loader_parse_vertex( obj_loader_t* ol, bool verbose, char** line_token_ctx )
{
	obj_vertex_t v;

	char *token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );
	v.x = atof( token );

	token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );
	v.y = atof( token );

	token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );
	v.z = atof( token );

	lc_vector_push( ol->vertices, v );
}

static void obj_loader_parse_normal( obj_loader_t* ol, bool verbose, char** line_token_ctx )
{
	obj_normal_t n;

	char *token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );
	n.nx = atof( token );

	token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );
	n.ny = atof( token );

	token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );
	n.nz = atof( token );

	lc_vector_push( ol->normals, n );
}

static void obj_loader_parse_texture_coord( obj_loader_t* ol, bool verbose, char** line_token_ctx )
{
	obj_texture_coord_t t;

	char *token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );
	t.u = atof( token );

	token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );
	t.v = atof( token );

	lc_vector_push( ol->texture_coords, t );
}

static void obj_loader_parse_face( obj_loader_t* ol, bool verbose, char** line_token_ctx, obj_group_t** current_group )
{
	char *token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );

	if( *current_group == NULL ) // check if there are no groups;
	{
		if( verbose )
		{
			printf( "[OBJLoader] No group defined yet, so I am adding a default group.\n" );
		}

		obj_group_t g;
		g.name = "default";

		lc_vector_push( ol->groups, g );
		*current_group = &lc_vector_last( ol->groups );
	}

	obj_face_t face = {
		.v_indices = NULL,
		.t_indices = NULL,
		.n_indices = NULL
	};

	lc_vector_create( face.v_indices, 3 );
	lc_vector_create( face.t_indices, 3 );
	lc_vector_create( face.n_indices, 3 );

	while( token != NULL )
	{
		char* part_token_ctx = NULL;
		char* v_index_str = string_tokenize_r( token, "/", &part_token_ctx );
		char* t_index_str = string_tokenize_r( NULL, "/", &part_token_ctx );
		char* n_index_str = string_tokenize_r( NULL, "/", &part_token_ctx );

		if( v_index_str && *v_index_str != '\0')
		{
			string_trim( v_index_str, WHITESPACE );
			size_t v_idx = atol( v_index_str );
			// We subtract 1 because the OBJ format counts from 1 (instead of 0)
			lc_vector_push( face.v_indices, v_idx - 1 );
		}

		if( t_index_str && *t_index_str != '\0')
		{
			string_trim( t_index_str, WHITESPACE );
			size_t t_idx = atol( t_index_str );
			// We subtract 1 because the OBJ format counts from 1 (instead of 0)
			lc_vector_push( face.t_indices, t_idx - 1 );
		}
		if( n_index_str && *n_index_str != '\0')
		{
			string_trim( n_index_str, WHITESPACE );
			size_t n_idx = atol( n_index_str );
			// We subtract 1 because the OBJ format counts from 1 (instead of 0)
			lc_vector_push( face.n_indices, n_idx - 1 );
		}

		token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );
	}

	lc_vector_push( (*current_group)->faces, face );
}

static void obj_loader_parse_group( obj_loader_t* ol, bool verbose, char** line_token_ctx, obj_group_t** current_group )
{
	// BUG BUG: Need to ensure that we never exceed 512 bytes.
	char name[512];
	name[0] = '\0';

	char *token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );

	// concatenate all tokens to form group name.
	while( token != NULL )
	{
		strcat( name, token );
		strcat( name, " " );
		token = string_tokenize_r( NULL, DELIMETERS, line_token_ctx );
	}

	name[ strlen(name) - 1 ] = '\0'; // remove last space char

	if( strcmp(name, "default") != 0 )
	{
		bool is_new_group = true;

		for( size_t i = 0; is_new_group && i < lc_vector_size(ol->groups); i++ )
		{
			const obj_group_t* g = &ol->groups[ i ];

			if( strcmp(g->name, name) == 0 )
			{
				is_new_group = false;
			}
		}

		if( is_new_group )
		{
			// Create a new group and add it...
			obj_group_t g;
			g.name = string_dup( name );
			g.faces = NULL;

			lc_vector_create( g.faces, 1 );
			lc_vector_push( ol->groups, g );
		}
		*current_group = &lc_vector_last( ol->groups );
	}
}

obj_loader_t* obj_loader_create_from_file( const char *filename, bool verbose )
{
    assert( filename != NULL && *filename != '\0' ); // must be a valid filename

    obj_loader_t* ol = NULL;
    FILE* file = fopen( filename, "r" );

    if( file )
    {
        ol = obj_loader_create( );
        obj_group_t* current_group = NULL;

        if( ol )
        {
            char line[ 512 ];
            char* line_token_ctx = NULL;

            while( !feof(file) ) // read in the file...
            {
                if( fgets( line, sizeof(line) - 1, file ) )
                {
                	line[ sizeof(line) - 1 ] = '\0';
                }
                else if( feof(file) )
                {
                    break;
                }
                else
                {
                    fprintf( stderr, "Unable to read line.\n" );
                    break;
                }

				string_trim( line, WHITESPACE );

                if( *line == '\0' ) continue; // skip empty lines...

                char *token = string_tokenize_r( line, DELIMETERS, &line_token_ctx );
                if( !token ) continue;
                obj_elem_t elem = obj_loader_element( token );

                switch( elem )
                {
                    case ELM_VERTEX:
                    {
						obj_loader_parse_vertex( ol, verbose, &line_token_ctx );
                        break;
                    }
                    case ELM_NORMAL:
                    {
						obj_loader_parse_normal( ol, verbose, &line_token_ctx );
                        break;
                    }
                    case ELM_TEXTURECOORD:
                    {
						obj_loader_parse_texture_coord( ol, verbose, &line_token_ctx );
                        break;
                    }
                    case ELM_FACE:
                    {
						obj_loader_parse_face( ol, verbose, &line_token_ctx, &current_group );
                        break;
                    }
                    case ELM_GROUP:
                    {
						obj_loader_parse_group( ol, verbose, &line_token_ctx, &current_group );
                        break;
                    }
                    case ELM_OTHER:
                    default:
                    {
                        if( verbose )
                        {
                            printf( "[OBJLoader] Unknown element; Skipping line...\n\t %s\n", line );
                        }
                        continue;
                    }
                }

                line[ 0 ] = '\0';
            }
        }

        fclose(file);
    }

    return ol;
}

obj_loader_t* obj_loader_create( void )
{
    obj_loader_t* ol = malloc( sizeof(obj_loader_t) );

    if( ol )
    {
        ol->groups         = NULL;
        ol->vertices       = NULL;
        ol->texture_coords = NULL;
        ol->normals        = NULL;

        lc_vector_create( ol->groups, 1 );
        lc_vector_create( ol->vertices, 1 );
        lc_vector_create( ol->texture_coords, 1 );
        lc_vector_create( ol->normals, 1 );
    }

    return ol;
}

void obj_loader_destroy( obj_loader_t** ol )
{
    if( ol && *ol )
    {
        obj_loader_clear( *ol );
        free( *ol );
        *ol = NULL;
    }
}

void obj_loader_clear( obj_loader_t* ol )
{
    assert( ol );

    lc_vector_destroy( ol->groups );
    ol->groups         = NULL;
    lc_vector_destroy( ol->vertices );
    ol->vertices       = NULL;
    lc_vector_destroy( ol->texture_coords );
    ol->texture_coords = NULL;
    lc_vector_destroy( ol->normals );
    ol->normals        = NULL;

    for( int g = 0; g < lc_vector_size(ol->groups); g++ )
    {
        obj_group_t* group = &ol->groups[ g ];

        for( int f = 0; f < lc_vector_size(group->faces); f++ )
        {
            obj_face_t* face = &group->faces[ f ];

            lc_vector_destroy( face->v_indices );
            lc_vector_destroy( face->t_indices );
            lc_vector_destroy( face->n_indices );
        }

        free( group->name );
        group->name = NULL;

        lc_vector_destroy( group->faces );
        group->faces = NULL;
    }

    lc_vector_destroy( ol->groups );
    ol->groups = NULL;
}

obj_elem_t obj_loader_element( const char *token )
{
    obj_elem_t result = ELM_UNKNOWN;

    if( strncmp( token, "vt", 2 ) == 0 )
    {
        result = ELM_TEXTURECOORD;
    }
    else if( strncmp( token, "vn", 2 ) == 0 )
    {
        result = ELM_NORMAL;
    }
    else if( strncmp( token, "v", 1 ) == 0 )
    {
        result = ELM_VERTEX;
    }
    else if( strncmp( token, "f", 1 ) == 0 )
    {
        result = ELM_FACE;
    }
    else if( strncmp( token, "g", 1 ) == 0 )
    {
        result = ELM_GROUP;
    }
    else
    {
        result = ELM_OTHER;
    }

    return result;
}

const obj_group_t* obj_loader_group_at( const obj_loader_t* ol, size_t index )
{
    return &ol->groups[ index ];
}

size_t obj_loader_group_count( obj_loader_t* ol )
{
    return lc_vector_size(ol->groups);
}

const char* obj_group_name( const obj_group_t* g )
{
    return g->name;
}

const obj_face_t* obj_group_faces( const obj_group_t* g, size_t index )
{
    return &g->faces[ index ];
}

size_t obj_group_faces_count( const obj_group_t* g )
{
    return lc_vector_size( g->faces );
}

const size_t* obj_face_vertex_indices( const obj_face_t* f )
{
    return f->v_indices;
}

size_t obj_face_vertex_indices_count( const obj_face_t* f )
{
    return lc_vector_size( f->v_indices );
}

const size_t* obj_face_texture_coords_indices( const obj_face_t* f )
{
    return f->t_indices;
}

size_t obj_face_texture_coords_indices_count( const obj_face_t* f )
{
    return lc_vector_size( f->t_indices );
}

const size_t* obj_face_normal_indices( const obj_face_t* f )
{
    return f->n_indices;
}

size_t obj_face_normal_indices_count( const obj_face_t* f )
{
    return lc_vector_size( f->n_indices );
}

const obj_vertex_t* obj_loader_vertices( const obj_loader_t* ol )
{
    return ol->vertices;
}

const obj_texture_coord_t* obj_loader_texture_coords( const obj_loader_t* ol )
{
    return ol->texture_coords;
}

const obj_normal_t* obj_loader_normals( const obj_loader_t* ol )
{
    return ol->normals;
}
