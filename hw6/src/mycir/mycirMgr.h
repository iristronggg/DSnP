/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <set>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirGate.h"
#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){
       CirGate *g = new ConstGate(0); //const 0 gate
       _gateList.push_back(g);
   }
   ~CirMgr() {
       _gateList.clear();
       _dfsList.clear();
       _piList.clear();
       _poList.clear();
       _aigList.clear();
       _comments.clear();
   }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
       for (unsigned i = 0; i < _gateList.size(); i++)
       {
           if (_gateList[i] == NULL)
               continue;
           else if (_gateList[i]->getID() == gid)
               return _gateList[i];
       }
       return 0;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
    GateList _dfsList;
    

private:
    void readHeader();
    void readInput(unsigned gateID, unsigned lineNo);
    void readOutput(unsigned gateID, unsigned lineNo, unsigned fanin, unsigned inv);
    void readAig(unsigned gateID, unsigned lineNo, unsigned fanin0, unsigned fanin1, unsigned inv0, unsigned inv1);
    void connect();
    void genDFSList(const IdList&); //傳入_poList
    bool myStr2Unsigned(const string& str, unsigned& num);
    
    unsigned m, i, l, o, a;
    GateList _gateList;
    //GateList _dfsList;
    IdList _piList;
    IdList _poList;
    IdList _aigList;
    vector<string> _comments;
};

#endif // CIR_MGR_H
