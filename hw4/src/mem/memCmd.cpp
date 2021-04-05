/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
    vector<string> opts;
    if (!CmdExec::lexOptions(option, opts))
        return CMD_EXEC_ERROR;
    
    size_t nOpts = opts.size();
    
    // 1 <= nOpts <= 3
    if(nOpts > 3)
        return CmdExec::errorOption(CMD_OPT_EXTRA, opts[3]);
    else if(nOpts < 1)
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    
    int obj = 0, arrsize = 0;
    bool arr = false;
    
    //Object
    if(nOpts == 1){
        if(!myStr2Int(opts[0], obj))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[0]);
        if(obj <= 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, std::to_string(obj));
    }
    
    //Array
    else{
        for(size_t i = 0; i < nOpts; i++){
            if(myStrNCmp("-Array", opts[i], 2) == 0){
                if(arr)
                    return CmdExec::errorOption(CMD_OPT_EXTRA, opts[i]); //-Array command already exist
                arr = true;
                i++;
                if(nOpts == i)
                    return CmdExec::errorOption(CMD_OPT_MISSING, "");
                if(!myStr2Int(opts[i], arrsize))
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[i]);
                if(arrsize <= 0)
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[i]);
            }
            else if(myStr2Int(opts[i], obj));
            else{
              return errorOption(CMD_OPT_ILLEGAL, opts[i]);
            }
        }
    }
    
    obj = (size_t)obj;
    arrsize = (size_t)arrsize;
    
    if(arr){
        try{
            mtest.newArrs(obj, arrsize);
        }
        catch(bad_alloc){
            return CMD_EXEC_ERROR;
        }
    }
    else{
        try{
            mtest.newObjs(obj);
        }
        catch(bad_alloc){
            return CMD_EXEC_ERROR;
        }
    }
    
   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
    vector<string> opts;
    if (!CmdExec::lexOptions(option, opts))
        return CMD_EXEC_ERROR;
    
    size_t nOpts = opts.size();
    
    // 1 <= nOpts <= 3
    if(nOpts < 1){
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    }
    
    int idx = 0, num = 0;
    size_t arr_pos = 0, rOpts = 0;
    bool idx_flag = false, r_flag = false, arr = false;
    
    //find where -Array command is
    for(size_t i = 0; i < nOpts; i++){
        if(myStrNCmp("-Array", opts[i], 2) == 0){
            if(arr)
                return CmdExec::errorOption(CMD_OPT_EXTRA, opts[i]); //-Array command already exist
            arr = true;
            arr_pos = i;
        }
    }
    
    //First option: "-Array"/"-Index"/"-Random"
    if((myStrNCmp("-Index", opts[0], 2) != 0) && (myStrNCmp("-Random", opts[0], 2) != 0) && (myStrNCmp("-Array", opts[0], 2) != 0))
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[0]);
    
    //if(nOpts > 2 && !arr)
    
    for(size_t i = 0; i < nOpts; i++){
        if(myStrNCmp("-Index", opts[i], 2) == 0){
            if(idx_flag || r_flag)
                return CmdExec::errorOption(CMD_OPT_EXTRA, opts[i]); //-Ramdom/-Index command already exist
            idx_flag = true;
            i++;
            if(i == nOpts || i == arr_pos)
                return CmdExec::errorOption(CMD_OPT_MISSING, "");
            if(!myStr2Int(opts[i], idx))
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[i]);
            if(idx < 0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[i]);
            if(arr){
                if(idx >= mtest.getArrListSize()){
                    cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << idx << "!!" << endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[i]); //??????
                }
            }
            else if(!arr){
                if(idx >= mtest.getObjListSize()){
                    cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << idx << "!!" << endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[i]); //????????
                }
            }
        }
        else if(myStrNCmp("-Random", opts[i], 2) == 0){  //-Random
            if(idx_flag || r_flag)
                return CmdExec::errorOption(CMD_OPT_EXTRA, opts[i]); //-Random/-Index command already exist
            r_flag = true;
            rOpts = i;
            i++;
            if(i == nOpts || i == arr_pos)
                return CmdExec::errorOption(CMD_OPT_MISSING, "");
            if(!myStr2Int(opts[i], num))
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[i]);
            if(num < 0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[i]);
            
/*            if(arr){
                if(mtest.getArrListSize() == 0){
                    cerr << "Size of array list is 0!!" << endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[i-1]);
                }
            }
            else if(!arr){
                if(mtest.getObjListSize() == 0){
                    cerr << "Size of object list is 0!!" << endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[i-1]);
                }
            }*/
        }
        /*else{
          return errorOption(CMD_OPT_ILLEGAL, opts[i]);
        }*/
            
    }
    
    if(!idx_flag && !r_flag)
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    if(nOpts > 3){
        return CmdExec::errorOption(CMD_OPT_EXTRA, opts[3]);
    }
    
    //Delete array
    if(arr){
        if(r_flag){
            if(mtest.getArrListSize() > 0){
                for(int i = 0; i < num; i++)
                    mtest.deleteArr(rnGen(mtest.getArrListSize()));
            }
            else if(mtest.getArrListSize() == 0){
                cerr << "Size of array list is 0!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[rOpts]);
            }
        }
        else if(idx_flag){
            mtest.deleteArr((size_t)idx);
        }
    }
    else{  //Delete object
        if(r_flag){
            if(mtest.getObjListSize() > 0){
                for(int i = 0; i < num; i++)
                    mtest.deleteObj(rnGen(mtest.getObjListSize()));
            }
            else if(mtest.getObjListSize() == 0){
                cerr << "Size of object list is 0!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, opts[rOpts]);
            }
        }
        else if(idx_flag){
            mtest.deleteObj((size_t)idx);
        }
    }
                                    
    return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


