#include "ofApp.h"

/*
 
TODO GENERALE :
Lire la clé USB depuis cette application
Lire les fichiers timecode depuis cette application
Message ne peut pas baisser la luminosité de la vidéo
Playdirectly once the playlist is done

 
 */

//--------------------------------------------------------------
//                      SETUP
//--------------------------------------------------------------
void ofApp::setup(){
    notifiedVideoClientState = STOPPED;
    error = errorManager();
    video = new vidPlayer(&error);
    video->init();
    message = messagePlayer(&error);
    receiver.setup(OSC_PORT_RECEIVE);
    
    oscsend = new oscSender("127.0.0.1", OSC_PORT_SEND);
    
    //UPDATE POINTER
    video->time.oscsender = oscsend;
    
    //HIDE CURSOR
    ofHideCursor();
    averageOSCSendIntervalMs = 100;
    lastAverageOSCSendTime = 0;
    
    //USB KEY
    /*
     consider movies stored on usb key
     if usbkeyuse is true, universal Media Player is scanning the
     usb port to find files, and add them to playlist.
     */
    usbKeyUse = true;
    usbKeyInserted = false;
    //IMAGE
    imgNoUsbKey.load("usbstick.png");
    imgNoFile.load("nofile.png");
    
    
}

//--------------------------------------------------------------
//                      UPDATE
//--------------------------------------------------------------
void ofApp::update(){

    if(video->getSize() == 0){
        //PLAYLIST IS EMPTY, SCAN
        if(usbKeyUse){
            scanVideoFiles();
        }
        
    }else{
        
        video->update();

    }
    

    
    // PAUSE PLAYER WHILE MESSAGE DISPLAYING
    if(video->pauseOnMessage)
    {
        if(message.isMessageOnScreen()){
            video->pause();
        }else{
            video->resume();
        }
    }
    
    
    //OSC UPDATE
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        processOscMessage(m);
   }
    auto currentVidState = video->getIsPlaying()?PLAYING:STOPPED; // purely asynchronous 
    if(notifiedVideoClientState!=currentVidState){
        oscsend->send("/videoPlayingState",(int)currentVidState);
        notifiedVideoClientState = currentVidState;
        
    }
}

//--------------------------------------------------------------
//                      DRAW
//--------------------------------------------------------------
void ofApp::draw(){
    
    //CLEAR
    ofEnableAlphaBlending();
    ofClear(0);
    ofBackground(0);
    
    //DRAW VIDEO IN AVERAGE COLOR, CALCULATE, SEND OVER OSC
    if(average.isActivated ){
        average.fboBegin();
        ofSetColor(255);
        video->drawMini(AVERAGE_SIZE, AVERAGE_SIZE);
        average.fboEnd();
        average.calculate();
        unsigned long int now =  ofGetElapsedTimef()*1000;
        if ((now - lastAverageOSCSendTime)>averageOSCSendIntervalMs){
            oscsend->send("/averageColor/color", average.smoothR, average.smoothG, average.smoothB);
            lastAverageOSCSendTime = now;
            
        }
    }
    
    //DRAW VIDEO
    //float darkPercentage = message.getAlpha()/255.0f * 100;
    float darkPercentage = 0;
    video->draw(darkPercentage);
    
    // DRAW IMAGE OF USB KEY
    if( usbKeyUse && video->getSize()==0){
        if(!usbKeyInserted){
            imgNoUsbKey.draw((ofGetWidth()-imgNoUsbKey.getWidth())/2, ofGetHeight()*0.3);
        }
        else{
            imgNoFile.draw((ofGetWidth()-imgNoUsbKey.getWidth())/2, ofGetHeight()*0.3);
        }
    }
   

    
    //DRAW AVERAGE COLOR
    if(average.isDraw){
        average.averageFbo.draw(0, 0, 300, 300);
    }
    
    //DRAW MESSAGE
    message.draw();

    //OF SET COLOR ... bug issue
    ofSetColor(255);
    
    // SHOW VERBOSE ONLY IF NECESSARY
    if(!error.isHidden)error.draw();
    
}

//--------------------------------------------------------------
//                      KEY PRESSED
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch(key){
            
        case 'p': video->doPrintPlaylist = !video->doPrintPlaylist;
            break;
        case 'f': video->doPrintFrame = !video->doPrintFrame;
            break;
        case 't': video->time.doPrintTimeCode = !video->time.doPrintTimeCode;
            break;
            
            
            
    }
}

