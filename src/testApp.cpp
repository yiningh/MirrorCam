#include "testApp.h"

#define NUM 10000
float xpos[NUM], ypos[NUM];
float a[NUM], d[NUM];

void drawFrame(){
    ofPushStyle();
    ofFill();
    ofSetColor(245);
    ofRect(0, 0, 80, 10);
    ofRect(0, 0, 10, 80);
    ofPopStyle();
}

//--------------------------------------------------------------
void testApp::setup(){
    for(int i = 0; i < NUM; i++){
        xpos[i] = ofRandom(ofGetWidth());
        ypos[i] = ofRandom(ofGetHeight());
        a[i] = 1;
        d[i] = 1;
    }
    ofBackground(250);
    ofSetBackgroundAuto(true);

	#ifdef _USE_LIVE_VIDEO
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(320,240);
	#else
        vidPlayer.loadMovie("fingers2.mov");
        vidPlayer.play();
	#endif
    flowFinder.allocate(320,240);
    frames[0].allocate(320,240);
    frames[1].allocate(320,240);
    colorImg.allocate(320,240);
	grayImage.allocate(320,240); //<--max BlobTracking area (340, 240)

	threshold = 40;

    ofAddListener(blobTracker.blobAdded, this, &testApp::blobAdded);
    ofAddListener(blobTracker.blobMoved, this, &testApp::blobMoved);
    ofAddListener(blobTracker.blobDeleted, this, &testApp::blobDeleted);

    shutter.loadImage("shutter.png");
    shutterh.loadImage("shutter-h.png");
    timer.loadImage("timer.png");
    timerh.loadImage("timer-h.png");
    zoom.loadImage("zoom.png");
    qr.loadImage("qr2.png");
    qrPix = qr.getPixelsRef();

    snapCounter = 1;
    secTimer = false;
    ofTimer.setup(600, false);
    ofSecTimer.setup(3000, false);
    reactTimer.setup(600, false);
    blackoutTimer.setup(600, false);
    myfont.loadFont("DagnyPro.otf", 200);
    alpha = 50;
    blackout = false;
}

//--------------------------------------------------------------
void testApp::update(){
    
    vidGrabber.update();
	if (vidGrabber.isFrameNew()){
		#ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixelsRef());
            colorImg.mirror(false, true);
	    #else
            colorImg.setFromPixels(vidPlayer.getPixels(), 720,576);
        #endif
        frames[0] = frames[1];
        frames[1] = colorImg;
        flowFinder.calc(frames[1], frames[0],3);
        ofPoint f = flowFinder.flowInRegion(0,0,80,80);
        ofPoint fTimer = flowFinder.flowInRegion(310,10,40,40);
        fff = flowFinder.flowInRegion(0,0,320,240);
        grayImage = colorImg;
        blobTracker.update(grayImage, threshold);

        for (int i = 0; i < blobTracker.size(); i++){
            int circleX = blobTracker[i].centroid.x * 960;
            int circleY = blobTracker[i].centroid.y * 720; 
            ofFill();
            ofSetColor(255,0,0);
            ofCircle(circleX,circleY,10);
            ofSetColor(0);
            ofDrawBitmapString(ofToString( blobTracker[i].id ),
                               blobTracker[i].centroid.x * ofGetWidth(),
                               blobTracker[i].centroid.y * ofGetHeight());
            if(ofDist(circleX, circleY, 80, 130) <= 90){
                if( f.length() > 200 ){
                        shoot = true;
                }
            } 
            if(ofDist(circleX, circleY, 860, 100) <= 100){
                if( fTimer.length() > 200 ){
                    reactTimer.startTimer();
                    if(reactTimer.isTimerFinished()){
                        reactTimer.stopTimer();
                        reactTimer.reset();
                        secTimer = !secTimer;
                    }
                }
            } 
        }
	}

    if(secTimer){
        if(shoot){
            secTimer = true;
            timeleftSec = ofSecTimer.getTimeLeftInSeconds();
            cout << timeleftSec << endl;
            ofSecTimer.startTimer();
            if(ofSecTimer.isTimerFinished()){
                blackout = true;
                ofSecTimer.stopTimer();
                screenShot = colorImg.getPixelsRef();
                ofSetDataPathRoot("/Users/testuser/Desktop/of_v0073/apps/cam/YNcam1202_4/bin/data/photos/");
                screenShot.saveImage("photo"+ofToString(snapCounter)+ ofToString(ofGetDay())+ ofToString(ofGetHours())+ ofToString(ofGetMinutes())+ofToString(ofGetSeconds())+".jpg");
                snapCounter++;
                thumbnails.push_back(screenShot);
                for(int i = 0; i < thumbnails.size(); i++ ){
                    if(thumbnails.size() > 3){
                        thumbnails.erase( thumbnails.begin() + i );
                    }
                }
                cout << ofGetFrameNum() <<"shooooooooooooooooot" << endl;
                for (int i = 0; i < NUM; i++){
                    d[i] = 0;
                    a[i] = 0;
                }
                ofSecTimer.reset();
                shoot = false;
            }
        }
    }else if(secTimer == false){
    
	if(shoot){
        int timeleft = ofTimer.getTimeLeftInMillis();
        cout << timeleft << endl;
        ofTimer.startTimer();
        if(ofTimer.isTimerFinished()){
            blackout = true;
            ofTimer.stopTimer();
            screenShot = colorImg.getPixelsRef();
            ofSetDataPathRoot("/Users/testuser/Desktop/of_v0073/apps/cam/YNcam1202_4/bin/data/photos/");
            screenShot.saveImage("photo"+ofToString(snapCounter)+ ofToString(ofGetDay())+ ofToString(ofGetHours())+ ofToString(ofGetMinutes())+ofToString(ofGetSeconds())+".jpg");
            snapCounter++;
            thumbnails.push_back(screenShot);
            for(int i = 0; i < thumbnails.size(); i++ ){
                if(thumbnails.size() > 3){
                    thumbnails.erase( thumbnails.begin() + i );
                }
            }
            cout << ofGetFrameNum() <<"shooooooooooooooooot" << endl;
            ofTimer.reset();
            shoot = false;
        }
	}
        
    }
}

