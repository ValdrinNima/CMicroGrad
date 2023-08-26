#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>

#define init_value(d) {.data=d, .prev_count=0, .grad=0.0f, .prev=NULL, .op=0} // TODO: Move this to a function instead of a macro
                                                        // TODO: Add a condition which checks what type
enum Operation {
    ADD,
    MULT
};

typedef struct Value {
    double data;
    double grad;
    struct Value **prev;
    size_t prev_count;
    enum Operation op;
} Value;



void value_print(Value *value, bool print_children);
Value *value_add(Value *self, Value *other);
Value *value_mult(Value *self, Value *other);
void _value_add_backward(Value *self, Value *other, double upstream_grad);
void _value_mult_backward(Value *self, Value *other, double upstream_grad);
void build_topo(Value* v, int visited[], Value* topo[], size_t* topoIndex);
void backprop(Value *output);
Value *value_create();

#define MAX_VALUES 100

int main(void) {
    Value _a = init_value(10.0f);
    Value _b = init_value(5.0f);
    Value *a = value_mult(&_a, &_b);

    Value f = init_value(7.0f);
    Value *af = value_mult(&f, a);

    Value b = init_value(3.0f);
    Value *ab = value_mult(a, &b);

    Value *afb = value_mult(af, ab);

    Value c = init_value(2.0f);
    Value d = init_value(4.0f);
    Value *cd = value_mult(&c, &d);

    Value *res = value_mult(afb, cd);

    backprop(res);

    printf("____\n");
    printf("_a ");
    value_print(&_a,false);
    printf("_b ");
    value_print(&_b,false);
    printf("*a ");
    value_print(a,false);
    printf("f ");
    value_print(&f,false);
    printf("af ");
    value_print(af,false);
    printf("b ");
    value_print(&b,false);
    printf("ab ");
    value_print(ab,false);
    printf("afb ");
    value_print(afb,false);
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
        switch (current->op) {
            case ADD:
                _value_add_backward(current->prev[0], current->prev[1], current->grad);
                break;
            case MULT:
                _value_mult_backward(current->prev[0], current->prev[1], current->grad);
                break;
        }
        // Enqueue the previous nodes for further traversal
        for (size_t i = 0; i < current->prev_count; i++) {
            if (current->prev[i] != NULL) {
                queue[rear++] = current->prev[i];

            }
        }
    }
}

Value* create_value(double data , ...) {
    Value* new_value = (Value*)malloc(sizeof(Value));
    if (new_value == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the fields of the new Value struct
    new_value->data = data;
    new_value->grad = 0.0; // Default gradient value
    new_value->prev_count = 0;

     // Initialize the previous nodes array with provided arguments
//    va_list args;
//    va_start(args, data);
//    for (size_t i = 0; i < prev_count; i++) {
//        new_value->prev[i] = va_arg(args, Value*);
//    }
//    va_end(args);

    return new_value;
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
    out->op = ADD;
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
    out->op = MULT;
    out->prev[0] = self;
    out->prev[1] = other;
    // TODO: This should be freed at some point
    return out;
}

void _value_mult_backward(Value *self, Value *other, double upstream_grad){
    self->grad += other->data * upstream_grad;
    other->grad += self->data * upstream_grad;
}
