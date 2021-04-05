/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <string>
#include <set>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

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
   MISSING_ID,
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
      case MISSING_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << " literal ID!!" << endl;
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
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
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
    
    ifstream inf(fileName);
    string head;
    if(!inf.is_open()) {
       cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
       return false;
    }
    
    /*
    unsigned num = 0;
    char *ptr;
    string str;
    vector<string> opts;
    bool space;

    // read Header
    f.getline(buf, MAX_BUF_LEN, "\n");
    try{
        if(buf[0] == " "){
            throw EXTRA_SPACE;
        }
        
        /*while(buf[colNo+1] == " "){
            colNo++;
            if(colNo > 1){
                space = true;
            }
        }
        
        
        
        ptr = strtok(buf, " ");
        while (ptr != NULL)
        {
            str.assign(ptr); // char* to string
            opts.push_back(s_str);
            ptr = strtok(NULL, " ");
        }
        if(opts[0] != "aag"){
            errMsg = opts[0];
            throw ILLEGAL_IDENTIFIER;
        }
        
        //read m i l o a
        for(unsigned i = 1; i <= 5; i++){
            if(opts[i] == NULL){
                errMsg = "number of variables";
                catch MISSING_NUM;
            }
            if(i == 1){ //m
                if(!myStr2Unsigned(opts[i], m)){
                    errMsg = "number of variables(" + opts[i] + ")";
                    catch ILLEGAL_NUM;
                }
            }
            else if(i == 2){ //i
                if(!myStr2Unsigned(opts[i], i)){
                    errMsg = "number of PIs(" + opts[i] + ")";
                    catch ILLEGAL_NUM;
                }
            }
            else if(i == 3){ //l
                if(!myStr2Unsigned(opts[i], l)){
                    errMsg = "number of Layouts(" + opts[i] + ")";
                    catch ILLEGAL_NUM;
                }
            }
            else if(i == 4){ //o
                if(!myStr2Unsigned(opts[i], o)){
                    errMsg = "number of POs(" + opts[i] + ")";
                    catch ILLEGAL_NUM;
                }
            }
            else if(i == 5){ //a
                if(!myStr2Unsigned(opts[i], a)){
                    errMsg = "number of AIGs(" + opts[i] + ")";
                    catch ILLEGAL_NUM;
                }
            }
     
        }
            
            
    }*/
    
        
    
    
    
    
    
    
    try{
        inf.getline(buf,1024);
        string str;
        unsigned headnum;
        str = buf;
        size_t pos = 0;
        if(buf[0] == ' '){
            throw EXTRA_SPACE;
        }
        pos = myStrGetTok(buf, head, pos, ' ');
        if(head != "aag"){
            if(!myStr2Unsigned(head.substr(head.size()-1,1), headnum)){
                errMsg = head;
                throw ILLEGAL_IDENTIFIER;
            }
            else{
                colNo = head.size()-1;
                throw MISSING_SPACE;
            }
        }
        if(buf[pos+1] == ' '){
            colNo = pos+1;
            throw EXTRA_SPACE;
        }
        
        
        colNo = 3;
        pos = 3;
        string m1, i1, l1, o1, a1;
        for(int x = 0; x < 5; x++)
        {
            if(buf[pos+1] == ' ' && pos != string::npos){
                colNo++;
                throw EXTRA_SPACE;
            }
            if(x == 0){
                if(pos == string::npos || buf[pos+1] == '\0'){
                    colNo = pos+1;
                    errMsg = "number of variables";
                    throw MISSING_NUM;
                }
                pos = myStrGetTok(buf, m1, pos, ' ');
                colNo = pos;
                if(!myStr2Unsigned(m1, m)){
                    errMsg = "number of variables(" + m1 + ")";
                    throw ILLEGAL_NUM;
                }
            }
            else if(x == 1){
                if(pos == string::npos || buf[pos+1] == '\0'){
                    errMsg = "number of PIs";
                    throw MISSING_NUM;
                }
                pos = myStrGetTok(buf, i1, pos, ' ');
                colNo = pos;
                if(!myStr2Unsigned(i1, i)){
                    errMsg = "number of PIs(" + i1 + ")";
                    throw ILLEGAL_NUM;
                }
            }
            else if(x == 2){
                if(pos == string::npos || buf[pos+1] == '\0'){
                   errMsg = "number of latches";
                   throw MISSING_NUM;
                }
                pos = myStrGetTok(buf, l1, pos, ' ');
                colNo = pos;
                if(!myStr2Unsigned(l1, l)){
                    errMsg = "number of latches(" + l1 + ")";
                    throw ILLEGAL_NUM;
                }
                if(l % 2 != 0){
                    errMsg = "latches";
                    throw ILLEGAL_NUM;
                }
            }
            else if(x == 3){
                if(pos == string::npos || buf[pos+1] == '\0'){
                    errMsg = "number of POs";
                    throw MISSING_NUM;
                }
                pos = myStrGetTok(buf, o1, pos, ' ');
                colNo = pos;
                if(!myStr2Unsigned(o1, o)){
                    errMsg = "number of POs(" + o1 + ")";
                    throw ILLEGAL_NUM;
                }
            }
            else if(x == 4){
                if(pos == string::npos || buf[pos+1] == '\0'){
                    errMsg = "number of AIGs";
                    throw MISSING_NUM;
                }
                pos = myStrGetTok(buf, a1, pos, ' ');
                colNo = pos;
                if(!myStr2Unsigned(a1, a)){
                    errMsg = "number of AIGs(" + a1 + ")";
                    throw ILLEGAL_NUM;
                }
                if(buf[pos] != '\0'){
                    throw MISSING_NEWLINE;
                }
            }
        }
        if(m < (i+a)){
            errMsg = "Number of variables";
            errInt = m;
            throw NUM_TOO_SMALL;
        }
        //cout << m << i << l << o << a;
        //lineNo++;
        _gateList.resize(m+o+1);
        
        
        //readInput
        colNo= 0;
        for(unsigned x = 0; x < i; x++){
            lineNo++;
            unsigned gateID;
            if(!getline(inf, str)){
                errMsg = "PI";
                throw MISSING_DEF;
            }
            if(str == ""){
                errMsg = "PI";
                throw MISSING_ID;
            }
            if(str[0] == ' ')
                throw EXTRA_SPACE;
            for(unsigned x = 0; x < str.size(); x++){
                if(str[x] == ' '){
                    colNo = x;
                    throw MISSING_NEWLINE;
                }
            }
            if(!myStr2Unsigned(str, gateID)){
                errMsg = "PI literal ID(" + str + ")";
                throw ILLEGAL_NUM;
            }
            else{
                for(unsigned z = 0; z < _piList.size(); z++){
                    if(gateID/2 == _piList[z]){
                        errInt = gateID;
                        errGate = _gateList[gateID/2];
                        throw REDEF_GATE;
                    }
                }
                if(gateID == 0 || gateID == 1){
                    errInt = gateID;
                    throw REDEF_CONST;
                }
                if(gateID % 2 != 0){
                    errMsg = "PI";
                    errInt = gateID;
                    throw CANNOT_INVERTED;
                }
                if(gateID > (m*2)){
                    errInt = gateID;
                    throw MAX_LIT_ID;
                }
                
                readInput(gateID/2, lineNo+1);
            }
        }
        
        //readOutput
        colNo = 0;
        for(unsigned x = 0, b = 1; x < o; x++, b++){
            lineNo++;
            unsigned gateID = m + b;
            unsigned fanin;
            bool inv;
            if(!getline(inf, str)){
                errMsg = "PO";
                throw MISSING_DEF;
            }
            if(str == ""){
                errMsg = "PO";
                throw MISSING_ID;
            }
            if(str[0] == ' ')
                throw EXTRA_SPACE;
            for(unsigned x = 0; x < str.size(); x++){
                if(str[x] == ' '){
                    colNo = x;
                    throw MISSING_NEWLINE;
                }
            }
            if(!myStr2Unsigned(str, fanin)){
                errMsg = "PO literal ID(" + str + ")";
                throw ILLEGAL_NUM;
            }
            else{
                if(fanin > (m*2+1)){
                    errInt = fanin;
                    throw MAX_LIT_ID;
                }
                if(fanin % 2 == 0) inv = false;
                else inv = true;
                readOutput(gateID, lineNo+1, fanin/2, inv);
            }
        }
        
        
        //readAIG
        for(unsigned x = 0; x < a; x++){
            lineNo++;
            colNo = 0;
            unsigned gateID, fanin0, fanin1;
            if(!getline(inf, str)){
                errMsg = "AIG";
                throw MISSING_DEF;
            }
            if(str == ""){
                errMsg = "AIG gate";
                throw MISSING_ID;
            }
            if(str[0] == ' ')
                throw EXTRA_SPACE;
            

            pos = 0;
            string gate_id, f_in0, f_in1;
            //read each aig define
            for(int x = 0; x < 3; x++){
              if(x != 0 && str[pos+1] == ' ' && pos != string::npos){
                  colNo = pos+1;
                  throw EXTRA_SPACE;
              }
              if(x == 0){ //aig gate
                  colNo = pos;
                  
                  pos = myStrGetTok(str, gate_id, pos, ' ');
                  if(!myStr2Unsigned(gate_id, gateID)){
                      errMsg = "AIG gate literal ID(" + gate_id + ")";
                      throw ILLEGAL_NUM;
                  }
                  //cout << gateID;
                  if(gateID == 0 || gateID == 1){
                    errInt = gateID;
                    throw REDEF_CONST;
                  }
                  if(gateID % 2 != 0){
                      errMsg = "AIG";
                      errInt = gateID;
                      throw CANNOT_INVERTED;
                  }
                  if(gateID > (m*2)){
                    errInt = gateID;
                    throw MAX_LIT_ID;
                  }
                  
                  for(unsigned z = 0; z < _piList.size(); z++){
                      if(gateID/2 == _piList[z]){
                          errInt = gateID;
                          errGate = _gateList[gateID/2];
                          throw REDEF_GATE;
                      }
                  }
                  for(unsigned z = 0; z < _aigList.size(); z++){
                      if(gateID/2 == _aigList[z]){
                          errInt = gateID;
                          errGate = _gateList[gateID/2];
                          throw REDEF_GATE;
                      }
                  }
              }
              else if(x == 1){ //fanin0
                  
                  if(pos == string::npos){
                      colNo = colNo+gate_id.size();
                      throw MISSING_SPACE;
                  }
                  colNo = pos;
                  if(str[pos+1] == '\0'){
                      colNo = pos+1;
                      errMsg = "AIG input";
                      throw MISSING_ID;
                  }
                  pos = myStrGetTok(str, f_in0, pos, ' ');
                  if(!myStr2Unsigned(f_in0, fanin0)){
                      errMsg = "AIG input literal ID(" + f_in0 + ")";
                      throw ILLEGAL_NUM;
                  }
                  if(fanin0 > (m*2+1)){
                    errInt = fanin0;
                    throw MAX_LIT_ID;
                  }
              }
              else if(x == 2){ //fanin1
                  
                  if(pos == string::npos){
                      colNo = colNo+f_in0.size()+1;
                      throw MISSING_SPACE;
                  }
                  colNo = pos;
                  if(str[pos+1] == '\0'){
                      colNo = pos+1;
                      errMsg = "AIG input";
                      throw MISSING_ID;
                  }
                  pos = myStrGetTok(str, f_in1, pos, ' ');
                  if(pos == string::npos){ //end correctly
                      if(!myStr2Unsigned(f_in1, fanin1)){
                          errMsg = "AIG input literal ID(" + f_in1 + ")";
                          throw ILLEGAL_NUM;
                      }
                      if(fanin1 > (m*2+1)){
                        errInt = fanin1;
                        throw MAX_LIT_ID;
                      }
                  }
                  else{
                      colNo = pos;
                      throw MISSING_NEWLINE;
                  }
                  
                  
              }
            } //end for reading AIG define
            //cout << gateID << fanin0 << fanin1;
            
            bool inv0, inv1;
            if(fanin0 % 2 == 0) inv0 = false;
            else inv0 = true;
            if(fanin1 % 2 == 0) inv1 = false;
            else inv1 = true;

            readAig(gateID/2, lineNo+1, fanin0/2, fanin1/2, inv0, inv1);
            
        }
        
        /*
        //readInput
        for(unsigned x = 0; x < i; x++){
            unsigned gateID;
            inf >> gateID;
            lineNo++;
            readInput(gateID/2, lineNo);
        }
        
        //readOutput
        for(unsigned x = 0, b = 1; x < o; x++, b++){
            unsigned gateID = m + b;
            unsigned fanin;
            bool inv;

            inf >> fanin;
            lineNo++;

            if(fanin % 2 == 0) inv = false;
            else inv = true;
            readOutput(gateID, lineNo, fanin/2, inv);

        }
         
        //readAig
        for(unsigned x = 0; x < a; x++){
            unsigned gateID, fanin0, fanin1;
            inf >> gateID >> fanin0 >> fanin1;
            lineNo++;

            bool inv0, inv1;
            if(fanin0 % 2 == 0) inv0 = false;
            else inv0 = true;
            if(fanin1 % 2 == 0) inv1 = false;
            else inv1 = true;

            readAig(gateID/2, lineNo+1, fanin0/2, fanin1/2, inv0, inv1);
        }
        */
         
        //read symbol
        int num;
        //inf.ignore();
        string symbol;
        char *ptr;

        while(getline(inf, str)){
            lineNo++;
            colNo = 0;
            char first = str[0];
            if(first == ' '){
                throw EXTRA_SPACE;
            }
            if(first == 'i'){
                //symbol = str.substr(1, 1);
                //symbol = str.substr(1, str.find(" "));
                size_t pos = myStrGetTok(str, symbol, 1, ' ');
                if(myStr2Int(symbol, num)){
                    size_t pos1 = myStrGetTok(str, symbol, pos, ' ');
                    if(_gateList[_piList[num]]->_symbol != NULL)
                    {
                        errMsg = i;
                        errInt = num;
                        throw REDEF_SYMBOLIC_NAME;
                    }
                    _gateList[_piList[num]]->_symbol = new string(symbol);
                }
            }
            else if(first == 'o'){
                size_t pos = myStrGetTok(str, symbol, 1, ' ');
                if(myStr2Int(symbol, num)){
                    size_t pos1 = myStrGetTok(str, symbol, pos, ' ');
                    if(_gateList[_poList[num]]->_symbol != NULL)
                    {
                        errMsg = o;
                        errInt = num;
                        throw REDEF_SYMBOLIC_NAME;
                    }
                    _gateList[_poList[num]]->_symbol = new string(symbol);
                }
            }
            else if(first == 'c'){
                while(getline(inf, str)){
                    _comments.push_back(str);
                }
                break;
                }
        }
        inf.ignore();
        //inf.close();
        
     
        connect();
        genDFSList(_poList);
        
    }
    catch(CirParseError err)
    {
        parseError(err);
        lineNo = 0;
        colNo = 0;
        errInt = 0;
        errMsg = "";
        return false;
    }
    




    /*for(unsigned i = 0; i < dfsAig.size(); i++){
    cout << dfsAig[i]->gateID << " " << dfsAig[i]->_fanin0 << " " << dfsAig[i]->_invPhase0 << endl;
    cout << dfsAig[i]->gateID << " " << dfsAig[i]->_fanin1 << " " << dfsAig[i]->_invPhase1 << endl;
    }*/

    /*for(unsigned x = 0; x < _gateList.size(); x++){
    if((_gateList[x] != NULL) && (_gateList[x]->_symbol != NULL))
    cout << *(_gateList[x]->_symbol) << endl;
    //_gateList[x]->getFin0()->printGate();
    //_gateList[x]->getFin1()->printGate();
    //cout << _gateList[i]->getFanin0() << " " << _gateList[i]->getFanin1() << endl;

    }
    for (unsigned i = 0; i < _comments.size(); i++)
    cout << _comments[i] << endl;*/
    
 /*   inf >> head;
    inf.getline(buf,1024);
    string str;
    str = buf;
    //vector<string*> opts;
    try{
    size_t pos = 0;
    
    string m1, i1, l1, o1, a1;
    for(int x = 0; x < 5; x++)
    {
        if(buf[pos+1] == ' ' && pos != string::npos)
            throw EXTRA_SPACE;
        if(x == 0){
            if(pos == string::npos || buf[pos+1] == '\0'){
                errMsg = "number of variables";
                throw MISSING_NUM;
            }
            pos = myStrGetTok(buf, m1, pos, ' ');
            if(!myStr2Unsigned(m1, m)){
                errMsg = "number of variables(" + m1 + ")";
                throw ILLEGAL_NUM;
            }
        }
        else if(x == 1){
            if(pos == string::npos || buf[pos+1] == '\0'){
                errMsg = "number of PIs";
                throw MISSING_NUM;
            }
            pos = myStrGetTok(buf, i1, pos, ' ');
            if(!myStr2Unsigned(i1, i)){
                errMsg = "number of PIs(" + i1 + ")";
                throw ILLEGAL_NUM;
            }
        }
        else if(x == 2){
            if(pos == string::npos || buf[pos+1] == '\0'){
               errMsg = "number of latches";
               throw MISSING_NUM;
            }
            pos = myStrGetTok(buf, l1, pos, ' ');
            if(!myStr2Unsigned(l1, l)){
                errMsg = "number of latches(" + l1 + ")";
                throw ILLEGAL_NUM;
            }
        }
        else if(x == 3){
            if(pos == string::npos || buf[pos+1] == '\0'){
                errMsg = "number of POs";
                throw MISSING_NUM;
            }
            pos = myStrGetTok(buf, o1, pos, ' ');
            if(!myStr2Unsigned(o1, o)){
                errMsg = "number of POs(" + o1 + ")";
                throw ILLEGAL_NUM;
            }
        }
        else if(x == 4){
            if(pos == string::npos || buf[pos+1] == '\0'){
                errMsg = "number of AIGs";
                throw MISSING_NUM;
            }
            pos = myStrGetTok(buf, a1, pos, ' ');
            if(!myStr2Unsigned(a1, a)){
                errMsg = "number of AIGs(" + a1 + ")";
                throw ILLEGAL_NUM;
            }
        }
    }
        
        for(int x = 0; x < i; x++){
            if(!getline(inf, str)){
                errMsg = "PI";
                throw MISSING_DEF;
            }
        }
    }
    catch(CirParseError err)
    {
        parseError(err);
        lineNo = 0;
        colNo = 0;
        errInt = 0;
        errMsg = "";
        return false;
    }
    
*/
    

    return true;

}

