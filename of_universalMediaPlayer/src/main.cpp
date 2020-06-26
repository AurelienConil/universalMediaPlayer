#include "ofMain.h"
#include "PlatformConfig.h"


#include "ofApp.h"

//========================================================================
int main( ){
//       #ifndef TARGET_OPENGLES
//     static bool inited = false;
//     if (!inited) {
//         glewExperimental = GL_TRUE;
//         GLenum err = glewInit();
//         if (GLEW_OK != err)
//         {
//             /* Problem: glewInit failed, something is seriously wrong. */
//             ofLogError("ofAppRunner") << "couldn't init GLEW: " << glewGetErrorString(err);
//             return;
//         }
//         ofLogError("ofAppRunnerGlew");

//         inited = true;
//     }
// #endif
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
    
    //Change to YUV system for B&W video issue
    // https://forum.openframeworks.cc/t/videoplayerexample-on-raspberrypi/18525/14
    ofGLESWindowSettings settings;
#ifndef RADIOLOGIC_OMX
    
    settings.setGLESVersion(2);
#endif
    
    // SET FULLSCREEN ON RPI
    #ifdef __arm__
    settings.windowMode = OF_FULLSCREEN;
    
    #endif
    // SET WINDOWED ON MAC
    #ifdef __APPLE__
    settings.windowMode = OF_WINDOW;
    #endif
	#ifdef _WIN32
	settings.windowMode = OF_WINDOW;
	#endif
    ofCreateWindow(settings);


    // ofSetupOpenGL(1024,768, OF_WINDOW);         // <-------- setup the GL context

    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    cout << "Radiologic updated version";
    ofRunApp( new ofApp());

}
