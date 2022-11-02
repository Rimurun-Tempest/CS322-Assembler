/*
Name: Divyanshu Chandra
Roll No: 2001CS25
*/

#include<bits/stdc++.h>
using namespace std;

string filename;
vector<int> obj;
map<int, string> mnemonics;

int mem[1000000];
int pc = 0, sp = 0, A = 0, B = 0;
int total = 0;
FILE *output_fptr;

void initialize();
bool advance();
pair<int, bool> read_operand(string &operand);
int argumentrun(int silent = 0);
void dump();
void executeOpcode(int opcode, int operand);
void help();
void run();

int main(int argc, char * argv[]){
    initialize();
    if(argc < 2){
        printf("Error: No File Name Given\n");        
        return 0;
    }
    string file_name = argv[1];
    filename = file_name.substr(0, file_name.length() - 2) + ".txt";
    ifstream f_ptr(file_name, ios::in | ios::binary);
    char char_array[filename.length()];
    strcpy(char_array, filename.c_str());
    output_fptr = fopen(char_array, "w");
    if(!f_ptr.is_open()){
        fprintf(output_fptr, "Error: No Object File Found");
        printf("Error: No Object File Found");
        return 0;        
    }
    int num;
    while(f_ptr.read((char*)&num, sizeof(int))){
        obj.push_back(num);
    }
    for(int i = 0 ; i < obj.size(); i++) {
        mem[i] = obj[i];
        // printf("%08x %d\n", i, mem[i]);
        // cout << mem[i] << endl;
    }
    printf("-t Trace\n-dump\tMemory Dump\n-all\tExecute all lines\n-run\tRun the program\n-exit\tTo Exit\n-help\tFor Help\n");
    while(advance()){
    }
    printf("Total instructions executed %d\n", total);
    fprintf(output_fptr, "Total instructions executed %d\n", total);    
    fclose(output_fptr);
}

bool advance(){
    string temp;
    printf("Emulator Input: ");
    cin >> temp;
    if(temp == "-t" or temp == "-T"){
        int temp2 = argumentrun();
        if(temp2 == 0){
            return false;
        }
        printf("A = %08X, B = %08X, PC = %08X, SP = %08X\n", A, B, pc, sp);
    }
    else if(temp == "-all" or temp == "-ALL"){
        while(argumentrun()){
            fprintf(output_fptr,"A = %08X, B = %08X, PC = %08X, SP = %08X\n", A, B, pc, sp );
            printf("A = %08X, B = %08X, PC = %08X, SP = %08X\n", A, B, pc, sp); 
        }
    }
    else if(temp == "-dump" or temp == "-DUMP"){
        dump();
    }
    else if(temp == "-help" or temp == "-HELP"){
        help();
    }
    else if(temp == "-run" or temp == "-RUN"){
        run();
    }
    else if(temp == "-exit" or temp == "-EXIT"){
        return false;
    }
    else{
        printf("Error: Invalid Input\n");
    }
    return true;
}

void help(){
    printf("-t Trace\n-dump\tMemory Dump\n-all\tExecute all lines\n-run\tRun the program\n-exit\tTo Exit\n-help\tFor Help\n");
}

void run(){
    while(argumentrun(1)){
    }
    printf("Total instructions executed %d\n", total);
}
pair<int, bool> read_operand(string &operand){
    if (!(int)operand.size()) {
        return {0, 0};
    }
    int len = (int)operand.size();
    char *str = (char *)malloc (len *  sizeof(char));
    for (int i = 0; i < (int)operand.size(); i++) {
        str[i] = operand[i];
    }
    for (int i = (int)operand.size(); i < strlen(str); i++) {
        str[i] = '\0';
    }
    char *end;
    long int num;
    num = strtol(str, &end, 10);
    if (!*end) {
        return {num, 1};
    }
    num = strtol(str, &end, 16);
    if (!*end) {
        return {num, 1};
    }
    return {-1, 0};
}

void dump(){
    string operand, offset;
    int baseAddress;
    printf("Base Address: ");
    cin >> operand;
    pair<int, bool> value = read_operand(operand);
    baseAddress = value.first;
    printf("No. of values: ");
    cin >> offset;
    pair<int, bool> value_2 = read_operand(offset);
	for (int i = baseAddress; i < baseAddress + value_2.first; i = i + 4) {
		printf("%08X %08X %08X %08X %08X\n", i, mem[i], mem[i+1], mem[i+2], mem[i+3]);	
	}
}

int argumentrun(int silent) {
    if(pc >= obj.size()){
        cout << "Error: Segmentation Fault" << endl;
        fprintf(output_fptr, "Error: Segmentation Fault\n");
        return 0;
    }
    int opcode = 0xFF & obj[pc];
    int operand = obj[pc] >> 8;
    if(!silent){
        string instruction = mnemonics[opcode];
        char char_array[instruction.length()];
        strcpy(char_array, instruction.c_str());
        fprintf(output_fptr, "%s", char_array);
        for(int i = 0; i < 8 - instruction.length(); i++) {
            fprintf(output_fptr, " ");
        }        
        fprintf(output_fptr, "%08X\n", operand);
        printf("%s\t%08X\n", char_array, operand);        
    }    
    total++;
    if(opcode == 18){
        return 0;
    }
    executeOpcode(opcode, operand);
    pc++;
    return 1;
}

void executeOpcode(int opcode, int operand) {
    if(opcode == 0) {
        B = A;
        A =  operand;
    }
    else if(opcode == 1 ){
        A += operand;
    }
    else if(opcode == 2){
        B = A;
        A = mem[sp + operand];
    }
    else if(opcode == 3){
        mem[sp + operand] = A;
        A = B;
    }
    else if(opcode == 4){
        A = mem[A + operand];
    }
    else if(opcode == 5){
        mem[A + operand] = B;
    }
    else if(opcode == 6){
        A = B + A;
    }
    else if(opcode == 7){
        A = B - A;
    }
    else if(opcode == 8){
        A = B << A;
    }
    else if(opcode == 9){
        A = B >> A;
    }
    else if(opcode == 10){
        sp += operand;
    }
    else if(opcode == 11){
        sp = A;
        A = B;
    }
    else if(opcode == 12){
        B = A;
        A = sp;
    }                   
    else if(opcode == 13){
        B = A;
        A = pc;
        pc = operand - 1;
    }
    else if(opcode == 14){
        pc = A;
        A = B;
    }
    else if(opcode == 15){
        if(A == 0){
            pc += operand;
        }
    }                   
    else if(opcode == 16){
        if(A < 0){
            pc += operand;
        }
    }                   
    else if(opcode == 17){
        pc += operand;
    }
    else{
        cout << "Invalid opcode. Incorrect machine code. Aborting" << endl;
        exit(0);
    }                  
}

void initialize(){
    mnemonics.insert({0, "ldc"});
    mnemonics.insert({1, "adc"});
    mnemonics.insert({2, "ldl"});
    mnemonics.insert({3, "stl"});
    mnemonics.insert({4, "ldnl"});
    mnemonics.insert({5, "stnl"});
    mnemonics.insert({6, "add"});
    mnemonics.insert({7, "sub"});
    mnemonics.insert({8, "shl"});
    mnemonics.insert({9, "shr"});
    mnemonics.insert({10, "adj"});
    mnemonics.insert({11, "a2sp"});
    mnemonics.insert({12, "sp2a"});
    mnemonics.insert({13, "call"});
    mnemonics.insert({14, "return"});
    mnemonics.insert({15, "brz"});
    mnemonics.insert({16, "brlz"});
    mnemonics.insert({17, "br"});
    mnemonics.insert({18, "HALT"});    
}