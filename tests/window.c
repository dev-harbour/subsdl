/*
 *
 */

#include "subsdl.h"

int main()
{
   bool quit = T;
   int key;
   Sub *sub = sub_createWindow( 830, 450, "Test window", "" );

   while( quit )
   {
      key = sub_waitEvent( sub );

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
         // ...
      sub_endDraw( sub );
   }

   sub_clearAll( sub );
   return 0;
}
