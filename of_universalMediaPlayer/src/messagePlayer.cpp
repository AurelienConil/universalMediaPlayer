//
//  messagePlayer.cpp
//  Radiologic
//
//  Created by Admin Mac on 28/07/2019.
//

#include "messagePlayer.hpp"

//------------------------------------------------
messagePlayer::messagePlayer(){
    
    init();
}


//------------------------------------------------
messagePlayer::messagePlayer(errorManager* e){
    
    error = e;
    init();
}

//------------------------------------------------
//                  INIT
//------------------------------------------------
void messagePlayer::init(){
    
    currentMsg = "";
    fontName = "font.ttf";
    //SetSize take as argument the biggest size
    setSize(120);


    //font.setSpaceSize(1);
    //font.setLineHeight(1);
    
    
    //Display
    displayMsg = 0;  //0 : nothing | MSG_CLASSIC | MSG_COUNTDOWN
    timeMessageClear = 0;
    messageOnScreen = false;
    messageDuration = 2;
    fadeInDuration = 3;
    fadeOutDuration = 2;
    alpha = 0;
    
}

//------------------------------------------------
void messagePlayer::setMessage(string message){
    
         
        // If message is empty => clear the current msg
        if(message.size()>1){
                //Force message to be directly display
                currentMsg = message;
                countdown = 0;
                displayMsg = MSG_CLASSIC;
                currentCountdown=0;
                displayMsg = true;
                timeMessageReceived = ofGetElapsedTimef();
                timeMessageClear = 0;
                alpha = 0;
        }
        else{
            clear();
        }
        


}

//------------------------------------------------
void messagePlayer::setMessageWithCountdown(string message, int count){
 
    //if message is empty => clear current  message
    if(message.size()>1){
        currentMsg = message;
        setMessage(message);
        countdown = count;
        displayMsg = MSG_COUNTDOWN;
        currentCountdown = count;
    }else{
        clear();
    }
    
       
}

//------------------------------------------------
void messagePlayer::clear(){
    
    if(getDisplay()){
        
        countdown = 0;
        currentCountdown = 0;
        timeMessageClear = ofGetElapsedTimef();
        alpha = 255;
        
        
    }else{
        //SEND ERROR ... already clearing
        
    }

}

//------------------------------------------------
void messagePlayer::update(){
 
    //TODO : never change video luminosity, but can add a grey filter on top of the video, then draw message.
    
    //UPDATE COUNTDOWN
    /*
     DisplayCountdown = display Msg with countdown
     Using this method, fadeInDuration in not taken into account
     only a count down, written directly on screen
     */
    if(displayMsg == MSG_COUNTDOWN){
        
        //1st part : update countdown
        if((ofGetElapsedTimef() - timeMessageReceived) < countdown){
            currentCountdown = std::floor(countdown - ( ofGetElapsedTimef() - timeMessageReceived ));
            alpha = 255;
        }
        else{
            //2nd part : update msg displayed
            float timeOnMessageDisplayed = timeMessageReceived + countdown;
            //2nd part : update msg displayed
            if(timeMessageClear <= 0){
                alpha = 255;
            }
            //3nd part : fade out msg displayed
            else{
                float timeFromFade = ofGetElapsedTimef() - timeMessageClear;
                float percentage = 1.0 - (timeFromFade/fadeOutDuration);
                alpha = 255*percentage;
                if(alpha <= 0){
                    displayMsg = 0;
                    currentMsg = "";
                }
            }
            
            
        }
    }
    
    if(displayMsg == MSG_CLASSIC){
        
        //1st part : update fadein
        if((ofGetElapsedTimef() - timeMessageReceived) < fadeInDuration){

                float percentage = (ofGetElapsedTimef() - timeMessageReceived)/fadeInDuration;
                alpha = 255*percentage;
        }
        else{
            float timeOnMessageDisplayed = timeMessageReceived + fadeInDuration;
            //2nd part : update msg displayed
            if(timeMessageClear <= 0 ){
                alpha = 255;
            }
            //3nd part : fade out msg displayed
            else{
                float timeFromFade = ofGetElapsedTimef() - timeMessageClear;
                float percentage = 1.0 - (timeFromFade/fadeOutDuration);
                alpha = 255*percentage;
                if(alpha <= 0){
                    displayMsg = 0;
                    currentMsg = "";
                }
            }
            
            
        }
    }
            
}

//------------------------------------------------
void messagePlayer::draw(){
        
    if(displayMsg== MSG_CLASSIC){
        drawAutoSizedMsg(currentMsg);
    }
    if(displayMsg == MSG_COUNTDOWN){
        drawCountdownMsg();
    }
    
    
    //This is not good
    if(alpha> 0){
        messageOnScreen = true;
    }else{
        messageOnScreen =false;
    }
    
    
    
}