void CirMgr::readHeader(){

}

void CirMgr::readInput(unsigned gateID, unsigned lineNo){
    CirGate* pi = new CirPiGate(gateID, lineNo);
    _piList.push_back(gateID);
    _gateList[gateID] = pi;
}

void
CirMgr::readOutput(unsigned gateID, unsigned lineNo, unsigned fanin, unsigned inv){
    CirGate* po = new CirPoGate(gateID, lineNo, fanin, inv);
    _poList.push_back(gateID);
    //_gateList[gateID] = po;
    _gateList[gateID] = po;
}

void
CirMgr::readAig(unsigned gateID, unsigned lineNo, unsigned fanin0, unsigned fanin1, unsigned inv0, unsigned inv1){
    CirGate* aig = new CirAigGate(gateID, lineNo, fanin0, fanin1, inv0, inv1);
    _aigList.push_back(gateID);
    //_gateList[gateID] = aig;
    _gateList[gateID] = aig;
}

void
CirMgr::connect(){
    for(unsigned x = 0; x < _poList.size(); x++){
        unsigned y = _gateList[_poList[x]]->getFanin0();
        if(_gateList[y] == NULL){ //undefined
            CirGate *u = new UndefGate(y);
            _gateList[y] = u;
            _gateList[_poList[x]]->addFin0(u);
        }
        else if(_gateList[y] != NULL){
            CirGate *g = _gateList[y];
            _gateList[_poList[x]]->addFin0(g);
        }
    }
    for(unsigned x = 0; x < _aigList.size(); x++){
        unsigned in0 = _gateList[_aigList[x]]->getFanin0();
        unsigned in1 = _gateList[_aigList[x]]->getFanin1();
        if(_gateList[in0] == NULL){ //undefined
            CirGate *u = new UndefGate(in0);
            _gateList[in0] = u;
            _gateList[_aigList[x]]->addFin0(u);
        }
        else if(_gateList[in0] != NULL){
            CirGate *g = _gateList[in0];
            _gateList[_aigList[x]]->addFin0(g);
        }
        if(_gateList[in1] == NULL){ //undefined
            CirGate *u = new UndefGate(in1);
            _gateList[in1] = u;
            _gateList[_aigList[x]]->addFin1(u);
        }
        else if(_gateList[in1] != NULL){
            CirGate *g = _gateList[in1];
            _gateList[_aigList[x]]->addFin1(g);
        }
    }
    
    
    /*
    for(unsigned x = 0; x < _gateList.size(); x++){
        if((_gateList[x] != NULL) && (_gateList[x]->getTypeStr() == "PO")){  //output
            for(unsigned y = 0; y < _gateList.size(); y++){
                if(_gateList[x]->getFanin0() == y){
                    if(_gateList[y] == NULL){ //undefined
                        CirGate *u = new UndefGate(y);
                        _gateList[y] = u;
                        _gateList[x]->addFin0(u);
                    }
                    else if(_gateList[y] != NULL){
                        CirGate *g = _gateList[y];
                        _gateList[x]->addFin0(g);
                    }
                }
            }
        }
        else if((_gateList[x] != NULL) && (_gateList[x]->getTypeStr() == "AIG")){  //aig
            for(unsigned y = 0; y < _gateList.size(); y++){
                if(_gateList[x]->getFanin0() == y){
                    if(_gateList[y] == NULL){ //undefined
                        CirGate *u = new UndefGate(y);
                        _gateList[y] = u;
                        _gateList[x]->addFin0(u);
                    }
                    else{
                        CirGate *g = _gateList[y];
                        _gateList[x]->addFin0(g);
                    }
                }
                if(_gateList[x]->getFanin1() == y){
                    if(_gateList[y] == NULL){ //undefined
                        CirGate *u = new UndefGate(y);
                        _gateList[y] = u;
                        _gateList[x]->addFin1(u);
                    }
                    else{
                        CirGate *g = _gateList[y];
                        _gateList[x]->addFin1(g);
                    }
                }
            }
        }
    }*/
}

