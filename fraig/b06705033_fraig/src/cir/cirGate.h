/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;
class AigGate;
class PiGate;
class PoGate;
class ConstGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
friend class AigGate;
friend class PiGate;
friend class PoGate;
friend class ConstGate;
friend class UndefGate;
friend class CirMgr;
friend class GateNode;
friend class FECnode;
public:
    static unsigned cur_mark;
    static unsigned cur_print_idx;
    CirGate(): _id(0), _mark(0) {
        for(int i = 0; i < 2; i++){
         _fanin_idx[i] = 0;
         _fanin_invert[i] = false;
         _fanin[i] = NULL;
        }
    }
    CirGate(unsigned id) : _id(id), _mark(0) {
       for(int i = 0; i < 2; i++){
           _fanin_idx[i] = 0;
           _fanin_invert[i] = false;
           _fanin[i] = NULL;
       }
    }
    virtual ~CirGate() {}

    // Basic access methods
    /*string getTypeStr() const { return ""; }
    unsigned getLineNo() const { return 0; }*/
    virtual bool isAig() const { if(this->getTypeStr() == "AIG")
                                    return true;
                                return false; }
    virtual string getTypeStr() const = 0;
    virtual unsigned getLineNo() const { return 0; }
    void mark_node() { _mark = cur_mark; }
    bool marked() const { return (_mark == cur_mark); }
    /*bool operator == (const CirGate* &g) const {
        if((this->getTypeStr() == g->getTypeStr()) && (this->_fanin[0]->_id == g->_fanin[0]->_id) && (this->_fanin[1]->_id == g->_fanin[1]->_id) && (this->_fanin_invert[0] == g->_fanin_invert[0]) && (this->_fanin_invert[1] == g->_fanin_invert[1]))
            return true;
        return false; }*/

    // Printing functions
    virtual void printGate() const = 0;
    virtual void genDFS(GateList& _dfsList) = 0;
    virtual void reportGate() const = 0;
    virtual void reportFanin(int level) const = 0;
    virtual void reportFanout(int level) const = 0;
    void rmFromFanin();
    void rmFromFanout();
    void replaceFanin(int x); //replace with _fanin[x], for opt
    void replaceConst(CirGate* &g); //replace with CONST0, for opt
    void mergeGate(CirGate* g); //replace with g, for str
    void setSim(); //to set _simVal
    size_t getSim(){ return _simVal; }
    void setGroupNum(int n);
    //int getGroupNum(){ return _grpN; }
   
private:

protected:
    unsigned  _id;
    unsigned  _mark;
    unsigned  _fanin_idx[2]; //from start
    bool      _fanin_invert[2]; //from start
    size_t    _simVal = 0;
    int  _grpN = -1;
    CirGate*  _fanin[2]; //connect
    GateList  _fanout; //connect
    virtual void printFanin(bool is_neg, int self_level, int rest_level) const = 0;
    virtual void printFanout(unsigned previous_id, int self_level, int rest_level) const = 0;
    virtual int aig_under() const = 0;
    virtual void print_aig_under(ostream& outfile) const = 0;
};

class AigGate : public CirGate
{
friend class PiGate;
friend class PoGate;
friend class ConstGate;
friend class UndefGate;
friend class CirMgr;
public:
    AigGate() : CirGate(), _line(0) {}
    AigGate(unsigned id, unsigned line) : CirGate(id), _line(line) {}
    ~AigGate() {}
    string getTypeStr() const { return "AIG"; }
    unsigned getLineNo() const { return _line; }
    void printGate() const;
    void reportGate() const;
    void reportFanin(int level) const { CirGate::cur_mark++; printFanin(false, 0, level); }
    void reportFanout(int level) const { CirGate::cur_mark++; printFanout(-1, 0, level); }
    int aig_under() const;
    void print_aig_under(ostream& outfile) const;
    void genDFS(GateList& _dfsList);
    
private:
    unsigned _line;
    void printFanin(bool is_neg, int self_level, int rest_level) const;
    void printFanout(unsigned previous_id, int self_level, int rest_level) const;
};

class PiGate : public CirGate
{
friend class AigGate;
friend class PoGate;
friend class ConstGate;
friend class UndefGate;
friend class CirMgr;
public:
    PiGate() : CirGate(), _line(0) {}
    PiGate(unsigned id, unsigned line) : CirGate(id), _line(line) {}
    ~PiGate() {}
    string getTypeStr() const { return "PI"; }
    unsigned getLineNo() const { return _line; }
    void printGate() const;
    //void genDFS() const;
    void reportGate() const;
    void reportFanin(int level) const{ CirGate::cur_mark++; printFanin(false, 0, level); }
    void reportFanout(int level) const { CirGate::cur_mark++; printFanout(-1, 0, level); }
    int aig_under() const { return 0; }
    void print_aig_under(ostream& outfile) const { return; }
    void genDFS(GateList& _dfsList){ return; }
private:
    unsigned _line;
    string   _symbol;
    void printFanin(bool is_neg, int self_level, int rest_level) const;
    void printFanout(unsigned previous_id, int self_level, int rest_level) const;
};

