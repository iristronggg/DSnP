/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <string>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

#include <fstream>
#include <sstream>

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream file;
   file.open(fileName);
   if(!file.is_open()){
       cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
       return false;
   }
   
   //header
   /*file.getline(buf, 1024);
   stringstream ss(&buf[4]);
   unsigned m, i, l, o, a;
   ss >> m >> i >> l >> o >> a;
   lineNo++;
   for(size_t j = 0; j < m + 1; j++)
       _gates.push_back(NULL);
   _gates[0] = new ConstGate();
   for(size_t j = 0; j < o; j++)
       _pos.push_back(NULL);
   for(size_t j = 0; j < i; j++)
       _pis_idx.push_back(0);*/

   lineNo = 0;
   colNo = 0;
   errMsg = "";

   //header
   if(!(file.getline(buf, 1024))){
       errMsg = "aag";
       return (parseError(MISSING_IDENTIFIER));
   }
   stringstream ss(buf);
   string token[6];
   if(buf[colNo] == ' ')                   //extra space
       return (parseError(EXTRA_SPACE));
   if(isspace(buf[colNo])){                //extra non-white space
       errInt = buf[colNo];
       return (parseError(ILLEGAL_WSPACE));
   }
   if(!(ss >> token[0])){                  //"aag" not specified
       errMsg = "aag";
       return (parseError(MISSING_IDENTIFIER));
   }
   if(token[0] != "aag"){                  //something other than "aag" is specified
       errMsg = token[0];
       return (parseError(ILLEGAL_IDENTIFIER));
   }
   char space = 0;
   colNo += token[0].length();
   for(int i = 0; i < 5; i++){
       ss.get(space);
       colNo++;
       if(space != ' '){                   //missing space
           errInt = space;
           return (parseError(MISSING_SPACE));
       }
       if(buf[colNo] == ' ')               //extra space
           return (parseError(EXTRA_SPACE));
       if(isspace(buf[colNo])){            //extra non-space white space
           errInt = buf[colNo];
           return (parseError(ILLEGAL_WSPACE));
       }
       if(!(ss >> token[i + 1])){          //missing variables
           errMsg = "number of variables";
           return (parseError(MISSING_NUM));
       }
       for(size_t j = 0; j < token[i + 1].length(); j++){ //not a number
           if(!isdigit(token[i + 1][j])){
               errMsg = "number of variables";
               return (parseError(ILLEGAL_NUM));
           }
       }
       colNo += token[i + 1].length();
   }
   if(colNo != strlen(buf))                //missing newline (or additional variables)
       return (parseError(MISSING_NEWLINE));

   //unsigned m, i, l, o, a;
   m = (unsigned)stoi(token[1]);
   i = (unsigned)stoi(token[2]);
   l = (unsigned)stoi(token[3]);
   o = (unsigned)stoi(token[4]);
   a = (unsigned)stoi(token[5]);
   if(m < (i + a)){                        //m < i + a
       errMsg = "Number of variables";
       errInt = (int)m;
       return (parseError(NUM_TOO_SMALL));
   }
   if(l != 0){                             //illegal latches
       errMsg = "latches";
       return (parseError(ILLEGAL_NUM));
   }
   for(size_t j = 0; j < m + 1; j++)
       _gates.push_back(NULL);
   _gates[0] = new ConstGate();
   for(size_t j = 0; j < o; j++)
       _pos.push_back(NULL);
   for(size_t j = 0; j < i; j++)
       _pis_idx.push_back(0);


   /*
   //input
   for(size_t j = 0; j < i; j++){
       unsigned idx = 0;
       file >> idx;
       idx /= 2;
       _pis_idx[j] = idx;
       _gates[idx] = new PiGate(idx, ++lineNo);
   }*/

   //input
   for(unsigned j = 0; j < i; j++){
       lineNo++;
       colNo = 0;
       if(!(file.getline(buf, 1024))){     //missing pi definition
           errMsg = "PI";
           return (parseError(MISSING_DEF));
       }
       stringstream s(buf);
       if(buf[colNo] == ' ')               //extra space
           return (parseError(EXTRA_SPACE));
       if(isspace(buf[colNo])){            //extra non-space white space
           errInt = buf[colNo];
           return (parseError(ILLEGAL_WSPACE));
       }
       string literal;
       unsigned idx = 0;
       if(!(s >> literal)){                //missing pi definition
           errMsg = "PI";
           return parseError(MISSING_DEF);
       }
       idx = (unsigned)stoi(literal);
       if(idx % 2 == 1){                   //pi literal cannot be odd
           errInt = (int)idx;
           return (parseError(CANNOT_INVERTED));
       }
       idx /= 2;
       if(idx > m){                        //pi index too big
           errInt = (idx * 2);
           return (parseError(MAX_LIT_ID));
       }
       
       colNo += literal.length();
       if(colNo != strlen(buf))            //missing newline (or additional variables)
           return (parseError(MISSING_NEWLINE));
       for(unsigned k = 0; k < literal.length(); k++){ //not a number
           if(!isdigit(literal[k])){
               errMsg = "PI literal";
               return (parseError(ILLEGAL_NUM));
           }
       }

       _pis_idx[j] = idx;
       if(_gates[idx] != NULL){            //redefine
           if(idx == 0)
               return (parseError(REDEF_CONST));
           else{
               errInt = stoi(literal);
               errGate = _gates[idx];
               return (parseError(REDEF_GATE));
           }
       }
       _gates[idx] = new PiGate(idx, lineNo + 1);
   }

   /*
   //output
   for(size_t j = 0; j < o; j++){
       unsigned literal = 0;
       file >> literal;
       _pos[j] = new PoGate(m + j + 1, ++lineNo);
       _pos[j]->_fanin_idx[0] = literal / 2;
       _pos[j]->_fanin_invert[0] = literal % 2;
   }*/

   //output
   for(size_t j = 0; j < o; j++){
       lineNo++;
       colNo = 0;
       if(!(file.getline(buf, 1024))){         //missing po definition
           errMsg = "PO";
           return (parseError(MISSING_DEF));
       }
       stringstream s(buf);
       if(buf[colNo] == ' ')                   //extra space
           return (parseError(EXTRA_SPACE));
       if(isspace(buf[colNo])){                //extra non-space white space
           errInt = buf[colNo];
           return (parseError(ILLEGAL_WSPACE));
       }
       string literal;
       if(!(s >> literal)){                    //missing pi definition
           errMsg = "PO";
           return parseError(MISSING_DEF);
       }

       colNo += literal.length();
       if(colNo != strlen(buf)){               //missing newline (or additional variables)
           return (parseError(MISSING_NEWLINE));
       }
       for(unsigned k = 0; k < literal.length(); k++){ //not a number
           if(!isdigit(literal[k])){
               errMsg = "PO literal";
               return (parseError(ILLEGAL_NUM));
           }
       }
       unsigned literal_num = (unsigned)stoi(literal);
       _pos[j] = new PoGate(m + j + 1, lineNo + 1);
       _pos[j]->_fanin_idx[0] = literal_num / 2;
       _pos[j]->_fanin_invert[0] = literal_num % 2;
   }

   /*
   //Aig gate
   for(size_t j = 0; j < a; j++){
       unsigned self_literal, fanin_literal[2];
       file >> self_literal >> fanin_literal[0] >> fanin_literal[1];
       _gates[self_literal / 2] = new AigGate(self_literal / 2, ++lineNo);
       for(size_t k = 0; k < 2; k++){
           _gates[self_literal / 2]->_fanin_idx[k] = fanin_literal[k] / 2;
           _gates[self_literal / 2]->_fanin_invert[k] = fanin_literal[k] % 2;
       }
   }*/

   //Aig gate
   for(size_t j = 0; j < a; j++){
       lineNo++;
       colNo = 0;
       if(!(file.getline(buf, 1024))){         //missing aig definition
           errMsg = "AIG";
           return (parseError(MISSING_DEF));
       }
       stringstream s(buf);
       if(buf[colNo] == ' ')                   //extra space
           return (parseError(EXTRA_SPACE));
       if(isspace(buf[colNo])){                //extra non-space white space
           errInt = buf[colNo];
           return (parseError(ILLEGAL_WSPACE));
       }

       string self_literal_str, fanin_literal_str[2];
       unsigned self_literal, fanin_literal[2];
       if(!(s >> self_literal_str)){           //missing definition
           errMsg = "AIG";
           return (parseError(MISSING_DEF));
       }
       for(unsigned k = 0; k < self_literal_str.length(); k++){ //not a number
           if(!isdigit(self_literal_str[k])){
               errMsg = "AIG literal";
               return (parseError(ILLEGAL_NUM));
           }
       }
       self_literal = (unsigned)stoi(self_literal_str);
       if(self_literal % 2 == 1){              //cannot be invert
           errMsg = "AIG index";
           errInt = self_literal;
           return (parseError(CANNOT_INVERTED));
       }
       if((self_literal / 2) > m){               //id > m
           errInt = self_literal;
           return (parseError(MAX_LIT_ID));
       }
       colNo += self_literal_str.length();
       for(int k = 0; k < 2; k++){
           if(!s.get(space))
               return (parseError(MISSING_SPACE));
           colNo++;
           if(space != ' ')                    //missing space
               return (parseError(MISSING_SPACE));
           if(buf[colNo] == ' ')               //extra space
               return (parseError(EXTRA_SPACE));
           if(isspace(buf[colNo])){            //extra non-space white space
               errInt = buf[colNo];
               return (parseError(ILLEGAL_WSPACE));
           }
           if(!(s >> fanin_literal_str[k])){   //missing fanin
               errMsg = "AIG input literal ID";
               return (parseError(MISSING_NUM));
           }
           for(unsigned p = 0; p < fanin_literal_str[k].length(); p++){ //not a number
               if(!isdigit(fanin_literal_str[k][p])){
                   errMsg = "AIG input literal ID";
                   return (parseError(ILLEGAL_NUM));
               }
           }
           fanin_literal[k] = (unsigned)stoi(fanin_literal_str[k]);
           colNo += fanin_literal_str[k].length();
       }
       if(_gates[self_literal / 2] != NULL){       //redefine gate
           errInt = self_literal;
           errGate = _gates[self_literal / 2];
            
           if(self_literal / 2 == 0)
               return (parseError(REDEF_CONST));
           return (parseError(REDEF_GATE));
       }
       _gates[self_literal / 2] = new AigGate(self_literal / 2, lineNo + 1);
       for(size_t k = 0; k < 2; k++){
           _gates[self_literal / 2]->_fanin_idx[k] = fanin_literal[k] / 2;
           _gates[self_literal / 2]->_fanin_invert[k] = fanin_literal[k] % 2;
       }
   }

   //symbol
   /*char type = '\0';
   while(file >> type){
     //reach the end of the symbol part
     if(file.eof()){
         file.clear();
         break;
     }
     else if(type == 'c')
         break;
     
     unsigned position;
     file >> position;
     file.ignore();
     file.getline(buf, 1024);
     switch(type){
         case 'i':
             dynamic_cast<PiGate*>(_gates[_pis_idx[position]])->_symbol = buf;
             break;
         case 'o':
             _pos[position]->_symbol = buf;
             break;
     }
   }*/

   char type = 0;
   while(file.getline(buf, 1024)){
       lineNo++;
       colNo = 0;
       stringstream s(buf);
       if(strlen(buf) == 1 && buf[0] == 'c')
           break;
       if(!s.get(type)){
           errInt = '\n';
           return (parseError(ILLEGAL_WSPACE));
       }
       if(type == ' ')                     //extra space
           return (parseError(EXTRA_SPACE));
       if(isspace(type)){                  //illegal white space
           errInt = type;
           return (parseError(ILLEGAL_WSPACE));
       }
       if(!(type == 'i' || type == 'o')){  //illegal symbol type
           errMsg = type;
           return (parseError(ILLEGAL_SYMBOL_TYPE));
       }
       colNo++;
       if(strlen(buf) <= colNo){           //extra newline
           errMsg = "symbol name";
           return (parseError(MISSING_IDENTIFIER));
       }
       if(buf[colNo] == ' ')               //extra space
           return (parseError(EXTRA_SPACE));
       if(isspace(buf[colNo])){            //extra space
           errInt = buf[colNo];
           return (parseError(ILLEGAL_WSPACE));
       }
       string position_str;
       unsigned position;
       if(!(s >> position_str)){           //missing symbol position
           errMsg = "symbol position";
           return (parseError(MISSING_NUM));
       }
       for(unsigned k = 0; k < position_str.length(); k++){ //not a number
           if(!isdigit(position_str[k])){
               errMsg = "symbol index";
               return (parseError(ILLEGAL_NUM));
           }
       }
       colNo += position_str.length();
       position = (unsigned)stoi(position_str);
       if(!s.get(space))
           return (parseError(MISSING_SPACE));
       colNo++;
       char name[1024];
       if(!s.getline(name, 1024) || strlen(name) == 0){         //missing symbol name
           errMsg = "symbol name";
           return (parseError(MISSING_IDENTIFIER));
       }
       for(unsigned k = 0; k < strlen(name); k++){     //illegal symbol name, unprintable
           if(!isprint(name[k])){
               errInt = name[k];
               colNo += k;
               return (parseError(ILLEGAL_SYMBOL_NAME));
           }
       }
      if(type == 'i'){
           if(position > _pis_idx.size()){           //symbol index too big
               errMsg = "symbol index";
               errInt = (int)position;
               return (parseError(NUM_TOO_BIG));
           }
           string symbol(name);
           if(dynamic_cast<PiGate*>(_gates[_pis_idx[position]])->_symbol != ""){ //redefine symbol
               errMsg = "i";
               errInt = position;
               return (parseError(REDEF_SYMBOLIC_NAME));
           }
           dynamic_cast<PiGate*>(_gates[_pis_idx[position]])->_symbol = symbol;
       }
       else if(type == 'o'){
           if(position > _pos.size()){                //symbol index too big
               errMsg = "symbol index";
               errInt = (int)position;
               return (parseError(NUM_TOO_BIG));
           }
           string symbol(name);
           if(_pos[position]->_symbol != ""){        //redefine symbol
               errMsg = "o";
               errInt = position;
               return (parseError(REDEF_SYMBOLIC_NAME));
           }
           _pos[position]->_symbol = symbol;
       }
   }

   
   //connect the gates
   for(size_t j = 1; j <= m; j++){
       if(_gates[j] == NULL)
           continue;
       //Aig gate
       else if(_gates[j]->getTypeStr() == "AIG"){
           for(size_t k = 0; k < 2; k++){
               unsigned fanin_idx = _gates[j]->_fanin_idx[k];
               //const0
               if(fanin_idx == 0){
                   _gates[j]->_fanin[k] = _gates[0];
                   _gates[0]->_fanout.push_back(_gates[j]);
               }
               //undef
               else if(fanin_idx > m || _gates[fanin_idx] == NULL){
                   CirGate* u = new UndefGate(fanin_idx);
                   _gates[j]->_fanin[k] = u;
                   _gates[fanin_idx] = u;
                   _gates[fanin_idx]->_fanout.push_back(_gates[j]);
               }
               //valid fanin
               else{
                   _gates[j]->_fanin[k] = _gates[fanin_idx];
                   _gates[fanin_idx]->_fanout.push_back(_gates[j]);
               }
           }
       }
   }

   //connect Po gate
   for(size_t j = 0; j < _pos.size(); j++){
       unsigned fanin_idx = _pos[j]->_fanin_idx[0];
       //const 0
       if(fanin_idx == 0){
           _pos[j]->_fanin[0] = _gates[0];
           _gates[0]->_fanout.push_back(_pos[j]);
       }
       //undef
       else if(fanin_idx > m || _gates[fanin_idx] == NULL){
           CirGate* u = new UndefGate(fanin_idx);
           _pos[j]->_fanin[0] = u;
           _gates[fanin_idx] = u;
           _gates[fanin_idx]->_fanout.push_back(_pos[j]);
       }
       //valid
       else{
         _pos[j]->_fanin[0] = _gates[_pos[j]->_fanin_idx[0]];
         _pos[j]->_fanin[0]->_fanout.push_back(_pos[j]);
       }
   }
    
    //dfs
    dfsTraversal();
    /*for(size_t j = 0; j < _gates.size(); j++){
        if(_gates[j]->getTypeStr() == "UNDEF")
            cout << _gates[j]->_fanin_idx[0] << " " << _gates[j]->_fanin_idx[1] ;
    }*/
    
    

   return true;
}

