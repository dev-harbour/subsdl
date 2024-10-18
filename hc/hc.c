/*
 * Copyright 2014 - 2024 Rafał Jopek ( rafaljopek at hotmail com )
 *
 * This work has been migrated from my project,
 * which was originally developed in the Harbour language. The code is
 * inspired by previous functions and structures that were implemented
 * in that project. This note serves to highlight the development history
 * and the source of inspiration for the current project.
 *
 */

#include "hc.h"

static HC *lPanel = NULL;
static HC *rPanel = NULL;
static HC *aPanel = NULL;

int main( void )
{
   bool quit = T;
   int key;
   GT *gt = gt_createWindow( 720, 450, "Harbour Commander", "F1F1F1" );

   gt_loadFont( gt, "../docs/assets/font/9x18.pcf.gz", 18 );

   lPanel = hc_init();
   rPanel = hc_init();

   hc_fetchList( lPanel, gt_cwd() );
   hc_fetchList( rPanel, gt_cwd() );

   aPanel = lPanel;

   while( quit )
   {
      key = gt_procEvent( gt );

      switch( key )
      {
         case SDL_QUIT:
            quit = F;
            break;

         case SDLK_ESCAPE:
            quit = F;
            break;

         case SDL_WINDOWEVENT_CLOSE:
            return SDL_QUIT;

         case SDLK_TAB:

            if( aPanel == lPanel )
            {
               aPanel = rPanel;
            }
            else
            {
               aPanel = lPanel;
            }
            break;

         default:
            break;
      }

      gt_beginDraw( gt );

         hc_autosize( gt );
         hc_drawPanel( gt, lPanel );
         hc_drawPanel( gt, rPanel );
         hc_drawComdLine( gt, aPanel );

      gt_endDraw( gt );
   }

   hc_free( lPanel );
   hc_free( rPanel );
   aPanel = NULL;

   gt_clearAll( gt );
}

static HC *hc_init( void )
{
   HC *hc = malloc( sizeof( HC ) );
   if( !hc )
   {
      fprintf( stderr, "Failed to allocate memory for HC. \n" );
      return NULL;
   }

   memset( hc, 0, sizeof( HC ) );

   hc->cmdLine = "";

   hc->isFirstDirectory  = T;
   hc->isHiddenDirectory = F;
   hc->isFirstFile       = F;
   hc->isHiddenFile      = F;

   return hc;
}

static void hc_free( HC *hc )
{
   if( hc )
   {
      if( hc->files )
      {
         gt_freeDirectory( hc->files );
      }
      free( hc );
   }
}

void hc_fetchList( HC *hc, const char *currentDir )
{
   if( currentDir == NULL || ! gt_isValidPath( currentDir ) )
   {
      const char *dir = gt_cwd();
      gt_safeCopy( hc->currentDir, dir, sizeof(  hc->currentDir ) );
   }
   else
   {
      gt_safeCopy( hc->currentDir, currentDir, sizeof( hc->currentDir ) );
   }

   gt_freeDirectory( hc->files );
   hc->filesCount = 0;
   hc->files = gt_directory( hc->currentDir, &hc->filesCount );

   if( hc->isFirstDirectory )
   {
      qsort( hc->files, hc->filesCount, sizeof( FileInfo ), hc_compareFiles );
   }
}

static int hc_compareFiles( const void *a, const void *b )
{
   FileInfo *fileA = ( FileInfo * ) a;
   FileInfo *fileB = ( FileInfo * ) b;

   // The ".." Directory always comes first
   if( strcmp( fileA->name, ".." ) == 0 ) return -1;
   if( strcmp( fileB->name, ".." ) == 0 ) return 1;

   // Directories before files
   bool isDirA = strchr( fileA->attr, 'D' ) != NULL;
   bool isDirB = strchr( fileB->attr, 'D' ) != NULL;
   if( isDirA != isDirB )
   {
      return IIF( isDirA, -1, 1 );
   }

   // Hidden files/directories after regular ones
   bool isHiddenA = strchr( fileA->attr, 'H' ) != NULL;
   bool isHiddenB = strchr( fileB->attr, 'H' ) != NULL;
   if( isHiddenA != isHiddenB )
   {
      return IIF( isHiddenA, 1, -1 );
   }

   return strcmp( fileA->name, fileB->name );
}

static void hc_autosize( GT *gt )
{
   int maxCol = gt_maxCol( gt );
   int maxRow = gt_maxRow( gt );

   if( maxCol == 0 || maxRow == 0 )
   {
      fprintf( stderr, "Error: Invalid maxCol or maxRow.\n" );
      return;
   }

   hc_resize( lPanel, 0, 0, maxCol / 2, maxRow - 1 );
   hc_resize( rPanel, maxCol / 2, 0, maxCol / 2, maxRow - 1 );
}

static void hc_resize( HC *hc, int col, int row, int maxCol, int maxRow )
{
   hc->col    = col;
   hc->row    = row;
   hc->maxCol = maxCol;
   hc->maxRow = maxRow;
}

