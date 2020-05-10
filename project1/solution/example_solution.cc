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

#include "IR.h"
#include "IRMutator.h"
#include "IRPrinter.h"


using namespace Boost::Internal;

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

void parseVar(std::string var, std::string& name , std::vector<size_t>& shape, std::vector<std::string>& args){
    int begin1, end1, begin2, end2;
    std::vector<std::string> temp;
    begin1 = var.find('<');
    end1 = var.find('>');
    begin2 = var.find('[');
    end2 = var.find(']');
    if(begin1 == std::string::npos){
        name = var;
        return;
    }
    name = var.substr(0, begin1);
    temp = split(var.substr(begin1 + 1, end1 - begin1 - 1), ",");
    for(auto val: temp)
        shape.push_back(atoi(val.c_str()));
    if(begin2 != std::string::npos)
        args = split(var.substr(begin2 + 1, end2 - begin2 - 1), ",");
    
    std::cout<<"Var string: "<<var<<std::endl;
    std::cout<<"Var name: "<<name<<std::endl;
    std::cout<<"Var shape: "<<std::endl;
    for(auto v: shape)
        std::cout<<"\t"<<v<<std::endl;
    std::cout<<"Var args: "<<std::endl;
    for(auto v: args)
        std::cout<<"\t"<<v<<std::endl;
}

void buildIRtree(std::string filename){
    std::string name, data_type;
    std::vector<std::string> ins, outs, stmts;
    if(parseJSON(filename, name, ins, outs, data_type, stmts) < 0)
        return;
    for(auto stmt: stmts){
        std::string tmp;
        int idx = 0;
        std::cout<<stmt<<std::endl;
        while((tmp = getitem(stmt, idx)).length()){
            std::cout<<tmp<<std::endl;
        }
    }
}

int main() {
    for(int i=1;i<=10;++i){
        buildIRtree("./cases/case" + std::to_string(i) + ".json");
    }
    std::string name;
    std::vector<size_t> shape;
    std::vector<std::string> args;
    parseVar("B<2, 16, 7, 7>[n, c, p + r, q + s]", name, shape, args);

    std::string cheat_src =
"// this is supposed to be generated by codegen tool\n\
#include \"../run.h\"\n\
\n\
void kernel_example(float (&B)[32][16], float (&C)[32][16], float (&A)[32][16]) {\n\
    for (int i = 0; i < 32; ++i) {\n\
        for (int j = 0; j < 16; ++j) {\n\
            A[i][j] = B[i][j] * C[i][j];\n\
        }\n\
    }\n\
}";
    std::ofstream ofile("./kernels/kernel_example.cc", std::ios::out);
    ofile << cheat_src;
    ofile.close();
    return 0;
}