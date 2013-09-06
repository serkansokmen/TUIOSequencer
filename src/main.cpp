#include "ofMain.h"
#include "App.h"
#include "ofAppGlutWindow.h"


//========================================================================
int main(){
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1280, 960, OF_FULLSCREEN);
	ofRunApp(new App());
}
