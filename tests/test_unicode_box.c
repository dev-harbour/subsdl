/*
 *
 */

#include "subsdl.h"

int main()
{
   bool quit = T;
   int key;
   GT *gt = gt_createWindow( 830, 450, "Box drawing alignment tests", "FFFFFF" );

   gt_loadFont( gt, "../docs/assets/font/9x18.pcf.gz", 18 );

   const char *text[] =
   {
      "",
      "                                                                    █",
      "                                                                    ▉",
      "╔══╦══╗  ┌──┬──┐  ╭──┬──╮  ╭──┬──╮  ┏━━┳━━┓  ┎┒┏┑   ╷  ╻ ┏┯┓ ┌┰┐    ▊ ╱╲╱╲╳╳╳",
      "║┌─╨─┐║  │╔═╧═╗│  │╒═╪═╕│  │╓─╁─╖│  ┃┌─╂─┐┃  ┗╃╄┙  ╶┼╴╺╋╸┠┼┨ ┝╋┥    ▋ ╲╱╲╱╳╳╳",
      "║│╲ ╱│║  │║   ║│  ││ │ ││  │║ ┃ ║│  ┃│ ╿ │┃  ┍╅╆┓   ╵  ╹ ┗┷┛ └┸┘    ▌ ╱╲╱╲╳╳╳",
      "╠╡ ╳ ╞╣  ├╢   ╟┤  ├┼─┼─┼┤  ├╫─╂─╫┤  ┣┿╾┼╼┿┫  ┕┛┖┚     ┌┄┄┐ ╎ ┏┅┅┓ ┋ ▍ ╲╱╲╱╳╳╳",
      "║│╱ ╲│║  │║   ║│  ││ │ ││  │║ ┃ ║│  ┃│ ╽ │┃  ░░▒▒▓▓██ ┊  ┆ ╎ ╏  ┇ ┋ ▎",
      "║└─╥─┘║  │╚═╤═╝│  │╘═╪═╛│  │╙─╀─╜│  ┃└─╂─┘┃  ░░▒▒▓▓██ ┊  ┆ ╎ ╏  ┇ ┋ ▏",
      "╚══╩══╝  └──┴──┘  ╰──┴──╯  ╰──┴──╯  ┗━━┻━━┛  ▗▄▖▛▀▜   └╌╌┘ ╎ ┗╍╍┛ ┋  ▁▂▃▄▅▆▇█",
      "                                             ▝▀▘▙▄▟"
   };

   int num_lines = LEN( text );

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

         for( int i = 0; i < num_lines; i++ )
         {
            gt_drawFont( gt, 0, i, text[ i ], "FFFFFF/000000" );
         }

         gt_dispBox( gt, 0, 11, 77, 5, BOX_SINGLE, "FFFFFF/000000" );
         gt_dispBox( gt, 0, 16, 77, 5, BOX_DOUBLE, "FFFFFF/000000" );
         gt_dispBox( gt, 0, 21, 77, 5, BOX_SINGLE_DOUBLE, "FFFFFF/000000" );
         gt_dispBox( gt, 0, 26, 77, 5, BOX_DOUBLE_SINGLE, "FFFFFF/000000" );

      gt_endDraw( gt );
   }

   gt_clearAll( gt );
   return 0;
}
