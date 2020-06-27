//
//  messagePlayer.hpp
//  Radiologic
//
//  Created by Admin Mac on 28/07/2019.
//

#ifndef messagePlayer_hpp
#define messagePlayer_hpp

#include <stdio.h>
#include "ofMain.h"
#include "errorManager.hpp"

#define MSG_CLASSIC ( 1)
#define MSG_COUNTDOWN ( 2)

class messagePlayer{
    
public:
    messagePlayer();
    messagePlayer(errorManager* e);
    void init();
    void setFadeTime(int millis );
    void setFont(string f);
    void setMessage(string message);
    void setMessageWithCountdown(string message, int count, int duration);
    void setSize(int size);
    void setFadeIn(int duration);
    void setFadeOut(int duration);
    int getAlpha();
    bool getDisplay();
    void setAlpha(int a);
    void clear();
    void update();
    void draw();
    void drawAutoSizedMsg(string msg);
    void drawUnderlinedMsg(string msg);
    void drawCountdownMsg();
    
    string currentMsg; // Final message
    string msgToDisplay; // Message to display, including countdown
    bool isMessageOnScreen();
    bool isFading();
    
    //Error Manager
    errorManager* error;
    
    
    
private:
    
    //Font to display
    ofTrueTypeFont fontBig;
    ofTrueTypeFont fontMedium;
    ofTrueTypeFont fontSmall;
    string fontName;
    int fontSizeBig;
    int fontSizeMedium;
    int fontSizeSmall;
    
    //Fade settings
    int displayMsg; //0 : nothing | MSG_CLASSIC | MSG_COUNTDOWN
    bool messageOnScreen;
    int fadeInDuration;
    int fadeOutDuration;
    int messageDuration;
    float timeMessageReceived;
    float timeMessageClear;
    int countdown; // in sec. Display countdown before printing message. Setted to 0 to display directly
    int currentCountdown;
    int alpha;
    
    
    
};

#endif /* messagePlayer_hpp */
