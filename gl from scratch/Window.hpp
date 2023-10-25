//
//  Window.hpp
//  Rendering
//
//  Created by Isobel Mahon on 10/04/2023.
//

#ifndef Window_hpp
#define Window_hpp

#include <stdio.h>

class Window {
public:
	Window(int width, int height);
	GLFWwindow* window;
	void setSize(int width, int height);
	void processInput();
	
//	void dragAndDropLoop();
//	const char* filePath;

private:
	void createWindow(int width, int height);
	void setup();
	bool initGlad();
//	void drop_callback();
	//	void loop();
};

#endif /* Window_hpp */
