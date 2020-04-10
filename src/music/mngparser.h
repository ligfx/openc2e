#pragma once

#include <string>
#include <mpark/variant.hpp>
#include <vector>

struct newMNGFunction;
struct newMNGAssignment;
using newMNGExpression = mpark::variant<newMNGFunction, float, std::string>;
using newMNGNode = mpark::variant<newMNGFunction, newMNGAssignment>;

struct newMNGFunction {
    std::string name;
    bool has_args = false;
    std::vector<newMNGExpression> args;
    bool has_block = false;
    std::vector<newMNGNode> block;
};

struct newMNGAssignment {
    std::string name;
    newMNGExpression expression;
};

std::vector<newMNGNode> mngparse(const char *p);