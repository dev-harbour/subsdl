/*
 *
 */

#include "subsdl.h"

int main()
{
   bool quit = T;
   int key;
   GT *gt = gt_createWindow( 830, 450, "Test Unicode", "FFFFFF" );

   gt_loadFont( gt, "../docs/assets/font/18x18ko.pcf.gz", 18 );

   const char *text[] =
   {
      "",
      "Mathematics and sciences:",
      "",
      "  ∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i),      ⎧⎡⎛┌─────┐⎞⎤⎫",
      "                                            ⎪⎢⎜│a²+b³ ⎟⎥⎪",
      "  ∀x∈ℝ: ⌈x⌉ = −⌊−x⌋, α ∧ ¬β = ¬(¬α ∨ β),    ⎪⎢⎜│───── ⎟⎥⎪",
      "                                            ⎪⎢⎜⎷ c₈   ⎟⎥⎪",
      "  ℕ ⊆ ℕ₀ ⊂ ℤ ⊂ ℚ ⊂ ℝ ⊂ ℂ,                   ⎨⎢⎜       ⎟⎥⎬",
      "                                            ⎪⎢⎜ ∞     ⎟⎥⎪",
      "  ⊥ < a ≠ b ≡ c ≤ d ≪ ⊤ ⇒ (⟦A⟧ ⇔ ⟪B⟫),      ⎪⎢⎜ ⎲     ⎟⎥⎪",
      "                                            ⎪⎢⎜ ⎳aⁱ-bⁱ⎟⎥⎪",
      "  2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ, ⌀ 200 mm     ⎩⎣⎝i=1    ⎠⎦⎭",
      "",
      "Linguistics and dictionaries:",
      "",
      "  ði ıntəˈnæʃənəl fəˈnɛtık əsoʊsiˈeıʃn",
      "  Y [ˈʏpsilɔn], Yen [jɛn], Yoga [ˈjoːgɑ]",
      "",
      "APL:",
      "",
      "  ((V⍳V)=⍳⍴V)/V←,V    ⌷←⍳→⍴∆∇⊃‾⍎⍕⌈"
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
      gt_endDraw( gt );
   }

   gt_printFontInfo( gt );

   gt_clearAll( gt );
   return 0;
}
