#include "autograd.h"

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
            case NOOP: break;
            case ADD:
                _value_add_backward(current->prev[0], current->prev[1], current->grad);
                break;
            case SUB:
                _value_sub_backward(current->prev[0], current->prev[1], current->grad);
                break;
            case MULT:
                _value_mult_backward(current->prev[0], current->prev[1], current->grad);
                break;
            case DIV:
                _value_div_backward(current->prev[0], current->prev[1], current->grad);
                break;
            case POW:
                _value_pow_backward(current->prev[0], current->prev[1], current->grad);
                break;
            case RELU:
                _value_relu_backward(current->prev[0], current->grad);
                break;
        }
        // Enqueue the previous nodes for further traversal
        for (size_t i = 0; i < current->prev_count; i++) {
            if (current->prev[i] != NULL && !current->prev[i]->visited) {
                current->prev[i]->visited = true;
                queue[rear++] = current->prev[i];
            }
        }
    }
}

Value* value_create(double data) {
    Value* new_value = (Value*)malloc(sizeof(Value));
    if (new_value == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    // Initialize the fields of the new Value struct
    new_value->data = data;
    new_value->grad = 0.0;
    new_value->prev = NULL;
    new_value->prev_count = 0;
    new_value->op = NOOP;
    new_value->visited = false;

    return new_value;
}

void value_destroy(Value *value) {
    if (value == NULL) return;
    if (value->prev != NULL) {
        free(value->prev);
    }
    free(value);
}

// TODO: Write a function which frees the entire graph given the output node

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
    return out;
}

void _value_add_backward(Value *self, Value *other, double upstream_grad){
    self->grad += upstream_grad;
    other->grad += upstream_grad;
}

Value *value_sub(Value *self, Value *other) {
    assert((self != NULL && other != NULL) && "Cannot pass null values as operands.");
    Value *out = malloc(sizeof(Value));
    out->data = self->data - other->data;
    out->prev = malloc(2 * sizeof(Value *));
    out->prev_count = 2;
    out->op = ADD;
    out->prev[0] = self;
    out->prev[1] = other;
    return out;
}

void _value_sub_backward(Value *self, Value *other, double upstream_grad){
    self->grad += upstream_grad;
    other->grad += -1.0 * upstream_grad;
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
    return out;
}

void _value_mult_backward(Value *self, Value *other, double upstream_grad){
    self->grad += other->data * upstream_grad;
    other->grad += self->data * upstream_grad;
}

Value *value_div(Value *self, Value *other) {
    assert((self != NULL && other != NULL) && "Cannot pass null values as operands.");
    Value *out = malloc(sizeof(Value));
    // TODO: Rewrite this using other operators
    out->data = self->data / other->data;
    out->prev = malloc(2 * sizeof(Value *));
    out->prev_count = 2;
    out->op = DIV;
    out->prev[0] = self;
    out->prev[1] = other;
    return out;
}

void _value_div_backward(Value *self, Value *other, double upstream_grad){
    self->grad += 1 / other->data * upstream_grad;
    other->grad += self->data * upstream_grad;
}

Value *value_pow(Value *self, Value *other) {
    assert((self != NULL && other != NULL) && "Cannot pass null values as operands.");
    Value *out = malloc(sizeof(Value));
    out->data = pow(self->data, other->data);
    out->prev = malloc(2 * sizeof(Value *));
    out->prev_count = 2;
    out->op = POW;
    out->prev[0] = self;
    out->prev[1] = other;
    return out;
}

void _value_pow_backward(Value *self, Value *other, double upstream_grad){
    self->grad += other->data * pow(self->data, other->data - 1) * upstream_grad;
    other->grad += log(self->data) * pow(self->data, other->data) * upstream_grad;
}

Value *value_relu(Value *self) {
    assert((self != NULL) && "Cannot pass null values as operands.");
    Value *out = malloc(sizeof(Value));
    out->data = (self->data >= 0) ? self->data : 0;
    out->prev = malloc(1 * sizeof(Value *));
    out->prev_count = 1;
    out->op = RELU;
    out->prev[0] = self;
    return out;
}

void _value_relu_backward(Value *self, double upstream_grad){
    self->grad += (self->data > 0) * upstream_grad;
}

Value *value_sigmoid(Value *self) {
    assert((self != NULL) && "Cannot pass null values as operands.");
    Value *out = malloc(sizeof(Value));
    out->data = (self->data >= 0) ? self->data : 0;
    out->prev = malloc(1 * sizeof(Value *));
    out->prev_count = 1;
    out->op = RELU;
    out->prev[0] = self;
    return out;
}

void _value_sigmoid_backward(Value *self, double upstream_grad){
    self->grad += (self->data > 0) * upstream_grad;
}