//--------------------------------------------------------------
void testApp::draw(){
	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
    //grayImage.draw(20,20);
    colorImg.draw(0, 0, 960, 720);
    
    for (int i = 0; i < blobTracker.size(); i++){
        int circleX = blobTracker[i].centroid.x * 960;
        int circleY = blobTracker[i].centroid.y * 720;
        
        for (int i = 0; i < NUM; i++){
            if (ofDist(xpos[i], ypos[i], circleX, circleY) < 500 ){
                a[i] += ofMap(ofDist(xpos[i], ypos[i], circleX, circleY), 0, 500, 4*PI, PI)/300;
                d[i] = ofMap((ofDist(xpos[i], ypos[i], circleX, circleY)), 0, 500, -25, 20);
            }
        }
        
        for (int i = 0; i < NUM; i++){
            d[i] *= 0.98;
            if(d[i] < 0){
                d[i] = 0;
            }
        }
        if(fff.length()>=80){
            d[i] = 0;
            a[i] = 0;
        }
        for(int i =0; i <NUM; i++){
            ofSetColor(255);
            ofLine(xpos[i],ypos[i], xpos[i] + d[i] * cos(a[i]), ypos[i] + d[i] * sin(a[i]));
        }
    }

    
	for(int y = 0; y < qrPix.getHeight(); y++){
        for(int x = 0; x < qrPix.getWidth(); x++){
            ofColor c = qrPix.getColor(x,y);
            ofSetColor(c);
            ofRect(980+x,620+y, 1,1);
        }
    }

    ofEnableAlphaBlending();
    if(blackout){
        blackoutTimer.startTimer();
        ofSetColor(255,255,255,200);
        ofRect(0,0,960,720);
        if(blackoutTimer.isTimerFinished()){
            blackoutTimer.stopTimer();
            blackoutTimer.reset();
            blackout = false;
        }
        
    }
    if(shoot){
        shutterh.draw(30,60);
    }else{
        shutter.draw(30,60);
    }
	//zoom.draw(855, 252);
    if(secTimer == false){
        timer.draw(855, 62);
    }else 
    if(secTimer == true){
        timerh.draw(855, 52);
    }
    ofPushMatrix();
    ofScale(-1,1);
    ofSetColor(255, 255, 255, 80);
    if(secTimer){
        if(shoot){
            if( timeleftSec == 2){
                myfont.drawString("3", -500, 410);
            }
            if(timeleftSec == 1){
                myfont.drawString("2", -500, 410);
            }
            if(timeleftSec == 0){
                myfont.drawString("1", -500, 410);
            }
            else{ myfont.drawString(" ", -500, 410);
            }
        }
    }
    ofPopMatrix();
    
    ofPushMatrix();
    ofScale(0.7, 0.7);
    if(thumbnails.size() <3 ){
        if (thumbnails.size() < 2){
            for(int i = 0; i < thumbnails.size(); i++){
                for(int y = 0; y < thumbnails[i].getHeight(); y++){
                    for(int x = 0; x < thumbnails[i].getWidth(); x++){
                        ofColor c = thumbnails[i].getColor(x,y);
                        ofSetColor(c);
                        ofRect(1415+x,35+y, 1,1);
                    }
                }
            }
        }
        
        if (thumbnails.size() == 2){
            for(int i = 0; i < thumbnails.size(); i++){
                for(int y = 0; y < thumbnails[i].getHeight(); y++){
                    for(int x = 0; x < thumbnails[i].getWidth(); x++){
                        ofColor c = thumbnails[i].getColor(x,y);
                        ofSetColor(c);
                        ofRect(1415+x,315+i*-280 + y, 1,1);
                    }
                }
            }
        }
    }else{
        for(int i = 0; i < thumbnails.size(); i++){
            for(int y = 0; y < thumbnails[i].getHeight(); y++){
                for(int x = 0; x < thumbnails[i].getWidth(); x++){
                    ofColor c = thumbnails[i].getColor(x,y);
                    ofSetColor(c);
                    ofRect(1415+x,595+ i*-280+y, 1,1);
                }
            }
        }
    }
    ofPopMatrix();
    
	ofPushMatrix();
	ofTranslate(150,90);
	drawFrame();
	ofPopMatrix();

    ofPushMatrix();
	ofTranslate(795,90);
	ofScale(-1,1);
	drawFrame();
	ofPopMatrix();

	ofPushMatrix();
	ofTranslate(795,665);
	ofScale(-1,-1);
	drawFrame();
	ofPopMatrix();

	ofPushMatrix();
	ofTranslate(150,665);
	ofScale(1,-1);
	drawFrame();
	ofPopMatrix();
	ofDisableAlphaBlending();

	// then draw the contours:
//    ofPushMatrix();
//    ofScale(3,3);
//	blobTracker.draw(0,0);
//    //contourFinder.draw();
//    ofPopMatrix();
}