void CirMgr::genDFSList(const IdList& sinkList){
    CirGate::setGlobalRef();
    for(const unsigned &i : sinkList)
        _gateList[i]->dfsTraversal(_gateList[i], _dfsList);
    
    /*Node::setGlobalRef();
    for_each_sink(node, sinkList)
       node->dfsTraversal(_dfsList);*/

}

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
    cout << endl << "Circuit Statistics" << endl;
    cout << "==================" << endl;
    cout << "  PI" << setw(12) << right << i << endl;
    cout << "  PO" << setw(12) << right << o << endl;
    cout << "  AIG" << setw(11) << right << a << endl;
    cout << "------------------" << endl;
    cout << "  Total" << setw(9) << right << (i+o+a) << endl;
}

void
CirMgr::printNetlist() const
{
    unsigned no = 0;
    cout << endl;
    for(unsigned i = 0; i < _dfsList.size(); i++){
        if(_dfsList[i]->getTypeStr() == "UNDEF"){
            continue; //skip undefined
        }
        cout << "[" << no << "] ";
        _dfsList[i]->printGate();
        
        cout << endl;
        no++;
    }
}

void
CirMgr::printPIs() const
{
    cout << "PIs of the circuit:";
    for(unsigned i = 0; i < _piList.size(); i++){
        cout << " " << _gateList[_piList[i]]->getID();
    }
    cout << endl;
}