void
CirMgr::clear(){
    for(size_t i = 0; i < _gates.size(); i++)
        if(_gates[i] != NULL)
            delete _gates[i];
      for(size_t i = 0; i < _pos.size(); i++)
        if(_pos[i] != NULL)
            delete _pos[i];
    _gates.clear();
    _pis_idx.clear();
    _pos.clear();
}

//void CirMgr::setSimLog(ofstream *logFile){ _simLog = logFile; }

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
    unsigned aig_cnt = 0;
    for(size_t i = 1; i < _gates.size(); i++)
        if(_gates[i] != NULL && _gates[i]->getTypeStr() == "AIG")
            aig_cnt++;

    cout << endl
         << "Circuit Statistics" << endl
         << "==================" << endl
         << "  PI"    << std::setw(12) << _pis_idx.size() << endl
         << "  PO"    << std::setw(12) << _pos.size()     << endl
         << "  AIG"   << std::setw(11) << aig_cnt << endl
         << "------------------" << endl
         << "  Total" << std::setw(9)  << _pis_idx.size() + _pos.size() + aig_cnt << endl;
}

void
CirMgr::printNetlist() const
{
    CirGate::cur_mark++;
    CirGate::cur_print_idx = 0;
    cout << endl;
    for(size_t i = 0; i < _pos.size(); i++)
        _pos[i]->printGate();
    
    /*for(size_t i = 0; i < _dfsList.size(); i++)
        cout << _dfsList[i]->_id << " ";*/
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(size_t i = 0; i < _pis_idx.size(); i++)
      cout << " " << _pis_idx[i];
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(size_t i = 0; i < _pos.size(); i++)
      cout << " " << _pos[i]->_id;
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
    vector<unsigned> floating;
    vector<unsigned> not_used;
    for(size_t i = 1; i < _gates.size(); i++){
        if(_gates[i] == NULL)
            continue;
        if(_gates[i]->_fanout.size() == 0)
            not_used.push_back(i);
        if(_gates[i]->getTypeStr() == "AIG" && (_gates[i]->_fanin[0]->getTypeStr() == "UNDEF" || _gates[i]->getTypeStr() == "UNDEF"))
            floating.push_back(i);
    }
    for(size_t i = 0; i < _pos.size(); i++)
        if(_pos[i]->_fanin[0]->getTypeStr() == "UNDEF")
            floating.push_back(_pos[i]->_id);

    if(floating.size() != 0){
        cout << "Gates with floating fanin(s):";
        for(size_t i = 0; i < floating.size(); i++)
            cout << " " << floating[i];
        cout << endl;
    }
    if(not_used.size() != 0){
        cout << "Gates defined but not used  :";
        for(size_t i = 0; i < not_used.size(); i++)
            cout << " " << not_used[i];
        cout << endl;
    }
}

