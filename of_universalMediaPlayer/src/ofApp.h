#pragma once
#include "PlatformConfig.h"
#define OSC_PORT_RECEIVE 12343
#define OSC_PORT_SEND 12344
//#define USE_ICON 

#include "ofMain.h"
#include "videoPlayer.hpp"
#include "messagePlayer.hpp"
#include "ofxOscReceiver.h"
#include "oscSender.hpp"
#include "errorManager.hpp"
#include "averageColor.hpp"
#include "json.hpp"
#include "ofParameter.h"
#include "ofxGLWarper.h"

// for convenience
using ofJson = nlohmann::json;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
        void processOscMessage( ofxOscMessage msg);
        void windowResized(ofResizeEventArgs & resize);
        void scanVideoFiles();
		char scanUsbKeyWin32();
		string scanUsbKeyUnix();
#if defined(_WIN32)
#define PLATFORM_NAME "windows" // Windows

#elif defined(__APPLE__)
#endif
        vidPlayer* video;
        messagePlayer message;
        errorManager error;
        ofxOscReceiver receiver;
        averageColor average;
        oscSender* oscsend;

        //SEND VIDEO STATE CONTINUSLY
        int averageOSCSendIntervalMs;
        unsigned long int lastAverageOSCSendTime;
        
        typedef enum{
            STOPPED = 0,
            PLAYING = 1

        } videoState;
        videoState notifiedVideoClientState;
    
        //load the first file when USB is inserted
        bool readFirstFileAtStart;
    
        //QUAD WARPER
        // Quad Warper
        ofxGLWarper warper;
    
        //USE OF USB KEY
        //Extra image
        ofImage imgNoUsbKey;
        ofImage imgNoFile;
        bool usbKeyUse;
        bool usbKeyInserted;
		string usbFolderName;
    
		
};
