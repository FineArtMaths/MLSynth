#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {

	// this should point to the json file containing your audio files and tsne coordinates
	// follow instructions in ml4a.github.io/guides/AudioTSNEViewer/
	tsnePath = ofToDataPath("C:\\Programs\\tSNE\\out-tSNE.json");

	bLoad.addListener(this, &ofApp::eLoad);
	gui.setup();
	gui.setName("Audio t-SNE");
	gui.add(maxDuration.set("maxDuration", 1.0, 0.1, 2.0));
	gui.add(mouseRadius.set("mouseRadius", 250, 100, 500));
	gui.add(pauseLength.set("pauseLength", 2.0, 0.2, 5.0));
	gui.add(bLoad.setup("Load model"));

	load(tsnePath);
}

//--------------------------------------------------------------
void ofApp::eLoad() {
	ofFileDialogResult result = ofSystemLoadDialog("Which xml file to load?", true);
	if (result.bSuccess) {
		load(result.filePath);
	}
}

//--------------------------------------------------------------
void ofApp::load(string filename) {
	tsnePath = filename;

	ofJson js;
	ofFile file(tsnePath);
	parsingSuccessful = file.exists();

	if (!parsingSuccessful) {
		ofLog(OF_LOG_ERROR) << "parsing not successful";
		return;
	}

	sounds.clear();

	file >> js;

	ofVec2f minPoint(1e8, 1e8);
	ofVec2f maxPoint(-1e8, -1e8);
	for (auto & entry : js) {
		if (!entry.empty()) {
			string path = entry["path"];
			float x = entry["point"][0];
			float y = entry["point"][1];
			minPoint.x = min(minPoint.x, x);
			minPoint.y = min(minPoint.y, y);
			maxPoint.x = max(maxPoint.x, x);
			maxPoint.y = max(maxPoint.y, y);
			AudioClip newSound;
			newSound.sound.load(path);
			newSound.point.set(x, y);
			newSound.t = 0;
			sounds.push_back(newSound);
		}
	}

	// normalize the points
	for (int i = 0; i<sounds.size(); i++) {
		sounds[i].point.set(ofMap(sounds[i].point.x, minPoint.x, maxPoint.x, 0, 1),
			ofMap(sounds[i].point.y, minPoint.y, maxPoint.y, 0, 1));
	}
}

//--------------------------------------------------------------
void ofApp::update() {
	if (!parsingSuccessful) {
		return;
	}

	for (int i = 0; i<sounds.size(); i++) {
		if (sounds[i].sound.isPlaying() && sounds[i].sound.getPositionMS() > maxDuration * 1000) {
			sounds[i].sound.stop();
		}
	}
	ofSoundUpdate();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackgroundGradient(ofColor(100), ofColor(20));
	if (!parsingSuccessful) {
		ofDrawBitmapString("Could not find file " + tsnePath + "\nSee the instructions for how to create one.", 50, 50);
		return;
	}
	for (int i = 0; i<sounds.size(); i++) {
		if (sounds[i].sound.isPlaying()) {
			ofSetColor(0, 255, 0, 180);
		}
		else {
			ofSetColor(255, 180);
		}
		ofDrawCircle(ofGetWidth() * sounds[i].point.x, ofGetHeight() * sounds[i].point.y, 4);
	}
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	for (int i = 0; i<sounds.size(); i++) {
		float distanceToMouse = ofDistSquared(x, y, ofGetWidth() * sounds[i].point.x, ofGetHeight() * sounds[i].point.y);
		if (distanceToMouse < mouseRadius && !sounds[i].sound.isPlaying() && (ofGetElapsedTimef() - sounds[i].t > pauseLength)) {
			sounds[i].t = ofGetElapsedTimef();
			sounds[i].sound.play();
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}