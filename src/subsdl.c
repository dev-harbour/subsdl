#include "subsdl.h"

/* ------------------------------------------------------------------------- */
// Static
static int sub_handleEvents( Sub *sub, SDL_Event *event )
{
   switch( event->type )
   {
      case SDL_QUIT:
         return SDL_QUIT;

      case SDL_KEYDOWN:
         return event->key.keysym.sym;

      case SDL_WINDOWEVENT:
         if( event->window.event == SDL_WINDOWEVENT_RESIZED )
         {
            int newWidth;
            int newHeight;
            SDL_GetWindowSize( sub->window, &newWidth, &newHeight );

            sub->winWidth = newWidth;
            sub->winHeight = newHeight;

            return 0;
         }
         break;

      default:
         return 0;
   }
   return 0;
}

/* ------------------------------------------------------------------------- */
// API functions
Sub *sub_createWindow( int width, int height, const char *title, const char *hexColor )
{
   setlocale( LC_ALL, "en_US.UTF-8" );

   Sub *sub = malloc( sizeof( Sub ) );
   if( !sub )
   {
      fprintf( stderr, "Memory allocation failed for Sub structure.\n" );
      return NULL;
   }

   memset( sub, 0, sizeof( Sub ) );
   sub->winWidth   = width;
   sub->winHeight  = height;
   sub->background = hexColor;

   if( SDL_Init( SDL_INIT_VIDEO ) != 0 )
   {
      fprintf( stderr, "Unable to initialize SDL: %s\n", SDL_GetError() );
      free( sub );
      return NULL;
   }

   if( TTF_Init() == -1 )
   {
      fprintf( stderr, "Unable to initialize SDL_ttf: %s\n", TTF_GetError() );
      SDL_Quit();
      free( sub );
      return NULL;
   }

   sub->window = SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
   if( !sub->window )
   {
      fprintf( stderr, "Could not create window: %s\n", SDL_GetError() );
      SDL_Quit();
      free( sub );
      return NULL;
   }

   sub->renderer = SDL_CreateRenderer( sub->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
   if( !sub->renderer )
   {
      fprintf( stderr, "Could not create renderer: %s\n", SDL_GetError() );
      SDL_DestroyWindow( sub->window );
      SDL_Quit();
      free( sub );
      return NULL;
   }

   SDL_SetWindowMinimumSize( sub->window, width, height );

   return sub;
}

void sub_clearAll( Sub *sub )
{
   if( sub )
   {
      // ---
      if( sub->fonts )
      {
         for( int i = 0; i < sub->fontsCount; ++i )
         {
            if( sub->fonts[ i ] )
            {
               sub_freeFont( sub->fonts[ i ] );
            }
         }
         free( sub->fonts );
         sub->fonts = NULL;
      }
      // ---
      if( sub->images )
      {
         for( int i = 0; i < sub->imagesCount; ++i )
         {
            if( sub->images[ i ] )
            {
               sub_freeImage( sub->images[ i ] );
            }
         }
         free( sub->images );
         sub->images = NULL;
      }
      // ---
      if( sub->renderer != NULL )
      {
         SDL_DestroyRenderer( sub->renderer );
         sub->renderer = NULL;
      }
      // ---
      if( sub->window != NULL )
      {
         SDL_DestroyWindow( sub->window );
         sub->window = NULL;
      }
      // ---
      TTF_Quit();
      IMG_Quit();
      SDL_Quit();

      free( sub );
      sub = NULL;
   }
}

void sub_beginDraw( Sub *sub )
{
   SDL_Color bgColor = { 50, 50, 50, 255 };
   if( sub->background && strlen( sub->background ) > 0 )
   {
      bgColor = sub_hexToColor( sub->background );
   }

   SDL_SetRenderDrawColor( sub->renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a );
   SDL_RenderClear( sub->renderer );
}

void sub_endDraw( Sub *sub )
{
   SDL_RenderPresent( sub->renderer );
}

/* ------------------------------------------------------------------------- */
int sub_procEvent( Sub *sub )
{
   int result = 0;
   SDL_Event event;
   if( SDL_WaitEvent( &event ) )
   {
      do
      {
         result = sub_handleEvents( sub, &event );
      }
      while( SDL_PollEvent( &event ) );
   }

   return result;
}

int sub_pollEvent( Sub *sub )
{
   int result = 0;
   SDL_Event event;
   while( SDL_PollEvent( &event ) )
   {
      result = sub_handleEvents( sub, &event );
   }
   return result;
}

int sub_waitEvent( Sub *sub )
{
   int result = 0;
   SDL_Event event;
   if( SDL_WaitEvent( &event ) )
   {
      result = sub_handleEvents( sub, &event );
   }
   return result;
}

/* ------------------------------------------------------------------------- */
bool sub_loadFont( Sub *sub, Font *font, const char *fontPath, int fontSize )
{
   if( !sub || !font || !fontPath )
   {
      fprintf( stderr, "Invalid parameters provided to sub_loadFont.\n" );
      return F;
   }

   sub->fontsCount++;

   Font **newFonts = realloc( sub->fonts, sub->fontsCount * sizeof( Font * ) );
   if( !newFonts )
   {
      fprintf( stderr, "Failed to allocate memory for Font array.\n" );
      sub->fontsCount--;
      return F;
   }

   sub->fonts = newFonts;
   sub->fonts[ sub->fontsCount - 1 ] = font;

   font->font = TTF_OpenFont( fontPath, fontSize );
   if( !font->font )
   {
      fprintf( stderr, "Failed to load font: %s\n", TTF_GetError() );
      sub->failedFontsCount++;
      sub->fontsCount--;
      if( sub->fontsCount == 0 )
      {
         free( sub->fonts );
         sub->fonts = NULL;
      }
      memset( font, 0, sizeof( Font ) );

      return F;
   }

   int advance;
   TTF_GlyphMetrics( font->font, 'M', NULL, NULL, NULL, NULL, &advance );

   font->fontCellWidth  = advance;
   font->fontCellHeight = TTF_FontHeight( font->font );
   font->fontSize       = fontSize;

   return T;
}

void sub_drawFont( Sub *sub, Font *font, int col, int row, const wchar_t *string, const char *hexColor )
{
   int x = col * font->fontCellWidth;
   int y = row * font->fontCellHeight;

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

         bgColor = sub_hexToColor( bgColorStr );
         fgColor = sub_hexToColor( separator + 1 );
      }
   }

   size_t len = wcslen( string );
   if( len == 0 )
   {
      SDL_Rect rect = { x, y, font->fontCellWidth, font->fontCellHeight };
      SDL_SetRenderDrawColor( sub->renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a );
      SDL_RenderFillRect( sub->renderer, &rect );
      return;
   }

   SDL_Rect rect = { x, y, font->fontCellWidth * len, font->fontCellHeight };
   SDL_SetRenderDrawColor( sub->renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a );
   SDL_RenderFillRect( sub->renderer, &rect );

   size_t utf8Len = len * 4 + 1;
   char *utf8Text = ( char * ) malloc( utf8Len );

   if( utf8Text )
   {
      size_t result = wcstombs( utf8Text, string, utf8Len );
      if( result == ( size_t ) -1 )
      {
         printf( "Error converting wide string to multibyte string.\n" );
         free( utf8Text );
         return;
      }

      SDL_Surface *textSurface = TTF_RenderUTF8_Shaded( font->font, utf8Text, fgColor, bgColor );
      SDL_Texture *textTexture = NULL;

      if( textSurface )
      {
         textTexture = SDL_CreateTextureFromSurface( sub->renderer, textSurface );
         SDL_FreeSurface( textSurface );
      }
      else
      {
         printf( "Error rendering string: %s\n", TTF_GetError() );
      }

      free( utf8Text );

      if( textTexture )
      {
         SDL_Rect textRect = { x, y, 0, 0 };
         SDL_QueryTexture( textTexture, NULL, NULL, &textRect.w, &textRect.h );
         SDL_RenderCopy( sub->renderer, textTexture, NULL, &textRect );

         SDL_DestroyTexture( textTexture );
      }
   }
}

