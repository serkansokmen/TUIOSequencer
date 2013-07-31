#include "ofMain.h"
#include "DetectorApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main(){
    ofAppGlutWindow window;
    
    window.setGlutDisplayString("rgba double depth samples>=4");
    window.setGlutDisplayString("rgba double depth alpha samples>=4");
    
	ofSetupOpenGL(&window, 1440, 900, OF_FULLSCREEN);
	ofRunApp(new DetectorApp());
}
