// this is a silly solution
// just to show you how different
// components of this framework work
// please bring your wise to write
// a 'real' solution :)

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <queue>

#include "IR.h"
#include "IRMutator.h"
#include "IRPrinter.h"


using namespace Boost::Internal;

Type index_type = Type::int_scalar(32);
Type data_type = Type::float_scalar(32);

std::stack<Expr> con;
bool ini = false;
std::string a = "a_tmp";
std::vector<std::string> arg_tmp;
std::vector<std::string> arg_new;
std::vector<std::pair<std::string, size_t>> constraint;

std::string getJSONcontent(std::string& s, int idx){
    int pos_begin = -1, pos_end;
    for(int i=idx;i<s.length();++i){
        if(s[i] == '"'){
            if(pos_begin == -1)
                pos_begin = i+1;
            else{
                pos_end = i;
                break;
            }
        }
    }
    if(pos_begin!=-1)
        return s.substr(pos_begin, pos_end - pos_begin);
    else
        return "";
}

// get the content between the double quotation marks example: "content"
std::string getJSONcontent(std::string& s, const std::string& ss){
    int i = s.find(ss) + ss.length() + 2;
    int pos_begin = -1, pos_end;
    for(;i<s.length();++i){
        if(s[i] == '"'){
            if(pos_begin == -1)
                pos_begin = i+1;
            else{
                pos_end = i;
                break;
            }
        }
    }
    if(pos_begin!=-1)
        return s.substr(pos_begin, pos_end - pos_begin);
    else
        return "";
}

// get the content in Json file (list)
std::vector<std::string> getJSONlist(std::string& s, const std::string& ss){
    std::vector<std::string> ans;
    std::string temp;
    
    int i = s.find(ss) + ss.length() + 2;
    for(;i<s.length();++i){
        if(s[i] == '[' || s[i] == ','){
            temp = getJSONcontent(s, i);
            if(temp!="")
                ans.push_back(temp);
            else
                break;
        }
    }
    return ans;
}

// split the string "str" with delim
std::vector<std::string> split(const std::string& str, const std::string& delim){
    std::vector<std::string> res;
    if("" == str)
        return res;
    char * strs = new char[str.length() + 1];
    strcpy(strs, str.c_str());
    char * d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());
    char * p = strtok(strs, d);
    while(p){
        std::string s = p;
        s.erase(0, s.find_first_not_of(' '));
        s.erase(s.find_last_not_of(' ') + 1);        
        res.push_back(s);
        p = strtok(NULL, d);
    }
    return res;
}

std::string replace_all(std::string& str, const std::string& old_value, const std::string& new_value){
    for (int pos = 0; pos != std::string::npos; pos += new_value.length()){
        if ((pos = str.find(old_value, pos)) != std::string::npos)
            str.replace(pos, old_value.length(), new_value);
        else
            break;
        }
    return str;
}

// parse the JSON file, return name, ins, outs, data_type and stmtlist
int parseJSON(std::string jsonfile, std::string& name, std::vector<std::string>& ins, std::vector<std::string>& outs, std::string& data_type, std::vector<std::string>& stmts, std::vector<std::string>& grads){
    std::ifstream infile(jsonfile);
    std::string line;
    std::string kernel;
    if(infile){
        getline(infile, line);
        // name
        getline(infile, line);
        name = getJSONcontent(line, "name");
        
        getline(infile, line);
        ins = getJSONlist(line, "ins");


        getline(infile, line);
        outs = getJSONlist(line, "outs");


        getline(infile, line);
        data_type = getJSONcontent(line, "data_type");


        getline(infile, line);
        kernel = getJSONcontent(line, "kernel");
        kernel = replace_all(kernel, "//", " // ");
        kernel = replace_all(kernel, "%", " % ");
        stmts = split(kernel, ";");


        getline(infile, line);
		grads = getJSONlist(line, "grad_to");
		
		getline(infile, line);
        return 0;
    }
    else{
        std::cout<<"no such file"<<std::endl;
        return -1;
    }
}

// to get item from the stmt
std::string getitem(std::string& str, int& idx){
    for(;idx<str.length();++idx)
        if(str[idx] != ' ')
            break;
    if(idx == str.length())
        return "";
    if(str[idx] == '(' || str[idx] == ')'){
        return str.substr(idx++, 1);
    }
    int pos_begin = idx;
    bool in_expr = false;
    for(;idx<str.length();++idx){
        if(str[idx] == '<' || str[idx] == '[')
            in_expr = true;
        if(str[idx] == '>' || str[idx] == ']')
            in_expr = false;
        if(!in_expr && (str[idx] == ' ' || str[idx] == ')'))
            break;
    }
    return str.substr(pos_begin, idx - pos_begin);
}

