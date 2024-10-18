/*
 *
 */

#include "subsdl.h"

int main()
{
   bool quit = T;
   GT *gt = gt_createWindow( 830, 450, "Test Image 8x8", "323232" );
   int key;

   Image image;
   gt_loadImage( gt, &image, "../docs/assets/img/8x8.png" );

   while( quit )
   {
      key = gt_procEvent( gt );

      switch( key )
      {
         case SDL_QUIT:
            printf( "Window closed (key: %d)\n", key );
            quit = F;
            break;

         case SDLK_ESCAPE:
            printf( "Esc pressed (key: %d)\n", key );
            quit = F;
            break;

         case SDL_WINDOWEVENT_CLOSE:
            printf( "Window close requested\n" );
            return SDL_QUIT;

         default:
            if( key != 0 )
               printf( "Other key pressed (key: %d)\n", key );
            break;
      }

      gt_beginDraw( gt );

         for( int i = 0; i < gt->winWidth; i += 8 )
         {
            for( int j = 0; j < gt->winHeight; j += 8 )
            {
               gt_drawImage( gt, &image, i, j, 0, 0 );
            }
         }

      gt_endDraw( gt );
   }

   gt_clearAll( gt );
}
