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
} Value;


void value_print(Value *value, bool print_children);
Value *value_add(Value *self, Value *other);
Value *value_mult(Value *self, Value *other);
void _value_add_backward(Value *self, Value *other, double upstream_grad);
void _value_mult_backward(Value *self, Value *other, double upstream_grad);

int main(void) {
    Value a = init_value(2.0f);
    Value b = {.data=4.0f};
    Value *c = value_mult(&a, &b);
    value_print(&a, false);
    value_print(&b, false);
    value_print(c, true);
    _value_mult_backward(&a, &b, 1);
    value_print(&a, false);
    value_print(&b, false);
    return 0;
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
    out->prev[0] = self;
    out->prev[1] = other;
    // TODO: This should be freed at some point
    return out;
}

void _value_mult_backward(Value *self, Value *other, double upstream_grad){
    self->grad += other->data * upstream_grad;
    other->grad += self->data * upstream_grad;
}