static void hc_drawPanel( GT *gt, HC *hc )
{
   int row, i = 0;
   int longestName = 4;
   int longestSize = 0;
   int longestAttr = 0;

   longestName = MAX( longestName, hc_findLongestName( hc ) );
   longestSize = MAX( longestSize, hc_findLongestSize( hc ) );
   longestAttr = MAX( longestAttr, hc_findLongestAttr( hc ) );

   if( aPanel == hc )
   {
      gt_dispBox( gt, hc->col, hc->row, hc->maxCol, hc->maxRow, BOX_DOUBLE, "F1F1F1/FF0000" );
   }
   else
   {
      gt_dispBox( gt, hc->col, hc->row, hc->maxCol, hc->maxRow, BOX_SINGLE, "F1F1F1/323232" );
   }

   i += hc->rowNo;
   for( row = hc->row + 1; row < hc->maxRow -1; row++ )
   {
      if( i < hc->filesCount )
      {
         const char *paddedString = hc_paddedString( hc, longestName, longestSize, longestAttr,
                                                     hc->files[ i ].name,
                                                     hc->files[ i ].size,
                                                     hc->files[ i ].date,
                                                     hc->files[ i ].time,
                                                     hc->files[ i ].attr );

         char *paddedResult = gt_padR( paddedString, hc->maxCol - 2 );

         gt_drawFont( gt, hc->col + 1, row, paddedResult,
            IIF( aPanel == hc && i == hc->rowBar + hc->rowNo,
            IIF( hc->files[ i ].state == T, "323232/FF4D4D", "323232/00FF00" ),
            hc_selectColor( hc->files[ i ].attr, hc->files[ i ].state ) ) );

         free( paddedResult );

         ++i;
      }
      else
      {
         break;
      }
   }
}

static int hc_findLongestName( HC *hc )
{
   int longestName = 0;

   for( int i = 0; i < hc->filesCount; i++ )
   {
      int currentNameLength = gt_utf8Len( hc->files[ i ].name );
      if( currentNameLength > longestName )
      {
         longestName = currentNameLength;
      }
   }

   return longestName;
}

static int hc_findLongestSize( HC *hc )
{
   int longestSize = 0;

   for( int i = 0; i < hc->filesCount; i++ )
   {
      int currentSizeLength = gt_utf8Len( hc->files[ i ].size );
      if( currentSizeLength > longestSize )
      {
         longestSize = currentSizeLength;
      }
   }

   return longestSize;
}

static int hc_findLongestAttr( HC *hc )
{
   int longestAttr = 0;

   for( int i = 0; i < hc->filesCount; i++ )
   {
      int currentAttrLength = gt_utf8Len( hc->files[ i ].attr );
      if( currentAttrLength > longestAttr )
      {
         longestAttr = currentAttrLength;
      }
   }

   return longestAttr;
}

static const char *hc_paddedString( HC *hc, int longestName, int longestSize, int longestAttr, const char *name, const char *size, const char *date, const char *time, const char *attr )
{
   int lengthName = longestName;
   int lengthSize = longestSize;
   int lengthAttr = longestAttr;
   int lengthDate = 10;
   int lengthTime = 8;
   int parentDir  = 4;
   int space      = 1;
   int nullTerminator = 1;

   static char formattedLine[ PATH_MAX ];

   if( strchr( attr, 'D' ) != NULL )
   {
      size = "DIR";
   }

   char *padLAttr = gt_padL( attr, lengthAttr );
   char *padLSize = gt_padL( size, lengthSize );

   char sizeAttrDateTime[ lengthSize + space + lengthAttr + space + lengthDate + space + lengthTime + nullTerminator ];
   snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s %s %s", padLSize, padLAttr, date, time );

   if( strcmp( name, ".." ) == 0 )
   {
      const char *lBracket = "[";
      const char *rBracket = "]";

      char *padLSizeAttrDateTime = gt_padL( sizeAttrDateTime, hc->maxCol - 2 - parentDir );
      snprintf( formattedLine, sizeof( formattedLine ), "%s%s%s%s", lBracket, name, rBracket, padLSizeAttrDateTime );

      free( padLSizeAttrDateTime );
   }
   else
   {
      char *padLSizeAttrDateTime = gt_padL( sizeAttrDateTime, hc->maxCol - 2 - lengthName );

      int availableWidthForName = hc->maxCol - 2 - strlen( padLSizeAttrDateTime );

      char *cutName = gt_left( name, availableWidthForName );

      char *padRName = gt_padR( cutName, availableWidthForName );

      snprintf( formattedLine, sizeof( formattedLine ), "%s%s", padRName, padLSizeAttrDateTime );

      free( cutName );
      free( padRName );
      free( padLSizeAttrDateTime );
   }

   free( padLAttr );
   free( padLSize );

   // printf( "Debugging: '%s'\n", formattedLine );

   return formattedLine;
}

static const char *hc_selectColor( const char *attr, bool state )
{
   if( state == T )
   {
      return "EAEAEA/B30000"; // Kolor na zaznaczonych plikach
   }
   else if( strcmp( attr, "DH" ) == 0 || strcmp( attr, "AH" ) == 0 )
   {
      return "EAEAEA/72A0E5";
   }
   else
   {
      return "EAEAEA/323232"; // Kolor dla pozostałych plików
   }
}

static void hc_drawComdLine( GT *gt, HC *hc )
{
   int maxCol = gt_maxCol( gt );
   int maxRow = gt_maxRow( gt );
   const char *promptEnd;

   if( strcmp( "Windows", gt_os() ) == 0 )
   {
      promptEnd = ">";
   }
   else
   {
      promptEnd = "$ ";
   }

   char *currentDir = gt_dirDeleteLastSeparator( hc->currentDir );

   char *combinedDir = gt_addStr( currentDir, promptEnd, NULL );
   char *cmdLinePart = gt_subStr( hc->cmdLine, 1 + hc->cmdColNo, maxCol + hc->cmdColNo );
   char *combinedCmdLine = gt_addStr( combinedDir, cmdLinePart, NULL );
   char *paddedCmdLine = gt_padR( combinedCmdLine, maxCol );

   gt_drawFont( gt, 0, maxRow - 1, paddedCmdLine, "323232/00FF00" );

   free( combinedDir );
   free( cmdLinePart );
   free( combinedCmdLine );
   free( paddedCmdLine );
}
