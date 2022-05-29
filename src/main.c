/*
 * Copyright (C) 2016-2022 by Joseph A. Marrero. https://joemarrero.com/
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
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <xtd/console.h>
#include <xtd/filesystem.h>
#include <xtd/string.h>
#include <libobj.h>

#define VERSION "1.2.0"

static void about( int argc, char* argv[] );
static void print_error(const char* format, ...);

#define INCLUDE_TEXTURES  (1 << 0)
#define INCLUDE_NORMALS   (1 << 1)

typedef struct {
	const char* input_filename;
	const char* output_filename;
	char* variable_name;
	int include;
} app_args_t;


int main( int argc, char* argv[] )
{
	int status_code = 0;
	app_args_t args = {
		.input_filename  = NULL,
		.output_filename = NULL,
		.include = INCLUDE_TEXTURES | INCLUDE_NORMALS
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
				if( (arg + 1) < argc )
				{
					args.input_filename = argv[ arg + 1 ];
					arg++;
				}
				else
				{
					print_error( "Missing required parameter for '%s' operation.\n", argv[arg] );
					about( argc, argv );
					return -2;
				}
			}
			else if( strcmp( "-o", argv[arg] ) == 0 || strcmp( "--output", argv[arg] ) == 0 )
			{
				if( (arg + 1) < argc )
				{
					args.output_filename = argv[ arg + 1 ];
					arg++;
				}
				else
				{
					print_error( "Missing required parameter for '%s' operation.\n", argv[arg] );
					about( argc, argv );
					return -2;
				}
			}
			else if( strcmp( "-v", argv[arg] ) == 0 || strcmp( "--variable-name", argv[arg] ) == 0 )
			{
				if( (arg + 1) < argc )
				{
					args.variable_name = string_dup(argv[ arg + 1 ]);
					arg++;
				}
				else
				{
					print_error( "Missing required parameter for '%s' operation.\n", argv[arg] );
					about( argc, argv );
					return -2;
				}
			}
			else if( strcmp( "-xt", argv[arg] ) == 0 || strcmp( "--exclude-textures", argv[arg] ) == 0 )
			{
				args.include &= ~INCLUDE_TEXTURES;
			}
			else if( strcmp( "-xn", argv[arg] ) == 0 || strcmp( "--exclude-normals", argv[arg] ) == 0 )
			{
				args.include &= ~INCLUDE_NORMALS;
			}
			else if( strcmp( "-h", argv[arg] ) == 0 || strcmp( "--help", argv[arg] ) == 0 )
			{
				about( argc, argv );
				goto done;
			}
			else
			{
				print_error( "Unrecognized command line option '%s'\n", argv[arg] );
				about( argc, argv );
				status_code = -2;
				goto done;
			}
		}
	}

	if( !args.input_filename )
	{
		print_error( "Need to specify input OBJ file.\n" );
		about( argc, argv );
		status_code = -3;
		goto done;
	}

	if( !args.output_filename )
	{
		print_error( "Need to specify output JavaScript file.\n" );
		about( argc, argv );
		status_code = -3;
		goto done;
	}

	if( !args.variable_name )
	{
		args.variable_name = string_dup( file_basename( args.input_filename ) );
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

		const obj_vertex_t* vertices = obj_loader_vertices( ol );
		const obj_texture_coord_t* texture_coords = obj_loader_texture_coords( ol );
		const obj_normal_t* normals = obj_loader_normals( ol );

		fprintf( out, "/* let vertex = {\n" );
        if (obj_loader_vertices_count(ol) > 0 )
        {
            fprintf( out, " *   x,\n" );
            fprintf( out, " *   y,\n" );
            fprintf( out, " *   z,\n" );
        }
		if( (args.include & INCLUDE_TEXTURES) && obj_loader_texture_coords_count(ol) > 0)
		{
			fprintf( out, " *   u,\n" );
			fprintf( out, " *   v,\n" );
		}
		if( (args.include & INCLUDE_NORMALS) && obj_loader_normals_count(ol) > 0 )
		{
			fprintf( out, " *   nx,\n" );
			fprintf( out, " *   ny,\n" );
			fprintf( out, " *   nz,\n" );
		}
		fprintf( out, " * };\n" );
		fprintf( out, " */\n" );
		fprintf( out, "export const %s = {\n", args.variable_name );

		const size_t group_count = obj_loader_group_count( ol );
		for( size_t i = 0; i < group_count; i++ )
		{
			const obj_group_t* group = obj_loader_group_at( ol, i );
			fprintf( out, "\t\"%s\": ", obj_group_name( group ) );
			fprintf( out, "[\n" );

            if (obj_loader_vertices_count(ol) > 0 )
            {
			    fprintf( out, "\t\t//%16s,%16s,%16s", "position-X", "position-Y", "position-Z");
            }

			int divider_length = 50 + 8;

			if( (args.include & INCLUDE_TEXTURES) && obj_loader_texture_coords_count(ol) > 0 )
			{
				fprintf( out, ",%16s,%16s", "texture-U", "texture-V");
				divider_length += 34;
			}
			if( (args.include & INCLUDE_NORMALS) && obj_loader_normals_count(ol) > 0 )
			{
				fprintf( out, ",%16s,%16s,%16s\n", "normal-X", "normal-Y", "normal-Z" );
				divider_length += 50;
			}
			fprintf(out, "\n");

			fprintf( out, "\t\t//");
			while( divider_length-- > 0 )
			{
				fprintf(out, "-");
			}
			fprintf(out, "\n");

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
                    if (obj_loader_vertices_count(ol) > 0 )
                    {
                        const obj_vertex_t* v = &vertices[ v_indices[vi] ];
                        fprintf( out, "\t\t  %+16.10f,%+16.10f,%+16.10f", v->x, v->y, v->z );
                    }

					if( (args.include & INCLUDE_TEXTURES) && obj_loader_texture_coords_count(ol) > 0 )
					{
					    const obj_texture_coord_t* t = &texture_coords[ t_indices[vi] ];
						fprintf( out, ",%+16.10f,%+16.10f", t->u, t->v);
					}

					if( (args.include & INCLUDE_NORMALS) && obj_loader_normals_count(ol) > 0 )
					{
					    const obj_normal_t* n = &normals[ n_indices[vi] ];
						fprintf( out, ",%+16.10f,%+16.10f,%+16.10f", n->nx, n->ny, n->nz );
					}

					bool is_last_vertex = j == (faces_count - 1) && vi == (v_index_count - 1);
					fprintf( out, "%s\n", is_last_vertex ? "" : "," );
				}
			}

			fprintf( out, "\t]" );
			fprintf( out, "%s\n", i != (group_count - 1) ? "," : "" );
		}
		fprintf( out, "};\n" );

        // Export module if used from Node.js
        //fprintf( out, "\n// Export module if executing in Node.js\n");
        //fprintf( out, "if (typeof window === 'undefined') {\n");
        //fprintf( out, "\tmodule.exports = %s;\n", args.variable_name);
        //fprintf( out, "}\n");

		fclose( out );
	}

