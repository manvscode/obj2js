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
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libutility/utility.h>
#include <libutility/console.h>
#include "libobj.h"

#define VERSION "1.0"

static void about( int argc, char* argv[] );

typedef struct known_port {
    unsigned short port;
    unsigned short type; // 1 for tcp;
    const char* desc;
} known_port_t;


typedef struct {
    const char* input_filename;
    const char* output_filename;
    char* variable_name;
} app_args_t;


int main( int argc, char* argv[] )
{
	int status_code = 0;
    app_args_t args = {
        .input_filename  = NULL,
        .output_filename = NULL
    };

    if( argc < 2 )
    {
        about( argc, argv );
		status_code = -1;
		goto done;
    }
    else
    {
        for( int arg = 1; arg < argc; arg++ )
        {
            if( strcmp( "-i", argv[arg] ) == 0 || strcmp( "--input", argv[arg] ) == 0 )
            {
                args.input_filename = argv[ arg + 1 ];
                arg++;
            }
            else if( strcmp( "-o", argv[arg] ) == 0 || strcmp( "--output", argv[arg] ) == 0 )
            {
                args.output_filename = argv[ arg + 1 ];
                arg++;
            }
            else if( strcmp( "-v", argv[arg] ) == 0 || strcmp( "--variable-name", argv[arg] ) == 0 )
            {
                args.variable_name = argv[ arg + 1 ];
                arg++;
            }
            else
            {
                console_fg_color_256( stderr, CONSOLE_COLOR256_RED );
                printf( "\n" );
                fprintf( stderr, "ERROR: " );
                console_reset( stderr );
                fprintf( stderr, "Unrecognized command line option '%s'\n", argv[arg] );
                about( argc, argv );
				status_code = -2;
				goto done;
            }
        }
    }

    if( !args.input_filename )
    {
        console_fg_color_256( stderr, CONSOLE_COLOR256_RED );
        fprintf( stderr, "ERROR: " );
        console_reset( stderr );
        fprintf( stderr, "Need to specify input OBJ file." );
        printf( "\n" );
        about( argc, argv );
		status_code = -3;
		goto done;
    }

    if( !args.output_filename )
    {
        console_fg_color_256( stderr, CONSOLE_COLOR256_RED );
        fprintf( stderr, "ERROR: " );
        console_reset( stderr );
        fprintf( stderr, "Need to specify output JavaScript file." );
        printf( "\n" );
        about( argc, argv );
		status_code = -3;
		goto done;
    }

	if( !args.variable_name )
	{
		args.variable_name = strdup( file_basename( args.input_filename ) );
		char* dot_char = strrchr( args.variable_name, '.' );
		if( dot_char )
		{
			*dot_char = '\0';
		}
	}


    FILE* out = fopen( args.output_filename, "w" );
    if( out )
    {
        obj_loader_t* ol = obj_loader_create_from_file( args.input_filename, false );

        fprintf( out, "/* var vertex = {\n" );
        fprintf( out, " *   x,\n" );
        fprintf( out, " *   y,\n" );
        fprintf( out, " *   z,\n" );
        fprintf( out, " *   u,\n" );
        fprintf( out, " *   v,\n" );
        fprintf( out, " *   nx,\n" );
        fprintf( out, " *   ny,\n" );
        fprintf( out, " *   nz,\n" );
        fprintf( out, " * };\n" );
        fprintf( out, " */\n" );
        fprintf( out, "var %s = {\n", args.variable_name );


        const obj_vertex_t* vertices = obj_loader_vertices( ol );
        const obj_texture_coord_t* texture_coords = obj_loader_texture_coords( ol );
        const obj_normal_t* normals = obj_loader_normals( ol );


        const size_t group_count = obj_loader_group_count( ol );
        for( size_t i = 0; i < group_count; i++ )
        {
            const obj_group_t* group = obj_loader_group_at( ol, i );
            fprintf( out, "\t\"%s\": ", obj_group_name( group ) );
            fprintf( out, "[\n" );

            fprintf( out, "\t\t//%16s,%16s,%16s,%16s,%16s,%16s,%16s,%16s\n", "postion-X", "position-Y", "position-Z", "texture-U", "texture-V", "normal-X", "normal-Y", "normal-Z" );
            fprintf( out, "\t\t//----------------------------------------------------------------------------------------------------------------------------------------\n" );

            const size_t faces_count = obj_group_faces_count( group );
            for( size_t j = 0; j < faces_count; j++ )
            {
                const obj_face_t* face = obj_group_faces( group, j );

                const size_t v_index_count = obj_face_vertex_indices_count( face );
                const size_t* v_indices = obj_face_vertex_indices( face );
                const size_t* t_indices = obj_face_texture_coords_indices( face );
                const size_t* n_indices = obj_face_normal_indices( face );

                for( size_t vi = 0; vi < v_index_count; vi++ )
                {
                    const obj_vertex_t* v = &vertices[ v_indices[vi] ];
                    const obj_texture_coord_t* t = &texture_coords[ t_indices[vi] ];
                    const obj_normal_t* n = &normals[ n_indices[vi] ];

#if 0
                    fprintf( out, "\t\t%f,\n", v->x );
                    fprintf( out, "\t\t%f,\n", v->y );
                    fprintf( out, "\t\t%f,\n", v->z );
                    fprintf( out, "\t\t%f,\n", t->u );
                    fprintf( out, "\t\t%f,\n", t->v );
                    fprintf( out, "\t\t%f,\n", n->nx );
                    fprintf( out, "\t\t%f,\n", n->ny );
                    fprintf( out, "\t\t%f%s\n", n->nz, j != (faces_count - 1) ? "," : "" );
#else
                    fprintf( out, "\t\t  %+16.10f,%+16.10f,%+16.10f,%+16.10f,%+16.10f,%+16.10f,%+16.10f,%+16.10f", v->x, v->y, v->z, t->u, t->v, n->nx, n->ny, n->nz );
                    //fprintf( out, "%s\n", j != (faces_count - 1) ? "," : "" );
					bool is_last_vertex = j == (faces_count - 1) && vi == (v_index_count - 1);

					fprintf( out, "%s\n", is_last_vertex ? "" : "," );
#endif
                }
            }

            fprintf( out, "\t]" );
            fprintf( out, "%s\n", i != (group_count - 1) ? "," : "" );
        }
        fprintf( out, "};\n" );
        fclose( out );
    }

done:
	if( args.variable_name ) free( args.variable_name );
    return status_code;
}

void about( int argc, char* argv[] )
{
    printf( "obj2js v%s\n", VERSION );
    printf( "Copyright (c) 2016, Joe Marrero.\n\n");

    printf( "Usage:\n" );
    printf( "    %s -i <obj-file> -o <javascript-file>\n\n", argv[0] );

    printf( "Command Line Options:\n" );
    printf( "    %-2s, %-12s   %-50s\n", "-i", "--input", "The input OBJ file." );
    printf( "    %-2s, %-12s   %-50s\n", "-o", "--output", "The output Javascript file." );
    printf( "    %-2s, %-12s   %-50s\n", "-v", "--variable-name", "The variable name for the JavaScript object." );
    printf( "\n" );
}
