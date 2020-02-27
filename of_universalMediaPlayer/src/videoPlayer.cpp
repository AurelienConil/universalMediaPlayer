//
//  videoPlayer.cpp
//  Radiologic
//
//  Created by Admin Mac on 28/07/2019.
//

#include "videoPlayer.hpp"

//------------------------------------------------------
vidPlayer::vidPlayer(){
    
    init();
}

//------------------------------------------------------
vidPlayer::vidPlayer(errorManager* e){
    
    error = e;
}

//------------------------------------------------------
void vidPlayer::init(){
    
    isPlaying = false;
    isLoaded = false;
    minBrightness = 50;             //In percentage 0% - 100%
    
    //PLAYLIST
    playlist.clear();
    playlistIndex = -1;
    doPrintPlaylist = false;
    doPrintFPS = false;
    doPrintOMXUse = false;
    doPrintFrame = false;
    autoNext = true;
    
    //PAUSE
    pauseOnMessage = false;
    
    //ALPHA TEST
    alpha = 127;
    
    // CHOICE PLAYER
#ifdef RADIOLOGIC_OMX
    isOMXPlayer = true;
#else
    player.setPixelFormat(OF_PIXELS_NATIVE);
    isOMXPlayer = false;
#endif
    
    //GEOMETRY
    videoWidth = 0;
    videoHeight = 0;
    videoRatio = 0;
    screenWidth = ofGetWidth();
    screenHeight = ofGetHeight();
    screenRatio = screenWidth/screenHeight;
    flipV = false;
    flipH = false;
    
    //TIMECODE
    actualFrame = 0;
    time = timeCode();
    
}


//------------------------------------------------------
//                      UPDATE
//------------------------------------------------------
void vidPlayer::update(){

    //FIRST UPDATE PLAYER AND CHECK IS FILE IS ENDED
    bool endOfFile;
#ifndef RADIOLOGIC_OMX
    if(player.isLoaded()){
        player.update();
        actualFrame = player.getCurrentFrame();
        endOfFile =  player.getIsMovieDone();
    }
    else{
       endOfFile =  false;
    }
#else
    // TODO : check first the player is playing
    endOfFile =  player.getCurrentFrame()>= (player.getTotalNumFrames() -1);
    actualFrame =player.getCurrentFrame();
    
    
#endif
    
    time.update(actualFrame);
    
    //IF VIDEO FILE IS ENDED
    if(endOfFile  ){
        if (autoNext )
        {
            if(isPlaying){
                error->setCurrentInfo("Update : end of movie - jump next");
                goNext();
            }
            
        }
        else if(getIsPlaying()){
            stop();
            actualFrame = 0;
        }
    }
}


