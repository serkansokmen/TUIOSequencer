#include "ofMain.h"
#include "App.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main(){
    ofAppGlutWindow window;
    
    window.setGlutDisplayString("rgba double depth samples>=4");
    window.setGlutDisplayString("rgba double depth alpha samples>=4");
    
	ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
	ofRunApp(new App());
}
