/*
Name: Divyanshu Chandra
Roll No: 2001CS25
*/

#include<bits/stdc++.h>
using namespace std;

string filename;
map<string, int> labels;
map<string, int> mnemonics;
map<int, int> memory;
vector<pair<int, string> > errors;
set<int> no_operands;
set<int> offset_operands;
vector<string> code;
map<int,int> original_line_number;
vector<string> listing;
vector<string> obj;


string remove_comments(string s);
void initialize();
int is_valid(string s);
bool is_decimal(string s);
bool is_octal(string s);
bool is_hexadecimal(string s);
void second_pass();
string to_hex (int decimal_value, int len);
bool is_valid_label(string s);
void write_obj();
void write_log();
void write_list();

int main(int argc, char* argv[]){

    initialize();
    if(argc < 2) {
        cout << "Error: Input File Name" << endl;
        return 0;
    }

    string file_name = argv[1];
    fstream f_ptr(file_name, ios::in);   
    
    filename = file_name.substr(0, file_name.length() - 4); 
    
    if(!f_ptr.is_open()) {
        cout << "Error: File Not Found" << endl;
        return 0;
    }
    
    int line_number = 0;
    int program_counter = 0;
    while(f_ptr.eof() == 0 && ++line_number ) {
        string s;
        getline(f_ptr, s);
        string line = remove_comments(s), word;

        original_line_number[code.size()] = line_number;
        if(line.length() == 0){
            continue;
        }
        code.push_back(line);

        stringstream count(line);
        int cnt = 0;
        while(count >> word){
            cnt++;
        }
        stringstream ss(line);
        ss >> word;
        string label;

        //duplicate lablel check
        if(word.back() == ':') {
            cnt--;
            word.pop_back();
            label = word;
            if(labels.find(label) != labels.end()) {
                errors.push_back({line_number, "Duplicate Label"});
                program_counter++;
                continue;
            }
            else{
                if(is_valid_label(label))
                    labels.insert({label, program_counter});
                else
                    errors.push_back({line_number, "Label Not Alphanumeric"});
            }
            if(cnt == 0)
                continue;
            else
                ss >> word;
        }

        // Is mnemonic correct
        string instruction = word;
        if(mnemonics.find(word) != mnemonics.end()) {
            if(no_operands.find(mnemonics[word]) != no_operands.end()){
                if(cnt != 1) {
                    errors.push_back({line_number, "Incorrect number of operand"});
                }
            }
            else if(no_operands.find(mnemonics[word]) == no_operands.end()){
                if(cnt == 1) {
                    errors.push_back({line_number, "Missing operand"});
                }
                else if(cnt > 2) {
                    errors.push_back({line_number, "Incorrect number of operand"});
                }
            }
        }
        else if(instruction == "SET") {
            ss >> word;
            if(cnt > 1 and is_valid(word)) {
                labels[label] = stoi(word, nullptr, is_valid(word));
            }
            else{
                errors.push_back({line_number, "Operand not a number"});
            }
        }        
        else{
            if(word == "data") {
                if(cnt == 1) {
                    errors.push_back({line_number, "No value given for data"});
                }
                else{
                    ss >> word;
                    if(cnt > 1 and is_valid(word))
                        memory[program_counter] = stoi(word, nullptr, is_valid(word));
                    else
                        errors.push_back({line_number, "Incorrect Operand"}); 
                }
            }
            else
                errors.push_back({line_number, "Incorrect Instruction"});
        }
        program_counter++;
    }
    second_pass();
    if(errors.empty())
        write_obj();
    write_log();
    write_list();

    f_ptr.close();
}

bool is_valid_label(string s) {
    if(!(s[0]>='a' and s[0] <='z'))
        return false;

    for(int i = 1; i < s.length(); i++) {
        if(!((s[i]>='a' and s[i] <= 'z') or (s[i] >= '0' and s[i] <= '9') or(s[i] == '_')))
            return false;
    }
    return true;
}
int is_valid(string s) {
    if(is_decimal(s))
        return 10;
    if(is_octal(s))
        return 8;
    if(is_hexadecimal(s))
        return 16;
    return 0;
}