done:
	if( args.variable_name ) free( args.variable_name );
	return status_code;
}

void about( int argc, char* argv[] )
{
	printf( "obj2js v%s\n", VERSION );
	printf( "Copyright (c) 2016, Joe Marrero. https://joemarrero.com/\n\n");

	printf( "This tool will convert an Alias Wavefront OBJ model into Javascript\n" );
	printf( "arrays that are suitable for loading directly with WebGL.\n\n");

	printf( "Usage:\n" );
	printf( "    %s -i <obj-file> -o <javascript-file>\n\n", argv[0] );

	printf( "Command Line Options:\n" );
	printf( "    %3s, %-20s  %-50s\n", "-i", "--input", "The input OBJ file." );
	printf( "    %3s, %-20s  %-50s\n", "-o", "--output", "The output Javascript file." );
	printf( "    %3s, %-20s  %-50s\n", "-v", "--variable-name", "The variable name for the JavaScript object." );
	printf( "    %3s, %-20s  %-50s\n", "-xt", "--exclude-textures", "Exclude parsing of textures." );
	printf( "    %3s, %-20s  %-50s\n", "-xn", "--exclude-normals", "Exclude parsing of normals." );
	printf( "    %3s, %-20s  %-50s\n", "-h", "--help", "Show program usage." );


	printf( "\n" );
}

void print_error(const char* format, ...)
{
    va_list args;
    va_start( args, format );
    #if defined(_WIN32) || defined(_WIN64)
    fprintf( stderr, "ERROR: " );
    vfprintf( stderr, format, args );
    #else
    console_fg_color_8( stderr, CONSOLE_COLOR8_RED );
    fprintf( stderr, "ERROR: " );
    console_reset( stderr );
    vfprintf( stderr, format, args );
    #endif
    va_end( args );
}
