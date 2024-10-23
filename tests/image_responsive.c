/*
 *
 */

#include "subsdl.h"

int main( int argc, char *argv[] )
{
   UNUSED( argc );
   UNUSED( argv );

   bool quit = F;
   GT *gt = gt_createWindow( 830, 450, "Image Responsive", "000000" );

   Image image;
   gt_loadImage( gt, &image, "path_to_your_image" );

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

         // Keep image proportions
         float imageAspectRatio = ( float ) image.width / ( float ) image.height;
         float windowAspectRatio = ( float ) gt->width / ( float ) gt->height;

         int drawWidth, drawHeight;

         // Scaling an image while maintaining proportions
         if( windowAspectRatio > imageAspectRatio )
         {
            // Window height limitation
            drawHeight = gt->height;
            drawWidth = ( int ) ( drawHeight * imageAspectRatio );
         }
         else
         {
            // Ograniczenie szerokością okna
            drawWidth = gt->width;
            drawHeight = ( int ) ( drawWidth / imageAspectRatio );
         }

         // Center the image
         int x = ( gt->width - drawWidth ) / 2;
         int y = ( gt->height - drawHeight ) / 2;

         gt_drawImage( gt, &image, x, y, drawWidth, drawHeight );

      gt_endDraw( gt );
   }

   gt_clearAll( gt );
}
