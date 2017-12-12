
/* Music Visualizer!
 - Samantha Gunzl
 - Used program 4 (heightmapping) base code from CPE 471 - Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "camera.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

#include "OpenAL/OpenAL.h"
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "AL/alut.h"

#include "Accelerate/accelerate.h"

#include <list>

using std::list;

/**** OpenAL setup ****/
#define FREQ 22050   // Sample rate
#define CAP_SIZE 2048 // How much to capture at a time (affects latency)

ALuint helloBuffer[16], helloSource[1];
ALint sample;
ALint availBuffers=0; // Buffers to be recovered
short buffer[FREQ*2]; // A buffer to hold captured audio
ALCint samplesIn=0;  // How many samples are captured
ALuint myBuff; // The buffer we're using
ALuint buffHolder[16]; // An array to hold catch the unqueued buffers
list<ALuint> bufferQueue; // A quick and dirty queue of buffer objects
ALCdevice* inputDevice;
ALCcontext* audioContext;
ALenum errorCode=0;
ALCdevice* audioDevice;

GLubyte texels[100*100*4];
GLubyte amplitudes2[1024];
float amplitudes[1024];
float buf2[2048];

Boolean draw = false;

class Application : public EventCallbacks
{

public:
    
    
	WindowManager * windowManager = nullptr;

	// Our shader programs
	std::shared_ptr<Program> prog, prog2, prog3, prog5, prog6, prog7;

	// Shape to be used (from obj file)
	shared_ptr<Shape> shape, star, skysphere;
	
	//camera
	camera mycam;

	//textures
	GLuint Texture, TextureSky, Texture2;

