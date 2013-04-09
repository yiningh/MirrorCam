#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxBlobTracker.h"
#include "ofxCvOpticalFlowLK.h"
#include "ofxTimer.h"

#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file

class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void blobAdded(ofxBlob &_blob);
    void blobMoved(ofxBlob &_blob);
    void blobDeleted(ofxBlob &_blob);

    ofImage shutter, timer, zoom, frame, shutterh, timerh, zoomh, frameh, qr;
    ofPixels qrPix;

#ifdef _USE_LIVE_VIDEO
    ofVideoGrabber 		vidGrabber;
#else
    ofVideoPlayer 		vidPlayer;
#endif
    ofImage screenShot, thumbnail1, thumbnail2, thumbnail3;
    ofxCvColorImage			colorImg;
    ofxCvGrayscaleImage 	grayImage;
    ofxBlobTracker          blobTracker;
    ofPoint fff;

    int  threshold, snapCounter, timeleftSec, alpha;
    bool shoot, secTimer, bFullscreen, blackout;
    vector <ofImage> thumbnails;
    ofxCvOpticalFlowLK flowFinder;
    ofxCvGrayscaleImage frames[2];
    ofxTimer ofTimer, ofSecTimer, reactTimer, blackoutTimer;
    ofTrueTypeFont myfont;
    char countDown[1];
};

