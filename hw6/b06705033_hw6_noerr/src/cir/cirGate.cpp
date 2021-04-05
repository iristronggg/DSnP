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
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;
unsigned CirGate::_globalRef = 0;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
    string str = "";
   cout << "==================================================" << endl;
   str = "= " + type + "(" + to_string(gateID) + ")";
   if (_symbol != NULL)
       str += ("\"" + *_symbol + "\"");
   str += (", line " + to_string(lineNo));
   cout << setw(49) << left << str << "=" << endl;
   cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
    assert (level >= 0);
    setGlobalRef();
    printFanin(this, false, level, 0);
}

void
CirGate::reportFanout(int level) const
{
    assert (level >= 0);
    
}

void CirGate::printFanin(const CirGate* node, bool inv, int level, int now_level) const
{
    assert (level >= 0);
    
    if(now_level > level)
        return;
    
    for(int i = 0; i < now_level; i++)
        cout << "  ";
    if(inv)
        cout << "!";
    cout << node->getTypeStr() << " " << node->getID();
    
    if(now_level == level){
        cout << endl;
        return;
    }
    
    if(isGlobalRef()){
        cout << " (*)" << endl;
    }
    else{
        cout << endl;
        
        for(unsigned i = 0; i < node->_inList0.size(); i++){
            node->_inList0[i]->printFanin(node->_inList0[i], node->_invPhase0, level, now_level+1);
        }
        for(unsigned i = 0; i < node->_inList1.size(); i++){
            node->_inList1[i]->printFanin(node->_inList1[i], node->_invPhase1, level, now_level+1);
        }
        if(!_inList0.empty() || !_inList1.empty())
            setToGlobalRef();
    }
}

void CirGate::addFin0(CirGate *&g)
{
    _inList0.push_back(g);
    //cout << g->getID();
    g->_outList.push_back(this); // connect g's _outList to myself
}

void CirGate::addFin1(CirGate *&g)
{
    _inList1.push_back(g);
    g->_outList.push_back(this);
}

void CirGate::addFout(CirGate *&g)
{
    _outList.push_back(g);
}


// DFS traversal
void CirGate::dfsTraversal(CirGate* node, vector<CirGate*>& dfsList)
{
    for(CirGate *&i : _inList0){
        if (!i->isGlobalRef()){
            i->setToGlobalRef();
            i->dfsTraversal(i, dfsList);
        }
    }
    for(CirGate *&i : _inList1){
        if (!i->isGlobalRef()){
            i->setToGlobalRef();
            i->dfsTraversal(i, dfsList);
        }
    }
    dfsList.push_back(node);
    
    /*for_each_predecessor(next, _predecessors)
       if (!next->isGlobalRef()) {
          next->setToGlobalRef();
          next->dfsTraversal(dfsList);
       }
    d.push_back(this);*/

}

/**************************************/
/*   ConstGate   */
/**************************************/
void ConstGate::printGate() const {
    cout << "CONST0";
}

/**************************************/
/*   UndefGate   */
/**************************************/
void UndefGate::printGate() const {
    cout << "UNDEF " << gateID;
}

/**************************************/
/*   PiGate   */
/**************************************/
void CirPiGate::printGate() const {
    cout << "PI  " << gateID;
}

/**************************************/
/*   PoGate   */
/**************************************/
void CirPoGate::printGate() const {
    cout << "PO  " << gateID << " ";
    if(_inList0[0]->getTypeStr() == "UNDEF")
        cout << "*";
    if(_invPhase0)
        cout << "!";
    cout << _fanin0;
}

/**************************************/
/*   AigGate   */
/**************************************/
void CirAigGate::printGate() const {
    cout << "AIG " << gateID << " ";
    
    if(_inList0[0]->getTypeStr() == "UNDEF")
        cout << "*";
    if(_invPhase0)
        cout << "!";
    cout << _fanin0 << " ";
    
    if(_inList1[0]->getTypeStr() == "UNDEF")
        cout << "*";
    if(_invPhase1)
        cout << "!";
    cout << _fanin1;
}