int
CirMgr::valid_aig() const {
    CirGate::cur_mark++;
    int sum = 0;
    for(unsigned i = 0; i < _pos.size(); i++)
        sum += _pos[i]->aig_under();
    return sum;
}

void
CirMgr::print_valid_aig(ostream& outfile) const {
    CirGate::cur_mark++;
    for(unsigned i = 0; i < _pos.size(); i++)
        _pos[i]->print_aig_under(outfile);
}

void
CirMgr::printFECPairs() const
{
    vector<size_t> _FECsortgroup;
    for(size_t i = 0; i < _FECgroup.size(); i++){
        _FECsortgroup.push_back(_FECgroup[i][0].getG()->_id);
    }
    
    std::sort(_FECsortgroup.begin(), _FECsortgroup.end());
    
    for(size_t k = 0; k < _FECsortgroup.size(); k++){
        for(size_t i = 0; i < _FECgroup.size(); i++){
            if(_FECgroup[i][0].getG()->_id == _FECsortgroup[k]){
                bool flag = _FECgroup[i][0].isIFEC();
                cout << "[" << k << "]";
                for(size_t j = 0; j < _FECgroup[i].size(); j++){
                    cout << " ";
                    if(_FECgroup[i][j].isIFEC() != flag)
                        cout << "!";
                    cout << _FECgroup[i][j].getG()->_id;
                }
                cout << endl;
            }
        }
    }
    cout << endl;
        
}

