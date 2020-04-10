#pragma once

#include <string>

class MNGLexer {
public:
    enum token {
        TOK_name,
        TOK_number,
        TOK_comment,
        TOK_eoi,
        TOK_lparen,
        TOK_rparen,
        TOK_lcurly,
        TOK_rcurly,
        TOK_comma,
        TOK_equals
    };
        
    static std::string tokenToString(token tok);
        
    MNGLexer(const char *p);
    token lex();
    
    std::string stringval;
    float numberval;
    int yylineno;

private:
    const char *p;
};