class PoGate : public CirGate
{
friend class AigGate;
friend class PiGate;
friend class ConstGate;
friend class UndefGate;
friend class CirMgr;
public:
    PoGate() : CirGate(), _line(0) {}
    PoGate(unsigned id, unsigned line) : CirGate(id), _line(line) {}
    ~PoGate() {}
    string getTypeStr() const { return "PO"; }
    unsigned getLineNo() const { return _line; }
    void printGate() const;
    //void genDFS() const;
    void reportGate() const;
    void reportFanin(int level) const { CirGate::cur_mark++; printFanin(false, 0, level); }
    void reportFanout(int level) const { CirGate::cur_mark++; printFanout(-1, 0, level); }
    int aig_under() const;
    void print_aig_under(ostream& outfile) const;
    void genDFS(GateList& _dfsList);
private:
    unsigned  _line;
    string    _symbol;
    void printFanin(bool is_neg, int self_level, int rest_level) const;
    void printFanout(unsigned previous_id, int self_level, int rest_level) const;
};

class ConstGate : public CirGate
{
friend class AigGate;
friend class PiGate;
friend class PoGate;
friend class UndefGate;
friend class CirMgr;
public:
    ConstGate() : CirGate(0) {}
    ~ConstGate() {}
    string getTypeStr() const { return "CONST"; }
    void printGate() const;
    //void genDFS() const;
    void reportGate() const;
    void reportFanin(int level) const { CirGate::cur_mark++; printFanin(false, 0, level); }
    void reportFanout(int level) const { CirGate::cur_mark++; printFanout(-1, 0, level); }
    int aig_under() const { return 0; }
    void print_aig_under(ostream& outfile) const { return; }
    void genDFS(GateList& _dfsList){ return; }
private:
    void printFanin(bool is_neg, int self_level, int rest_level) const;
    void printFanout(unsigned previous_id, int self_level, int rest_level) const;
};

class UndefGate : public CirGate
{
friend class AigGate;
friend class PiGate;
friend class PoGate;
friend class ConstGate;
friend class CirMgr;
public:
    UndefGate() : CirGate(){}
    UndefGate(unsigned id) : CirGate(id){ }
    ~UndefGate() {}
    string getTypeStr() const { return "UNDEF"; }
    void printGate() const {return;}
    void reportGate() const;
    void reportFanin(int level) const { CirGate::cur_mark++; printFanin(false, 0, level); }
    void reportFanout(int level) const { CirGate::cur_mark++; printFanout(-1, 0, level); }
    int aig_under() const { return 0; }
    void print_aig_under(ostream& outfile) const { return; }
    void genDFS(GateList& _dfsList){ return; }
private:
    void printFanin(bool is_neg, int self_level, int rest_level) const {return;}
    void printFanout(unsigned previous_id, int self_level, int rest_level) const {return;}
};


class GateNode{
public:
    GateNode() {}
    GateNode(CirGate* g){
        _gate = g;
        if(g->_fanin_invert[0]){
            _fanin0 = g->_fanin[0]->_id*2+1;
        }
        else{
            _fanin0 = g->_fanin[0]->_id*2;
        }
        if(g->_fanin_invert[1]){
            _fanin1 = g->_fanin[1]->_id*2+1;
        }
        else{
            _fanin1 = g->_fanin[1]->_id*2;
        }
    }
    ~GateNode() {}
    size_t operator()() const { return (size_t)(_fanin0*_fanin1); }
    bool operator == (const GateNode& k) const {
        //if((k.getG()->getTypeStr() == _gate->getTypeStr()) && (k.get0() == _fanin0) && (k.get1() == _fanin1))
        if((k._fanin0 == _fanin0) && (k._fanin1 == _fanin1)){
            return true;
        }
        else if(k._fanin0 == _fanin1){
            return (k._fanin1 == _fanin0); //opposite
        }
        return false;
    }
    CirGate* getG() const { return _gate; }
    unsigned getGid() const { return _gate->_id; }
    unsigned get0() const { return _fanin0; }
    unsigned get1() const { return _fanin1; }
private:
    CirGate* _gate;
    unsigned _fanin0; //*2 or *2+1
    unsigned _fanin1;
    
};

class FECnode{
public:
    FECnode(){}
    FECnode(CirGate* g, bool iFEC){
        _gate = g;
        IFEC = iFEC;
        if(IFEC)
            _simVal = ~(g->_simVal);
        else
            _simVal = g->_simVal;
    }
    ~FECnode() {}
    size_t operator()() const { return _simVal; }
    bool operator == (const FECnode& k) const{
        return (_simVal == k._simVal);
    }
    CirGate* getG() const { return _gate; }
    bool isIFEC() const { return IFEC; }
    void setSim(size_t n) { _simVal = n; }
private:
    CirGate* _gate;
    size_t _simVal;
    bool IFEC;
    
};





#endif // CIR_GATE_H