int is_op(std::string& str){
    if(str == "+" || str == "-"){
        return 1;
    }
    else if (str == "*" || str == "/" || str == "%" || str == "//"){
        return 2;
    }
    else if(str == "("){
        return -1;
    }
    else if(str == ")"){
        return -2;
    }
    return 0;
}

std::map<std::string, int> indexset;
std::map<std::string, std::vector<size_t>> varset;

std::map<std::string, BinaryOpType> op2type{
    {"+", BinaryOpType::Add},
    {"-", BinaryOpType::Sub},
    {"*", BinaryOpType::Mul},
    {"/", BinaryOpType::Div},
    {"//", BinaryOpType::IDiv},
    {"%", BinaryOpType::Mod}
};


Expr getIndex(std::string name, int bound= 0x3f3f3f3f){
    if(indexset.count(name))
        indexset[name] = std::min(indexset[name], bound);
    else
        indexset[name] = bound;
    Expr dom = Dom::make(index_type, 0, indexset[name], name);
    
    return Index::make(index_type, name, dom, IndexType::Spatial);
}

Expr parseItem(std::string str, int bound){
    if('0' <= str[0] && str[0] <= '9'){
        if(str.find('.') != std::string::npos)
            return Expr(atof(str.c_str()));
        else
            return Expr(atoi(str.c_str()));
    }
    else{
        return getIndex(str, bound);
    }
}

Expr parseArg(std::string str, int shape){
    std::vector<std::string> args = split(str, " ");
    std::stack<Expr> argstack;
    std::stack<std::string> opstack;
    int idx = 0;
    std::string temp;
    while((temp = getitem(str,idx)).length()){
        if(temp == "("){
            opstack.push(temp);
        }
        else if(temp == ")"){
            while(opstack.top()!="("){
                auto rightval = argstack.top();
                argstack.pop();
                auto leftval = argstack.top();
                argstack.pop();
                argstack.push(Binary::make(index_type, op2type[opstack.top()], leftval, rightval));
                opstack.pop();                
            }
            opstack.pop();
        }
        else if(is_op(temp)){
            while((!opstack.empty()) && is_op(temp) <= is_op(opstack.top())){
                auto rightval = argstack.top();
                argstack.pop();
                auto leftval = argstack.top();
                argstack.pop();
                argstack.push(Binary::make(index_type, op2type[opstack.top()], leftval, rightval));
                opstack.pop();
            }
            opstack.push(temp);
        }
        else{
	    if(temp == str){
	        argstack.push(parseItem(temp, shape));
	    }
	    else{
		argstack.push(parseItem(temp, 0x3f3f3f3f));
	    }
        }
    }
    while(!opstack.empty()){
        auto rightval = argstack.top();
        argstack.pop();
        auto leftval = argstack.top();
        argstack.pop();
        argstack.push(Binary::make(index_type, op2type[opstack.top()], leftval, rightval));
        opstack.pop();
    }
    return argstack.top();
}

bool transform(std::string str){
    std::string temp;
    int idx = 0;
    temp = getitem(str, idx);
    if (temp == str)
        return false;
    else{
        a[0] = a[0] + 1;
        return true;
    }
}

std::map<std::string, char> name_id;
std::string grad_name;
std::map<std::string, std::string> final_var;

