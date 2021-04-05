/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>
#include <bitset>
#include <cmath>
#include <vector>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include "myHashSet.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
unsigned n = 0;
size_t simCnt = 0;
bool FECset = false;


/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
    FECset = false;
    RandomNumGen rnGen;
    int notchange = 0, old = 0;
    unsigned simCnt = 0, acnt = 0;
    
    for(size_t j = 1; j < _gates.size(); j++)
        if(_gates[j] != NULL && _gates[j]->isAig())
            acnt++;
    
    //size_t max = sqrt(acnt);
    size_t max = sqrt(acnt+_pis_idx.size()+_pos.size());
    
    if(max>20){
        if(sqrt(max)/4 > 20)
            max = sqrt(max)/4;
        else max = 20;
    }
    
    while(notchange <= max){
        for(unsigned i = 0; i < 64; i++){
            for(unsigned j = 0; j < _pis_idx.size(); j++){
                _gates[_pis_idx[j]]->_simVal *= 2;
                _gates[_pis_idx[j]]->_simVal += rnGen(2);
            }
        }
        simCnt += 64;
        simTraversal();
        if(FECset)  updateFEC();
        else    setFEC();
        
        if(_FECgroup.size() == old){
            notchange++;
        }
        else{
            notchange = 0;
            old = _FECgroup.size();
        }
    }
    cout << simCnt << " patterns simulated." << endl;
    
    
    
}

void
CirMgr::fileSim(ifstream& patternFile)
{
    FECset = false;
    string line;
    unsigned lineCnt = 0, simCnt = 0;
    bool format_err = false;
    
    n=0;
    while(patternFile >> line){
        lineCnt++;
        
        if(!checkPatten(line)){
            format_err = true;
            break;
        }
        
        simPI(line);
        if(lineCnt % 64 == 0){
            //cout << "1";
            n = 64;
            simTraversal();
            if(FECset)  updateFEC();
            else    setFEC();
            //---clear---
            for(unsigned i = 0; i < _pis_idx.size(); i++){
                //in[i] = _gates[_pis_idx[i]]->_simVal;
                _gates[_pis_idx[i]]->_simVal = 0;
            }
            /*for(unsigned i = 0; i < _dfsList.size(); i++){
                if(_dfsList[i]->getTypeStr() == "AIG" || _dfsList[i]->getTypeStr() == "PO"){
                    _dfsList[i]->setSim();
                }
            }*/
            //-----------
            n = 0;
            
            simCnt += 64;
            
        }
    }
    
    if(lineCnt % 64 != 0 && (!format_err)){
        //cout << "2";
        /*for(unsigned i = 0; i < _pis_idx.size(); i++){
            //in[i] = _gates[_pis_idx[i]]->_simVal;
            bitset<64> b(_gates[_pis_idx[i]]->_simVal);
            cout << b.to_string() << " ";
            //cout<<_gates[_pis_idx[i]]->_simVal << " ";
        }*/
        
        n = (lineCnt % 64);
        //cout << n;
        simTraversal();
        if(FECset)  updateFEC();
        else    setFEC();
        simCnt += (lineCnt % 64);
        
    }
    
    cout << simCnt << " patterns simulated." << endl;
    patternFile.close();
    
    
}


bool
CirMgr::checkPatten(const string &str){
    if(str.size() != _pis_idx.size()){
        cerr << "Error: Pattern(" << str << ") length(" << str.size()
            << ") does not match the number of inputs(" << _pis_idx.size() << ") in a circuit!!" << endl;
        return false;
    }
    
    for(unsigned i = 0; i < str.size(); i++){
        if(str[i] != '0' && str[i] != '1'){
            cerr << "Error: Pattern(" << str << ")  contains a non-0/1 character('"<< str[i] << "')." << endl;
            return false;
        }
    }
    
    return true;
}


void
CirMgr::simTraversal(){
    
    for(unsigned i = 0; i < _dfsList.size(); i++){
        if(_dfsList[i]->getTypeStr() == "AIG" || _dfsList[i]->getTypeStr() == "PO"){
            _dfsList[i]->setSim();
        }
    }
    
    //-----------------------------------------
    vector<size_t> in(_pis_idx.size(),0);
    vector<size_t> out(_pos.size(),0);
    vector<string> inString(_pis_idx.size(), "");
    vector<string> outString(_pos.size(), "");
    
    for(unsigned i = 0; i < _pos.size(); i++){
        out[i] = _pos[i]->_simVal;
        bitset<64> b(out[i]);
        outString[i] = b.to_string();
        //cout << outString[i] << endl;
    }
    //cout << endl;
    for (unsigned i = 0; i < _pis_idx.size(); ++i){
        in[i] = _gates[_pis_idx[i]]->_simVal;
        bitset<64> b(in[i]);
        inString[i] = b.to_string();
        //cout << inString[i] << endl;
    }
    //cout << endl;

    
    if(_simLog != NULL){
        //cout << "in";
        for(int i = 63; i >= (64-n); i--){
            if(i == -1)
                break;
            for(unsigned j = 0; j < _pis_idx.size(); j++){
                //size_t s = (_gates[_pis_idx[j]]->_simVal >> i) % 2;
                *_simLog << inString[j][i];
            }
            *_simLog << " ";
            for(unsigned j = 0; j < _pos.size(); j++){
                //size_t s = (_pos[j]->_simVal >> i) % 2;
                *_simLog << outString[j][i];
            }
            *_simLog << endl;
        }
    }
    
}