//------------------------------------------------
/*
            DRAW CLASSIC MSG
 this function draw a centered msg on screen, adjusting
 the size of the font to fit in.
 3 font size are avalaible, the app try all from the biggest one
 until the size of the bounding box fits the screen
 */
void messagePlayer::drawAutoSizedMsg(string msgToPrint){
    
    if(alpha > 0){
        //DISPLAY MESSAGE ONLY IF ALPHA > 0
        ofSetColor(255, 255, 255, alpha);
        
        //Create margin
        int marginX = 0;
        int marginY = 0;
        
        //TRY THE BIG FONT
        ofRectangle rectBig = fontBig.getStringBoundingBox(msgToPrint, 10, ofGetHeight());
        if(rectBig.width < ofGetWidth()){
            
            marginX = (rectBig.width ) / 2;
            marginY = ( rectBig.height) /2;
            fontBig.drawString(msgToPrint, ofGetWidth()/2 - marginX, ofGetHeight()/2 + marginY);
            
        }
        else
        {
            //TRY MEDIUM FONT
            ofRectangle rectMedium = fontMedium.getStringBoundingBox(msgToPrint, 10, ofGetHeight());
            if(rectMedium.width < ofGetWidth()){
                
                marginX = (rectMedium.width ) / 2;
                marginY = ( rectMedium.height) /2;
                fontMedium.drawString(msgToPrint, ofGetWidth()/2 - marginX, ofGetHeight()/2 + marginY);
                
            }
            else
            {
                //TRY SMALLER FONT
                ofRectangle rectSmall = fontSmall.getStringBoundingBox(msgToPrint, 10, ofGetHeight());
                if(rectSmall.width < ofGetWidth()){
                    
                    marginX = (rectSmall.width ) / 2;
                    marginY = ( rectSmall.height) /2;
                    fontSmall.drawString(msgToPrint, ofGetWidth()/2 - marginX, ofGetHeight()/2 + marginY);
                
                }
                else
                {
                    
                    //SHRINK THE MESSAGE

                    marginY = ( rectSmall.height) /2;
                    while(fontSmall.getStringBoundingBox(msgToPrint+"...", 10, ofGetHeight()).width > ofGetWidth()){
                        msgToPrint = msgToPrint.substr(0, msgToPrint.size()-2);
                    }
                    fontSmall.drawString(msgToPrint+"...", 1, ofGetHeight()/2 + marginY);
                    error->setCurrentError("message too big, try to shrink it");
                }

            }


        }
    
    }//alpha>0
}

//------------------------------------------------
void messagePlayer::drawUnderlinedMsg(string msgToPrint){
 
    ofRectangle rectSmall = fontSmall.getStringBoundingBox(msgToPrint, 10, ofGetHeight());
    if(rectSmall.width < ofGetWidth()){
        
        int marginX = (rectSmall.width ) / 2;
        int marginY = ( rectSmall.height) /2 + ofGetHeight()/4;
        fontSmall.drawString(msgToPrint, ofGetWidth()/2 - marginX, ofGetHeight()/2 + marginY);
    
    }
    
}

//------------------------------------------------
void messagePlayer::drawCountdownMsg(){
    if(alpha > 0){
    
        if(currentCountdown> 0){
            drawAutoSizedMsg(ofToString(currentCountdown));
            drawUnderlinedMsg( "avant "+currentMsg);
        }else{
            drawAutoSizedMsg(currentMsg);
        }
        
    }
}

//------------------------------------------------
void messagePlayer::setSize(int s){
    
    if(s > 12 && s < 150){
        fontSizeBig = s;
        fontSizeMedium = s-30;
        fontSizeSmall = s-60;
        
        setFont(fontName);
        
    }
    
    
}


//------------------------------------------------
void messagePlayer::setFont(string f){
    
    ofFile file = ofFile(f);
    if(file.exists()){
        
        fontBig.load(fontName, fontSizeBig);
        fontMedium.load(fontName, fontSizeMedium);
        fontSmall.load(fontName, fontSizeSmall);
        
    }else{
        
        error->setCurrentError("Font file does not exist");
        
    }
}

//------------------------------------------------
int messagePlayer::getAlpha(){
    
    return alpha;
}

//------------------------------------------------
void messagePlayer::setAlpha(int a){
    
        alpha = a;
    
}

//------------------------------------------------
bool messagePlayer::getDisplay(){
    
    return (displayMsg>0);
    
}

//------------------------------------------------
void messagePlayer::setFadeIn(int duration){
    
    if(duration>0 && duration < 20){
        fadeInDuration = duration;
    }
    else{
        error->setCurrentError("Fade in, out of range value");
    }
    
}

//------------------------------------------------
void messagePlayer::setFadeOut(int duration){
    
    if(duration>0 && duration < 20){
        fadeOutDuration = duration;
    }
    else{
        error->setCurrentError("Fade out, out of range value");
    }
    
}

//-------------------------------------------------
bool messagePlayer::isMessageOnScreen(){
    
    return messageOnScreen;
    
}
