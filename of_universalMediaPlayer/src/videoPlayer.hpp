//
//  videoPlayer.hpp
//  Radiologic
//
//  Created by Admin Mac on 28/07/2019.
//

#ifndef videoPlayer_hpp
#define videoPlayer_hpp


#include <stdio.h>
#include "ofMain.h"
#include "errorManager.hpp"
#include "timeCode.hpp"
#include "PlatformConfig.h"
#ifdef RADIOLOGIC_OMX
#include "ofxOMXPlayer.h"
#endif

class movie;

class vidPlayer{
    
public :

    vidPlayer();
    vidPlayer(errorManager* e);
    void init();
    
    // CLINIC FUNCTION
    void update();
    void draw(int dark);
    void drawMini(int w, int h);
    bool isOMXPlayer;
    
    // PAUSE WHEN MESSAGE APPEAR
    bool pauseOnMessage;
    
    // PLAYING FUNCTION
    void play();        // playindex(playlistInddex) && controle la lumiere
    void stop();        //unload movie && redonne le contrôle lumière
    void pause();
    void resume();
    void setMinBrightness(int p);
    void setVolume(float p);
    void reduceVolumePercentage(float percent);
    bool autoNext;      // Go to the next movie automagically
    float currentVolume;
    
    //PLAYING FUNCTION
    void loadFile(string file);
    bool getIsLoaded();
    bool getIsPlaying();
    void printPlaylist();
    
    //PLAYLIST FUNCTION
    int  addFile(string file);          //Return index of the added file
    void goNext();
    void selectIndex(int i);
    void playIndex(int i);
    int getCurrentIndex();
    int getSize();
    int isInsidePlaylist(string file);  //Return index in playlist or -1 if does not
	void sendOSCPlaylist();
	void clearPlaylist();
    
    
    //GEOMETRY
    void calculateGeometry();
    
    //DISPLAY
    int alpha;
    
    //ERROR MANAGER POINTER
    errorManager* error;
    
    //IMAGE PNG icon etc...
    ofImage readyToPlay;
    ofImage waitingForSelection;
    
    //TIMECODE
    int actualFrame;
    timeCode time;

	//OSC SENDER
	oscSender* oscsender;
    
    //VIGNETTE MANAGEMENT
    void addVignetteToWebApp(string moviePath, int index);
    void clearAllVignette();
    string vignetteFolderPath;
    
    
    //PRINT EXTRA STUFF
    bool doPrintPlaylist;
    bool doPrintFPS;
    bool doPrintOMXUse;
    bool doPrintFrame; //Still need to code it
    
    //GEOMETRY MEMBER
    float videoWidth;
    float videoHeight;
    float videoRatio;
    float screenWidth;
    float screenHeight;
    float screenRatio;
    float playerX;
    float playerY;
    float playerW;
    float playerH;
    bool flipV;
    bool flipH;
    
    
    //OF Video Player
#ifdef RADIOLOGIC_OMX
    ofxOMXPlayer player;
#else
    ofVideoPlayer player;
#endif
    
    
private:
    bool isPlaying;
    bool isLoaded;
    int minBrightness;   //Percentage 0% - 100%
    vector<movie> playlist;
    int playlistIndex;
    
    
    

    
};

#endif /* videoPlayer_hpp */





class movie {
public:
	movie() {};
	movie(string _name, int _index, string _path) {
		name = _name;
		index = _index;
		path = _path;
		duration = ofRandomf() * 160;
	};
	string name;
	int index;
	string path;
	float duration;
};
