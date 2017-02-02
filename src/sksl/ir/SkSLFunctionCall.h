/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
 
#ifndef SKSL_FUNCTIONCALL
#define SKSL_FUNCTIONCALL

#include "SkSLExpression.h"
#include "SkSLFunctionDeclaration.h"

namespace SkSL {

/**
 * A function invocation.
 */
struct FunctionCall : public Expression {
    FunctionCall(Position position, const Type& type, const FunctionDeclaration& function,
                 std::vector<std::unique_ptr<Expression>> arguments)
    : INHERITED(position, kFunctionCall_Kind, type)
    , fFunction(std::move(function))
    , fArguments(std::move(arguments)) {}

    bool hasSideEffects() const override {
        if (!fFunction.fBuiltin) {
            // conservatively assume that user-defined functions have side effects
            return true;
        }
        for (const auto& arg : fArguments) {
            if (arg->hasSideEffects()) {
                return true;
            }
        }
        // Note that we are assuming no builtin functions have side effects. This is true for the
        // moment, but could change as our support for GLSL functions expands.
        return false;
    }

    SkString description() const override {
        SkString result = fFunction.fName + "(";
        SkString separator;
        for (size_t i = 0; i < fArguments.size(); i++) {
            result += separator;
            result += fArguments[i]->description();
            separator = ", ";
        }
        result += ")";
        return result;
    }

    const FunctionDeclaration& fFunction;
    std::vector<std::unique_ptr<Expression>> fArguments;

    typedef Expression INHERITED;
};

} // namespace

#endif