void sub_freeFont( Font *font )
{
   if( font && font->font )
   {
      TTF_CloseFont( font->font );
      font->font = NULL;
   }
}

void sub_printFontInfo( const Font *font )
{
   printf( "\033[2J" );
   printf( "\033[H" );

   if( font && font->font )
   {
      int minx, maxx, miny, maxy, advance;
      TTF_GlyphMetrics( font->font, 'M', &minx, &maxx, &miny, &maxy, &advance );

      int height   = TTF_FontHeight( font->font );
      int ascent   = TTF_FontAscent( font->font );
      int descent  = TTF_FontDescent( font->font );
      int lineSkip = TTF_FontLineSkip( font->font );

      printf( "Font Info\n" );
      printf( "[\n" );
      printf( "   TTF_Font      : %p\n", font->font );
      printf( "   Font minx     : %d\n", minx );
      printf( "   Font maxx     : %d\n", maxx );
      printf( "   Font miny     : %d\n", miny );
      printf( "   Font maxy     : %d\n", maxy );
      printf( "   Font advance  : %d\n", advance );
      printf( "   Font height   : %d\n", height );
      printf( "   Font ascent   : %d\n", ascent );
      printf( "   Font descent  : %d\n", descent );
      printf( "   Font lineSkip : %d\n", lineSkip );
      printf( "   Font size     : %d\n", font->fontSize );
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
bool sub_loadImage( Sub *sub, Image *image, const char *imagePath )
{
   if( !sub || !image || !imagePath )
   {
      fprintf( stderr, "Invalid parameters provided to sub_loadImage.\n" );
      return F;
   }

   sub->imagesCount++;

   Image **newImage = realloc( sub->images, sub->imagesCount * sizeof( Image * ) );
   if( !newImage )
   {
      fprintf( stderr, "Failed to allocate memory for Image array.\n" );
      sub->imagesCount--;
      return F;
   }

   sub->images = newImage;
   sub->images[ sub->imagesCount - 1 ] = image;

   const char *ext = strrchr( imagePath, '.' );
   if( !ext )
   {
      fprintf( stderr, "File extension not found in path: %s\n", imagePath );
      sub->imagesCount--;
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
      sub->imagesCount--;
      return F;
   }

   if( !( IMG_Init( imgFlags ) & imgFlags ) )
   {
      fprintf( stderr, "Failed to initialize SDL_image: %s\n", IMG_GetError() );
      sub->imagesCount--;
      return F;
   }

   surface = IMG_Load( imagePath );
   if( !surface )
   {
      fprintf( stderr, "Failed to load image: %s\n", IMG_GetError() );
      sub->failedImagesCount++;
      sub->imagesCount--;
      return F;
   }

   image->texture = SDL_CreateTextureFromSurface( sub->renderer, surface );
   if( !image->texture )
   {
      fprintf( stderr, "Failed to create texture from image: %s\n", SDL_GetError() );
      SDL_FreeSurface( surface );
      sub->failedImagesCount++;
      sub->imagesCount--;
      return F;
   }

   image->width = surface->w;
   image->height = surface->h;

   SDL_FreeSurface( surface );

   return T;
}

void sub_drawImage( Sub *sub, Image *image, int x, int y, int width, int height )
{
   if( !sub || !image || !image->texture )
   {
      fprintf( stderr, "Invalid parameters provided to sub_drawImage.\n" );
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

   SDL_RenderCopy( sub->renderer, image->texture, NULL, &dstRect );
}

void sub_freeImage( Image *image )
{
   if( image && image->texture )
   {
      SDL_DestroyTexture( image->texture );
      image->texture = NULL;
   }
}

/* ------------------------------------------------------------------------- */
SDL_Color sub_hexToColor( const char *hexColor )
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
      fprintf( stderr, "Invalid hex color format: sub_hexToColor() - defaulting to gray.\n" );
      color.r = 128;
      color.g = 128;
      color.b = 128;
      color.a = 255;
   }

   return color;
}

char *sub_strdup( const char *str )
{
   size_t len = strlen( str ) + 1;
   char *dup = malloc( len );
   if( dup )
   {
      memcpy( dup, str, len );
   }
   return dup;
}
