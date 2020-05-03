/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser,
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  push: push a new origin matrix onto the origin stack

  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix
                     based on the provided values, then
                     multiply the current top of the
                     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a
                    temporary matrix, multiply it by the
                    current top of the origins stack, then
                    call draw_polygons.

  line: create a line based on the provided values. Store
        that in a temporary matrix, multiply it by the
        current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the screen
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"
#include "gmath.h"

void my_main() {

  int i;
  struct matrix *tmp;
  struct matrix *polygons;
  struct stack *systems;
  screen t;
  zbuffer zb;
  color g;
  double step_3d = 20;

  //Lighting values here for easy access
  color ambient;
  ambient.red = 50;
  ambient.green = 50;
  ambient.blue = 50;

  double light[2][3];
  light[LOCATION][0] = 0.5;
  light[LOCATION][1] = 0.75;
  light[LOCATION][2] = 1;

  light[COLOR][RED] = 255;
  light[COLOR][GREEN] = 255;
  light[COLOR][BLUE] = 255;

  double view[3];
  view[0] = 0;
  view[1] = 0;
  view[2] = 1;

  //default reflective constants if none are set in script file
  struct constants white;
  white.r[AMBIENT_R] = 0.1;
  white.g[AMBIENT_R] = 0.1;
  white.b[AMBIENT_R] = 0.1;

  white.r[DIFFUSE_R] = 0.5;
  white.g[DIFFUSE_R] = 0.5;
  white.b[DIFFUSE_R] = 0.5;

  white.r[SPECULAR_R] = 0.5;
  white.g[SPECULAR_R] = 0.5;
  white.b[SPECULAR_R] = 0.5;

  //constants are a pointer in symtab, using one here for consistency
  struct constants *reflect;
  reflect = &white;

  systems = new_stack();
  polygons = new_matrix(4, 1000);
  clear_screen( t );
  clear_zbuffer(zb);
  g.red = 0;
  g.green = 0;
  g.blue = 0;

  print_symtab();
  for (i=0;i<lastop;i++) {

    switch (op[i].opcode) {
      case PUSH:
        printf("pushing\n");
        push(systems);
        break;
      case POP:
        printf("popping\n");
        pop(systems);
        break;
      case SAVE:
        printf("saving\n");
        save_extension(t, op[i].op.save.p->name);
      case MOVE:
        printf("moving\n");
        tmp = make_translate(op[i].op.move.d[0], op[i].op.move.d[1], op[i].op.move.d[2]);
        matrix_mult(peek(systems), tmp);
        copy_matrix(tmp, peek(systems));
        break;
      case ROTATE:
        printf("rotating\n");
        switch (op[i].op.rotate.axis) {
          case 0:
            tmp = make_rotX(op[i].op.rotate.degrees);
            break;
          case 1:
            tmp = make_rotY(op[i].op.rotate.degrees);
            break;
          case 2:
            tmp = make_rotZ(op[i].op.rotate.degrees);
            break;
        }
        matrix_mult(peek(systems), tmp);
        copy_matrix(tmp, peek(systems));
        break;
      case SCALE:
        printf("scaling\n");
        tmp = make_scale(op[i].op.scale.d[0], op[i].op.scale.d[1], op[i].op.scale.d[2]);
        matrix_mult(peek(systems), tmp);
        copy_matrix(tmp, peek(systems));
        break;
      case BOX:
        printf("making box\n");
        add_box(polygons, op[i].op.box.d0[0], op[i].op.box.d0[1], op[i].op.box.d0[2], op[i].op.box.d1[0], op[i].op.box.d1[1], op[i].op.box.d1[2]);
        matrix_mult(peek(systems), polygons);
        if (op[i].op.box.constants)
          reflect = op[i].op.box.constants->s.c;
        else
          reflect = &white;
        draw_polygons(polygons, t, zb,
                      view, light, ambient, reflect);
        polygons->lastcol = 0;
        break;
      case TORUS:
        printf("making torus\n");
        add_torus(polygons, op[i].op.torus.d[0], op[i].op.torus.d[1], op[i].op.torus.d[2],
                  op[i].op.torus.r0, op[i].op.torus.r1, step_3d);
        matrix_mult(peek(systems), polygons);
        if (op[i].op.torus.constants)
          reflect = op[i].op.torus.constants->s.c;
        else
          reflect = &white;
        draw_polygons(polygons, t, zb,
                      view, light, ambient, reflect);
        polygons->lastcol = 0;
        break;
      case SPHERE:
        printf("making sphere\n");
        add_sphere(polygons, op[i].op.sphere.d[0], op[i].op.sphere.d[1], op[i].op.sphere.d[2],
                    op[i].op.sphere.r, step_3d);
        if (op[i].op.torus.constants)
          reflect = op[i].op.sphere.constants->s.c;
        else
          reflect = &white;
        draw_polygons(polygons, t, zb,
                      view, light, ambient, reflect);
        polygons->lastcol = 0;
        break;
      default:
        printf("%d\n", op[i].opcode);
    }

    printf("stack size: %d\n", systems->size);
  }
}