//------------------------------------------------------
//                      DRAW
// video is darker according to darkPercentage.
// if darkPercentage is 100% , the video goes to MinBrightness value
// if darkPercentrage is 0%, the video is at the standard brightness
//------------------------------------------------------
void vidPlayer::draw( int darkPercentage){

    int minBrightnessValue = minBrightness/100.0f * 255;                        //Apply percentange to 255
    int opacity = (255 - minBrightnessValue )*((100 - darkPercentage)/100.0f);   //darkness
    opacity += minBrightnessValue;
    //Minimum of brightness
    
    ofSetColor(opacity, opacity, opacity, 255);
    
    if(playlist.size()>0){
#ifdef RADIOLOGIC_OMX
        if(player.getIsOpen()&& getIsPlaying())
        {
                player.draw(playerX, playerY, playerW, playerH);
        }
#else
        if(player.isLoaded() && getIsPlaying())
        {
                player.draw(playerX, playerY, playerW, playerH);
        }
#endif
        else{
            ofFill();
            ofSetColor(ofColor::black);
            ofDrawRectangle(playerX, playerY, playerW, playerH);
            // Movie is probably stop
            ofPushMatrix();
            ofSetColor(255);
            ofTranslate ( 30, ofGetHeight()-120);
            int st = 0;
            for(auto & c:"Radiologic"){
                ofDrawBitmapString(ofToString(c),0,st);
                st+=12;
            }

            ofPopMatrix();
            // ofDrawBitmapStringHighlight("Selectionner un film", ofGetWidth()/2, ofGetHeight()/2);
        }
   
    }

    

    // PRINT PLAYLIST
    if(doPrintPlaylist){
        printPlaylist();
    }
    
    // PRINT TIMECODE
    if(time.doPrintTimeCode){
        time.printTimeCode();
    }
    
    // PRINT USE of OMX PLAYER
    if(doPrintOMXUse){
        if(isOMXPlayer){
            ofDrawBitmapString("OMX player", ofGetWidth()/4, ofGetHeight()*0.75 );
        }
        else{
            ofDrawBitmapString("OF videoPlayer", ofGetWidth()/4, ofGetHeight()*0.75 );
        }
    }
    
    //PRINT FPS
    if(doPrintFPS){
        ofDrawBitmapString("of fps"+ofToString(ofGetFrameRate()), ofGetWidth()*0.75, ofGetHeight()*0.75 );
#ifdef RADIOLOGIC_OMX
        ofDrawBitmapString("OMX fps"+ofToString(player.getFPS()), ofGetWidth()*0.75, ofGetHeight()*0.85);
#endif
        
    }
    if(doPrintFrame){
         ofDrawBitmapString("actual FRAME "+ofToString(actualFrame), ofGetWidth()*0.70, ofGetHeight()*0.80);
    }
    
}

//------------------------------------------------------
//              DRAW MINI
// draw the player in a small fbo
// in order to calculate the average color
// The result is not supposed to be displayed
//------------------------------------------------------
void vidPlayer::drawMini(int w , int h){
    
#ifdef RADIOLOGIC_OMX
    if(player.getIsOpen()&& getIsPlaying())
    {
        player.draw(0, 0, w, h);
    }
#else
    if(player.isLoaded() && getIsPlaying())
    {
        player.draw(0, 0, w, h);
    }
#endif
    
}

//------------------------------------------------------
//PLAYLING -  PLAY
// play the movie of selected index.
// playlist_index is choosen from the first movie, after loadMovie call.
//------------------------------------------------------
void vidPlayer::play(){
    
    if(playlistIndex>= 0 && playlist.size()>0){
        
        playIndex(playlistIndex);
        
    }
    
    
}

//------------------------------------------------------
//PLAYLING -  STOP
// Stop and unload the movie
//------------------------------------------------------
void vidPlayer::stop(){
    
#ifdef RADIOLOGIC_OMX
    player.close();
#else
    player.closeMovie();
#endif
    
    isPlaying = false;
    
}

//------------------------------------------------------
//PLAYING - PAUSE
//------------------------------------------------------
void vidPlayer::pause(){
    
#ifdef RADIOLOGIC_OMX
    if(player.getIsOpen() &&  !player.isPaused())
    {
        player.setPaused(true);
    }
#else
    if(player.isLoaded() && !player.isPaused())
    {
        player.setPaused(true);
    }
#endif


    
    
}

//------------------------------------------------------
//ISPLAYING
//------------------------------------------------------

bool vidPlayer::getIsPlaying(){
//    #ifdef RADIOLOGIC_OMX
//#else
    return player.isPlaying();
//#endif

}
//------------------------------------------------------
//PLAYING - RESUME
//------------------------------------------------------
void vidPlayer::resume(){
    
 #ifdef RADIOLOGIC_OMX
    if(player.getIsOpen() &&  player.isPaused())
    {
        player.setPaused(false);
    }
#else
    if(player.isLoaded() && player.isPaused())
    {
        player.setPaused(false);
    }
    
#endif
    
}

