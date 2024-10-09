#ifndef SUBSDL_H_
#define SUBSDL_H_

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

typedef enum bool
{
   F = 0,
   T = ( ! 0 )
} bool;

typedef struct _Sub   Sub;
typedef struct _Font  Font;
typedef struct _Image Image;

struct _Font
{
   TTF_Font *font;
   int       fontCellWidth;
   int       fontCellHeight;
   int       fontSize;
};

struct _Image
{
   SDL_Texture *texture;
   int width;
   int height;
};

struct _Sub
{
   SDL_Window   *window;
   SDL_Renderer *renderer;
   SDL_Event     event;
   int           winWidth;
   int           winHeight;

   Font **fonts;
   int    fontsCount;
   int    failedFontsCount;

   Image **images;
   int     imagesCount;
   int     failedImagesCount;

   const char *background;
};

Sub *sub_createWindow( int width, int height, const char *title, const char *hexColor );
void sub_clearAll( Sub *sub );
void sub_beginDraw( Sub *sub );
void sub_endDraw( Sub *sub );

int sub_pollEvent( Sub *sub );
int sub_waitEvent( Sub *sub );
int sub_procEvent( Sub *sub );

bool sub_loadFont( Sub *sub, Font *font, const char *fontPath, int fontSize );
void sub_drawFont( Sub *sub, Font *font, int col, int row, const wchar_t *string, const char *hexColor );
void sub_freeFont( Font *font );
void sub_printFontInfo( const Font *font );

bool sub_loadImage( Sub *sub, Image *image, const char *imagePath );
void sub_drawImage( Sub *sub, Image *image, int x, int y, int width, int height );
void sub_freeImage( Image *image );

SDL_Color sub_hexToColor( const char *hex );

char *sub_strdup( const char *str );

#define MAX( a, b ) \
   ( { typeof( a ) _a = ( a ); typeof( b ) _b = ( b ); _a < _b ? _b : _a; } )
#define MIN( a, b ) \
   ( { typeof( a ) _a = ( a ); typeof( b ) _b = ( b ); _a < _b ? _a : _b; } )
#define UNUSED( n ) ( ( void )( n ) )
#define LEN( n ) ( sizeof( n ) / sizeof( ( n )[ 0 ] ) )

#endif /* End SUBSDL_H_ */