void testApp::blobAdded(ofxBlob &_blob){
    //ofLog(OF_LOG_NOTICE, "Blob ID " + ofToString(_blob.id) + " added" );
}

void testApp::blobMoved(ofxBlob &_blob){
    //ofLog(OF_LOG_NOTICE, "Blob ID " + ofToString(_blob.id) + " moved" );
}

void testApp::blobDeleted(ofxBlob &_blob){
    //ofLog(OF_LOG_NOTICE, "Blob ID " + ofToString(_blob.id) + " deleted" );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key){
		case ' ':
			blobTracker.bUpdateBackground = true;
            for(int i = 0; i < NUM; i++){
                a[i] = 1;
            }
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    if(key == 'c'){
        blackout = true;
        screenShot = colorImg.getPixelsRef();
        ofSetDataPathRoot("Users/testuser/Desktop/of_v0073/apps/cam/YNcam1202_4/bin/data/photos/");
        screenShot.saveImage("photo"+ofToString(snapCounter)+ofToString(ofGetDay())+ ofToString(ofGetHours())+ ofToString(ofGetMinutes())+ofToString(ofGetSeconds())+".jpg");
        snapCounter++;
        thumbnails.push_back(screenShot);
        for(int i = 0; i < thumbnails.size(); i++ ){
            if(thumbnails.size() > 3){
                thumbnails.erase( thumbnails.begin() + i );
            }
        }
    }

    if(key == 'f'){
            bFullscreen = !bFullscreen;
            if(!bFullscreen){
                ofSetWindowShape(1150,720);
                ofSetFullscreen(false);
            } else if(bFullscreen == 1){
                ofSetFullscreen(true);

            }
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
