float Azimuth, Pitch, Roll;
float newAzimuth, newPitch, newRoll;
float AzimuthCAL = 0, PitchCAL = 0, RollCAL = 0;
float X,Y;
std::byte M1_M2_W_A_S_D_R_DPI = (std::byte)0;
std::byte prevM1_M2_W_A_S_D_R_DPI = (std::byte)0;

float HorizontalPixelPerRadians;
float VerticalPixelPerRadians;

float ResWidth;
float ResHeight;

std::string stringPressed = "";
std::string stringReleased = "";

void convertDatatoFloat(char* RecvBuf){
    newAzimuth = *(float*)RecvBuf;
    newPitch = *(float*)&RecvBuf[4];
    newRoll = *(float*)&RecvBuf[8];
    M1_M2_W_A_S_D_R_DPI = (std::byte)RecvBuf[12];
}

void GetDesktopResolution()
{
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   ResWidth = desktop.right;
   ResHeight = desktop.bottom;
}

void initialCalibration(){
    Azimuth = newAzimuth - AzimuthCAL;
    Pitch = newPitch- PitchCAL;
    Roll = newRoll - RollCAL;  
    GetDesktopResolution();
    HorizontalPixelPerRadians = ((((ResWidth/2)/2)/4)/1.57079633f)*(float)(65535/ResWidth);
    VerticalPixelPerRadians = ((((ResHeight/2)/2)/4)/1.57079633f)*(float)(65535/ResHeight);
}

bool in4thQuadrant(float Angle){
    return (6.28318531 >= Angle && Angle > 4.71238898);
}

bool in1stQuadrant(float Angle){
    return (1.57079633 > Angle && Angle >= 0);
}

//to cut to three decimal places
void threePointFloat(){
    newAzimuth = (float)((int)(newAzimuth*100))/100;
    newPitch = (float)((int)(newPitch*100))/100;
    newRoll = (float)((int)(newRoll*100))/100;
}

void XandYs(char* RecvBuf){
    convertDatatoFloat(RecvBuf); 

    //Convert every negative value to equivalent angle
    newAzimuth = (newAzimuth < 0) ? 6.28318531f + newAzimuth : newAzimuth;
    newPitch = (newPitch < 0) ? 6.28318531f + newPitch : newPitch;


    if(in1stQuadrant(newAzimuth) && in4thQuadrant(Azimuth)){
        X = ((6.28318531f+(newAzimuth - Azimuth))*HorizontalPixelPerRadians);
    }
    else if(in4thQuadrant(newAzimuth) && in1stQuadrant(Azimuth)){
        X = ((6.28318531f-(newAzimuth - Azimuth))*HorizontalPixelPerRadians);
    }
    else{
        X = (newAzimuth - Azimuth)*HorizontalPixelPerRadians;
    }

    if(in1stQuadrant(newPitch) && in4thQuadrant(Pitch)){
        Y = ((6.28318531f+(newPitch - Pitch))*VerticalPixelPerRadians);
    }
    else if(in4thQuadrant(newPitch) && in1stQuadrant(Pitch)){
        Y = ((6.28318531f-(newPitch - Pitch))*VerticalPixelPerRadians);
    }
    else{
        Y = (newPitch - Pitch)*VerticalPixelPerRadians;
    }
        X = (float)((int)(X*1000))/1000;
        Y = (float)((int)(Y*1000))/1000;

        Azimuth = newAzimuth;
        Pitch = newPitch;      
}

void leftNright(){
    newRoll = (newRoll < 0) ? 6.28318531f + newRoll : newRoll;

    if((newRoll - RollCAL) < -0.349066f) M1_M2_W_A_S_D_R_DPI |= (std::byte)0b00010000;
    if((newRoll - RollCAL) > 0.349066f) M1_M2_W_A_S_D_R_DPI |= (std::byte) 0b00000100;
    //std::cout<<(int)M1_M2_W_A_S_D_R_DPI<<' '<<newRoll<<' '<<RollCAL<<'\n';
}

char characterSelector(uint8_t* i){

    switch (*i){
    case 0 : return 'n'; //dpi switch not in use for now
    case 1 : return 'r';
    case 2 : return 'd';
    case 3 : return 's';
    case 4 : return 'a';
    case 5 : return 'w';
    case 6 : return 'x';
    case 7 : return 'z';
    }
    return 'n';
}

void keyboardkeyFeeder(){

    //identify which keys are pressed and which are released
    //if latest input's bit is 0, then it is pressed else released.
    //std::cout<<(int)M1_M2_W_A_S_D_R_DPI<<' '<<(int)M1_M2_W_A_S_D_R_DPI<<'\n';
    for(uint8_t i = 0 ; i < 6; i++){
        if(!(bool)(prevM1_M2_W_A_S_D_R_DPI & ((std::byte)0x01 << i)) && (bool)(M1_M2_W_A_S_D_R_DPI & ((std::byte)0x01 << i) )){
            stringPressed += characterSelector(&i);
        }
        else if((bool)(prevM1_M2_W_A_S_D_R_DPI & ((std::byte)0x01 << i)) && !(bool)(M1_M2_W_A_S_D_R_DPI & ((std::byte)0x01 << i) )){
            stringReleased += characterSelector(&i);
        }
    }
    //std::cout<<stringPressed<<' '<<stringReleased<<'\n';
}

void mouseClickFeeder(){
    stringPressed = "";
    stringReleased = "";

    for(uint8_t i = 6 ; i < 8; i++){
        if(!(bool)(prevM1_M2_W_A_S_D_R_DPI & ((std::byte)0x01 << i)) && (bool)(M1_M2_W_A_S_D_R_DPI & ((std::byte)0x01 << i) )){
            stringPressed += characterSelector(&i);
        }
        else if((bool)(prevM1_M2_W_A_S_D_R_DPI & ((std::byte)0x01 << i)) && !(bool)(M1_M2_W_A_S_D_R_DPI & ((std::byte)0x01 << i) )){
            stringReleased += characterSelector(&i);
        }
    }
}


