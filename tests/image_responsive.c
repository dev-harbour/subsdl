/*
 *
 */

#include "subsdl.h"

int main()
{
   bool quit = T;
   Sub *sub = sub_createWindow( 830, 450, "Image Responsive", "000000" );
   int key;

   Image image;
   sub_loadImage( sub, &image, "path to image" );

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

         // Keep image proportions
         float imageAspectRatio = ( float ) image.width / ( float ) image.height;
         float windowAspectRatio = ( float ) sub->winWidth / ( float ) sub->winHeight;

         int drawWidth, drawHeight;

         // Scaling an image while maintaining proportions
         if( windowAspectRatio > imageAspectRatio )
         {
            // Window height limitation
            drawHeight = sub->winHeight;
            drawWidth = ( int ) ( drawHeight * imageAspectRatio );
         }
         else
         {
            // Ograniczenie szerokością okna
            drawWidth = sub->winWidth;
            drawHeight = ( int ) ( drawWidth / imageAspectRatio );
         }

         // Center the image
         int x = ( sub->winWidth - drawWidth ) / 2;
         int y = ( sub->winHeight - drawHeight ) / 2;

         sub_drawImage( sub, &image, x, y, drawWidth, drawHeight );

      sub_endDraw( sub );
   }

   sub_clearAll( sub );
}
