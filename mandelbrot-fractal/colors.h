//
//  colors.h
//  mandelbrot-fractal
//
// paleta de cores que estao sendo utilizadas

#ifndef colors_h
#define colors_h

static int palette[] = {
  4333071,
  1640218,
  590127,
  263241,
  1892,
  797834,
  1594033,
  3767761,
  8828389,
  13888760,
  15854015,
  16304479,
  16755200,
  13402112,
  10049280,
  6960131
};

//escolhe a quantidade de cores que queremos que tenha no desenho
static void colors_init(int *colors, int length) {
  for (int i= 0; i < length - 1; i++) {
    colors[i] = palette[i % 16];
  }
}


#endif /* colors_h */
