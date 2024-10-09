/*
 *
 */

#include "subsdl.h"

int main()
{
   bool quit = T;
   int key;
   Sub *sub = sub_createWindow( 830, 450, "Test Unicode", "FFFFFF" );

   Font font;
   sub_loadFont( sub, &font, "../docs/assets/font/9x18.pcf.gz", 18 );

   const wchar_t *text[] =
   {
      L"",
      L"Mathematics and sciences:",
      L"",
      L"  ∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i),      ⎧⎡⎛┌─────┐⎞⎤⎫",
      L"                                            ⎪⎢⎜│a²+b³ ⎟⎥⎪",
      L"  ∀x∈ℝ: ⌈x⌉ = −⌊−x⌋, α ∧ ¬β = ¬(¬α ∨ β),    ⎪⎢⎜│───── ⎟⎥⎪",
      L"                                            ⎪⎢⎜⎷ c₈   ⎟⎥⎪",
      L"  ℕ ⊆ ℕ₀ ⊂ ℤ ⊂ ℚ ⊂ ℝ ⊂ ℂ,                   ⎨⎢⎜       ⎟⎥⎬",
      L"                                            ⎪⎢⎜ ∞     ⎟⎥⎪",
      L"  ⊥ < a ≠ b ≡ c ≤ d ≪ ⊤ ⇒ (⟦A⟧ ⇔ ⟪B⟫),      ⎪⎢⎜ ⎲     ⎟⎥⎪",
      L"                                            ⎪⎢⎜ ⎳aⁱ-bⁱ⎟⎥⎪",
      L"  2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ, ⌀ 200 mm     ⎩⎣⎝i=1    ⎠⎦⎭",
      L"",
      L"Linguistics and dictionaries:",
      L"",
      L"  ði ıntəˈnæʃənəl fəˈnɛtık əsoʊsiˈeıʃn",
      L"  Y [ˈʏpsilɔn], Yen [jɛn], Yoga [ˈjoːgɑ]",
      L"",
      L"APL:",
      L"",
      L"  ((V⍳V)=⍳⍴V)/V←,V    ⌷←⍳→⍴∆∇⊃‾⍎⍕⌈"
   };

   int num_lines = LEN( text );

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
         for( int i = 0; i < num_lines; i++ )
         {
            sub_drawFont( sub, &font, 0, i, text[ i ], "FFFFFF/000000" );
         }
      sub_endDraw( sub );
   }

   sub_printFontInfo( &font );

   sub_clearAll( sub );
   return 0;
}