void
CirMgr::printPOs() const
{
    cout << "POs of the circuit:";
    for(unsigned i = 0; i < _poList.size(); i++){
        cout << " " << _gateList[_poList[i]]->getID();
    }
    cout << endl;
}

void
CirMgr::printFloatGates() const
{
    IdList floating;
    IdList notused;
    
    for(unsigned i = 0; i < _gateList.size(); i++){
        if (_gateList[i] == NULL)
            continue;
        if ((_gateList[i]->getTypeStr() == "CONST"))
            continue;
        //gate that has no fanout
        if((_gateList[i]->getTypeStr() != "PO") && (_gateList[i]->_outList.empty()))
            notused.push_back(i);
        
        //gates with one or multiple undefined fanins (at least one)
        for(unsigned j = 0; j < _gateList[i]->_inList0.size(); j++){
            if(_gateList[i]->_inList0[j]->getTypeStr() == "UNDEF"){
                floating.push_back(i);
                break;
            }
        }
        if(_gateList[i]->getTypeStr() == "PO")
            continue;
        if((!floating.empty()) && (floating.back() == i))
            continue;
        for(unsigned k = 0; k < _gateList[i]->_inList1.size(); k++){
            if(_gateList[i]->_inList1[k]->getTypeStr() == "UNDEF"){
                floating.push_back(i);
                break;
            }
        }
    }

    
    if(!floating.empty()){
        cout << "Gates with floating fanin(s):";
        for(unsigned i = 0; i < floating.size(); i++){
            cout << " " << floating[i];
        }
        cout << endl;
    }
    if(!notused.empty()){
        cout << "Gates defined but not used  :";
        for(unsigned i = 0; i < notused.size(); i++){
            cout << " " << notused[i];
        }
        cout << endl;
    }
}

