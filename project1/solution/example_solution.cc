// this is a silly solution
// just to show you how different
// components of this framework work
// please bring your wise to write
// a 'real' solution :)

#include <iostream>
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

// get the content between the double quotation marks example: "content"
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

// get the content in Json file (list)
std::vector<std::string> getJSONlist(std::string& s, int idx){
    std::vector<std::string> ans;
    std::string temp;
    for(int i=idx;i<s.length();++i){
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
int parseJSON(std::string jsonfile, std::string& name, std::vector<std::string>& ins, std::vector<std::string>& outs, std::string& data_type, std::vector<std::string>& stmts){
    std::ifstream infile(jsonfile);
    std::string line;
    std::string kernel;
    if(infile){
        getline(infile, line);
        // name
        getline(infile, line);
        name = getJSONcontent(line, 11);
        
        getline(infile, line);
        ins = getJSONlist(line, 10);


        getline(infile, line);
        outs = getJSONlist(line, 11);


        getline(infile, line);
        data_type = getJSONcontent(line, 16);


        getline(infile, line);
        kernel = getJSONcontent(line, 13);
        kernel = replace_all(kernel, "//", " // ");
        kernel = replace_all(kernel, "%", " % ");
        stmts = split(kernel, ";");


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

    if(shape.size() == 1 && shape[0] == 1){
        return Var::make(data_type, name, {}, {1});
    }
    else{
        std::vector<Expr> args;
        for(int i=0;i<argnames.size();++i){
            args.push_back(parseArg(argnames[i], shape[i]));
            Expr tmp = Compare::make(data_type, CompareOpType::LT, parseArg(argnames[i], shape[i]), Expr((int)shape[i]));
            if (ini == false){
                ini = true; 
                con.push(tmp);
            }
            else
                con.push(Binary::make(data_type, BinaryOpType::And, con.top(), tmp));
        }
        return Var::make(data_type, name, args, shape);
    }
}

Stmt parseStmt(std::string stmt){
    indexset.clear();
    int idx = 0;
    std::string temp;
    std::stack<Expr> valstack;
    std::stack<std::string> opstack;
    Expr leftval = parseVar(getitem(stmt, idx));
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
    Stmt movestmt = Move::make(leftval, valstack.top(), MoveType::MemToMem);
    Stmt movestmt_invalid = Move::make(leftval, leftval, MoveType::MemToMem);
    Stmt range = IfThenElse::make(con.top(), movestmt, movestmt_invalid);
    std::vector<Expr> iters;
    for(auto i: indexset)
        iters.push_back(getIndex(i.first));
    Stmt loopstmt = LoopNest::make(iters, {range});
    return loopstmt;
}

Group buildIRtree(std::string filename){
    varset.clear();
    std::string name, datatype;
    std::vector<std::string> ins, outs, exps;
    std::vector<Stmt> stmts;
    if(parseJSON(filename, name, ins, outs, datatype, exps) < 0)
        return Kernel::make("error", {}, {}, {}, KernelType::CPU);
    
    if(datatype == "float")
        data_type = Type::float_scalar(32);
    else
        data_type = Type::int_scalar(32);
    
    for(auto stmt: exps){
        while (!con.empty()){
            con.pop();
        }
        ini = false;
        stmts.push_back(parseStmt(stmt));
    }

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
    return Kernel::make(name, inVar, outVar, stmts, KernelType::CPU);
}

int main() {
    
    for(int i=1;i<=10;++i){
        auto kernel =  buildIRtree("./cases/case" + std::to_string(i) + ".json");
        if(kernel.as<Kernel>()->name == "error")
            continue;
        std::ofstream ofile("./kernels/kernel_case" + std::to_string(i) + ".cc", std::ios::out);
        ofile << "#include \"../run.h\"\n" << std::endl;
        IRPrinter printer;
        ofile << printer.print(kernel) << std::endl;
        ofile.close();
    }

    return 0;
}
