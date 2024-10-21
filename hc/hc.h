/*
 *
 */

#ifndef HC_H_
#define HC_H_

#include "subsdl.h"

Uint32 DOUBLE_CLICK_TIME = 500;

typedef struct _HC   HC;

struct _HC
{
   int         col;
   int         row;
   int         maxCol;
   int         maxRow;
   char        currentDir[ PATH_MAX ];

   FileInfo   *files;
   int         filesCount;

   int         rowBar;
   int         rowNo;

   const char *cmdLine;
   int         cmdCol;
   int         cmdColNo;

   bool isFirstDirectory;
   bool isHiddenDirectory;
   bool isFirstFile;
   bool isHiddenFile;

   bool sizeVisible;
   bool attrVisible;
   bool dateVisible;
   bool timeVisible;
};

/* ------------------------------------------------------------------------- */
static HC         *hc_init( void );
void               hc_loadSettings( HC *hc, const char *configFilePath );
static void        hc_free( HC *hc );
static void        hc_fetchList( HC *hc, const char *currentDir );
static int         hc_compareFiles( const void *a, const void *b );
static void        hc_autosize( GT *gt );
static void        hc_resize( HC *hc, int col, int row, int maxCol, int maxRow );
static void        hc_drawPanel( GT *gt, HC *hc );
static int         hc_findLongestName( HC *hc );
static int         hc_findLongestSize( HC *hc );
static int         hc_findLongestAttr( HC *hc );
static const char *hc_paddedString( HC *hc, int longestName, int longestSize, int longestAttr, const char *name, const char *size, const char *date, const char *time, const char *attr );
static const char *hc_selectColor( const char *attr, bool state );
static void        hc_refreshPanel( GT *gt, HC *hc );
static void        hc_drawComdLine( GT *gt, HC *hc );
static void        hc_changeDir( HC *hc );
static void        hc_updateFetchList( HC *hc, const char *newDir );
static int         hc_dirIndexName( HC *hc, const char *tmpDir );

#endif /* End HC_H_ */