//--------------------------------------------------------------
//                      WINDOW RESIZED
//--------------------------------------------------------------
void ofApp::windowResized(ofResizeEventArgs &resize){
    video->calculateGeometry();
}


//--------------------------------------------------------------
//                 PROCESS OSC MESSAGE
//--------------------------------------------------------------
void ofApp::processOscMessage(ofxOscMessage m){
   
    auto splitted = ofSplitString(m.getAddress(), "/", true);
    
    if(splitted.size()==2 ){
        string toPrint = "\n OSC message :";
        
        // ----------- VIDEO PLAYER ----------------
        if(splitted[0] == "player"){
            toPrint +="player :";
            
            //PAUSE
            if(splitted[1] == "play"){
//                int value = m.getArgAsInt(0);
                video->play();
                toPrint +="play";
            }
            
            // STOP
            if(splitted[1] == "stop"){
//                int value = m.getArgAsInt(0);
                video->stop();
                toPrint +="stop";
            }
    
            //PAUSE
            if(splitted[1] == "pause"){
//                int value = m.getArgAsInt(0);
                video->pause();
                toPrint +="pause ";
            }
            
            //Volume
            if(splitted[1] == "volume"){
                float volume = m.getArgAsFloat(0);
                video->setVolume( volume);
                toPrint +="set volume: "+ofToString(volume);
            }
            //Vertical Flip
            if(splitted[1] == "vflip"){
                int vflip = m.getArgAsFloat(0);
                video->flipV = (vflip > 0);
                video->calculateGeometry();
                toPrint +="set Vflip: "+ofToString(vflip);
            }
            //Horizontal Flip
            if(splitted[1] == "hflip"){
                int hflip = m.getArgAsFloat(0);
                video->flipH = (hflip > 0);
                video->calculateGeometry();
                toPrint +="set Hflip: "+ofToString(hflip);
            }
            //LOAD A FILE -- and add to playlist -- and play it
            if(splitted[1] == "load"){
                string value = m.getArgAsString(0);
                video->loadFile(value);
                toPrint +="load ";
            
            }
            //ADD A FILE --
            if(splitted[1] == "add"){
                string value = m.getArgAsString(0);
                video->addFile(value);
                toPrint +="add ";
                
            }
            // MinimumBrightness in percentage 0% - 100%
            if(splitted[1] == "minBrightness"){
                int value = m.getArgAsInt(0);
                video->setMinBrightness(value);
                toPrint +="minBrigntess ";
                
            }
            // Print playlist
            if(splitted[1] == "printPlaylist"){
                int value = m.getArgAsInt(0);
                video->doPrintPlaylist = !video->doPrintPlaylist;
                toPrint +="printPlaylist ";
                
            }
            // Print playlist
            if(splitted[1] == "printTimeCode"){
                int value = m.getArgAsInt(0);
                video->time.doPrintTimeCode = !video->time.doPrintTimeCode;
                toPrint +="printPlaylist ";
                
            }
            // Print FPS
            if(splitted[1] == "printFPS"){
                int value = m.getArgAsInt(0);
                video->doPrintFPS = !video->doPrintFPS;
                toPrint +="printFPS ";
                
            }
            // Print OMX use
            if(splitted[1] == "printOMXUse"){
                int value = m.getArgAsInt(0);
                video->doPrintOMXUse = !video->doPrintOMXUse;
                toPrint +="printFPS ";
                
            }
            // Print actual FRAME
            if(splitted[1] == "printActualFrame"){
                int value = m.getArgAsInt(0);
                video->doPrintFrame = value>0;
                toPrint +="printActualFrame ";
                
            }
            // PAUSE ON MESSAGE
            if(splitted[1] == "pauseOnMessage"){
                int value = m.getArgAsInt(0);
                video->pauseOnMessage = (value ==1);
                toPrint +="PAUSE ON MESSAGE ";
                
            }
            
            // AUTO NEXT MOVIE ( false by default )
            if(splitted[1] == "autoNext"){
                int value = m.getArgAsInt(0);
                video->autoNext = (value ==1);
                toPrint +="AutoNext ";
                
            }
            
            // ALPHA ( test only )
            if(splitted[1] == "alpha"){
                int value = m.getArgAsInt(0);
                video->alpha = value;
                toPrint +="ALPHA TEST ";
                
            }
        }
        // ----------- MESSAGE PLAYER ----------------
        else if(splitted[0] == "message"){
            toPrint +="message :";
            
            // NEW MESSAGE
            if(splitted[1] == "message"){
                string value = m.getArgAsString(0);
                message.setMessage(value );
                toPrint += "message";
            }
            if(splitted[1] == "clear"){
                message.clear();
                toPrint += "clear";
            }
            if(splitted[1] == "size"){
                int value = m.getArgAsInt(0);
                message.setSize( value);
                toPrint += "size";
            }
            if(splitted[1] == "font"){
                string value = m.getArgAsString(0);
                message.setFont(value);
                toPrint += "font";
            }
            if(splitted[1] == "fadeIn"){
                int value = m.getArgAsInt(0);
                message.setFadeIn(value);
                toPrint += "fadeIn";
            }
            if(splitted[1] == "fadeOut"){
                int value = m.getArgAsInt(0);
                message.setFadeOut(value);
                toPrint += "fadeOut";
            }
            if(splitted[1] == "alpha"){
                int value = m.getArgAsInt(0);
                message.setAlpha(value);
                toPrint += "alpha";
            }
            
  
        }
        // ----------- ERROR - VERBOSE -DISPLAY ----------------
        else if(splitted[0] == "error"){
            toPrint += "error : ";
            
            if(splitted[1] == "info"){
                string value = m.getArgAsString(0);
                error.setCurrentInfo(value);
                toPrint += " info "+value;
            }
            if(splitted[1] == "error"){
                string value = m.getArgAsString(0);
                error.setCurrentError(value);
                toPrint += "error";
            }
            if(splitted[1] == "show"){
                error.isHidden = !error.isHidden;
                toPrint += "show";
            }
            
        }
        // ----------- AVERAGE COLOR----------------
        else if(splitted[0] == "averageColor"){
            toPrint += "averageColor : ";
            
            if(splitted[1] == "activate"){
                average.isActivated = (m.getArgAsInt(0) != 0 );
            }
            if(splitted[1] == "smooth"){
                float value = m.getArgAsFloat(0) ;
                if(value>0 and value<1){
                    average.colorSmooth = value;
                    toPrint += "smooth";
                }
            }
            if(splitted[1] == "draw"){
                average.isDraw = !average.isDraw;
                toPrint += "is draw";
            }
            
        }
        
        error.setCurrentInfo(toPrint);
        
        
    }else {
        error.setCurrentError("receive OSC : bad adress");
    }
    

}

