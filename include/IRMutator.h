/*
 * MIT License
 * 
 * Copyright (c) 2020 Size Zheng

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef BOOST_IRMUTATOR_H
#define BOOST_IRMUTATOR_H

#include "IR.h"


namespace Boost {

namespace Internal {

class IRMutator {
 public:
	std::string grad_val, grad_leftval;
	// std::vector<size_t> leftval_shape;
	// std::vector<Expr> leftval_args;
	// Ref<const Var> tmp;
 
	IRMutator() {}
	IRMutator(std::string val, std::string g_leftval) {
		grad_val = val;
		grad_leftval = g_leftval;
		
	}
	/*
	void init(std::vector<size_t> shape, std::vector<Expr> args) {
	    leftval_shape = shape;
		leftval_args = args;
	}*/
    std::pair<bool, Expr> mutate(const Expr&);
    std::pair<bool, Stmt> mutate(const Stmt&);
    std::pair<bool, Group> mutate(const Group&);

    virtual std::pair<bool, Expr> visit(Ref<const IntImm>);
    virtual std::pair<bool, Expr> visit(Ref<const UIntImm>);
    virtual std::pair<bool, Expr> visit(Ref<const FloatImm>);
    virtual std::pair<bool, Expr> visit(Ref<const StringImm>);
    virtual std::pair<bool, Expr> visit(Ref<const Unary>);
    virtual std::pair<bool, Expr> visit(Ref<const Binary>);
    virtual std::pair<bool, Expr> visit(Ref<const Select>);
    virtual std::pair<bool, Expr> visit(Ref<const Compare>);
    virtual std::pair<bool, Expr> visit(Ref<const Call>);
    virtual std::pair<bool, Expr> visit(Ref<const Var>);
    virtual std::pair<bool, Expr> visit(Ref<const Cast>);
    virtual std::pair<bool, Expr> visit(Ref<const Ramp>);
    virtual std::pair<bool, Expr> visit(Ref<const Index>);
    virtual std::pair<bool, Expr> visit(Ref<const Dom>);
    virtual std::pair<bool, Stmt> visit(Ref<const LoopNest>);
    virtual std::pair<bool, Stmt> visit(Ref<const IfThenElse>);
    virtual std::pair<bool, Stmt> visit(Ref<const Move>);
    virtual std::pair<bool, Group> visit(Ref<const Kernel>);
 private:
};

}  // namespace Internal

}  // namespace Boost


#endif  // BOOST_IRMUTATOR_H