Expr parseVar(std::string var){
    if('0' <= var[0] && var[0] <= '9'){
        if(var.find('.') != std::string::npos)
            return Expr(atof(var.c_str()));
        else
            return Expr(atoi(var.c_str()));
    }
    int begin1, end1, begin2, end2;
    std::string name;
    std::vector<size_t> shape;
    std::vector<std::string> argnames;
    begin1 = var.find('<');
    end1 = var.find('>');
    begin2 = var.find('[');
    end2 = var.find(']');
    name = var.substr(0, begin1);
    for(auto val: split(var.substr(begin1 + 1, end1 - begin1 - 1), ","))
        shape.push_back(atoi(val.c_str()));
    if(begin2 != std::string::npos)
        argnames = split(var.substr(begin2 + 1, end2 - begin2 - 1), ",");
    
    varset[name] = shape;
    if (name == grad_name) {
        if (name_id[name] == 0) name_id[name] = '0';
        else name_id[name] = name_id[name] + 1;
        name = name + "-" + name_id[name];
        final_var[name] = var;
    }

    if(shape.size() == 1 && shape[0] == 1){
        return Var::make(data_type, name, {}, {1});
    } else {
        std::vector<Expr> args;
        for(int i=0;i<argnames.size();++i){
            if (transform(argnames[i])){
                bool s = true;
                for (int j = 0; j < arg_tmp.size(); j++)
                    if (arg_tmp[j] == argnames[i]) 
                        s = false;
                if (s == true){
                    Expr tmp1 = Compare::make(data_type, CompareOpType::EQ, parseArg(argnames[i], shape[i]), parseArg(a, shape[i]));
                    if (ini == false){
                        ini = true; 
                        con.push(tmp1);
                    }
                    else
                        con.push(Binary::make(data_type, BinaryOpType::And, con.top(), tmp1));
                    arg_tmp.push_back(argnames[i]);
                    arg_new.push_back(a);
                    argnames[i] = a;
                }
                else{
                    for (int j = 0; j < arg_tmp.size(); j++)
                        if (arg_tmp[j] == argnames[i]) 
                            argnames[i] = arg_new[j];
                }
            }
            args.push_back(parseArg(argnames[i], shape[i]));
            bool s = true;
            for (int j = 0; j < constraint.size(); j++)
                if (constraint[j].first == argnames[i] && constraint[j].second == shape[i])
                    s = false;
            if (s == true){
                constraint.push_back(std::make_pair(argnames[i], shape[i]));
                Expr tmp = Compare::make(data_type, CompareOpType::LT, parseArg(argnames[i], shape[i]), Expr((int)shape[i]));
                if (ini == false){
                    ini = true; 
                    con.push(tmp);
                }
                else
                    con.push(Binary::make(data_type, BinaryOpType::And, con.top(), tmp));
            }
        }
        return Var::make(data_type, name, args, shape);
    }
}

Stmt get_init(std::string grad_name, std::string grad_item) {
    static std::string tmp[] = {"ii", "jj", "kk", "ll", "qq", "pp"};
    parseVar("d" + grad_item);
    int grad_d = varset[grad_name].size();
    
    std::vector<Expr> args;
    std::vector<Expr> iters;
    for (int i = 0; i < grad_d; ++i) {
        args.push_back(parseVar(tmp[i]));
        
        int bound = varset[grad_name][i];
        Expr dom = Dom::make(index_type, 0, bound, tmp[i]);
        Expr iter = Index::make(index_type, tmp[i], dom, IndexType::Spatial);
        iters.push_back(iter);
        
    }
    Expr grad = Var::make(data_type, "d" + grad_name, args, varset[grad_name]);
    Stmt range = Move::make((Expr)(grad), Expr(0), MoveType::MemToMem);
    return LoopNest::make(iters, {range});
}

void parseStmt(std::vector<Stmt>& stmts, std::string stmt, std::string grad_item){
    constraint.clear();
    std::map<std::string, char> empty_map1;
    std::map<std::string, std::string> empty_map2;
    name_id.swap(empty_map1);
    final_var.swap(empty_map2);
    
    indexset.clear();
    int idx = 0;
    std::string temp;
    std::stack<Expr> valstack;
    std::stack<std::string> opstack;
	
	// Expr grad = parseVar("d" + grad_name);
	
	std::string leftgrad = getitem(stmt, idx);
	// ttmp = leftgrad;
    Expr leftval = parseVar(leftgrad);
    
    IRPrinter printer;
    leftgrad = printer.print(leftval);
    
    // std::cout << leftgrad << std::endl;
    getitem(stmt,idx);
    while((temp = getitem(stmt,idx)).length()){
        if(temp == "("){
            opstack.push(temp);
        }
        else if(temp == ")"){
            while(opstack.top()!="("){
                auto rightval = valstack.top();
                valstack.pop();
                auto leftval = valstack.top();
                valstack.pop();
                valstack.push(Binary::make(data_type, op2type[opstack.top()], leftval, rightval));
                opstack.pop();                
            }
            opstack.pop();
        }
        else if(is_op(temp)){
            while((!opstack.empty()) && is_op(temp) <= is_op(opstack.top())){
                auto rightval = valstack.top();
                valstack.pop();
                auto leftval = valstack.top();
                valstack.pop();
                valstack.push(Binary::make(data_type, op2type[opstack.top()], leftval, rightval));
                opstack.pop();
            }
            opstack.push(temp);
        }
        else{
            valstack.push(parseVar(temp));
        }
    }
    while(!opstack.empty()){
        auto rightval = valstack.top();
        valstack.pop();
        auto leftval = valstack.top();
        valstack.pop();
        valstack.push(Binary::make(data_type, op2type[opstack.top()], leftval, rightval));
        opstack.pop();
    }
	
	stmts.push_back(get_init(grad_name, grad_item));
	
	
	// mutator.init(sshape, aargs);
	for (char i = '0'; i <= name_id[grad_name]; ++i) {
	    IRMutator mutator = IRMutator(grad_name + "-" + i, leftgrad);
	    auto tmp = mutator.mutate(valstack.top()).second;
	    // std::cout << "d" + final_var[grad_name + "-" + i] << "\n";
    	Expr grad = parseVar("d" + final_var[grad_name + "-" + i]);
	    auto tmp2 = Binary::make(data_type, BinaryOpType::Add, grad, tmp);
        Stmt movestmt = Move::make(grad, tmp2, MoveType::MemToMem);
        Stmt movestmt_invalid = Move::make(grad, grad, MoveType::MemToMem);
        Stmt range = IfThenElse::make(con.top(), movestmt, movestmt_invalid);
        
        std::vector<Expr> iters;
        for(auto i: indexset) iters.push_back(getIndex(i.first));
        Stmt loopstmt = LoopNest::make(iters, {range});
        stmts.push_back(loopstmt);
	}
}

