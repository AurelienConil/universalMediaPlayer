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
    autoNext = false;
    
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
    
    //IMAGES
    readyToPlay.load("readytoplay.png");
    waitingForSelection.load("playlistready.png");
    
    //VOLUME
    currentVolume = 1;
    setVolume(currentVolume);
    
    //VIGNETTE
    vignetteFolderPath = "../../../node/public/vignette";
    clearAllVignette();

    
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
        //send osc message percentage timeline
        if(actualFrame%25==0){
            float percentage = actualFrame * 100 /(1.0f * player.getTotalNumFrames()) ;
            oscsender->send("/playPercentage", int(percentage));
        }
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
            ofTranslate ( 0, 0);
            if(playlistIndex>=0 && playlistIndex<playlist.size()){
                readyToPlay.draw(20, 20);
            }
            if(playlistIndex<0 && getSize()>0){
                waitingForSelection.draw(20, 20);
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
         ofDrawBitmapString("actual FRAME "+ofToString(actualFrame)+" / "+ofToString(player.getTotalNumFrames()), ofGetWidth()*0.70, ofGetHeight()*0.80);
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

        string name = playlist[playlistIndex].path;

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
                    error-> setCurrentInfo("vidPlayer : SelectIndex: "+ofToString(playlistIndex));
                    calculateGeometry();
                    string csvName = (ofSplitString(name, "."))[0]+".csv";
                    time.loadFile(csvName);
                    isPlaying = true;
                    oscsender->send("/playIndex", playlistIndex);
                    
                } else {
                    // if not loaded
                    error->setCurrentError("vidPlayer : playIndex : error Loading file");
                }
        
        
        
    }else{
        // Index out of range
        error->setCurrentError("vidPlayer : playIndex : index out of range");
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
    bool isPlayerPlaying = player.isPlaying();
	// There is a problem on windows, player.isPlaying() is not working all the time.
	// but still everything is fine 

	//return isPlayerPlaying;
	return isPlaying;

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
// set volume of a movie . What are the bound : 0 to 5 ? ( need to be sur about it )
//------------------------------------------------------
void vidPlayer::setVolume(float v){
    
    currentVolume = v;
    
#ifdef RADIOLOGIC_OMX
    if( player.getIsOpen() )
    {
        player.setVolume(currentVolume);
    }
#else
    if(player.isLoaded())
    {
        player.setVolume(currentVolume);
    }
#endif
    
}

//------------------------------------------------------
//PLAYING - REDUCE VOLUME
// reduce temporarly the current volume in percentage
//------------------------------------------------------
void vidPlayer::reduceVolumePercentage(float percent){
    
    float temporaryVolume = currentVolume*percent/100.0f;
    
#ifdef RADIOLOGIC_OMX
    if( player.getIsOpen() )
    {
        player.setVolume(temporaryVolume);
    }
#else
    if(player.isLoaded())
    {
        player.setVolume(temporaryVolume);
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

			// At this point player should be playing

            
            
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
        if( file.getExtension() == "mov" || file.getExtension() == "mp4" || file.getExtension() == "MOV" || file.getExtension() == "avi" || file.getExtension() == "mkv"  ){
            
            //CHECK THAT FILE IS NOT ALREAY ADDED
            int alreadyAdded = -1;
            for (int i = 0; i<playlist.size(); i++){
                if(f == playlist[i].path ) alreadyAdded = i;
            }
            
            if(alreadyAdded >= 0){
                error->setCurrentError("vidPlayer : addFile : File already exist");
                return alreadyAdded;
            }else{
				string name = file.getFileName();

                int movieIndex = playlist.size();
				movie newMov = movie(name, movieIndex, f);
				playlist.push_back(newMov);
                
                addVignetteToWebApp(f, movieIndex);
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
//PLAYLIST -  SELECT INDEX
// Select on playlist and wait for play ---
//------------------------------------------------------
void vidPlayer::selectIndex(int i){
    
    if( i < playlist.size() && i >= 0){
            
        
        playlistIndex = i;
                    
    }else{
        // Index out of range
        error->setCurrentError("vidPlayer : playIndex : index out of range");
    }
    
}

//------------------------------------------------------
//PLAYLIST -  PLAY INDEX
// Select then play directly
//------------------------------------------------------
void vidPlayer::playIndex(int i){
    
    selectIndex(i);
    play();
    
}

//------------------------------------------------------
//PLAYLIST - GET CURRENT INDEX
//------------------------------------------------------
int vidPlayer::getCurrentIndex(){
    
    return playlistIndex;
    
}

//------------------------------------------------------
//PLAYLIST - GET SIZE OF PLAYLIST
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
    for( vector<movie>::iterator it = playlist.begin(); it<playlist.end(); it++){
        index ++;
        if( file == it->path) return index;
        
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
        ofDrawBitmapString(ofToString(i)+") "+playlist[i].name, ofGetWidth()/2, ofGetHeight()*0.75 + i*12);
        
    }
    
}

//------------------------------------------------------
//PLAYLIST  - SEND OVER OSC
//------------------------------------------------------
void vidPlayer::sendOSCPlaylist() {

	for (int i = 0; i < playlist.size(); i++) {

		oscsender->send("/addMovie", playlist[i].name, i);
		//ofSleepMillis(1); maybe uselfull in term of thousand of files 
	}

}



//------------------------------------------------------
//PLAYLIST  - CLEAR
//------------------------------------------------------
void vidPlayer::clearPlaylist() {

	playlist.clear();
	player.close();
	isLoaded = false;
	isPlaying = false;

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

//------------------------------------------------------
//VIGNETTE  - ADD VIGNETTE TO WEBAPP
// look for a jpg file next to the movie file, then copy it to webapp
//------------------------------------------------------
void vidPlayer::addVignetteToWebApp(string moviePath, int index){
    
    ofFile file;
    string vignetteFilePath = ofSplitString(moviePath, ".")[0]+".jpg";
     if(file.doesFileExist(vignetteFilePath) ){
         file.open(vignetteFilePath);
         cout << "vignette Path is : "+vignetteFilePath;
         file.copyTo(vignetteFolderPath+"/"+ofToString(index)+".jpg");
         
     }else{
         //if there is no vignette , copy the default vignette to vignette folder path
         ofFile defaultVignette;
         defaultVignette.open("defaultvignette.jpg");
         defaultVignette.copyTo(vignetteFolderPath+"/"+ofToString(index)+".jpg");
         
     }
    
}

//------------------------------------------------------
//VIGNETTE  - CLEAR ALL VIGNETTE
// look for a jpg file into web app vignette folder and delete them
//------------------------------------------------------
void vidPlayer::clearAllVignette(){
    
    ofDirectory dir;
    dir.open(vignetteFolderPath);
    if(dir.isDirectory() ){
        if(dir.listDir()>0){
            std::vector<ofFile> listOfFiles = dir.getFiles();
            for(std::vector<ofFile>::iterator it = listOfFiles.begin(); it < listOfFiles.end(); it++ ){
                it->remove();
            }
        }
    }
    
}