    //vertex array + buffer objects
	GLuint VertexArrayIDBox, VertexBufferIDBox, VertexBufferTex, VertexBufferNormal;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
        if (key == GLFW_KEY_E && action == GLFW_PRESS)
        {
            mycam.e = 1;
        }
        if (key == GLFW_KEY_E && action == GLFW_RELEASE)
        {
            mycam.e = 0;
        }
        if (key == GLFW_KEY_F && action == GLFW_PRESS)
        {
            mycam.f = 1;
        }
        if (key == GLFW_KEY_F && action == GLFW_RELEASE)
        {
            mycam.f = 0;
        }
        if (key == GLFW_KEY_R && action == GLFW_PRESS)
        {
            mycam.r = 1;
        }
        if (key == GLFW_KEY_R && action == GLFW_RELEASE)
        {
            mycam.r = 0;
        }
        if (key == GLFW_KEY_T && action == GLFW_PRESS)
        {
            mycam.t = 1;
        }
        if (key == GLFW_KEY_T && action == GLFW_RELEASE)
        {
            mycam.t = 0;
        }
    }
    
    void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
    
	void init(const std::string& resourceDirectory)
	{
        
		GLSL::checkVersion();
		
		// Set background color.
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		//culling:
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

		//transparency
		glEnable(GL_BLEND);
		//next function defines how to mix the background color with the transparent pixel in the foreground.
		//This is the standard:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		
		// ocean
		prog2 = make_shared<Program>();
		prog2->setVerbose(true);
		prog2->setShaderNames(resourceDirectory + "/simple_vert2.glsl", resourceDirectory + "/simple_frag2.glsl");
		if (!prog2->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog2->init();
		prog2->addUniform("P");
		prog2->addUniform("MV");
        prog2->addUniform("camPos");
		prog2->addAttribute("vertPos");
		prog2->addAttribute("vertNor");
		prog2->addAttribute("vertTex");

        // moving sphere
        prog3 = make_shared<Program>();
        prog3->setVerbose(true);
        prog3->setShaderNames(resourceDirectory + "/simple_vert3.glsl", resourceDirectory + "/simple_frag3.glsl");
        if (!prog3->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog3->init();
        prog3->addUniform("P");
        prog3->addUniform("MV");
        prog3->addAttribute("vertPos");
        prog3->addAttribute("vertNor");
        prog3->addAttribute("vertTex");
        
        // load sphere obj
        shape = make_shared<Shape>();
        shape->loadMesh(resourceDirectory + "/sphere.obj");
        shape->resize();
        shape->init();
        
        // moving blue sphere
        prog5 = make_shared<Program>();
        prog5->setVerbose(true);
        prog5->setShaderNames(resourceDirectory + "/simple_vert3.glsl", resourceDirectory + "/simple_frag_fade.glsl");
        if (!prog5->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog5->init();
        prog5->addUniform("P");
        prog5->addUniform("MV");
        prog5->addAttribute("vertPos");
        prog5->addAttribute("vertNor");
        prog5->addAttribute("vertTex");
        
        // skysphere
        prog6 = make_shared<Program>();
        prog6->setVerbose(true);
        prog6->setShaderNames(resourceDirectory + "/simple_vertsky.glsl", resourceDirectory + "/simple_fragsky.glsl");
        if (!prog6->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog6->init();
        prog6->addUniform("P");
        prog6->addUniform("MV");
        prog6->addAttribute("vertPos");
        prog6->addAttribute("vertNor");
        prog6->addAttribute("vertTex");
        
        // shooting star
        prog7 = make_shared<Program>();
        prog7->setVerbose(true);
        prog7->setShaderNames(resourceDirectory + "/simple_vertstar.glsl", resourceDirectory + "/simple_fragstar.glsl");
        if (!prog7->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog7->init();
        prog7->addUniform("P");
        prog7->addUniform("MV");
        prog7->addAttribute("vertPos");
        prog7->addAttribute("vertNor");
        prog7->addAttribute("vertTex");
        
        // load star obj
        star = make_shared<Shape>();
        star->loadMesh(resourceDirectory + "/star.obj");
        star->resize();
        star->init();
    }
    
    void initGeom(const std::string& resourceDirectory)
	{
        
        //generate the VAO
		glGenVertexArrays(1, &VertexArrayIDBox);
		glBindVertexArray(VertexArrayIDBox);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDBox);

		GLfloat *ver= new GLfloat[10000 * 18]; //100 x 100 squares * 6 vertices for a square * 3 dimensions
		int verc = 0;
			for (int i = 0; i < 100; i++) {
				for (int j = 0; j < 100; j++) {
                    //x, y, z
					ver[verc++] = 0.0 + j, ver[verc++] = 0.0 + i, ver[verc++] = 0.0;
					ver[verc++] = 1.0 + j, ver[verc++] = 0.0 + i, ver[verc++] = 0.0;
					ver[verc++] = 0.0 + j, ver[verc++] = 1.0 + i, ver[verc++] = 0.0;
					ver[verc++] = 1.0 + j, ver[verc++] = 0.0 + i, ver[verc++] = 0.0;
					ver[verc++] = 1.0 + j, ver[verc++] = 1.0 + i, ver[verc++] = 0.0;
					ver[verc++] = 0.0 + j, ver[verc++] = 1.0 + i, ver[verc++] = 0.0;
				}

		}
				
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, 10000*18*sizeof(float), ver, GL_STATIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferTex);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferTex);

		float t = 1. / 100.;
		GLfloat *cube_tex=new GLfloat[10000 * 12];
		int texc = 0;
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
                //x, y, get z from heightmap image
				cube_tex[texc++] = (GLfloat)j*t, cube_tex[texc++] = (GLfloat)i*t;
				cube_tex[texc++] = (GLfloat)(j + 1)*t, cube_tex[texc++] = (GLfloat)i*t;
				cube_tex[texc++] = (GLfloat)j*t, cube_tex[texc++] = (GLfloat)(i + 1)*t;
				cube_tex[texc++] = (GLfloat)(j + 1)*t, cube_tex[texc++] = 0.0 + (GLfloat)i*t;
				cube_tex[texc++] = (GLfloat)(j + 1)*t, cube_tex[texc++] = (GLfloat)(i + 1)*t;
				cube_tex[texc++] = (GLfloat)j*t, cube_tex[texc++] = (GLfloat)(i + 1)*t;
			}
		}

		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, 10000*12*sizeof(float), cube_tex, GL_STATIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(2);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        /** normals **/
        //generate vertex buffer to hand off to OGL
        glGenBuffers(1, &VertexBufferNormal);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferNormal);
        GLfloat *nor= new GLfloat[10000 * 18]; //100 x 100 squares * 6 vertices for a square * 3 dimensions
        for (int i=0; i<10000*18; i+=3) {
            nor[i] = 0.0;
            nor[i+1] = 0.0;
            nor[i+2] = 1.0;
        }
        
        //actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, 10000*18*sizeof(float), nor, GL_STATIC_DRAW);
        //we need to set up the vertex array
        glEnableVertexAttribArray(1);
        //key function to get up how many elements to pull out at a time (3)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		int width, height, channels;
		char filepath[1000];
        
        // dynamic audio texture
        glGenTextures(1, &Texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // ocean texture
		string str = resourceDirectory + "/oceantex.jpg";
		strcpy(filepath, str.c_str());
        unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
        
        // sky texture
        str = resourceDirectory + "/starstex.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &TextureSky);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, TextureSky);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //set the 2 textures (audio waves and ocean waves) to the correct samplers in the fragment shader:
        GLuint Tex1Location = glGetUniformLocation(prog2->pid, "tex");//tex, tex2... sampler in the fragment shader
        GLuint Tex2Location = glGetUniformLocation(prog2->pid, "tex2");
        // Then bind the uniform samplers to texture units:
        glUseProgram(prog2->pid);
        glUniform1i(Tex1Location, 0);
        glUniform1i(Tex2Location, 1);
        
	}

    
    void render()
    {
        // Get current frame buffer size.
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        glViewport(0, 0, width, height);
        auto P = std::make_shared<MatrixStack>();
        auto MV = std::make_shared<MatrixStack>();
        P->pushMatrix();
        P->perspective(70., width, height, 0.1, 1000.0f);
        glm::mat4 mv ,T, R, S;
        MV->pushMatrix();
        MV->translate(glm::vec3(0,0,-3));
        mv = MV->topMatrix();
        glm::mat4 V = mycam.process();
        
        // Clear framebuffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /**** draw skysphere ***/
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        prog6->bind();
        glUniformMatrix4fv(prog6->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
        glDisable(GL_DEPTH_TEST);
        glFrontFace(GL_CW);
    
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureSky);
        
        float pih = 3.1415926 / 2.0;
        R = glm::rotate(glm::mat4(1), pih, glm::vec3(1, 0, 0));
        T = glm::translate(glm::mat4(1),-mycam.pos);
        mv = V * T * R;
        glUniformMatrix4fv(prog6->getUniform("MV"), 1, GL_FALSE, &mv[0][0]);
        
        shape->draw(prog6);
        
        prog6->unbind();
        glEnable(GL_DEPTH_TEST);
        glFrontFace(GL_CCW);
        
        /**** draw ocean ***/
        
        prog2->bind();
        glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
        
        // Create 'ocean'
        int j=0;
        for (int i=0; i<100; i++) {
            for (int k=0; k<400; k+=4) {
                texels[(400*i)+k] = amplitudes2[j];
            }
            j+=10;
        }
    
        width = height = 100;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Texture2);
        
        glm::mat4 RV = mycam.getR();
        T = glm::translate(glm::mat4(1), glm::vec3(-50, -5, 0));
        RV = glm::transpose(RV);
        pih = -3.1415926 / 2.0;
        R = glm::rotate(glm::mat4(1),pih , glm::vec3(1, 0, 0));
        mv = V * T * R;
        glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, &mv[0][0]);
        glUniform3fv(prog2->getUniform("camPos"), 1, &mycam.pos.x);
        
        glBindVertexArray(VertexArrayIDBox);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glDrawArrays(GL_TRIANGLES, 0, 10000*6);
        prog2->unbind();
        
        /****** draw audio sphere *****/
        prog3->bind();

        glUniformMatrix4fv(prog3->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
        S = glm::scale(glm::mat4(1.0f), glm::vec3(12, 12, 12));
        T = glm::translate(glm::mat4(1), glm::vec3(4, 10, -90));
        mv = V * T * S;
        glUniformMatrix4fv(prog3->getUniform("MV"), 1, GL_FALSE, &mv[0][0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(3);
        shape->draw(prog3);
        
        prog3->unbind();
        
        /*** blue audio sphere ****/
        
        prog5->bind();
        glUniformMatrix4fv(prog5->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
        T = glm::translate(glm::mat4(1), glm::vec3(4, 10, -90));
        if (amplitudes2[100] > 230) {
            S = glm::scale(glm::mat4(1.0f), glm::vec3(18, 18, 18));
        }
        if (amplitudes2[1023] > 230) {
            S = glm::scale(glm::mat4(1.0f), glm::vec3(23, 23, 23));
        }
        mv = V * T * S;
        glUniformMatrix4fv(prog5->getUniform("MV"), 1, GL_FALSE, &mv[0][0]);
        shape->draw(prog5);
        prog5->unbind();
        
        /**** shooting star ****/
        prog7->bind();
        
        glUniformMatrix4fv(prog7->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
        static float x = 0.0;
        T = glm::translate(glm::mat4(1), glm::vec3(-60 + x, 10 + x, -130));
        S = glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2));
        float r = 3.14/4;
        R = glm::rotate(glm::mat4(1.0f), r, glm::vec3(0, 0, 1));
        mv = V * T * R * S;
        glUniformMatrix4fv(prog7->getUniform("MV"), 1, GL_FALSE, &mv[0][0]);
        if (amplitudes2[1023] > 230 && draw == false) {
            x = 0.1;
            star->draw(prog7);
            draw = true;
        }
        if (draw) {
            x += 1.0;
            star->draw(prog7);
            if (x > 40) {
                draw = false;
            }
        }
        
        prog7->unbind();

    }
    
    void fft(short buffer[]) {
        //******* set up fourier transform *******//
        
        //clear texel buffer
        for (int i=0; i<40000; i++) {
            texels[i] = 0;
        }
        
        //create float array from short array
        copy(buffer, buffer + 2048, buf2);
        
        const int n = 2048;
        const int log2n = 11; // 2^11 = 2048
        
        DSPSplitComplex a;
        a.realp = new float[n/2];
        a.imagp = new float[n/2];
        
        // prepare the fft algo (you want to reuse the setup across fft calculations)
        FFTSetup setup = vDSP_create_fftsetup(log2n, kFFTRadix2);
        
        // copy the input to the packed complex array that the fft algo uses
        vDSP_ctoz((DSPComplex *) buf2, 2, &a, 1, n/2);
        
        // calculate the fft
        vDSP_fft_zrip(setup, &a, 1, log2n, FFT_FORWARD);
        
        // do something with the complex spectrum
        int k=0;
        for (size_t i = 0; i < n/2; i++) {
            amplitudes[k] = a.imagp[i];
            k++;
        }
        
        //map each value from 'float' array to size of byte (originally size of short)
        for (int i=0; i<1024; i++) {
            amplitudes2[i] = (amplitudes[i] - SHRT_MIN)/(SHRT_MAX - SHRT_MIN) * 255;
        }
    }
    
    void initAL() {
        audioDevice = alcOpenDevice(NULL); // Request default audio device
        errorCode = alcGetError(audioDevice);
        audioContext = alcCreateContext(audioDevice,NULL); // Create the audio context
        alcMakeContextCurrent(audioContext);
        errorCode = alcGetError(audioDevice);
        // Request the default capture device with a half-second buffer
        inputDevice = alcCaptureOpenDevice(NULL,FREQ,AL_FORMAT_MONO16,FREQ/2);
        errorCode = alcGetError(inputDevice);
        alcCaptureStart(inputDevice); // Begin capturing
        errorCode = alcGetError(inputDevice);
        
        alGenBuffers(16,&helloBuffer[0]); // Create some buffer-objects
        errorCode = alGetError();
        
        // Queue our buffers onto an STL list
        for (int ii=0;ii<16;++ii) {
            bufferQueue.push_back(helloBuffer[ii]);
        }
        
        alGenSources (1, &helloSource[0]); // Create a sound source
        errorCode = alGetError();
    }
    
    void cleanAL() {
        // Stop capture
        alcCaptureStop(inputDevice);
        alcCaptureCloseDevice(inputDevice);
        
        // Stop the sources
        alSourceStopv(1,&helloSource[0]);
        for (int ii=0;ii<1;++ii) {
            alSourcei(helloSource[ii],AL_BUFFER,0);
        }
        
        // Clean-up
        alDeleteSources(1,&helloSource[0]);
        alDeleteBuffers(16,&helloBuffer[0]);
        errorCode = alGetError();
        alcMakeContextCurrent(NULL);
        errorCode = alGetError();
        alcDestroyContext(audioContext);
        alcCloseDevice(audioDevice);
    }
    
};


//*********************************************************************************************************
int main(int argc, char **argv)
{
	// Where the resources are loaded from
	std::string resourceDir = "../../resources";
	if (argc >= 2) {
		resourceDir = argv[1];
	}

	Application *application = new Application();
    
    // Set up window
	WindowManager *windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// Initialize appication
    application->init(resourceDir);
	application->initGeom(resourceDir);
    application->initAL();
    
	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
        alGetSourcei(helloSource[0],AL_BUFFERS_PROCESSED,&availBuffers);
        if (availBuffers>0) {
            alSourceUnqueueBuffers(helloSource[0],availBuffers,buffHolder);
            for (int ii=0;ii<availBuffers;++ii) {
                // Push the recovered buffers back on the queue
                bufferQueue.push_back(buffHolder[ii]);
            }
        }
        
        // Poll for captured audio
        alcGetIntegerv(inputDevice,ALC_CAPTURE_SAMPLES,1,&samplesIn);
        if (samplesIn>CAP_SIZE) {
            // Grab the sound
            alcCaptureSamples(inputDevice,buffer,CAP_SIZE);
            
            //***** Process/filter captured data here *****//
            
            // Stuff the captured data in a buffer-object ---- plays sound back
            if (!bufferQueue.empty()) { // We just drop the data if no buffers are available
                myBuff = bufferQueue.front(); bufferQueue.pop_front();
                alBufferData(myBuff,AL_FORMAT_MONO16,buffer,CAP_SIZE*sizeof(short),FREQ);
                
                // Queue the buffer
                alSourceQueueBuffers(helloSource[0],1,&myBuff);
                
                // Restart the source if needed
                ALint sState=0;
                alGetSourcei(helloSource[0],AL_SOURCE_STATE,&sState);

            }
        }
        
        //render scene
        application->fft(buffer);
        application->render();
        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
        
	}
    
    // quit + clean up program
    application->cleanAL();
	windowManager->shutdown();
	return 0;
    
}
