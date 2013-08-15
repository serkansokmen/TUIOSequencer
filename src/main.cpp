#include "ofMain.h"
#include "App.h"
#include "ofAppGlutWindow.h"


//========================================================================
int main(){
    ofAppGlutWindow window;
    
    window.setGlutDisplayString("rgba double depth samples>=4");
    window.setGlutDisplayString("rgba double depth alpha samples>=4");
    
	ofSetupOpenGL(&window, 1060, 640, OF_WINDOW);
	ofRunApp(new App());
}
