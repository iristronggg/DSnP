/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <vector>
#include <bitset>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;
unsigned CirGate::cur_mark = 0;
unsigned CirGate::cur_print_idx = 0;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
AigGate::reportGate() const {
    cout << "================================================================================" << endl;
    string info = "= AIG(" + to_string(_id) + "), line " + to_string(_line);
    cout << info << endl;
    cout << "= FECs:";
    //cout << CirMgr::_FECgroup[_grpN][i].getG()->_id;
    //cout << _id << _grpN;
    if(_grpN != -1)
        cirMgr->reportFEC(_id, _grpN);
    cout << endl;
    cout << "= Value: ";
    bitset<64> b(_simVal);
    string val = b.to_string();
    //cout << val << endl;
    for(size_t i = 0; i < 64; i++){
        if(i!=0 && (i % 8 == 0))
            cout << "_";
        cout << val[i];
        
    }
    cout << endl;
    cout << "================================================================================" << endl;
}
void
PiGate::reportGate() const {
    cout << "================================================================================" << endl;
    string info = "= PI(" + to_string(_id) + ")" + (_symbol == "" ? "" : "\"" + _symbol + "\"") + ", line " + to_string(_line);
    cout << info << endl << "= FECs:" << endl;
    cout << "= Value: ";
    bitset<64> b(_simVal);
    string val = b.to_string();
    for(size_t i = 0; i < 64; i++){
        if(i!=0 && (i % 8 == 0))
            cout << "_";
        cout << val[i];
    }
    cout << endl;
    cout << "================================================================================" << endl;
}
void
PoGate::reportGate() const {
    cout << "================================================================================" << endl;
    string info = "= PO(" + to_string(_id) + ")" + (_symbol == "" ? "" : "\"" + _symbol + "\"") + ", line " + to_string(_line);
    cout << info << endl << "= FECs:" << endl;
    cout << "= Value: ";
    bitset<64> b(_simVal);
    string val = b.to_string();
    for(size_t i = 0; i < 64; i++){
        if(i!=0 && (i % 8 == 0))
            cout << "_";
        cout << val[i];
    }
    cout << endl;
    cout << "================================================================================" << endl;
}
void
ConstGate::reportGate() const  {
    cout << "================================================================================" << endl
    << "= " << "CONST(0), line 0" << endl;
    cout << "= FECs:";
    //cout << CirMgr::_FECgroup[_grpN][i].getG()->_id;
    //cout << _id << _grpN;
    if(_grpN != -1)
        cirMgr->reportFEC(_id, _grpN);
    cout << endl;
    cout << "= Value: ";
    bitset<64> b(_simVal);
    string val = b.to_string();
    //cout << val << endl;
    for(size_t i = 0; i < 64; i++){
        if(i!=0 && (i % 8 == 0))
            cout << "_";
        cout << val[i];
        
    }
    cout << endl;
    cout << "================================================================================" << endl;
}
void
UndefGate::reportGate() const  {
    cout << "================================================================================" << endl
         << "= " << "UNDEF(" + to_string(_id) + "), line 0" << endl
         << "================================================================================" << endl;
}




//remove me from my fanin's _fanout list
void
CirGate::rmFromFanin(){
    if(_fanin[0] != NULL){
        for(size_t i = 0 ; i < _fanin[0]->_fanout.size(); i++){
            if(_fanin[0]->_fanout[i] == this)
                _fanin[0]->_fanout.erase(_fanin[0]->_fanout.begin()+i);
        }
    }
    if(_fanin[1] != NULL){
        for(size_t i = 0 ; i < _fanin[1]->_fanout.size(); i++){
            if(_fanin[1]->_fanout[i] == this)
                _fanin[1]->_fanout.erase(_fanin[1]->_fanout.begin()+i);
        }
    }
}

