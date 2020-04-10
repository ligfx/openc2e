#include "music/mngfile.h"
#include "music/mnglexer.h"
#include "music/mngparser.h"

#include <iostream>

struct MNGParser {
    MNGParser(const char *p) : lexer(MNGLexer(p)) {}

    newMNGFunction parse_function_with_name(std::string name);

    newMNGFunction parse_function();
    newMNGExpression parse_expression();
    newMNGNode parse_node();
    std::vector<newMNGNode> parse_toplevel();
    void next_token();

    MNGLexer::token current_token;
    MNGLexer lexer;
};

newMNGFunction MNGParser::parse_function_with_name(std::string name) {
    if (!(current_token == MNGLexer::TOK_lparen ||
          current_token == MNGLexer::TOK_lcurly)) {
        std::cout << "Expected lparen or lcurly, got "
                  << MNGLexer::tokenToString(current_token) << std::endl;
        abort();
    }

    newMNGFunction function;
    function.name = name;

    if (current_token == MNGLexer::TOK_lparen) {
        function.has_args = true;

        next_token();
        function.args.push_back(parse_expression());
        while (true) {
            printf("argloop\n");
            if (current_token == MNGLexer::TOK_comma) {
                next_token();
                function.args.push_back(parse_expression());
            } else if (current_token == MNGLexer::TOK_rparen) {
                printf("rparen break\n");
                next_token();
                break;
            } else {
                std::cout << "Expected comma or rparen, got "
                          << MNGLexer::tokenToString(current_token)
                          << std::endl;
                abort();
            }
        }
    }

    if (current_token == MNGLexer::TOK_lcurly) {
        function.has_block = true;
        next_token();

        while (current_token != MNGLexer::TOK_rcurly) {
            function.block.push_back(parse_node());
        }
        next_token();
    }

    // if (function.name == "Stage") {
    //     return new MNGStageNode
    // }

    return function;
}

newMNGFunction MNGParser::parse_function() {
    if (current_token != MNGLexer::TOK_name) {
        std::cout << "Expected number, name, or function, got "
                  << MNGLexer::tokenToString(current_token) << std::endl;
        abort();
    }

    std::string name = lexer.stringval;
    next_token();
    return parse_function_with_name(name);
}

newMNGExpression MNGParser::parse_expression() {
    if (current_token == MNGLexer::TOK_number) {
        next_token();
        // newthing return new MNGConstantNode(lexer.numberval);
        return lexer.numberval;
    }
    if (current_token != MNGLexer::TOK_name) {
        std::cout << "Expected number, name, or function, got "
                  << MNGLexer::tokenToString(current_token) << std::endl;
        abort();
    }

    std::string name = lexer.stringval;

    next_token();
    if (current_token == MNGLexer::TOK_lparen) {
        return parse_function_with_name(name);
    } else {
        // newthing return new MNGVariableNode(name);
        return name;
    }
}

newMNGNode MNGParser::parse_node() {
    if (current_token != MNGLexer::TOK_name) {
        std::cout << "Expected function or assignment, got "
                  << MNGLexer::tokenToString(current_token) << std::endl;
        abort();
    }
    std::string name = lexer.stringval;
    next_token();

    if (current_token == MNGLexer::TOK_lparen ||
        current_token == MNGLexer::TOK_lcurly) {
        return parse_function_with_name(name);
    } else if (current_token == MNGLexer::TOK_equals) {
        next_token();
        newMNGAssignment assignment;
        assignment.name = name;
        assignment.expression = parse_expression();
        return assignment;
    } else {
        std::cout << "Expected lparen, lcurly, or equals, got "
                  << MNGLexer::tokenToString(current_token) << std::endl;
        abort();
    }
}

std::vector<newMNGNode> MNGParser::parse_toplevel() {
    printf("parse toplevel\n");
    next_token();

    std::vector<newMNGNode> nodes;
    while (true) {
        if (current_token == MNGLexer::TOK_eoi) {
            break;
        }
        nodes.push_back(parse_node());
    }
    return nodes;
}

void MNGParser::next_token() {
    do {
        current_token = lexer.lex();
        std::cout << "token " << MNGLexer::tokenToString(current_token);
        if (current_token == MNGLexer::TOK_name) {
            std::cout << " " << lexer.stringval;
        }
        if (current_token == MNGLexer::TOK_number) {
            std::cout << " " << lexer.numberval;
        }
        std::cout << std::endl;
    } while (current_token == MNGLexer::TOK_comment);
}

static int indent = 0;

struct expression_print_visitor {
    void operator()(const newMNGFunction &func) {
        std::cout << func.name << "()";
        // TODO
    }
    void operator()(const float &f) { std::cout << f; }
    void operator()(const std::string &s) { std::cout << s; }
};

struct node_print_visitor {
    void operator()(const newMNGFunction &func) {
        for (int i = 0; i < indent; ++i)
            printf("    ");
        std::cout << func.name;
        if (func.has_args) {
            std::cout << "(";
            for (size_t i = 0; i < func.args.size(); i++) {
                if (i > 0) {
                    std::cout << ", ";
                }
                mpark::visit(expression_print_visitor(), func.args[i]);
            }
            std::cout << ")";
        }
        if (func.has_block) {
            std::cout << "{\n";
            indent += 1;
            for (auto n : func.block) {
                mpark::visit(node_print_visitor(), n);
            }
            indent -= 1;
            for (int i = 0; i < indent; ++i)
                std::cout << "    ";
            std::cout << "}";
        }
        std::cout << "\n";
    }
    void operator()(const newMNGAssignment &assign) {
        for (int i = 0; i < indent; ++i)
            printf("    ");
        std::cout << assign.name << " = ";
        mpark::visit(expression_print_visitor(), assign.expression);
        std::cout << "\n";
    }
};

std::vector<newMNGNode> mngparse(const char *p) {
    auto nodes = MNGParser(p).parse_toplevel();

    for (auto n : nodes) {
        mpark::visit(node_print_visitor(), n);
    }

    return nodes;
}