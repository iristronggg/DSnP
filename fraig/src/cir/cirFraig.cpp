/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashSet.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
    unsigned acnt = 0;
    for(size_t j = 1; j < _gates.size(); j++)
        if(_gates[j] != NULL && _gates[j]->isAig())
            acnt++;
    
    _gateHash.init(acnt); //set the size of _gateHash by number of AIGs
    
    for(size_t i = 0; i < _dfsList.size(); i++){
        if(!_dfsList[i]->isAig())
            continue;
        
        GateNode node = GateNode(_dfsList[i]);
        if(_gateHash.query(node)){  //already in _gateHash, replaced with the gate in hash
            cout << "Strashing: " << node.getGid() << " merging " << _dfsList[i]->_id << "..." << endl;
            _dfsList[i]->mergeGate(node.getG()); //replace this with hash's CirGate* _gate
            
            _gates[_dfsList[i]->_id] = NULL; //delete from gate list
        }
        else{
            _gateHash.insert(node);
            //cout << node.getGid() << endl;
        }
    }
    dfsTraversal(); //update dfsList
}



void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