//--------------------------------------------------------------
//                 SCAN VIDEO FILES ON USB KEY
//--------------------------------------------------------------
void ofApp::scanVideoFiles(){
    
    ofDirectory folder;
    string rootDir;
    int usbKeyIndex;
    #ifdef __arm__
    rootDir = "/media/pi";
    usbKeyIndex = 0;
    #else
    rootDir ="/Volumes";
    usbKeyIndex = 1;
    #endif
    
    folder.open(rootDir);
    folder.listDir();
    
    if(folder.listDir()>usbKeyIndex){
        string name = "";
        string usbKeyName = "";
        for(int i = 0; i < folder.size(); i++){
            cout << "USB NAME : "+folder.getPath(i)+"\n";
            name = folder.getPath(i);
            // >1 to avoid "/" folder is osx
            if(name.size()>1){
                usbKeyName = name;
                break;
            }
        }
        if(usbKeyName.size()>0){
            usbKeyInserted = true;
            ofDirectory folderKey;
            folderKey.open(usbKeyName);
            folderKey.allowExt("mp4");
            folderKey.allowExt("mov");
            folderKey.allowExt("MOV");
            ofSleepMillis(400);

            if( !folderKey.canRead()){
                usbKeyInserted = false;
                return;
            }
            folderKey.listDir();
            for(int i = 0; i < folderKey.listDir(); i++){
                cout << "FILE : "+folderKey.getPath(i)+"\n";
                if(i==0){
                    video->loadFile(folderKey.getPath(i));
                }
                else{
                    video->addFile(folderKey.getPath(i));
                }
            }
        }
    }
    else
    {
    usbKeyInserted = false;
    }
    
    
}