//remove me from my fanout's _fanin[0]/[1]
void
CirGate::rmFromFanout(){
    for(size_t i = 0; i < _fanout.size(); i++){
        for(size_t k = 0; k < 2; k++){
            if(_fanout[i]->_fanin[k] == this){
                _fanout[i]->_fanin[k] = NULL;
                _fanout[i]->_fanin_idx[k] = 0;
                _fanout[i]->_fanin_invert[k] = false; //初始化
            }
        }
    }
    
}


void
CirGate::replaceFanin(int x){
    cout << "Simplifying: " << _fanin[x]->_id << " merging ";
    if(_fanin_invert[x])
        cout << "!";
    cout << this->_id << "..." << endl;
    this->rmFromFanin(); //remove this from my fanin's _fanout list
    
    CirGate* g = _fanin[x]; //replace this with _fanin[x];
    for(size_t i = 0; i < _fanout.size(); i++){ //connect this's parent & children
        for(size_t k = 0; k < 2; k++){
            if(_fanin[k] == NULL) //parent is PO
                continue;
            if(_fanout[i]->_fanin[k] == this){
                _fanout[i]->_fanin[k] = g;
                _fanout[i]->_fanin_idx[k] = g->_id;
                _fanout[i]->_fanin_invert[k] = (_fanin_invert[x] ^ _fanout[i]->_fanin_invert[k]); //XOR
                g->_fanout.push_back(_fanout[i]);
            }
        }
    }
    
}

void
CirGate::replaceConst(CirGate* &g){  //g = CONST0
    cout << "Simplifying: 0 merging " << this->_id << "..." << endl;
    this->rmFromFanin();
    
    for(size_t i = 0; i < _fanout.size(); i++){ //connect this's parent & children
        for(size_t k = 0; k < 2; k++){
            if(_fanin[k] == NULL) //parent is PO
                continue;
            if(_fanout[i]->_fanin[k] == this){
                _fanout[i]->_fanin[k] = g;
                _fanout[i]->_fanin_idx[k] = g->_id;
                //_fanout[i]->_fanin_invert[k] = (_fanin_invert[x] ^ _fanout[i]->_fanin_invert[k]);//no change
                g->_fanout.push_back(_fanout[i]);
            }
        }
    }
    
}

void
CirGate::mergeGate(CirGate* g){
    this->rmFromFanin();
    
    for(size_t i = 0; i < _fanout.size(); i++){ //find my _fanout, connect to g
        for(size_t k = 0; k < 2; k++){
            if(_fanin[k] == NULL) //_fanout is PO
                continue;
            if(_fanout[i]->_fanin[k] == this){
                _fanout[i]->_fanin[k] = g;
                _fanout[i]->_fanin_idx[k] = g->_id;
                //_fanout[i]->_fanin_invert[k] = (_fanin_invert[x] ^ _fanout[i]->_fanin_invert[k]);//no change
                g->_fanout.push_back(_fanout[i]);
            }
        }
    }
}

void
CirGate::setSim(){
    if(this->getTypeStr() == "AIG"){
        size_t f0 = _fanin[0]->_simVal;
        size_t f1 = _fanin[1]->_simVal;
        if(_fanin_invert[0])
            f0 = ~f0;
        if(_fanin_invert[1])
            f1 = ~f1;
        _simVal = (f0 & f1);
    }
    else if(this->getTypeStr() == "PO"){
        size_t f0 = _fanin[0]->_simVal;
        if(_fanin_invert[0])
            f0 = ~f0;
        _simVal = f0;
    }
}

void
CirGate::setGroupNum(int n){
    _grpN = n;
}





/**************************************/
/*   class AigGate member functions   */
/**************************************/
void
AigGate::genDFS(GateList& _dfsList) {
    //fanins
    for(size_t i = 0; i < 2; i++){
        if(_fanin[i] != NULL && _fanin[i]->marked() == false){
            if(_fanin[i]->getTypeStr() == "AIG"){
                _fanin[i]->genDFS(_dfsList);
            }
            _dfsList.push_back(_fanin[i]);
            
            _fanin[i]->mark_node();
        }
    }
    //self
    //CirGate* g = this;
    //_dfs.push_back(g);
}

