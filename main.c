#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#define init_value(d) {.data=d, .grad=0.0f, .prev=NULL} // TODO: Move this to a function instead of a macro
                                                        // TODO: Add a condition which checks what type

typedef struct Value {
    double data;
    double grad;
    struct Value **prev;
    size_t prev_count;
} Value;

enum Operation {
    ADD,
    MULT
};


void value_print(Value *value, bool print_children);
Value *value_add(Value *self, Value *other);
Value *value_mult(Value *self, Value *other);
void _value_add_backward(Value *self, Value *other, double upstream_grad);
void _value_mult_backward(Value *self, Value *other, double upstream_grad);
void build_topo(Value* v, int visited[], Value* topo[], size_t* topoIndex);
void backprop(Value *output);

#define MAX_VALUES 100

int main(void) {
    Value a = init_value(2.0f);
    Value b = init_value(4.0f);
    Value *ab = value_mult(&a, &b);

    Value c = init_value(2.0f);
    Value d = init_value(4.0f);
    Value *cd = value_mult(&c, &d);

    Value *res = value_mult(ab, cd);

    backprop(res);

    printf("HELLO\n");
}

void backprop(Value *output) {
    printf("WE ARE AT THE TOP LEVEL\n");
    value_print(output, false);
    while (output->prev != NULL) {
        _value_mult_backward(output->prev[0], output->prev[1], 1);
        for (size_t i = 0; i < output->prev_count; i++) {
            backprop(output->prev[i]);
            printf("HELLO\n");
        }

    }
}

void value_print(Value *value, bool print_children) {
    printf("Value(%.2f, %.2f)\n", value->data, value->grad);
    if (print_children) {
        for (size_t i = 0; i < 2; i++) { // TODO: Don't hardcode this as 2, because there might be other operators.
            printf("\t Child %zu: Value(%.2f, %.2f)\n", i ,value->prev[i]->data, value->prev[i]->grad);
        }
    }
}

Value *value_add(Value *self, Value *other) {
    assert((self != NULL && other != NULL) && "Cannot pass null values as operands.");
    Value *out = malloc(sizeof(Value));
    out->data = self->data + other->data;
    out->prev = malloc(2 * sizeof(Value *));
    out->prev_count = 2;
    out->prev[0] = self;
    out->prev[1] = other;
    // TODO: This should be freed at some point
    return out;
}

void _value_add_backward(Value *self, Value *other, double upstream_grad){
    self->grad += upstream_grad;
    other->grad += upstream_grad;
}

Value *value_mult(Value *self, Value *other) {
    assert((self != NULL && other != NULL) && "Cannot pass null values as operands.");
    Value *out = malloc(sizeof(Value));
    out->data = self->data * other->data;
    out->prev = malloc(2 * sizeof(Value *));
    out->prev_count = 2;
    out->prev[0] = self;
    out->prev[1] = other;
    // TODO: This should be freed at some point
    return out;
}

void _value_mult_backward(Value *self, Value *other, double upstream_grad){
    self->grad += other->data * upstream_grad;
    other->grad += self->data * upstream_grad;
}