//------------------------------------------------------
//PLAYING
// Opacity is the minimum brightness , given in percentage
//------------------------------------------------------
void vidPlayer::setMinBrightness(int percentage){
    
    
    
    if(percentage<0)
        percentage = 0;
    if(percentage>100)
        percentage = 100;
    
    minBrightness = percentage;
    error->setCurrentInfo("change mininum brightness = "+ofToString(minBrightness)+"%");
    
}

//------------------------------------------------------
//PLAYING - SET VOLUMES
// set volume of a movie . What are the bound ??? 1 , 10 ,100 ??
//------------------------------------------------------
void vidPlayer::setVolume(float v){
    
#ifdef RADIOLOGIC_OMX
    if( player.getIsOpen() )
    {
        player.setVolume(v);
    }
#else
    if(player.isLoaded())
    {
        player.setVolume(v);
    }
#endif
    
}


//------------------------------------------------------
//PLAYING - LOAD FILE
//------------------------------------------------------
void vidPlayer::loadFile(string f){
    
    
    ofFile file;
    //CHECK IF FILE EXIST
    if(file.doesFileExist(f) ){
        
        file.open(f);
        //CHECK IS EXTENSION IS CORRECT = NO already done is python programm
        // if( file.getExtension() == "mov" || file.getExtension() == "mp4" || file.getExtension() == "MOV" ){
       if(1){     
            
            //CHECK IF EXIST IN PLAYLIST - IF NOT ADD IT
            int result = isInsidePlaylist(f);
            if (result < 0) {
                playlistIndex = addFile(f);
            }else{
                playlistIndex = result;
            }
            
            //play directly
            playIndex(playlistIndex);
           
            
            
        } else {
            error->setCurrentError("error LOADFILE : mauvaise extension");
        }
    }
    else{
        //file does not exist
        error->setCurrentError("error LOADFILE : fichier n'existe pas");
        isLoaded = false;
        return;
        
    }
    
    
}

//------------------------------------------------------
//PLAYLIST -  ADD   FILE
// return the index of this new file
//------------------------------------------------------
int vidPlayer::addFile(string f){
    
    ofFile file;
    //DOUBLE CHECK IF FILE EXIST
    if(file.doesFileExist(f) ){
        
        file.open(f);
        //CHECK IF EXTENSION IS CORRECT
        if( file.getExtension() == "mov" || file.getExtension() == "mp4" || file.getExtension() == "MOV" ){
            
            //CHECK THAT FILE IS NOT ALREAY ADDED
            int alreadyAdded = -1;
            for (int i = 0; i<playlist.size(); i++){
                if(f == playlist[i] ) alreadyAdded = i;
            }
            
            if(alreadyAdded >= 0){
                error->setCurrentError("vidPlayer : addFile : File already exist");
                return alreadyAdded;
            }else{
                playlist.push_back(f);
                error-> setCurrentInfo("vidPlayer :addFile: "+f+" nb of file : "+ofToString(playlist.size()));
                return (playlist.size() - 1);
            }
            
        }
    }
    
    return -1;
    
    
}


//------------------------------------------------------
//PLAYLIST -  GO NEXT
//------------------------------------------------------
void vidPlayer::goNext(){
    
    if(playlist.size()> 0){
    
        int nextIndex = playlistIndex + 1;
        if(nextIndex>( playlist.size()-1)){
            
            nextIndex = 0;                      //Go to start
        }
        playlistIndex = nextIndex;
        playIndex(playlistIndex);
        
    }else{
        error->setCurrentError("vidPlayer : goNext : playlist is empty");
    }
    
    
    
}