void
CirMgr::writeAag(ostream& outfile) const
{
    //header
    outfile << "aag "
            << _gates.size() - 1 << " "
            << _pis_idx.size() << " "
            << "0 "
            << _pos.size() << " "
            << valid_aig() << endl;
    //pi
    for(unsigned i = 0; i < _pis_idx.size(); i++)
        outfile << 2 * _pis_idx[i] << endl;
    //po
    for(unsigned i = 0; i < _pos.size(); i++)
        outfile << 2 * _pos[i]->_fanin_idx[0] + (_pos[i]->_fanin_invert[0]? 1 : 0) << endl;
    //aig
    print_valid_aig(outfile);
    //symbol
    for(unsigned i = 0; i < _pis_idx.size(); i++)
        if(dynamic_cast<PiGate*>(_gates[_pis_idx[i]])->_symbol != "")
            outfile << "i" << i << " " << dynamic_cast<PiGate*>(_gates[_pis_idx[i]])->_symbol << endl;
    for(unsigned i = 0; i < _pos.size(); i++)
        if(dynamic_cast<PoGate*>(_pos[i])->_symbol != "")
            outfile << "o" << i << " " << dynamic_cast<PoGate*>(_pos[i])->_symbol << endl;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
    
}

void
CirMgr::dfsTraversal()
{
    _dfsList.clear();
    CirGate::cur_mark++;
    for(size_t j = 0; j < _pos.size(); j++){
        if(_pos[j] != NULL){
            _pos[j]->genDFS(_dfsList);
            _dfsList.push_back(_pos[j]);
        }
    }
}
