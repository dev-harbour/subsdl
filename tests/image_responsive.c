/*
 *
 */

#include "subsdl.h"

int main()
{
   bool quit = T;
   GT *gt = gt_createWindow( 830, 450, "Image Responsive", "000000" );
   int key;

   Image image;
   gt_loadImage( gt, &image, "path_to_your_image" );

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

         // Keep image proportions
         float imageAspectRatio = ( float ) image.width / ( float ) image.height;
         float windowAspectRatio = ( float ) gt->winWidth / ( float ) gt->winHeight;

         int drawWidth, drawHeight;

         // Scaling an image while maintaining proportions
         if( windowAspectRatio > imageAspectRatio )
         {
            // Window height limitation
            drawHeight = gt->winHeight;
            drawWidth = ( int ) ( drawHeight * imageAspectRatio );
         }
         else
         {
            // Ograniczenie szerokością okna
            drawWidth = gt->winWidth;
            drawHeight = ( int ) ( drawWidth / imageAspectRatio );
         }

         // Center the image
         int x = ( gt->winWidth - drawWidth ) / 2;
         int y = ( gt->winHeight - drawHeight ) / 2;

         gt_drawImage( gt, &image, x, y, drawWidth, drawHeight );

      gt_endDraw( gt );
   }

   gt_clearAll( gt );
}
