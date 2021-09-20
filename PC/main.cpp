#define _WIN32_WINNT 0x0500
#include <iostream>
#include <string>
#include <cstddef>
#include <bitset>

#include "server.h"

#include <windows.h>
#include <winuser.h>    

#include "barrelFedder.h"

const char pairingPacket[13] = {(char)0xff, (char)0x00, (char)0x00, (char)0xff,
                                (char)0xff, (char)0x00, (char)0x00, (char)0xff,
                                (char)0xff, (char)0x00, (char)0x00, (char)0xff,
                                (char)0x00};

uint8_t u_counter = 0;


int main(int argc, char const *argv[]){

    SOCKET RecvSocket;
    char RecvBuf[13];
    int BufLen = 13;

    struct sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof(SenderAddr);

    std::cout<<"Setting up Server at Port:50001 \n";
    RecvSocket = setupServer();
    if (RecvSocket == 1){
        printf("Server Setup Failed, Exiting in 3 seconds");   
        Sleep(3000);
        return 1;           
    }

    std::cout<<"Server Setup Complete, Waiting for client to connect\n";
    if (!(getData(RecvSocket, RecvBuf, BufLen, &SenderAddr, &SenderAddrSize))){
        wprintf(L"Receiving failed %d\n", WSAGetLastError());
        // Clean up and exit.
        wprintf(L"Exiting.\n");
        WSACleanup();
    }

    //Pairing Packet Conversion
    convertDatatoFloat(RecvBuf); // converting Data and storing in newXXXXXX
    Azimuth = (*(float*)pairingPacket);  // converting pairing packet and storing in XXXXXX
    Pitch = (*(float*)&pairingPacket[4]);
    Roll = (*(float*)&pairingPacket[8]);

    if ( newAzimuth == Azimuth && newPitch == Pitch && newRoll == Roll){
        std::cout<<"Paired and ready to go B) \n";
    }
    else{
        std::cout<<"Pairing Failed :( , Try Recreating Android App and restarting this app \n";
        WSACleanup();
    }


    for(int i =0; i<1000; i++){
        getData(RecvSocket, RecvBuf, BufLen, &SenderAddr, &SenderAddrSize);
        convertDatatoFloat(RecvBuf);
        AzimuthCAL += newAzimuth;
        PitchCAL += newPitch;
        RollCAL += newRoll;
    }
    std::cout<<AzimuthCAL<<' '<<PitchCAL<<' '<<RollCAL<<'\n';
    AzimuthCAL /= 1000;
    PitchCAL /= 1000;
    RollCAL /= 1000;
    std::cout<<AzimuthCAL<<' '<<PitchCAL<<' '<<RollCAL<<'\n';
    AzimuthCAL = (AzimuthCAL*1000) - (int)(AzimuthCAL*1000);
    PitchCAL = (PitchCAL*1000) - (int)(PitchCAL*1000);
    //RollCAL = ((RollCAL*1000) - (int)(RollCAL*1000));
    std::cout<<AzimuthCAL<<' '<<PitchCAL<<' '<<RollCAL<<'\n';
    initialCalibration();





    float sumx = 0;
    float sumy = 0;
    bool moveFlag = false;
    std::cout<<"start fun";

    while(true){

        getData(RecvSocket, RecvBuf, BufLen, &SenderAddr, &SenderAddrSize);
        XandYs(RecvBuf);
        
        leftNright();
        keyboardkeyFeeder();   
        

        INPUT input[(int)stringPressed.size()+ (int)stringReleased.size()]; 
        if((int)stringPressed.size()+ (int)stringReleased.size() > 0)
        ZeroMemory(input,sizeof(input));

        //counter reset
        u_counter = 0 ;       

        for(char ch: stringPressed){
            input[u_counter].type = INPUT_KEYBOARD;
            input[u_counter].ki.dwFlags = KEYEVENTF_SCANCODE;
            input[u_counter].ki.wScan = MapVirtualKey(LOBYTE(VkKeyScan(ch)), 0);
            u_counter++;
        }

        for(char ch:stringReleased){
            input[u_counter].type = INPUT_KEYBOARD;
            input[u_counter].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
            input[u_counter].ki.wScan = MapVirtualKey(LOBYTE(VkKeyScan(ch)), 0);
            u_counter++;;
        }
        if(u_counter > 0) 
        SendInput(u_counter, input, sizeof(INPUT));
        
        mouseClickFeeder();
        INPUT inputXnY[1 + (int)stringPressed.size()+(int)stringReleased.size()] = {0};
        ZeroMemory(input,sizeof(input));
        
        inputXnY[0].type = INPUT_MOUSE;
        inputXnY[0].mi.dx = 0;
        inputXnY[0].mi.dy = 0;
        inputXnY[0].mi.dwFlags = MOUSEEVENTF_MOVE;
            
        sumx>0?(X>0? : sumx = 0) :(X<0? : sumx = 0);
        sumy>0?(Y>0? : sumy = 0) :(Y<0? : sumy = 0);
        sumx += X;
        sumy += Y;
        if(sumx >= 1 or sumx <= -1){
            inputXnY[0].mi.dx = (int)sumx;
            sumx = sumx - (int)sumx;
        }
        if(sumy >= 1 or sumy <= -1){
            inputXnY[0].mi.dy = (int)sumy;
            sumy = sumy - (int)sumy;
        }

        //counter reset
        u_counter = 1;
        for(char ch: stringPressed){
            inputXnY[u_counter].type = INPUT_MOUSE;
            if(ch == 'x') inputXnY[u_counter].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN ; 
            else inputXnY[u_counter].mi.dwFlags = MOUSEEVENTF_LEFTDOWN ;
            u_counter++;
        }

        for(char ch:stringReleased){
            inputXnY[u_counter].type = INPUT_MOUSE;
            if(ch == 'x') inputXnY[u_counter].mi.dwFlags = MOUSEEVENTF_RIGHTUP ; 
            else inputXnY[u_counter].mi.dwFlags = MOUSEEVENTF_LEFTUP ;
            u_counter++;
        }
        SendInput(u_counter, inputXnY, sizeof(INPUT));

        stringPressed = "";
        stringReleased = ""; 
        prevM1_M2_W_A_S_D_R_DPI = M1_M2_W_A_S_D_R_DPI;
    }
    
    closeServer(RecvSocket);   
    return 0;
}