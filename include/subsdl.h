/*
 *
 */

#ifndef SUBSDL_H_
#define SUBSDL_H_

#include <locale.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#if defined( _WIN32 ) || defined( _WIN64 )
   #include "_win.h"
#else
   #include "_linux.h"
#endif

#define BOX_SINGLE         "┌─┐│┘└"
#define BOX_DOUBLE         "╔═╗║╝╚"
#define BOX_SINGLE_DOUBLE  "╓─╖║╜╙"
#define BOX_DOUBLE_SINGLE  "╒═╕│╛╘"

typedef enum bool
{
   F = 0,
   T = ( ! 0 )
} bool;

typedef struct _GT       GT;
typedef struct _Image    Image;
typedef struct _FileInfo FileInfo;

struct _Image
{
   SDL_Texture *texture;
   int width;
   int height;
};

struct _FileInfo
{
   char name[ 512 ];
   char size[ 20 ];
   char date[ 11 ];
   char time[ 9 ];
   char attr[ 6 ];
   bool state;
};

struct _GT
{
   SDL_Window   *window;
   SDL_Renderer *renderer;
   SDL_Event     event;
   int           width;
   int           height;
   int           mouseX;
   int           mouseY;
   int           maxCols;
   int           maxRows;

   TTF_Font     *font;
   int           fontCellWidth;
   int           fontCellHeight;
   int           fontSize;

   Image       **images;
   int           imagesCount;
   int           failedImagesCount;

   const char *background;
};

/* ------------------------------------------------------------------------- */
GT  *gt_createWindow( int width, int height, const char *title, const char *hexColor );
void gt_clearAll( GT *gt );
void gt_beginDraw( GT *gt );
void gt_endDraw( GT *gt );

/* ------------------------------------------------------------------------- */
SDL_Color gt_hexToColor( const char *hex );

/* ------------------------------------------------------------------------- */
bool gt_loadFont( GT *gt, const char *fontPath, int fontSize );
void gt_drawFont( GT *gt, int x, int y, const char *string, const char *hexColor );
void gt_printFontInfo( const GT *gt );

/* ------------------------------------------------------------------------- */
bool gt_loadImage( GT *gt, Image *image, const char *imagePath );
void gt_drawImage( GT *gt, Image *image, int x, int y, int width, int height );
void gt_freeImage( Image *image );

/* ------------------------------------------------------------------------- */
int gt_maxCol( GT *gt );
int gt_maxRow( GT *gt );
int gt_maxWidth( GT *gt );
int gt_maxHeight( GT *gt );
int gt_fontCellWidth( GT *gt );
int gt_fontCellHeight( GT *gt );

/* ------------------------------------------------------------------------- */
char *gt_addStr( const char *firstString, ... );
int   gt_at( const char *search, const char *string );
char *gt_left( const char *string, int count );
char *gt_right( const char *string, int count );
char *gt_padL( const char *string, int length );
char *gt_padC( const char *string, int length );
char *gt_padR( const char *string, int length );
char *gt_space( int count );
void  gt_safeCopy( char *dest, const char *src, size_t destSize );
void  gt_safeCopyUtf8( char *dest, const char *src, size_t destSize );
char *gt_subStr( const char *string, int start, int count );
bool  gt_runApp( const char *commandLine );
bool  gt_run( const char *commandLine );
bool  gt_isExecutable( const char *filePath );

const char *gt_os( void );

/* ------------------------------------------------------------------------- */
void gt_dispBox( GT *gt, int x, int y, int width, int height, const char *boxString, const char *hexColor );

/* ------------------------------------------------------------------------- */
void        gt_Utf8CharExtract( const char *source, char *dest, size_t *index );
const char *gt_utf8CharPtrAt( const char *string, int characterOffset );
size_t      gt_utf8Len( const char *utf8String );
size_t      gt_utf8LenUpTo( const char *utf8String, const char *endPosition );

/* ------------------------------------------------------------------------- */
const char *gt_cwd( void );
bool        gt_isValidPath( const char *path );
const char *gt_pathSeparator( void );
const char *gt_dirDeleteLastPath( const char *path );
const char *gt_dirDeleteLastSeparator( const char *path );
const char *gt_dirLastName( const char *path );

/* ------------------------------------------------------------------------- */
FileInfo   *gt_directory( const char *currentDir, int *size );
void        gt_freeDirectory( FileInfo *pFiles );

/* ------------------------------------------------------------------------- */
char *gt_strdup( const char *string );

/* ------------------------------------------------------------------------- */
#define MAX( a, b ) \
   ( { typeof( a ) _a = ( a ); typeof( b ) _b = ( b ); _a < _b ? _b : _a; } )
#define MIN( a, b ) \
   ( { typeof( a ) _a = ( a ); typeof( b ) _b = ( b ); _a < _b ? _a : _b; } )
#define UNUSED( n ) ( ( void )( n ) )
#define LEN( n ) ( sizeof( n ) / sizeof( ( n )[ 0 ] ) )
#define IIF( condition, trueValue, falseValue ) ( ( condition ) ? ( trueValue ) : ( falseValue ) )

#endif /* End SUBSDL_H_ */