void
AigGate::printGate() const {
    //fanins
    for(size_t i = 0; i < 2; i++){
        if(_fanin[i] != NULL && _fanin[i]->marked() == false){
            _fanin[i]->printGate();
            _fanin[i]->mark_node();
        }
    }
    //self
    cout << "[" << cur_print_idx++ << "] " << "AIG " << _id << " ";
    if(_fanin[0] != NULL)
        cout << (_fanin[0]->getTypeStr() == "UNDEF"? "*" : "") << (_fanin_invert[0]? "!" : "") << _fanin_idx[0] << " ";
    if(_fanin[1] != NULL)
        cout << (_fanin[1]->getTypeStr() == "UNDEF"? "*" : "") << (_fanin_invert[1]? "!" : "") << _fanin_idx[1] << endl;
}
void
AigGate::printFanin(bool is_neg, int self_level, int rest_level) const {
    if(rest_level < 0)
        return;
  
    for(int i = 0; i < self_level; i++)
        cout << "  ";
    cout << (is_neg? "!" : "") << "AIG " << _id << ((marked() && rest_level > 0)? " (*)" : "") << endl;

    if(!marked() && rest_level > 0)
        for(int i = 0; i < 2; i++){
            if(_fanin[i] == NULL)
                continue;
            if(_fanin[i]->getTypeStr() == "UNDEF"){
                for(int i = 0; i < self_level + 1; i++)
                    cout << "  ";
                cout << (_fanin_invert[i]? "!" : "") << "UNDEF " << _fanin_idx[i] << endl;
            }
            else{
                _fanin[i]->printFanin(_fanin_invert[i], self_level + 1, rest_level - 1);
                if(rest_level > 1)
                    _fanin[i]->mark_node();
            }
        }
}
void
AigGate::printFanout(unsigned previous_id, int self_level, int rest_level) const {
    if(rest_level < 0)
        return;
        
    //print self
    for(int i = 0; i < self_level; i++)
        cout << "  ";
    bool is_invert = false;
    if((int)previous_id != -1)
        for(int i = 0; i < 2; i++)
            if(_fanin_idx[i] == previous_id)
                is_invert = _fanin_invert[i];
    cout << (is_invert? "!" : "") << "AIG " << _id << (marked() && rest_level > 0? " (*)" : "") << endl;
    
    //print fanouts
    if(rest_level > 0 && !marked())
        for(size_t i = 0; i < _fanout.size(); i++){
            _fanout[i]->printFanout(_id, self_level + 1, rest_level - 1);
            if(rest_level > 1)
                _fanout[i]->mark_node();
        }
}
int
AigGate::aig_under() const {
    if(marked())
        return 0;
    int sum = 1;
    for(int i = 0; i < 2; i++)
        if(_fanin[i] != NULL){
            sum += _fanin[i]->aig_under();
            _fanin[i]->mark_node();
        }
    return sum;
}
void
AigGate::print_aig_under(ostream& outfile) const {
    if(marked())
        return;

    for(int i = 0; i < 2; i++)
        if(_fanin[i] != NULL){
            _fanin[i]->print_aig_under(outfile);
            _fanin[i]->mark_node();
        }
    outfile << 2 * _id << " " << 2 * _fanin_idx[0] + (_fanin_invert[0]? 1 : 0) << " "
            << 2 * _fanin_idx[1] + (_fanin_invert[1]? 1 : 0) << endl;
}