void
CirMgr::setFEC(){ //only CONST0 and AIG
    FECset = true;
    unsigned acnt = 0;
    for(size_t j = 1; j < _gates.size(); j++)
        if(_gates[j] != NULL && _gates[j]->isAig())
            acnt++;
    
    _FECHash.init((acnt+1)*50);
    FECnode _fecNode = FECnode(_gates[0], false); //set CONST0
    _FECHash.insert(_fecNode);
    
    
    //sort _dfsList in ascending order
    vector<unsigned> _dfs_idx;
    for(size_t i = 0; i < _dfsList.size(); i++){
        if(_dfsList[i]->isAig())                    //set AIG
            _dfs_idx.push_back(_dfsList[i]->_id);
    }
    std::sort(_dfs_idx.begin(), _dfs_idx.end());
    
    //put into hash
    for(size_t i = 0; i < _dfs_idx.size(); i++){
        if(!_gates[_dfs_idx[i]]->isAig())
            continue;
        
        FECnode _fecNode = FECnode(_gates[_dfs_idx[i]], false);
        if(_FECHash.check(_fecNode)){ //there's FEC pair
            _FECHash.insertFEC(_fecNode);
        }
        else{ //there's no FEC pair, but maybe IFEC pair
            FECnode _IfecNode = FECnode(_gates[_dfs_idx[i]], true);
            if(_FECHash.check(_IfecNode)){  //I'm someone's IFEC pair
                _FECHash.insertFEC(_IfecNode);
            }
            else{ //there's no pair for me now
                _FECHash.insert(_fecNode);
            }
        }
    }
    
    
    //------group List-------

    for(size_t i = 0; i < _FECHash.numBuckets(); i++){
        if(_FECHash[i].size() > 1){
            for(size_t j = 0; j < _FECHash[i].size(); j++){
                _FECHash[i][j].getG()->setGroupNum(_FECgroup.size()); //from 0
            }
            //std::sort(_FECsubgroup.begin(), _FECsubgroup.end()); //排序
            _FECgroup.push_back(_FECHash[i]);
        }
    }
    
    
    /*for(size_t i = 0; i < _FECgroup.size(); i++){
        cout << i << " ";
        for(size_t j = 0; j < _FECgroup[i].size(); j++){
            cout << _FECgroup[i][j].getG()->_id << " ";
            bitset<64> b(_FECgroup[i][j].getG()->_simVal);
            string val = b.to_string();
            cout << _FECgroup[i][j].isIFEC();
            cout << " ";
            
        }
        cout << endl;
    }*/
    
}


void
CirMgr::updateFEC(){
    vector<vector<FECnode>> _newGroup;
    
    for(size_t i = 0; i < _FECgroup.size(); i++){ //each group divide
        _FECHash.clear();
        _FECHash.init(_FECgroup[i].size()*50);
        for(size_t j = 0; j < _FECgroup[i].size(); j++){
            if(_FECgroup[i][j].isIFEC())
                _FECgroup[i][j].setSim( ~(_FECgroup[i][j].getG()->getSim()) );
            else _FECgroup[i][j].setSim( _FECgroup[i][j].getG()->getSim() );
            
            _FECHash.insertFEC(_FECgroup[i][j]);
        }
        
        for(size_t k = 0; k < _FECHash.numBuckets(); k++){
            if(_FECHash[k].size() == 1){ //become inpendent this time
                _FECHash[k][0].getG()->setGroupNum(-1);
            }
            else if(_FECHash[k].size() > 1){
                for(size_t j = 0; j < _FECHash[k].size(); j++){
                    _FECHash[k][j].getG()->setGroupNum(_FECgroup.size()); //from 0
                }
                //std::sort(_FECsubgroup.begin(), _FECsubgroup.end()); //排序
                _newGroup.push_back(_FECHash[k]);
            }
        }
        
    }//end divide
    
    _FECgroup.clear();
    _FECgroup = _newGroup;
    
    
}






/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::simPI(const string &str){
    for(int i = 0; i < str.size(); i++){
        _gates[_pis_idx[i]]->_simVal += ((size_t)1 << simCnt)*(size_t)(str[i]-'0');
    }
    if(simCnt < 64){
        simCnt++;
    }
    else{
        simCnt=1;
    }
    /*if(simCnt == 64){
        simCnt = 1;
    }
    else{
        simCnt++;
    }*/
    //cout << simCnt << endl;
}



void
CirMgr::reportFEC(size_t id, int _grpN)
{
    bool flag;
    for(size_t i = 0; i < _FECgroup[_grpN].size(); i++){
        if(_FECgroup[_grpN][i].getG()->_id == id)
            flag = _FECgroup[_grpN][i].isIFEC();
    }
    
    for(size_t i = 0; i < _FECgroup[_grpN].size(); i++){
        if(_FECgroup[_grpN][i].getG()->_id == id)
            continue;
        cout << " ";
        if(flag != _FECgroup[_grpN][i].isIFEC())
            cout << "!";
        
        cout << _FECgroup[_grpN][i].getG()->_id;
    }
    
}