void
CirMgr::writeAag(ostream& outfile) const
{
    outfile << "aag " << m << " " << i << " " << l << " " << o << " " << a << endl;
    for(unsigned i = 0; i < _piList.size(); i++){  //cout PI
        outfile << _piList[i]*2 << endl;
    }
    for(unsigned i = 0; i < _poList.size(); i++){  //cout PO
        unsigned literal;
        if(_gateList[_poList[i]]->_invPhase0)
            literal = (_gateList[_poList[i]]->_fanin0)*2+1;
        else literal = (_gateList[_poList[i]]->_fanin0)*2;
        
        outfile << literal << endl;
    }
    
    vector<CirGate*> dfsAig;
    for(unsigned i = 0; i < _dfsList.size(); i++){
        if(_dfsList[i]->getTypeStr() == "AIG")
            dfsAig.push_back(_dfsList[i]);
    }
    for(unsigned i = 0; i < dfsAig.size(); i++){ //cout dfsAIG
        
        unsigned id0, id1;
        
        if(dfsAig[i]->_invPhase0)
            id0 = (dfsAig[i]->_fanin0)*2+1;
        else id0 = (dfsAig[i]->_fanin0)*2;
        
        if(dfsAig[i]->_invPhase1)
            id1 = (dfsAig[i]->_fanin1)*2+1;
        else id1 = (dfsAig[i]->_fanin1)*2;
        
        outfile << dfsAig[i]->gateID*2 << " " << id0 << " " << id1 << endl;
    }
    
    for(unsigned i = 0; i < _piList.size(); i++)
    {
        if (_gateList[_piList[i]]->_symbol != NULL)
            outfile << "i" << i << " " << *(_gateList[_piList[i]]->_symbol) << endl;
    }
    for(unsigned i = 0; i < _poList.size(); i++)
    {
        if (_gateList[_poList[i]]->_symbol != NULL)
            outfile << "o" << i << " " << *(_gateList[_poList[i]]->_symbol) << endl;
    }
    outfile << "c" << endl << "AAG output by Iristrong" << endl;
    /*for (unsigned i = 0; i < _comments.size(); i++)
        outfile << _comments[i] << endl;*/
    
}

bool
CirMgr::myStr2Unsigned(const string& str, unsigned& num)
{
   num = 0;
   size_t i = 0;
   int sign = 1;
   if (str[0] == '-') {
       sign = -1;
       i = 1;
       return false;
   }
   bool valid = false;
   for (; i < str.size(); ++i) {
      if (isdigit(str[i])) {
         num *= 10;
         num += int(str[i] - '0');
         valid = true;
      }
      else return false;
   }
   num *= sign;
   return valid;
}