/**************************************/
/*   class PiGate member functions   */
/**************************************/
void
PiGate::printGate() const {
    cout << "[" << cur_print_idx++ << "] " << "PI  " << _id;
    if(_symbol != "")
        cout << " (" << _symbol << ")";
    cout << endl;
}
void
PiGate::printFanin(bool is_neg, int self_level, int rest_level) const{
    for(int i = 0; i < self_level; i++)
        cout << "  ";
    cout << (is_neg? "!" : "") << "PI " << _id << endl;
}
void
PiGate::printFanout(unsigned previous_id, int self_level, int rest_level) const {
    if(rest_level < 0)
        return;
    cout << "PI " << _id << endl;
    if(rest_level > 0){
        for(size_t i = 0; i < _fanout.size(); i++){
            _fanout[i]->printFanout(_id, 1, rest_level - 1);
            if(rest_level > 1)
                _fanout[i]->mark_node();
        }
    }
}

/**************************************/
/*   class PoGate member functions   */
/**************************************/
void
PoGate::genDFS(GateList& _dfsList) {
    if(_fanin[0] != NULL && _fanin[0]->marked() == false){
        if(_fanin[0]->getTypeStr() == "AIG"){
            _fanin[0]->genDFS(_dfsList);
            _dfsList.push_back(_fanin[0]);
        }
        else{
            _dfsList.push_back(_fanin[0]);
        }
        _fanin[0]->mark_node();
    }
}

void
PoGate::printGate() const {
    //fanin
    if(_fanin[0] != NULL && _fanin[0]->marked() == false){
        _fanin[0]->printGate();
        _fanin[0]->mark_node();
    }
     
    //self
    cout << "[" << cur_print_idx++ << "] " << "PO  " << _id << " " << (_fanin[0]->getTypeStr() == "UNDEF"? "*" : "") << (_fanin_invert[0]? "!" : "") << (_fanin_idx[0]);
    if(_symbol != "")
        cout << " (" << _symbol << ")";
    cout << endl;
}
void
PoGate::printFanin(bool is_neg, int self_level, int rest_level) const{
    cout << "PO " << _id << endl;
    if(rest_level > 0 && _fanin[0]!=NULL){
        if(_fanin[0]->getTypeStr() == "UNDEF")
            cout << (_fanin_invert[0]? "!" : "") << "  UNDEF " << _fanin_idx[0] << endl;
        else
            _fanin[0]->printFanin(_fanin_invert[0], self_level + 1, rest_level - 1);
        if(rest_level > 1)
            _fanin[0]->mark_node();
    }
}
void
PoGate::printFanout(unsigned previous_id, int self_level, int rest_level) const {
    for(int i = 0; i < self_level; i++)
        cout << "  ";
    cout << (_fanin_invert[0] && (int)previous_id != -1 ? "!" : "") << "PO " << _id << endl;
}
int
PoGate::aig_under() const {
    if(_fanin[0] == NULL || _fanin[0]->marked())
        return 0;
    int sum = _fanin[0]->aig_under();
    _fanin[0]->mark_node();
    return sum;
}
void
PoGate::print_aig_under(ostream& outfile) const {
    if(_fanin[0] != NULL){
        _fanin[0]->print_aig_under(outfile);
        _fanin[0]->mark_node();
    }
}

/**************************************/
/*   class ConstGate member functions   */
/**************************************/
void
ConstGate::printGate() const {
    cout << "[" << cur_print_idx++ << "] " << "CONST0" << endl;
}
void
ConstGate::printFanin(bool is_neg, int self_level, int rest_level) const{
    for(int i = 0; i < self_level; i++)
        cout << "  ";
    cout << (is_neg? "!" : "") << "CONST " << _id << endl;
}
void
ConstGate::printFanout(unsigned previous_id, int self_level, int rest_level) const {
    if(rest_level < 0)
        return;
    cout << "CONST 0" << endl;
    if(rest_level > 0){
        for(size_t i = 0; i < _fanout.size(); i++){
            _fanout[i]->printFanout(0, 1, rest_level - 1);
            if(rest_level > 1)
                _fanout[i]->mark_node();
        }
    }
}

/**************************************/
/*   class UndefGate member functions   */
/**************************************/

