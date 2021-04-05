/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
    CirMgr::dfsTraversal();
    vector<bool> inDFS(m+o+1, false);
    IdList toRemove;
    for(size_t i = 0; i < _dfsList.size(); i++){
        inDFS[_dfsList[i]->_id] = true;
    }
    for(size_t i = 0; i < _gates.size(); i++){
        if((_gates[i]!=NULL) && (_gates[i]->getTypeStr()!="PI") && (_gates[i]->getTypeStr()!="CONST") && (!inDFS[i])){
            toRemove.push_back(_gates[i]->_id);
        }
    }
    /*
    for(size_t i = 0; i < _pos.size(); i++){
        if((_pos[i]->getTypeStr()!="PI") && (_pos[i]->getTypeStr()!="CONST") && (!inDFS[i])){
            toRemove.push_back(_pos[i]->_id);
        }
    }*/
    
    //sort(toRemove.begin(), toRemove.end());
    
    for(size_t i = 0; i < toRemove.size(); i++){
        if(_gates[toRemove[i]]->isAig())
            a--;
        
        //sweeping msg
        cout << "Sweeping: " << _gates[toRemove[i]]->getTypeStr() << "(" << toRemove[i] << ") removed..." << endl;
        
        _gates[toRemove[i]]->rmFromFanin();
        _gates[toRemove[i]]->rmFromFanout();
        _gates[toRemove[i]] = NULL;
        
    }
    //o = _pos.size();
    
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
    IdList toRemove;
    for(size_t i = 0; i < _dfsList.size(); i++){
        bool removeg = false;
        CirGate* g = _dfsList[i];
        
        //pass
        if(g->getTypeStr()=="PI" || g->getTypeStr()=="PO" || g->getTypeStr()=="CONST" || g->getTypeStr()=="UNDEF")
            continue;
        
        
        //--------------------CASE------------------------------
        if(g->_fanin[0]->_id == g->_fanin[1]->_id){
            if(g->_fanin_invert[0] == g->_fanin_invert[1]){ //---------case 3
                g->replaceFanin(0); //or 1, the same
            }
            else if(g->_fanin_invert[0] != g->_fanin_invert[1]){ //----case 4
                g->replaceConst(_gates[0]); //replace with CONST0
            }
            
            removeg = true;
        }
        else if(g->_fanin[0]->getTypeStr() == "CONST"){
            if(g->_fanin_invert[0]) // !CONST0-----------------------case 1
                g->replaceFanin(1); //replace with _fanin[1]
            else if(!g->_fanin_invert[0]) //CONST0-------------------case 2
                g->replaceConst(_gates[0]); //replace with CONST0
            
            removeg = true;
        }
        else if(g->_fanin[1]->getTypeStr() == "CONST"){
            if(g->_fanin_invert[1]) // !CONST0-----------------------case 1
                g->replaceFanin(0); //replace with _fanin[0]
            else if(!g->_fanin_invert[1]) //CONST0-------------------case 2
                g->replaceConst(_gates[0]); //replace with CONST0
            
            removeg = true;
        }
        
        if(removeg)
            toRemove.push_back(g->_id);
    }
    
    for(size_t i = 0; i < toRemove.size(); i++){  //delete from gateList
        if(_gates[toRemove[i]]->isAig())
            a--;
        _gates[toRemove[i]]->rmFromFanin();
        _gates[toRemove[i]]->rmFromFanout();
        _gates[toRemove[i]] = NULL;
    }
    
    dfsTraversal();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
