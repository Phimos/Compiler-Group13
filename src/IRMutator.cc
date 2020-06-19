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

#include "IRMutator.h"

namespace Boost {

namespace Internal {

#define pbe std::pair<bool, Expr>
#define mp std::make_pair

std::pair<bool, Expr> IRMutator::mutate(const Expr &expr) {
	return expr.mutate_expr(this);
}


std::pair<bool, Stmt> IRMutator::mutate(const Stmt &stmt) {
    return stmt.mutate_stmt(this);
}


std::pair<bool, Group> IRMutator::mutate(const Group &group) {
    return group.mutate_group(this);
}


pbe IRMutator::visit(Ref<const IntImm> op) {
    return mp(1, Expr(0));
}


pbe IRMutator::visit(Ref<const UIntImm> op) {
    return mp(1, Expr(0));
}


pbe IRMutator::visit(Ref<const FloatImm> op) {
    return mp(1, Expr(0));
}


pbe IRMutator::visit(Ref<const StringImm> op) {
    return mp(1, Expr(0));
}


pbe IRMutator::visit(Ref<const Unary> op) {
    pbe new_a = mutate(op->a);
    if (new_a.first) return mp(1, Expr(0));
    return mp(0, Unary::make(op->type(), op->op_type, new_a.second));
}

pbe IRMutator::visit(Ref<const Binary> op) {
    // std::cout<< "Binary\n";
	if (op->op_type == BinaryOpType::Add) {
	    // std::cout << "+\n";
		pbe new_a = mutate(op->a);
		pbe new_b = mutate(op->b);
		// std::cout << "-------" << new_a.first << "\n";
		// std::cout << "-------" << new_b.first << "\n";
		if (new_a.first) return new_b;
		else if (new_b.first) return new_a;
		Expr t = Binary::make(op->type(), op->op_type, new_a.second, new_b.second);
		return mp(0, t);
	}
	else if (op->op_type == BinaryOpType::Sub) {
		pbe new_a = mutate(op->a);
		pbe new_b = mutate(op->b);
		if (new_b.first) return new_a;
		return mp(0, Binary::make(op->type(), op->op_type, new_a.second, new_b.second));
	} else if (op->op_type == BinaryOpType::Mul) {
    	// std::cout << "*\n";
    	Expr a = op->a, b = op->b;
		pbe tmp_a = mutate(op->a);
		// std::cout << "visit b\n";
		pbe tmp_b = mutate(op->b);
		// std::cout<<tmp_a.first << " a\n";
		// std::cout<<tmp_b.first << " b\n";
		if (tmp_a.first && tmp_b.first) return mp(1, Expr(0));
		if (tmp_b.first)
		    return mp(0, Binary::make(op->type(), op->op_type, tmp_a.second, b));
		if (tmp_a.first)
		    return mp(0, Binary::make(op->type(), op->op_type, a, tmp_b.second));
		
		Expr new_a = Binary::make(op->type(), op->op_type, a, tmp_b.second);
		Expr new_b = Binary::make(op->type(), op->op_type, tmp_a.second, b);
		return mp(0, Binary::make(op->type(), BinaryOpType::Add, new_a, new_b));
	} else {
	    pbe new_a = mutate(op->a);
		if (new_a.first) return mp(1, Expr(0));
		return mp(0, Binary::make(op->type(), op->op_type, new_a.second, op->b));
	}
    return mp(0, Binary::make(op->type(), op->op_type, op->a, op->b));
}


pbe IRMutator::visit(Ref<const Compare> op) {
    Expr new_a = op->a;
    Expr new_b = op->b;
    return mp(0, Compare::make(op->type(), op->op_type, new_a, new_b));
}


pbe IRMutator::visit(Ref<const Select> op) {
    // Expr new_cond = mutate(op->cond);
    pbe new_true_value = mutate(op->true_value);
    pbe new_false_value = mutate(op->false_value);
    return mp(0, Select::make(op->type(), op->cond, new_true_value.second,
                              new_false_value.second));
}


pbe IRMutator::visit(Ref<const Call> op) {
    // std::vector<Expr> new_args;
    /*for (auto arg : op->args) {
        new_args.push_back(mutate(arg));
    }*/
    return mp(0, Call::make(op->type(), op->args, op->func_name, op->call_type));

}


pbe IRMutator::visit(Ref<const Cast> op) {
    pbe new_val = mutate(op->val);
    // std::cout << "Cast\n";
    return mp(new_val.first, Cast::make(op->type(), op->new_type, new_val.second));
}


pbe IRMutator::visit(Ref<const Ramp> op) {
    pbe new_base = mutate(op->base);
    // std::cout << "Ramp\n";
    return mp(new_base.first, Ramp::make(op->type(), new_base.second, op->stride, op->lanes));
}


pbe IRMutator::visit(Ref<const Var> op) {
	/*
    std::vector<Expr> new_args;
    for (auto arg : op->args) {
        new_args.push_back(mutate(arg));
    }
    return Var::make(op->type(), op->name, new_args, op->shape);
	*/
	// std::cout << op->name << std::endl;
	if (op->name == grad_val) {
	    int pos = grad_leftval.find("<");
	    // std::cout<< "d" + grad_leftval.substr(0, pos) << "\n";
		// return mp(0, Var::make(, .substr(0, pos), op->args, op->shape));//, leftval_args, leftval_shape));
		return mp(0, Var::make(op->type(), "d" + grad_leftval, {}, {1}));
    }
    pbe t = mp(1, Expr(0));
    // std::cout << "OK\n" << t.first << "\n";
	return t;
}


pbe IRMutator::visit(Ref<const Dom> op) {
    // Expr new_begin = mutate(op->begin);
    // Expr new_extent = mutate(op->extent);
    // std::cout << "Dom\n";
    return mp(0, Dom::make(op->type(), op->begin, op->extent, op->name));
}


pbe IRMutator::visit(Ref<const Index> op) {
    // Expr new_dom = mutate(op->dom);
    return mp(0, Index::make(op->type(), op->name, op->dom, op->index_type));
}


std::pair<bool, Stmt> IRMutator::visit(Ref<const LoopNest> op) {
	/*
    std::vector<Expr> new_index_list;
    std::vector<Stmt> new_body_list;
    for (auto index : op->index_list) {
        new_index_list.push_back(mutate(index));
    }
    for (auto body : op->body_list) {
        new_body_list.push_back(mutate(body));
    }
    return LoopNest::make(new_index_list, new_body_list);
	*/
    std::vector<Stmt> new_body_list;
    for (auto body : op->body_list)
        new_body_list.push_back(mutate(body).second);
    return mp(0, LoopNest::make(op->index_list, new_body_list));
}


std::pair<bool, Stmt> IRMutator::visit(Ref<const IfThenElse> op) {
	/*
    Expr new_cond = mutate(op->cond);
    Stmt new_true_case = mutate(op->true_case);
    Stmt new_false_case = mutate(op->false_case);
    return IfThenElse::make(new_cond, new_true_case, new_false_case);
	*/
	std::pair<bool, Stmt> new_true_case = mutate(op->true_case);
    std::pair<bool, Stmt> new_false_case = mutate(op->false_case);
	return mp(0, IfThenElse::make(op->cond, new_true_case.second, new_false_case.second));
}


std::pair<bool, Stmt> IRMutator::visit(Ref<const Move> op) {
	pbe new_dst = mutate(op->dst);
	pbe new_src = mutate(op->src);
    return mp(0, Move::make(new_dst.second, new_src.second, op->move_type));
}


std::pair<bool, Group> IRMutator::visit(Ref<const Kernel> op) {
    
    std::vector<Stmt> new_stmt_list;
    for (auto stmt : op->stmt_list) {
        new_stmt_list.push_back(mutate(stmt).second);
    }
    return mp(0, Kernel::make(op->name, op->inputs, op->outputs, new_stmt_list, op->kernel_type));
}


}  // namespace Internal

}  // namespace Boost


