#include "test_helpers.h"
#include "../src/ast.h"
#include "../src/token.h"
#include "../src/parser.h"
#include "../src/custom_string.h"

void test_ast_string() {
    // Initialize program
    int initial_cap = 2;
    program_t *program = malloc(sizeof(program_t) + sizeof(statement_t) * initial_cap);
    program->statement_cap = initial_cap;
    program->count = 0;

    // Allocate statement
    statement_t *statement = malloc(sizeof(statement_t));
    statement->type = LET_STATEMENT;
    statement->token.type = LET;
    statement->token.literal = strdup("let");
    
    // Set up identifier name
    statement->name.token.type = IDENT;
    statement->name.token.literal = strdup("myVar");
    statement->name.value = strdup("myVar");

    // Set up identifier value
    expression_t *value_expr = malloc(sizeof(expression_t));
    value_expr->type = IDENT_EXPR;
    value_expr->token.type = IDENT;
    value_expr->token.literal = strdup("anotherVar");
    value_expr->ident.token.type = IDENT;
    value_expr->ident.token.literal = strdup("anotherVar");
    value_expr->ident.value = strdup("anotherVar");

    statement->value = value_expr;
    // Add to program
    push_to_program(statement, program);

    // Test string conversion
    string_t *program_str = string_new();
    ast_string(program_str, program);
    string_t *expected_str = string_from("let myVar = anotherVar");
    
    assertf(string_equals(expected_str, program_str), "wrong ast string. expected %s, got %s\n", 
            expected_str->data, program_str->data);

    // Cleanup
    string_free(expected_str);
    string_free(program_str);

    // Free statement components
    free(value_expr->token.literal);
    free(value_expr->ident.token.literal);
    free(value_expr->ident.value);
    free(value_expr);

    // Free statement
    free(statement->token.literal);
    free(statement->name.token.literal); 
    free(statement->name.value);
    free(statement);

    // Free program 
    free(program);
}

int main(int argc, char *argv[]) {
	TEST(test_ast_string);
}
