#ifndef _H_AUTOGRAD_
#define _H_AUTOGRAD_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

enum Operation {
    NOOP,
    ADD,
    SUB,
    MULT,
    DIV,
    POW,
    RELU,
//    SIGMOID
};

typedef struct Value {
    double data;
    double grad;
    struct Value **prev;
    size_t prev_count;
    enum Operation op;
    bool visited;
} Value;

void value_print(Value *value, bool print_children);
Value *value_add(Value *self, Value *other);
Value *value_sub(Value *self, Value *other);
Value *value_mult(Value *self, Value *other);
Value *value_div(Value *self, Value *other);
Value *value_pow(Value *self, Value *other);
Value *value_relu(Value *self);
Value *value_sigmoid(Value *self);
void _value_add_backward(Value *self, Value *other, double upstream_grad);
void _value_sub_backward(Value *self, Value *other, double upstream_grad);
void _value_mult_backward(Value *self, Value *other, double upstream_grad);
void _value_div_backward(Value *self, Value *other, double upstream_grad);
void _value_pow_backward(Value *self, Value *other, double upstream_grad);
void _value_relu_backward(Value *self, double upstream_grad);
void _value_sigmoid_backward(Value *self, double upstream_grad);
void backprop(Value *output);
Value *value_create(double data);
void value_destroy(Value *value);

#endif