std::string get_total_grad(std::string &s, std::string &ss) {
    int pos = s.find(ss);
    std::string s_tmp = s.substr(pos);
    pos = s_tmp.find(']');
    return s_tmp.substr(0, pos + 1);
}

Group buildIRtree(std::string filename){
    varset.clear();
    std::string name, datatype;
    std::vector<std::string> ins, outs, exps, grads;
    std::vector<Stmt> stmts;
    
    if(parseJSON(filename, name, ins, outs, datatype, exps, grads) < 0)
        return Kernel::make("error", {}, {}, {}, KernelType::CPU);
    
    int highorder = 0;
    std::string str = exps[0];
    std::string str1 = grads[0];
    for(int idx=0;idx<str.length();++idx){
        if(str[idx] == str1[0]){
            for(;idx<str.length();++idx){
                if(str[idx] == '*'){
                    for(;idx<str.length();++idx){
                        if(str[idx] == str1[0])
                            highorder = 1;
                    }
                }
            }
        }
    }
        
    if(datatype == "float") data_type = Type::float_scalar(32);
    else data_type = Type::int_scalar(32);
    
    //int cnt=0;
	for (auto grad : grads) {     
		for(auto stmt: exps){
            /*if(cnt<5)
                std::cout <<"cnt:"<<cnt<<":"<< stmt<<std::endl;
            cnt++;*/
			while (!con.empty()){
				con.pop();
			}
			ini = false;
	        // std::cout << get_total_grad(stmt, grad) << std::endl;
	        std::string grad_item = get_total_grad(stmt, grad);
	        // std::cout << grad_item <<std::endl;
	        // stmts.push_back(get_init(grad, grad_item));
	        grad_name = grad;
			parseStmt(stmts, stmt, grad_item);
		}
	}
	
    /*
    std::vector<Expr> inVar, outVar;
    bool notrepeat = true;
    for(auto var: ins){
        inVar.push_back(Var::make(data_type, var, {}, varset[var]));
        if(var == outs[0])
            notrepeat=false;
    }
    
    for(auto var: outs){
        if(notrepeat)
            outVar.push_back(Var::make(data_type, var, {}, varset[var]));
    }
    */
    std::vector<Expr> inVar, outVar;
    for(auto var1:ins){
        for(auto var2:grads){
            if(var1 != var2)
                inVar.push_back(Var::make(data_type, var1, {}, varset[var1]));
        }
    }

    if(highorder == 1)
        inVar.push_back(Var::make(data_type, ins[0], {}, varset[ins[0]]));

    for(auto var: outs){
        auto tmpvar = "d" + var;
        outVar.push_back(Var::make(data_type, tmpvar, {}, varset[var]));
    }

    for(auto var: grads){
        auto tmpvar = "d" + var;
        outVar.push_back(Var::make(data_type, tmpvar, {}, varset[var]));
    }

    return Kernel::make(name, inVar, outVar, stmts, KernelType::CPU);
}

int main() {
    
    for(int i=1;i<=10;++i){
        auto kernel =  buildIRtree("./cases/case" + std::to_string(i) + ".json");
        arg_tmp.clear();
        arg_new.clear();
        constraint.clear();
        if(kernel.as<Kernel>()->name == "error")
            continue;
        std::ofstream ofile("./kernels/grad_case" + std::to_string(i) + ".cc", std::ios::out);
        ofile << "#include \"../run2.h\"\n" << std::endl;
        IRPrinter printer;
        ofile << printer.print(kernel) << std::endl;
        ofile.close();
        // IRPrinter printer;
        // std::cout << "OK" << std::endl;
        // std::cout << printer.print(kernel) << std::endl;
    }

    return 0;
}
