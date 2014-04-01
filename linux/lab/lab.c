#include <stdio.h>
#include <math.h>

#ifndef Map
#  define Map(x, imin, imax, omin, omax)                                   \
   (                                                                       \
      (omax) - (omin) > (imax) - (imin) ?                                  \
      ((x) - (imin)) * (((omax) - (omin)) / ((imax) - (imin))) + (omin) :  \
      ((x) - (imin)) / (((imax) - (imin)) / ((omax) - (omin))) + (omin)    \
   )
#endif

void main ()
{
   printf("%d\n", (int)Map(0.0f, 0.15f, 1.0f, (int)50, (int)11));
   printf("%d\n", (int)Map(0.25f, 0.15f, 1.0f, (int)50, (int)11));
   printf("%d\n", (int)Map(0.5f, 0.15f, 1.0f, (int)50, (int)11));
   printf("%d\n", (int)Map(0.75f, 0.15f, 1.0f, (int)50, (int)11));
   printf("%d\n", (int)Map(1.0f, 0.15f, 1.0f, (int)50, (int)11));
}