//------------------------------------------------------
//PLAYLIST -  PLAY INDEX
// play the movie here ---
//------------------------------------------------------
void vidPlayer::playIndex(int i){
    
    if( i < playlist.size() && i >= 0){
        
        string name = playlist[i];

#     ifdef RADIOLOGIC_OMX
        ofxOMXPlayerSettings settings;
        settings.videoPath = name;
        settings.useHDMIForAudio = false;    //default true
        settings.enableTexture = true;        //default true
        settings.enableLooping = true;        //default true
        settings.enableAudio = true;        //default true, save resources by disabling
        player.setup(settings);
        isLoaded = player.getIsOpen();
#     else
        player.load(name);
        isLoaded = player.isLoaded();
#     endif
        
        if( isLoaded){
            error-> setCurrentInfo("vidPlayer : playIndex: "+ofToString(playlistIndex));
            calculateGeometry();
            string csvName = (ofSplitString(name, "."))[0]+".csv";
            time.loadFile(csvName);
            isPlaying = true;
#       ifdef RADIOLOGIC_OMX
            player.start();
            player.disableLooping();
            
#       else
            player.setPixelFormat(OF_PIXELS_NATIVE);
            player.play();
            player.setLoopState(OF_LOOP_NONE);
            
#       endif
            
        } else {
            error->setCurrentError("vidPlayer : playIndex : error Loading file");
        }
        
    }else{
        error->setCurrentError("vidPlayer : playIndex : index out of range");
    }
    
}

//------------------------------------------------------
//PLAYLIST - GET CURRENT INDEX
//------------------------------------------------------
int vidPlayer::getCurrentIndex(){
    
    return playlistIndex;
    
}

//------------------------------------------------------
//PLAYLIST - GET SIZE
//------------------------------------------------------
int vidPlayer::getSize(){
    
    return playlist.size();
    
}

//------------------------------------------------------
//PLAYLIST - IS INSIDE PLAYIST
// if file is inside the playlist : return index
// if file is not inside the playlist = return -1
//------------------------------------------------------
int vidPlayer::isInsidePlaylist(string file){
    
    int index = -1;
    for( vector<string>::iterator it = playlist.begin(); it<playlist.end(); it++){
        index ++;
        if( file == *(it)) return index;
        
    }
    return -1;
}

//------------------------------------------------------
//PLAYLIST  - PRINT
//------------------------------------------------------
void vidPlayer::printPlaylist(){
    
    
    ofDrawBitmapString("playlist", ofGetWidth()/2, ofGetHeight()*0.75 - 12);
    for(int i=0; i<playlist.size(); i++){
        if(i == playlistIndex) ofSetColor(ofColor::red);
        else ofSetColor(255);
        ofDrawBitmapString(ofToString(i)+") "+playlist[i], ofGetWidth()/2, ofGetHeight()*0.75 + i*12);
        
    }
    
}

//------------------------------------------------------
//GEOMETRY  - CALCULATE GEOMETRY
// calculate playerX,Y,H,W according to both screen and videoformat
//------------------------------------------------------
void vidPlayer::calculateGeometry(){
    
    videoHeight = player.getHeight();
    videoWidth = player.getWidth();
    videoRatio = videoWidth/videoHeight;
    
    screenWidth = ofGetWidth();
    screenHeight = ofGetHeight();
    
    
    //CONSIDER THAT THE FINAL WIDTH IS THE SCREEN WIDTH
    float originalPlayerW = screenWidth;
    // if ZOOM > 1  : screen is bigger than video
    float zoom = screenWidth/videoWidth;
    float originalPlayerH = (videoWidth/videoRatio)*zoom;
    
    //CONSIDERING THE PREVIOUS POINT
    float originalPlayerX = 0;
    float originalPlayerY = (screenHeight - originalPlayerH) / 2.0f;
    
    //Vertical mirror ( flipV )
    if(flipV){
        playerY = originalPlayerY + originalPlayerH;
        playerH = -originalPlayerH;
    }else{
        playerY = originalPlayerY;
        playerH = originalPlayerH;
    }
    
    //Horizontal mirror ( flipH )
    if(flipH){
        playerX = originalPlayerX + originalPlayerW;
        playerW = -originalPlayerW;
    }else{
        playerX = originalPlayerX;
        playerW = originalPlayerW;
    }
    
    
}
