#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#define init_value(d) {.data=d, .prev_count=0, .grad=0.0f, .prev=NULL} // TODO: Move this to a function instead of a macro
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

    Value _a = init_value(10.0f);
    Value _b = init_value(5.0f);
    Value *a = value_mult(&_a, &_b);

    Value b = init_value(3.0f);
    Value *ab = value_mult(a, &b);

    Value c = init_value(2.0f);
    Value d = init_value(4.0f);
    Value *cd = value_mult(&c, &d);

    Value *res = value_mult(ab, cd);

    backprop(res);

    printf("____\n");
    printf("_a ");
    value_print(&_a,false);
    printf("_b ");
    value_print(&_b,false);
    printf("*a ");
    value_print(a,false);
    printf("b ");
    value_print(&b,false);
    printf("ab ");
    value_print(ab,false);
    printf("c ");
    value_print(&c,false);
    printf("d ");
    value_print(&d,false);
    printf("cd ");
    value_print(cd,false);
}

void backprop(Value *output) {
    Value* queue[100]; // Adjust the size as needed
    size_t front = 0;
    size_t rear = 0;

    // Enqueue the output node
    output->grad = 1;
    queue[rear++] = output;

    while (front < rear) {
        Value* current = queue[front++];
        if (current == NULL || current->prev == NULL) {
            continue;
        }
        _value_mult_backward(current->prev[0], current->prev[1], current->grad);
        // Enqueue the previous nodes for further traversal
        for (size_t i = 0; i < current->prev_count; i++) {
            if (current->prev[i] != NULL) {
                queue[rear++] = current->prev[i];

            }
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
