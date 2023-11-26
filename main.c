#include "autograd.h"
#include "graph_visualizer.h"
#include <stdio.h>

int main(void)
{
    Value *_a = value_create(10.0);
    Value *_b = value_create(-5.0);
    Value *a = value_add(_a, _b);

    Value *f = value_create(7.0);
    Value *af = value_mult(f, a);

    Value *b = value_create(3.0);
    Value *ab = value_mult(a, b);

    Value *afb = value_mult(af, ab);

    Value *c = value_create(2.0);
    Value *d = value_create(4.0);
    Value *cd = value_mult(c, d);

    Value *res = value_mult(afb, cd);

    /**
     *           b --
     * _a --          ⊢-- ab --
     *       ⊢-- a --           ⊢-- afb ---- c
     * _b --          ⊢-- af --             |
     *               |                      |
     * f ------------                       |
     *                    c --              |
     *                          ⊢-- cd  ----
     *                    d --
     *
     */

    backprop(res);

    // printf("____\n");
    // printf("_a ");
    // value_print(_a, false);
    // printf("_b ");
    // value_print(_b, false);
    // printf("*a ");
    // value_print(a, false);
    // printf("f ");
    // value_print(f, false);
    // printf("af ");
    // value_print(af, false);
    // printf("b ");
    // value_print(b, false);
    // printf("ab ");
    // value_print(ab, false);
    // printf("afb");
    // value_print(afb, false);
    // printf("c ");
    // value_print(c, false);
    // printf("d ");
    // value_print(d, false);
    // printf("cd ");
    // value_print(cd, false);
    // value_print(res, false);
    init();
}