bool is_decimal(string s){
    int flag = 0;
    if(s[0]== '-' or s[0] == '+'){
        flag = 1;
    }
    for(int i = flag; i < s.length(); i++) {
        if(s[i] > '9' or s[i] < '0')
            return false;
    }
    return true;
} 
bool is_octal(string s){
    if(s[0] == '0'){
        for(int i = 1; i < s.length(); i++) {
            if(s[i] > '7' or s[i] < '0')
                return false;
        }
        return true;
    }
    return false;
}
bool is_hexadecimal(string s){
    if(s[0] == '0' and s[1] == 'x'){
        for(int i = 2 ; i < s.length(); i++) {
            if((s[i] > '9' or s[i] < '0') and (s[i] < 'a' or s[i] > 'f') )
                return false;
        }
        return true;
    }
    return false;
}

string to_hex (int decimal_value, int len)
{
    stringstream ss;
    ss<< hex << decimal_value; // int decimal_value
    string res (ss.str());
    // Append 0's at back to make size = 4;
    reverse(res.begin(), res.end());
    while (res.size() < len) {
        res += '0';
    }
    while (res.size() > len) {
        res.pop_back();
    }
    reverse(res.begin(), res.end());
    return res;
}

void second_pass() {
    int line_number = 0;
    int program_counter = 0;
    for(int i = 0; i < code.size(); i++, line_number++) {
    // cout << code[i] << endl;
        int cnt = 0;
        stringstream count(code[i]);
        vector<string> line;
        string temp;
        while(count >> temp) {
            line.push_back(temp);
            cnt ++;
        }
        if(line[0].back() == ':') {
            string label = line[0];
            label.pop_back();
            if(cnt == 1) {
                string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                listing.push_back(str);
                continue;
            }
            else if(cnt > 1) {
                if(mnemonics.find(line[1]) != mnemonics.end()) {
                    if(no_operands.find(mnemonics[line[1]]) != no_operands.end()) {
                        if(cnt > 2) {
                            string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                            listing.push_back(str);                            
                        }
                        else {
                            string str = to_hex(program_counter, 8) + " " + to_hex(mnemonics[line[1]], 8) + " " + code[i];
                            string object = to_hex(mnemonics[line[1]], 8);
                            obj.push_back(object);
                            listing.push_back(str);                            
                        }
                    }
                    else {
                        if(cnt < 3) {
                            string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                            listing.push_back(str);                                
                        }
                        else{
                            string operand = line[2];
                            if(is_valid(operand)) {
                                string str = to_hex(program_counter, 8) + " " + to_hex(stoi(operand, nullptr, is_valid(operand)), 6) + to_hex(mnemonics[line[1]], 2) + " " + code[i];
                                string object = to_hex(stoi(operand, nullptr, is_valid(operand)), 6) + to_hex(mnemonics[line[1]], 2);
                                obj.push_back(object);
                                listing.push_back(str);                                    
                            }
                            else{
                                if(labels.find(operand) != labels.end()) {
                                    if(offset_operands.find(mnemonics[line[1]]) != offset_operands.end()){
                                        string str = to_hex(program_counter, 8) + " " + to_hex(labels[operand] - program_counter - 1, 6) + to_hex(mnemonics[line[1]], 2) + " " + code[i];
                                        string object = to_hex(labels[operand] - program_counter - 1, 6) + to_hex(mnemonics[line[1]], 2);
                                        obj.push_back(object);
                                        listing.push_back(str);                                    
                                    }
                                    else{
                                        string str = to_hex(program_counter, 8) + " " + to_hex(labels[operand], 6) + to_hex(mnemonics[line[1]], 2) + " " + code[i];
                                        string object = to_hex(labels[operand], 6) + to_hex(mnemonics[line[1]], 2);
                                        obj.push_back(object);
                                        listing.push_back(str);                                                                            
                                    }
                                }
                                else{
                                    if(!is_valid_label(operand)) {
                                        errors.push_back({original_line_number[line_number], "Not Valid Operand"});
                                    }
                                    else{
                                        errors.push_back({original_line_number[line_number], "No Label Found"});
                                    } 
                                    string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                                    listing.push_back(str);                                                                    
                                }
                            }
                        }
                    }
                }
                else if(line[1] == "SET"){
                    if(cnt < 3) {
                        string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                        listing.push_back(str);                            
                    }
                    else{
                        string operand = line[2];
                        if(is_valid(operand)) {
                            string str = to_hex(program_counter, 8) + " " + to_hex(stoi(operand, nullptr, is_valid(operand)), 8) + " " + code[i];
                            // string object = to_hex(stoi(operand, nullptr, is_valid(operand)), 8);
                            // obj.push_back(object);
                            listing.push_back(str);                                    
                        }                            
                        else{
                            string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                            listing.push_back(str);                                      
                        }
                    }
                }
                else if(line[1] == "data") {
                    if(cnt < 3) {
                        string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                        listing.push_back(str);                                  
                    }
                    else{
                        string operand = line[2];
                        if(is_valid(operand)) {
                            string str = to_hex(program_counter, 8) + " " + to_hex(stoi(operand, nullptr, is_valid(operand)), 8) + " " + code[i];
                            string object = to_hex(stoi(operand, nullptr, is_valid(operand)), 8);
                            obj.push_back(object);
                            listing.push_back(str);                                    
                        }                            
                        else{
                            string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                            listing.push_back(str);                                      
                        }                            
                    }
                }
                else{
                    string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                    listing.push_back(str);
                }                    
                
            }
        }
        else{
            if(mnemonics.find(line[0]) != mnemonics.end()) {
                if(no_operands.find(mnemonics[line[0]]) != no_operands.end()) {
                    if(cnt > 1) {
                        string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                        listing.push_back(str);                            
                    }
                    else {
                        string str = to_hex(program_counter, 8) + " " + to_hex(mnemonics[line[0]], 8) + " " + code[i];
                        string object = to_hex(mnemonics[line[0]], 8);
                        obj.push_back(object);
                        listing.push_back(str);                            
                    }
                }
                else {
                    if(cnt < 2) {
                        string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                        listing.push_back(str);                                
                    }
                    else{
                        string operand = line[1];
                        if(is_valid(operand)) {
                            string str = to_hex(program_counter, 8) + " " + to_hex(stoi(operand,nullptr, is_valid(operand)), 6) + to_hex(mnemonics[line[0]], 2) + " " + code[i];
                            string object = to_hex(stoi(operand, nullptr, is_valid(operand)), 6) + to_hex(mnemonics[line[0]], 2);
                            obj.push_back(object);
                            listing.push_back(str);                                    
                        }
                        else{
                            if(labels.find(operand) != labels.end()) {
                                if(offset_operands.find(mnemonics[line[0]]) != offset_operands.end()){
                                    string str = to_hex(program_counter, 8) + " " + to_hex(labels[operand] - program_counter - 1, 6) + to_hex(mnemonics[line[0]], 2) + " " + code[i];
                                    string object = to_hex(labels[operand] - program_counter - 1, 6) + to_hex(mnemonics[line[0]], 2);
                                    obj.push_back(object);
                                    listing.push_back(str);                                    
                                }
                                else{
                                    string str = to_hex(program_counter, 8) + " " + to_hex(labels[operand], 6) + to_hex(mnemonics[line[0]], 2) + " " + code[i];
                                    string object = to_hex(labels[operand], 6) + to_hex(mnemonics[line[0]], 2);
                                    obj.push_back(object);
                                    listing.push_back(str);                                    
                                }
                            }
                            else{
                                if(!is_valid_label(operand)) {
                                    errors.push_back({original_line_number[line_number], "Not Valid Operand"});
                                }
                                else{
                                    errors.push_back({original_line_number[line_number], "No Label Found"});
                                } 
                                string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                                listing.push_back(str);                                                                                                       
                            }
                        }
                    }
                }
            }
            else if(line[0] == "data") {
                if(cnt < 2) {
                    string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                    listing.push_back(str);                                  
                }
                else{
                    string operand = line[1];
                    if(is_valid(operand)) {
                        string str = to_hex(program_counter, 8) + " " + to_hex(stoi(operand, nullptr, is_valid(operand)), 8) + " " + code[i];
                        string object = to_hex(stoi(operand, nullptr, is_valid(operand)), 8);
                        obj.push_back(object);
                        listing.push_back(str);                                    
                    }                            
                    else{
                        string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                        listing.push_back(str);                                      
                    }                            
                }
            }
            else{
                string str = to_hex(program_counter, 8) + " " + to_hex(0, 8) + " " + code[i];
                listing.push_back(str);
            }            
        }
        program_counter++ ;
    }
}

