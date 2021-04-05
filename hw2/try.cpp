//
//  try.cpp
//  
//
//  Created by 莊海因 on 2019/9/30.
//
#include <iostream>
using namespace std;

int main() {
    char arr[]="jog";
    char *ptr = arr;
    
    cout << arr;
    return 0;
}
    
    /*for(int i = 0; i < length; i++) {
        cout << "&arr[" << i << "]: " << &arr[i];
        cout << "\tptr+" << i << ": " << ptr+i;
        cout << endl;
    }

/*int main ()
{
    cout << "Test\b";
    //cout << "\b";
    cout << "\n";
    
    return 0;
}*/
