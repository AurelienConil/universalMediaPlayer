//
//  averageColor.hpp
//  Radiologic
//
//  Created by Admin Mac on 25/09/2019.
//

#ifndef averageColor_hpp
#define averageColor_hpp

#include <stdio.h>
#include "ofMain.h"
#define AVERAGE_SIZE 8
class averageColor{
    
    public :
    averageColor();
    averageColor(bool _isActivated);
    void fboBegin();
    void fboEnd();
    void calculate();
    ofFbo averageFbo;
    
    bool isActivated;
    bool isDraw;
    
    ofColor finalColorRGB;
    float finalR;
    float finalG;
    float finalB;
    
    ofColor colRGB; //
    
    ofColor smoothColor;
    float smoothR;
    float smoothG;
    float smoothB;
    
    // This value smooth the final color
    float colorSmooth;

    
    
};



#endif /* averageColor_hpp */