string remove_comments(string s) {
    string ans = "", temp;
    stringstream current(s);
    while(current >> temp) {
        if(temp == ";" or temp.front() == ';')
            break;
        else if(temp.back() == ';'){
            temp.pop_back();
            ans += temp;
            ans += " ";
            break;
        }
        ans += temp;
        ans += " ";
    }
    temp.clear();
    int flag = 0;
    for(int i = 0; i < ans.length(); i++) {
        temp += ans[i];
        if(ans[i] == ':' and flag == 0){
            temp += " ";
            flag = 1;
        }
    }
    return temp;
}

void write_obj(){
    ofstream obj_file;
    obj_file.open(filename + ".o", ios::binary | ios::out);
    for(int i = 0; i < obj.size(); i++) {
        int x = 0;
        string temp = obj[i];
        reverse(temp.begin(), temp.end());
        for(int j = 0; j < 8; j++) {
            int val = 0;
            if (temp[j] >= 'a' and temp[j] <= 'f') {
                val = (temp[j]-'a'+10);
            } else {
                val = temp[j] - '0';
            }
            x += (val * (int)pow(16, j));            
        }
        static_cast<int>(x);
        obj_file.write((const char*)&x, sizeof(int));
    }
    obj_file.close();
}

void write_log(){
    ofstream log_file(filename + ".log");
    log_file << "Labels Value" << endl;
    for(auto it = labels.begin(); it != labels.end(); it++) {
        log_file << it->first << "->" << it->second << endl;
    }
    log_file << endl << "Labels Used" << endl;
    for(auto it = labels.begin(); it != labels.end(); it++) {
        log_file << it->first << endl;
    }    
    log_file << endl << "Errors" << endl;
    sort(errors.begin(), errors.end());
    for(int i = 0; i < errors.size(); i++) {

        log_file << "Line Number " << errors[i].first << " " << errors[i].second << endl;
        cout << "Line Number " << errors[i].first << " " << errors[i].second << endl;
    }
    log_file.close();
}

void write_list(){
    ofstream list_file(filename + ".lst");
    for(int i = 0; i < listing.size(); i++){
        list_file << listing[i] << endl;
    }
    list_file.close();
}

void initialize() {
    mnemonics.insert({"ldc",    0});
    mnemonics.insert({"adc",    1});
    mnemonics.insert({"ldl",    2});
    mnemonics.insert({"stl",    3});
    mnemonics.insert({"ldnl",   4});
    mnemonics.insert({"stnl",   5});
    mnemonics.insert({"add",    6});
    mnemonics.insert({"sub",    7});
    mnemonics.insert({"shl",    8});
    mnemonics.insert({"shr",    9});
    mnemonics.insert({"adj",    10});
    mnemonics.insert({"a2sp",   11});
    mnemonics.insert({"sp2a",   12});
    mnemonics.insert({"call",   13});
    mnemonics.insert({"return", 14});
    mnemonics.insert({"brz",    15});
    mnemonics.insert({"brlz",   16});
    mnemonics.insert({"br",     17});
    mnemonics.insert({"HALT",   18});
    no_operands.insert({6, 7, 8, 9, 11, 12, 14, 18});
    offset_operands.insert({15, 16, 17});
}