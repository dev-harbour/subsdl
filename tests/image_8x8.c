/*
 *
 */

#include "subsdl.h"

int main()
{
   bool quit = T;
   Sub *sub = sub_createWindow( 830, 450, "Test Image 8x8", "323232" );
   int key;

   Image image;
   sub_loadImage( sub, &image, "../docs/assets/img/8x8.png" );

   while( quit )
   {
      key = sub_procEvent( sub );

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

      sub_beginDraw( sub );

         for( int i = 0; i < sub->winWidth; i += 8 )
         {
            for( int j = 0; j < sub->winHeight; j += 8 )
            {
               sub_drawImage( sub, &image, i, j, 0, 0 );
            }
         }

      sub_endDraw( sub );
   }

   sub_clearAll( sub );
}
