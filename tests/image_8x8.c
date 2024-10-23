/*
 *
 */

#include "subsdl.h"

int main( int argc, char *argv[] )
{
   UNUSED( argc );
   UNUSED( argv );

   bool quit = F;
   GT *gt = gt_createWindow( 830, 450, "Test Image 8x8", "323232" );

   Image image;
   gt_loadImage( gt, &image, "../docs/assets/img/8x8.png" );

   while( !quit )
   {
      SDL_Event event;

      if( SDL_WaitEvent( &event ) )
      {
         do
         {
            switch( event.type )
            {
               case SDL_QUIT:
                  printf( "Window closed\n" );
                  quit = T;
                  break;

               case SDL_WINDOWEVENT:
                  if( event.window.event == SDL_WINDOWEVENT_CLOSE )
                  {
                     printf( "Window close requested\n" );
                     quit = T;
                  }
                  break;

               case SDL_KEYDOWN:
                  if( event.key.keysym.sym == SDLK_ESCAPE )
                  {
                     printf( "Esc pressed\n" );
                     quit = T;
                  }
                  else
                  {
                     printf( "Other key pressed (key: %d)\n", event.key.keysym.sym );
                  }
                  break;

               default:
                  printf( "Unhandled event (type: %d)\n", event.type );
                  break;
            }

         }
         while( SDL_PollEvent( &event ) );
      }

      gt_beginDraw( gt );

         for( int i = 0; i < gt->width; i += 8 )
         {
            for( int j = 0; j < gt->height; j += 8 )
            {
               gt_drawImage( gt, &image, i, j, 0, 0 );
            }
         }

      gt_endDraw( gt );
   }

   gt_clearAll( gt );
}
