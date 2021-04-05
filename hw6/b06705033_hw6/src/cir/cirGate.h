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
#include <set>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
    friend class CirMgr;
    CirGate(): _ref(0) {
        _symbol = NULL;
    }
    ~CirGate() {
        if (_symbol != NULL)
            delete _symbol;
        _symbol = NULL;
        _inList0.clear();
        _inList1.clear();
    }

   // Basic access methods
    virtual string getTypeStr() const = 0;
    unsigned getLineNo() const { return lineNo; }
    unsigned getID() const { return gateID; }
    virtual unsigned getFanin0() const { return _fanin0; }
    virtual unsigned getFanin1() const { return _fanin1; }
    void addFin0(CirGate *&);
    void addFin1(CirGate *&);
    void addFout(CirGate *&);

   // Printing functions
    virtual void printGate() const = 0;
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;
    
    // DFS traversal
    void dfsTraversal(CirGate*, GateList&); //傳入_dfslist來push入_dfslist
    void printFanin(const CirGate*, bool, int, int) const;
    void printFanout(const CirGate*, bool, int, int) const;
    bool isGlobalRef() const { return (_ref == _globalRef); }
    void setToGlobalRef() const { _ref = _globalRef; }
    static void setGlobalRef() { _globalRef++; }
    

    //virtual void setFin0(CirGate* fin) { }
    //virtual void setFin1(CirGate* fin) { }
    //virtual CirGate* getFin0() const { }
    //virtual CirGate* getFin1() const { }
    


private:
    static unsigned _globalRef;
    mutable unsigned _ref;
    //unsigned _fanin0;
    //unsigned _fanin1;
    //CirGate* _fin0;
    //CirGate* _fin1;
protected:
    string type;
    unsigned gateID;
    unsigned lineNo = 0;
    unsigned _fanin0;
    unsigned _fanin1;
    bool  _invPhase0;
    bool  _invPhase1;
    string *_symbol;
    GateList _inList0; //true if odd
    GateList _inList1;
    GateList _outList;
    
    CirGate(string type, unsigned ID, unsigned No): type(type), gateID(ID), lineNo(No) {};
    CirGate(string type, unsigned ID, unsigned No, unsigned fanin, bool inv): type(type), gateID(ID), lineNo(No), _fanin0(fanin), _invPhase0(inv) {}
    CirGate(string type, unsigned ID, unsigned No, unsigned fanin0, unsigned fanin1, bool inv0, bool inv1): type(type), gateID(ID), lineNo(No), _fanin0(fanin0), _fanin1(fanin1), _invPhase0(inv0), _invPhase1(inv1) {}
    //CirGate(string type, unsigned ID): type(type), gateID(ID) {}; //undef
    

};

class ConstGate : public CirGate
{
  public:
    ConstGate(unsigned ID): CirGate("CONST", 0, 0){_symbol = NULL;}
    ~ConstGate(){}
    string getTypeStr() const { return "CONST"; }
    void printGate() const;

  private:
};

class UndefGate : public CirGate
{
  public:
    UndefGate(unsigned ID): CirGate("UNDEF", ID, 0){_symbol = NULL;}
    ~UndefGate(){}
    string getTypeStr() const { return "UNDEF"; }
    void printGate() const;

  private:
};

class CirPiGate : public CirGate
{
public:
    CirPiGate(unsigned ID, unsigned No): CirGate("PI", ID, No){_symbol = NULL;}
     ~CirPiGate() {}
    void printGate() const;
    string getTypeStr() const { return "PI"; }
private:
    //string type;
    //unsigned gateID;
    //unsigned lineNo;
};
    
class CirPoGate : public CirGate
{
public:
    CirPoGate(unsigned ID, unsigned No, unsigned fanin, bool inv): CirGate("PO", ID, No, fanin, inv) {_symbol = NULL;}
     ~CirPoGate() {}
    void printGate() const;
    string getTypeStr() const { return "PO"; }
    unsigned getFanin0() const { return _fanin0; }
    //void setFin0(CirGate*& fin) { _fin0 = fin; }
    //CirGate* pi = new CirPiGate(gateID, lineNo)
private:
    //string type;
    //unsigned gateID;
    //unsigned lineNo;
    //unsigned _fanin0;
    //bool  _invPhase0;
    //CirGate* _fin0;

};

class CirAigGate : public CirGate
{
public:
    CirAigGate(unsigned ID, unsigned No, unsigned fanin0, unsigned fanin1, bool inv0, bool inv1): CirGate("AIG", ID, No, fanin0, fanin1, inv0, inv1) {}
     ~CirAigGate() {_symbol = NULL;}
    void printGate() const;
    string getTypeStr() const { return "AIG"; }
    unsigned getFanin0() const { return _fanin0; }
    unsigned getFanin1() const { return _fanin1; }
    //void setFin0(CirGate* fin) { _fin0 = fin; }
    //void setFin1(CirGate* fin) { _fin1 = fin; }
    //CirGate* getFin0() const { return _fin0; }
    //CirGate* getFin1() const { return _fin1; }
    //CirGate* pi = new CirPiGate(gateID, lineNo)
private:
    //string type;
    //unsigned gateID;
    //unsigned lineNo;
    //unsigned _fanin0;
    //unsigned _fanin1;
    //bool  _invPhase0;
    //bool  _invPhase1;
    //CirGate* _fin0;
    //CirGate* _fin1;

};

#endif // CIR_GATE_H
