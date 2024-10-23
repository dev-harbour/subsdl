/*
 *
 */

#include "subsdl.h"

/* ------------------------------------------------------------------------- */
// API functions
GT *gt_createWindow( int width, int height, const char *title, const char *hexColor )
{
   setlocale( LC_ALL, "en_US.UTF-8" );

   GT *gt = malloc( sizeof( GT ) );
   if( !gt )
   {
      fprintf( stderr, "Memory allocation failed for GT structure.\n" );
      return NULL;
   }

   memset( gt, 0, sizeof( GT ) );
   gt->width = width;
   gt->height = height;
   gt->background = hexColor;

   if( SDL_Init( SDL_INIT_VIDEO ) != 0 )
   {
      fprintf( stderr, "Unable to initialize SDL: %s\n", SDL_GetError() );
      free( gt );
      return NULL;
   }

   if( TTF_Init() == -1 )
   {
      fprintf( stderr, "Unable to initialize SDL_ttf: %s\n", TTF_GetError() );
      SDL_Quit();
      free( gt );
      return NULL;
   }

   gt->window = SDL_CreateWindow( title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
   if( !gt->window )
   {
      fprintf( stderr, "Could not create window: %s\n", SDL_GetError() );
      SDL_Quit();
      free( gt );
      return NULL;
   }

   gt->renderer = SDL_CreateRenderer( gt->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
   if( !gt->renderer )
   {
      fprintf( stderr, "Could not create renderer: %s\n", SDL_GetError() );
      SDL_DestroyWindow( gt->window );
      SDL_Quit();
      free( gt );
      return NULL;
   }

   SDL_SetWindowMinimumSize( gt->window, width, height );

   return gt;
}

void gt_clearAll( GT *gt )
{
   if( gt )
   {
      // ---
      if( gt->images )
      {
         for( int i = 0; i < gt->imagesCount; ++i )
         {
            if( gt->images[ i ] )
            {
               gt_freeImage( gt->images[ i ] );
            }
         }
         free( gt->images );
         gt->images = NULL;
      }
      // ---
      if( gt->font )
      {
         TTF_CloseFont( gt->font );
         gt->font = NULL;
      }
      // ---
      if( gt->renderer != NULL )
      {
         SDL_DestroyRenderer( gt->renderer );
         gt->renderer = NULL;
      }
      // ---
      if( gt->window != NULL )
      {
         SDL_DestroyWindow( gt->window );
         gt->window = NULL;
      }
      // ---
      TTF_Quit();
      IMG_Quit();
      SDL_Quit();

      free( gt );
      gt = NULL;
   }
}

void gt_beginDraw( GT *gt )
{
   int newWidth;
   int newHeight;
   int newWidthPos;
   int newHeightPos;
   int newMouseX;
   int newMouseY;

   SDL_GetWindowSize( gt->window, &newWidth, &newHeight );
   gt->width = newWidth;
   gt->height = newHeight;

   SDL_GetWindowPosition( gt->window, &newWidthPos, &newHeightPos );
   gt->widthPos = newWidthPos;
   gt->heightPos = newHeightPos;

   SDL_GetMouseState( &newMouseX, &newMouseY );
   gt->mouseX = newMouseX;
   gt->mouseY = newMouseY;

   SDL_Color bgColor = { 50, 50, 50, 255 };
   if( gt->background && strlen( gt->background ) > 0 )
   {
      bgColor = gt_hexToColor( gt->background );
   }

   SDL_SetRenderDrawColor( gt->renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a );
   SDL_RenderClear( gt->renderer );
}

void gt_endDraw( GT *gt )
{
   SDL_RenderPresent( gt->renderer );
}

/* ------------------------------------------------------------------------- */
SDL_Color gt_hexToColor( const char *hexColor )
{
   SDL_Color color = { 0, 0, 0, 255 };
   int r, g, b, a = 255;

   if( strlen( hexColor ) == 6 ) // Format: RRGGBB
   {
      sscanf( hexColor, "%02x%02x%02x", &r, &g, &b );
      color.r = r;
      color.g = g;
      color.b = b;
      color.a = 255;
   }
   else if( strlen( hexColor ) == 8 ) // Format: RRGGBBAA
   {
      sscanf( hexColor, "%02x%02x%02x%02x", &r, &g, &b, &a );
      color.r = r;
      color.g = g;
      color.b = b;
      color.a = a;
   }
   else
   {
      fprintf( stderr, "Invalid hex color format: gt_hexToColor() - defaulting to gray.\n" );
      color.r = 128;
      color.g = 128;
      color.b = 128;
      color.a = 255;
   }

   return color;
}

/* ------------------------------------------------------------------------- */
bool gt_loadFont( GT *gt, const char *fontPath, int fontSize )
{
   if( !gt || !fontPath )
   {
      fprintf( stderr, "Invalid parameters provided to gt_loadFont.\n" );
      return F;
   }

   gt->font = TTF_OpenFont( fontPath, fontSize );
   if( !gt->font )
   {
      fprintf( stderr, "Failed to load font: %s\n", TTF_GetError() );
      return F;
   }

   int advance;
   TTF_GlyphMetrics( gt->font, 'M', NULL, NULL, NULL, NULL, &advance );

   gt->fontCellWidth  = advance;
   gt->fontCellHeight = TTF_FontHeight( gt->font );
   gt->fontSize       = fontSize;

   return T;
}

void gt_drawFont( GT *gt, int x, int y, const char *string, const char *hexColor )
{
   x = x * gt->fontCellWidth;
   y = y * gt->fontCellHeight;

   SDL_Color bgColor = { 255, 255, 255, 255 };
   SDL_Color fgColor = { 0, 0, 0, 255 };

   if( hexColor && strlen( hexColor ) > 0 )
   {
      const char *separator = strchr( hexColor, '/' );
      if( separator && ( separator - hexColor == 6 ) && strlen( separator + 1 ) == 6 )
      {
         char bgColorStr[ 7 ];
         strncpy( bgColorStr, hexColor, 6 );
         bgColorStr[ 6 ] = '\0';

         bgColor = gt_hexToColor( bgColorStr );
         fgColor = gt_hexToColor( separator + 1 );
      }
   }

   size_t len = gt_utf8Len( string );
   if( len == 0 )
   {
      SDL_Rect rect = { x, y, gt->fontCellWidth, gt->fontCellHeight };
      SDL_SetRenderDrawColor( gt->renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a );
      SDL_RenderFillRect( gt->renderer, &rect );
      return;
   }

   SDL_Rect rect = { x, y, gt->fontCellWidth * len, gt->fontCellHeight };
   SDL_SetRenderDrawColor( gt->renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a );
   SDL_RenderFillRect( gt->renderer, &rect );

   SDL_Surface *textSurface = TTF_RenderUTF8_Shaded( gt->font, string, fgColor, bgColor );
   SDL_Texture *textTexture = NULL;

   if( textSurface )
   {
      textTexture = SDL_CreateTextureFromSurface( gt->renderer, textSurface );
      SDL_FreeSurface( textSurface );
   }
   else
   {
      fprintf( stderr, "Error: rendering string: %s\n", TTF_GetError() );
   }

   if( textTexture )
   {
      SDL_Rect textRect = { x, y, 0, 0 };
      SDL_QueryTexture( textTexture, NULL, NULL, &textRect.w, &textRect.h );
      SDL_RenderCopy( gt->renderer, textTexture, NULL, &textRect );

      SDL_DestroyTexture( textTexture );
   }
}

void gt_printFontInfo( const GT *gt )
{
   printf( "\033[2J" );
   printf( "\033[H" );

   if( gt && gt->font )
   {
      int minx, maxx, miny, maxy, advance;
      TTF_GlyphMetrics( gt->font, 'M', &minx, &maxx, &miny, &maxy, &advance );

      int height   = TTF_FontHeight( gt->font );
      int ascent   = TTF_FontAscent( gt->font );
      int descent  = TTF_FontDescent( gt->font );
      int lineSkip = TTF_FontLineSkip( gt->font );

      printf( "Font Info\n" );
      printf( "[\n" );
      printf( "   TTF_Font        : %p\n", gt->font );
      printf( "   Font minx       : %d\n", minx );
      printf( "   Font maxx       : %d\n", maxx );
      printf( "   Font miny       : %d\n", miny );
      printf( "   Font maxy       : %d\n", maxy );
      printf( "   Font advance    : %d\n", advance );
      printf( "   Font height     : %d\n", height );
      printf( "   Font ascent     : %d\n", ascent );
      printf( "   Font descent    : %d\n", descent );
      printf( "   Font lineSkip   : %d\n", lineSkip );
      printf( "   Font cell width : %d\n", gt->fontCellWidth );
      printf( "   Font cell height: %d\n", gt->fontCellHeight );
      printf( "   Font size       : %d\n", gt->fontSize );
      printf( "]\n" );
   }
   else
   {
      printf( "Font structure or TTF_Font is NULL\n" );
      return;
   }

   fflush( stdout );
}

/* ------------------------------------------------------------------------- */
bool gt_loadImage( GT *gt, Image *image, const char *imagePath )
{
   if( !gt || !image || !imagePath )
   {
      fprintf( stderr, "Invalid parameters provided to gt_loadImage.\n" );
      return F;
   }

   gt->imagesCount++;

   Image **newImage = realloc( gt->images, gt->imagesCount * sizeof( Image * ) );
   if( !newImage )
   {
      fprintf( stderr, "Failed to allocate memory for Image array.\n" );
      gt->imagesCount--;
      return F;
   }

   gt->images = newImage;
   gt->images[ gt->imagesCount - 1 ] = image;

   const char *ext = strrchr( imagePath, '.' );
   if( !ext )
   {
      fprintf( stderr, "File extension not found in path: %s\n", imagePath );
      gt->imagesCount--;
      return F;
   }

   int imgFlags = 0;
   SDL_Surface *surface = NULL;

   if( strcmp( ext, ".jpg" ) == 0 || strcmp( ext, ".jpeg" ) == 0 )
   {
      imgFlags = IMG_INIT_JPG;
   }
   else if( strcmp( ext, ".png" ) == 0 )
   {
      imgFlags = IMG_INIT_PNG;
   }
   else if( strcmp( ext, ".tif" ) == 0 )
   {
      imgFlags = IMG_INIT_TIF;
   }
   else if( strcmp( ext, ".webp" ) == 0 )
   {
      imgFlags = IMG_INIT_WEBP;
   }
   else if( strcmp( ext, ".jxl" ) == 0 )
   {
      imgFlags = IMG_INIT_JXL;
   }
   else if( strcmp( ext, ".avif" ) == 0 )
   {
      imgFlags = IMG_INIT_AVIF;
   }
   else
   {
      fprintf( stderr, "Unsupported image format: %s\n", ext );
      gt->imagesCount--;
      return F;
   }

   if( !( IMG_Init( imgFlags ) & imgFlags ) )
   {
      fprintf( stderr, "Failed to initialize SDL_image: %s\n", IMG_GetError() );
      gt->imagesCount--;
      return F;
   }

   surface = IMG_Load( imagePath );
   if( !surface )
   {
      fprintf( stderr, "Failed to load image: %s\n", IMG_GetError() );
      gt->failedImagesCount++;
      gt->imagesCount--;
      return F;
   }

   image->texture = SDL_CreateTextureFromSurface( gt->renderer, surface );
   if( !image->texture )
   {
      fprintf( stderr, "Failed to create texture from image: %s\n", SDL_GetError() );
      SDL_FreeSurface( surface );
      gt->failedImagesCount++;
      gt->imagesCount--;
      return F;
   }

   image->width = surface->w;
   image->height = surface->h;

   SDL_FreeSurface( surface );

   return T;
}

void gt_drawImage( GT *gt, Image *image, int x, int y, int width, int height )
{
   if( !gt || !image || !image->texture )
   {
      fprintf( stderr, "Invalid parameters provided to gt_drawImage.\n" );
      return;
   }

   SDL_Rect dstRect;

   float aspectRatio = ( float ) image->width / ( float ) image->height;

   if( width > 0 && height > 0 )
   {
      float newAspectRatio = ( float ) width / ( float ) height;

      if( newAspectRatio > aspectRatio )
      {
         dstRect.h = height;
         dstRect.w = ( int ) ( height * aspectRatio );
      }
      else
      {
         dstRect.w = width;
         dstRect.h = ( int ) ( width / aspectRatio );
      }
   }
   else
   {
      dstRect.w = image->width;
      dstRect.h = image->height;
   }

   dstRect.x = x;
   dstRect.y = y;

   SDL_RenderCopy( gt->renderer, image->texture, NULL, &dstRect );
}

void gt_freeImage( Image *image )
{
   if( image && image->texture )
   {
      SDL_DestroyTexture( image->texture );
      image->texture = NULL;
   }
}

/* ------------------------------------------------------------------------- */
int gt_maxCol( GT *gt )
{
   if( gt->fontCellWidth == 0 )
   {
      fprintf( stderr, "Error: fontCellWidth is zero. Make sure the font is loaded correctly.\n" );
      return 0;
   }
   return gt->width / gt->fontCellWidth;
}

int gt_maxRow( GT *gt )
{
   if( gt->fontCellHeight == 0 )
   {
      fprintf( stderr, "Error: fontCellHeight is zero. Make sure the font is loaded correctly.\n" );
      return 0;
   }
   return gt->height / gt->fontCellHeight;
}

int gt_maxWidth( GT *gt )
{
   return gt->width;
}

int gt_maxHeight( GT *gt )
{
   return gt->height;
}

int gt_fontCellWidth( GT *gt )
{
   return gt->fontCellWidth;
}

int gt_fontCellHeight( GT *gt )
{
   return gt->fontCellHeight;
}

/* ------------------------------------------------------------------------- */
char *gt_addStr( const char *firstString, ... )
{
   if( firstString == NULL )
   {
      return NULL;
   }

   size_t totalLength = 0;
   va_list args;

   va_start( args, firstString );
   const char *string = firstString;
   while( string != NULL )
   {
      totalLength += strlen( string );
      string = va_arg( args, const char * );
   }
   va_end( args );

   char *result = ( char * ) malloc( totalLength + 1 );
   if( !result )
   {
      return NULL;
   }

   result[ 0 ] = '\0';

   va_start( args, firstString );
   string = firstString;
   while( string != NULL )
   {
      strcat( result, string );
      string = va_arg( args, const char * );
   }
   va_end( args );

   return result;
}

int gt_at( const char *search, const char *string )
{
   char *occurrence = strstr( string, search );
   if( occurrence == NULL )
   {
      return -1;
   }

   int utf8Position = gt_utf8LenUpTo( string, occurrence );

   return utf8Position;
}

int gtAt( const char *search, const char *target )
{
   char *occurrence = strstr( target, search );
   if( occurrence == NULL )
   {
      return 0;
   }
   return ( int )( occurrence - target + 1 );
}


char *gt_left( const char *string, int count )
{
   int len = gt_utf8Len( string );

   if( count <= 0 )
   {
      return strdup( "" );
   }
   if( count >= len )
   {
      count = len;
   }

   const char *byteEnd = gt_utf8CharPtrAt( string, count );
   int byteCount = byteEnd - string;

   char *result = ( char * ) malloc( byteCount + 1 );
   if( result == NULL )
   {
      return NULL;
   }

   memcpy( result, string, byteCount );
   result[ byteCount ] = '\0';

   return result;
}

char *gt_right( const char *string, int count )
{
   int len = gt_utf8Len( string );

   if( count <= 0 )
   {
      return strdup( "" );
   }
   if( count > len )
   {
      count = len;
   }

   const char *byteStart = gt_utf8CharPtrAt( string, len - count );
   int byteCount = string + strlen( string ) - byteStart;

   char *result = ( char * ) malloc( byteCount + 1 );
   if( result == NULL )
   {
      return NULL;
   }

   memcpy( result, byteStart, byteCount );
   result[ byteCount ] = '\0';

   return result;
}

char *gt_padL( const char *string, int length )
{
   int len = gt_utf8Len( string );
   int byteLen = strlen( string );

   if( len >= length )
   {
      char *result = ( char * ) malloc( byteLen + 1 );
      if( !result )
      {
         return NULL;
      }

      memcpy( result, string, byteLen );
      result[ byteLen ] = '\0';
      return result;
   }
   else
   {
      int padding = length - len;

      char *result = ( char * ) malloc( padding + byteLen + 1 );
      if( !result )
      {
         return NULL;
      }

      memset( result, ' ', padding );
      memcpy( result + padding, string, byteLen );
      result[ padding + byteLen ] = '\0';
      return result;
   }
}

char *gt_padC( const char *string, int length )
{
   int len = gt_utf8Len( string );
   int byteLen = strlen( string );

   if( len >= length )
   {
      char *result = ( char * ) malloc( byteLen + 1 );
      if( !result )
      {
         return NULL;
      }

      memcpy( result, string, byteLen );
      result[ byteLen ] = '\0';
      return result;
   }
   else
   {
      int totalPadding = length - len;
      int paddingLeft  = totalPadding / 2;
      int paddingRight = totalPadding - paddingLeft;

      char *result = ( char * ) malloc( paddingLeft + byteLen + paddingRight + 1 );
      if( !result )
      {
         return NULL;
      }

      memset( result, ' ', paddingLeft );
      memcpy( result + paddingLeft, string, byteLen );
      memset( result + paddingLeft + byteLen, ' ', paddingRight );
      result[ paddingLeft + byteLen + paddingRight ] = '\0';
      return result;
   }
}

char *gt_padR( const char *string, int length )
{
   int len = gt_utf8Len( string );
   int byteLen = strlen( string );

   if( len >= length )
   {
      char *result = ( char * ) malloc( byteLen + 1 );
      if( !result )
      {
         return NULL;
      }

      memcpy( result, string, byteLen );
      result[ byteLen ] = '\0';
      return result;
   }
   else
   {
      int padding = length - len;

      char *result = ( char * ) malloc( byteLen + padding + 1 );
      if( !result )
      {
         return NULL;
      }

      memcpy( result, string, byteLen );
      memset( result + byteLen, ' ', padding );
      result[ byteLen + padding ] = '\0';
      return result;
   }
}

void gt_safeCopy( char *dest, const char *src, size_t destSize )
{
   if( destSize == 0 ) return;

   strncpy( dest, src, destSize - 1 );
   dest[ destSize - 1 ] = '\0';
}

void gt_safeCopyUtf8( char *dest, const char *src, size_t destSize )
{
   if( destSize == 0 ) return;

   size_t copiedBytes = 0;
   const char *srcPtr = src;

   while( *srcPtr && copiedBytes < destSize - 1 )
   {
      size_t charSize = 1;

      unsigned char byte = ( unsigned char ) *srcPtr;
      if( ( byte & 0x80 ) == 0 )         // ASCII (0xxxxxxx)
         charSize = 1;
      else if( ( byte & 0xE0 ) == 0xC0 ) // (110xxxxx)
         charSize = 2;
      else if( ( byte & 0xF0 ) == 0xE0 ) // (1110xxxx)
         charSize = 3;
      else if( ( byte & 0xF8 ) == 0xF0 ) // (11110xxx)
         charSize = 4;

      if( copiedBytes + charSize >= destSize - 1 )
         break;

      for( size_t i = 0; i < charSize; ++i )
      {
         dest[ copiedBytes++ ] = *srcPtr++;
      }
   }

   dest[copiedBytes] = '\0';
}

char *gt_space( int count )
{
   if( count <= 0 )
   {
      char *result = ( char * ) malloc( 1 );
      if( !result )
      {
         return NULL;
      }
      result[ 0 ] = '\0';
      return result;
   }

   char *result = ( char * ) malloc( count + 1 );
   if( !result )
   {
      return NULL;
   }

   memset( result, ' ', count );
   result[ count ] = '\0';
   return result;
}

char *gt_subStr( const char *string, int start, int count )
{
   int nSize = gt_utf8Len( string );

   if( start > nSize )
   {
      count = 0;
   }

   if( count > 0 )
   {
      if( start < 0 )
      {
         start = nSize + start;
      }
      if( start < 0 )
      {
         start = 0;
      }
      if( start + count > nSize )
      {
         count = nSize - start;
      }
   }
   else
   {
      if( start < 0 )
      {
         start = nSize + start;
      }
      if( start < 0 )
      {
         start = 0;
      }
      count = nSize - start;
   }

   const char *byteStart = gt_utf8CharPtrAt( string, start );
   const char *byteEnd   = gt_utf8CharPtrAt( byteStart, count );

   int byteCount = byteEnd - byteStart;

   char *result = ( char * ) malloc( byteCount + 1 );
   if( !result )
   {
      return NULL;
   }

   memcpy( result, byteStart, byteCount );
   result[ byteCount ] = '\0';
   return result;
}

/* ------------------------------------------------------------------------- */
void gt_dispBox( GT *gt, int x, int y, int width, int height, const char *boxString, const char *hexColor )
{
   if( strlen( boxString ) < 6 )
   {
      fprintf( stderr, "Error: boxString must contain at least 6 UTF-8 characters.\n" );
      return;
   }

   // Buffers for individual UTF-8 box-drawing characters (maximum 4 bytes + 1 for '\0')
   char topLeft[ 5 ]     = { 0 };
   char horizontal[ 5 ]  = { 0 };
   char topRight[ 5 ]    = { 0 };
   char vertical[ 5 ]    = { 0 };
   char bottomRight[ 5 ] = { 0 };
   char bottomLeft[ 5 ]  = { 0 };

   // Extract each UTF-8 character from boxString
   size_t index = 0;
   gt_Utf8CharExtract( boxString, topLeft, &index );
   gt_Utf8CharExtract( boxString, horizontal, &index );
   gt_Utf8CharExtract( boxString, topRight, &index );
   gt_Utf8CharExtract( boxString, vertical, &index );
   gt_Utf8CharExtract( boxString, bottomRight, &index );
   gt_Utf8CharExtract( boxString, bottomLeft, &index );

   gt_drawFont( gt, x, y, topLeft, hexColor );                  // top-left corner
   gt_drawFont( gt, x, y + height - 1, bottomLeft, hexColor );  // bottom-left corner

   for( int i = 1; i < width - 1; i++ )
   {
      gt_drawFont( gt, x + i, y, horizontal, hexColor );               // top edge
      gt_drawFont( gt, x + i, y + height - 1, horizontal, hexColor );  // bottom edge
   }

   for( int i = 1; i < height - 1; i++ )
   {
      gt_drawFont( gt, x, y + i, vertical, hexColor );              // left edge
      gt_drawFont( gt, x + width - 1, y + i, vertical, hexColor );  // right edge
   }

   gt_drawFont( gt, x + width - 1, y, topRight, hexColor );                  // top-right corner
   gt_drawFont( gt, x + width - 1, y + height - 1, bottomRight, hexColor );  // bottom-right corner
}

/* ------------------------------------------------------------------------- */
void gt_Utf8CharExtract( const char *source, char *dest, size_t *index )
{
   unsigned char firstByte = source[ *index ];
   size_t charLen = 1;

   if( ( firstByte & 0x80 ) == 0x00 )       // ASCII (0xxxxxxx)
      charLen = 1;
   else if( ( firstByte & 0xE0 ) == 0xC0 )  // (110xxxxx)
      charLen = 2;
   else if( ( firstByte & 0xF0 ) == 0xE0 )  // (1110xxxx)
      charLen = 3;
   else if( ( firstByte & 0xF8 ) == 0xF0 )  // (11110xxx)
      charLen = 4;

   strncpy( dest, source + *index, charLen );
   dest[ charLen ] = '\0';

   *index += charLen;
}

const char *gt_utf8CharPtrAt( const char *utf8String, int characterOffset )
{
   while( characterOffset > 0 && *utf8String )
   {
      unsigned char byte = *utf8String;

      // Determine how many bytes the current character uses
      if( ( byte & 0x80 ) == 0x00 )       // ASCII (0xxxxxxx)
         utf8String += 1;
      else if( ( byte & 0xE0 ) == 0xC0 )  // (110xxxxx)
         utf8String += 2;
      else if( ( byte & 0xF0 ) == 0xE0 )  // (1110xxxx)
         utf8String += 3;
      else if( ( byte & 0xF8 ) == 0xF0 )  // (11110xxx)
         utf8String += 4;
      else
         utf8String += 1;

      characterOffset--;
   }

   return utf8String;
}

size_t gt_utf8Len( const char *utf8String )
{
   size_t len = 0;
   while( *utf8String )
   {
      unsigned char byte = *utf8String;

      if( ( byte & 0x80 ) == 0 )         // ASCII (0xxxxxxx)
         utf8String += 1;
      else if( ( byte & 0xE0 ) == 0xC0 ) // (110xxxxx)
         utf8String += 2;
      else if( ( byte & 0xF0 ) == 0xE0 ) // (1110xxxx)
         utf8String += 3;
      else if( ( byte & 0xF8 ) == 0xF0 ) // (11110xxx)
         utf8String += 4;
      else
         utf8String += 1;

      len++;
   }

   return len;
}

size_t gt_utf8LenUpTo( const char *utf8String, const char *endPosition )
{
   size_t len = 0;
   while( utf8String < endPosition && *utf8String )
   {
      unsigned char byte = *utf8String;

      if( ( byte & 0x80 ) == 0 )         // ASCII (0xxxxxxx)
         utf8String += 1;
      else if( ( byte & 0xE0 ) == 0xC0 ) // (110xxxxx)
         utf8String += 2;
      else if( ( byte & 0xF0 ) == 0xE0 ) // (1110xxxx)
         utf8String += 3;
      else if( ( byte & 0xF8 ) == 0xF0 ) // (11110xxx)
         utf8String += 4;
      else
         utf8String += 1;

      len++;
   }

   return len;
}

/* ------------------------------------------------------------------------- */
const char *gt_cwd( void )
{
   static char result[ PATH_MAX ];
   size_t len;

   if( GET_CURRENT_DIR( result, sizeof( result ) ) != NULL )
   {
      len = strlen( result );
      char separator = gt_pathSeparator()[ 0 ];

      // Checking if the path length is within the buffer limits
      if( len >= ( sizeof( result ) - 2 ) )
      {
         fprintf( stderr, "Error: Path exceeds the allowed buffer size. \n" );
         return NULL;
      }

      // Checking if there is already a separator at the end of the path
      if( result[ len - 1 ] != separator )
      {
         result[ len ] = separator;
         result[ len + 1 ] = '\0';
      }

      return result;
   }
   else
   {
      fprintf( stderr, "Error: gt_cwd. \n" );
      return NULL;
   }
}

bool gt_isValidPath( const char *path )
{
   if( path == NULL || path[ 0 ] == '\0' )
   {
      return F;
   }

   if( strlen( path ) >= PATH_MAX )
   {
      return F;
   }

   return T;
}

const char *gt_dirDeleteLastPath( const char *path )
{
   static char result[ PATH_MAX ];
   char *lastPath;

   strncpy( result, path, sizeof( result ) - 1 );
   result[ sizeof( result ) - 1 ] = '\0';

   char separator = gt_pathSeparator()[0];

   lastPath = strrchr( result, separator );
   if( lastPath != NULL )
   {
      char *secondLastPath = NULL;
      *lastPath = '\0';
      secondLastPath = strrchr( result, separator );

      if( secondLastPath != NULL )
      {
         *lastPath = separator;
         *( secondLastPath + 1 ) = '\0';
      }
      else
      {
         *lastPath = separator;
      }
   }

   return result;
}

const char *gt_dirDeleteLastSeparator( const char *path )
{
   static char result[ PATH_MAX ];
   int maxBytes = PATH_MAX - 1;

   strncpy( result, path, maxBytes );
   result[ maxBytes ] = '\0';

   char separator = gt_pathSeparator()[0];

   char *lastSeparator = strrchr( result, separator );
   if( lastSeparator != NULL )
   {
      if( lastSeparator != result )
      {
         *lastSeparator = '\0';
      }
   }

   return result;
}

const char *gt_dirLastName( const char *path )
{
   static char result[ PATH_MAX ];
   int pathLength;

   if( path == NULL || ( pathLength = strlen( path ) ) == 0 )
   {
      result[ 0 ] = '\0';
      return result;
   }

   char separator = gt_pathSeparator()[ 0 ];

   if( path[ pathLength - 1 ] == separator )
   {
      pathLength--;
   }

   for( int i = pathLength - 1; i >= 0; i-- )
   {
      if( path[ i ] == separator )
      {
         int length = pathLength - i - 1;
         strncpy( result, path + i + 1, length );
         result[ length ] = '\0';
         return result;
      }
   }

   strncpy( result, path, pathLength );
   result[ pathLength ] = '\0';

   return result;
}

/* ------------------------------------------------------------------------- */
char *sub_strdup( const char *string )
{
   size_t len = strlen( string ) + 1;
   char *dup = malloc( len );
   if( dup )
   {
      memcpy( dup, string, len );
   }
   return dup;
}
