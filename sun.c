#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  FILE* file = fopen("sun.mdl", "w");
  fprintf(file, "constants yellow 0.3 0.5 0.2 0.1 0.6 0.5 0 0 0\nconstants orange 0.3 0.6 0.2 0.1 0.4 0 0.1 0 0\n");
  fprintf(file, "push\nmove 250 250 0\nsphere yellow 0 0 0 100\n");
  fprintf(file, "rotate x 15\nrotate y 25\nrotate z 35\n");

  int inc = 40;

  int i;
  for (i = 0; i <= 360; i += inc) {
    fprintf(file, "push\n");
    int j;
    for (j = 0; j <= 180; j += inc) {
      fprintf(file, "box orange 100 5 5 100 -10 -10\n");
      fprintf(file, "rotate z %d\n", inc);
    }
    fprintf(file, "pop\n");
    fprintf(file, "rotate x %d\n", inc);
  }

  fprintf(file, "display\nsave sun.png");

  fclose(file);
}
