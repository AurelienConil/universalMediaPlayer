//
//  averageColor.cpp
//  Radiologic
//
//  Created by Admin Mac on 25/09/2019.
//

#include "averageColor.hpp"


//------------------------------------------------------
averageColor::averageColor(){
    
    averageFbo.allocate(AVERAGE_SIZE, AVERAGE_SIZE);
    smoothColor = ofColor(127);
    finalR = 0;
    finalG = 0;
    finalB = 0;
    smoothR = 0;
    smoothG = 0;
    smoothB = 0;
    
    colorSmooth = 0.035;
    
    isActivated = false;
    isDraw = false;
}

//------------------------------------------------------
averageColor::averageColor(bool _isActivated){
    
    averageFbo.allocate(AVERAGE_SIZE, AVERAGE_SIZE);
    smoothColor = ofColor(127);
    finalR = 0;
    finalG = 0;
    finalB = 0;
    smoothR = 0;
    smoothG = 0;
    smoothB = 0;
    
    colorSmooth = 0.035;
    
    isActivated = _isActivated;
    isDraw = false;
}

//------------------------------------------------------
void averageColor::fboBegin(){
    
    averageFbo.begin();
}

//------------------------------------------------------
void averageColor::fboEnd(){
    
    averageFbo.end();
}


void averageColor::calculate(){
    
    ofPixels pix;
    averageFbo.readToPixels(pix);
    
    float averageRed = 0;
    float averageGreen = 0;
    float averageBlue = 0;
    float actualRed, actualGreen, actualBlue;
    float averageH = 0;
    float averageS = 0;
    float averageB = 0;
    float actualH, actualS, actualB;
    
    for(int i = 0; i<AVERAGE_SIZE; i++)
    for(int j = 0; j<AVERAGE_SIZE; j++){
            
        colRGB = pix.getColor(i, j);
        averageRed += colRGB.r;
        averageGreen += colRGB.g;
        averageBlue += colRGB.b;
            
    }
    
    finalR = averageRed / (AVERAGE_SIZE*AVERAGE_SIZE*1.0f);
    finalG = averageGreen / (AVERAGE_SIZE*AVERAGE_SIZE*1.0f);
    finalB = averageBlue / (AVERAGE_SIZE*AVERAGE_SIZE*1.0f);
    
    smoothR =  smoothR + (finalR - smoothR)*1.0f*colorSmooth;
    smoothG =  smoothG + (finalG - smoothG)*1.0f*colorSmooth;
    smoothB =  smoothB + (finalB - smoothB)*1.0f*colorSmooth;
    
    smoothColor = ofColor(smoothR, smoothG, smoothB);
    
}
