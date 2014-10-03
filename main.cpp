// The skeleton code for this project is a mixture of Jon Ventura's and Chris Sweeney's
// CS290I Homework 1 and http://www.videotutorialsrock.com/opengl_tutorial/terrain/text.php
// Modifications: Aleksandar, Fatih and Viktor (HS Karlsruhe Autonome Systems Lab) :-3

/*
 * FIXME:
 * 1)(FIXED): quiting window -> some kind of conflict between OpenCV and GLUT maybe but even when
 *	 specified key for quitting is pressed windows stays on screen, program doesn't close
 *	 but image is still (no new frames are captured)...weird...
 * 2)(FIXED): forgot to use tempimage.release())somewhere there is a HUGE memory leak. so far i have found out that the terrain-generator
 *   (used the separate project to abstract from OpenCV completely!) does increase memory usage
 *   as times goes by (couple of MB/s !!!), which means that it needs to be profiled (dunno how
 *   to this in Visual Studio with C++ since i've done profiling mostly in Eclipse and VS with C#)
 *   and patched; i've also compared the memory leak here and in the terrain-generator-only project
 *   and it seems that it's the same so the problem might not be enforced by incorrect data management
 *   here when using OpenCV (but still do check if a memory leak does occur in idle() where we
 *   capture new data although it's unlikely (considering that it basically follows plenty of online
 *   examples incl. those in the official OpenCV documentation))
 */


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <unistd.h>	// for sleep()

#define OPENGL_OPENCV_TEST
#define DRAW_OPENCV_FRAME	//replace with boolean trigger and keyboard event
#define DRAW_WORLD_AXES		//replace with boolean trigger and keyboard event
//#define ENABLE_TERRAIN_ROTATION
#define ENABLE_LIGHT_SOURCE_ROTATION	//replace with boolean trigger (also sun<-> ambient switch) and keyboard event
//#define SHOW_LIGHT_SOURCE
//#define OPENCV_WEBCAM
//#define TERRAIN_CLASS_TEST
//#define TERRAIN_FROM_OPENCV

// when creating a light source that works like a sun and goes above our terrain
// a problem may occur such as the sun going through the terrain if we have a fixed
// translation. so we set our sun using a couple of parameters; we translate the
// sun only along a single world axis (in our case - Y); as parameters we take
//		height = TERRAIN_HEIGHT (from Terrain.h)
//		color = 255 (the max color value in grayscale)
//
// the formula for the final translation is:
// sun_translation = height * ((color / 255.0f) - 0.5f) =
//				   = TERRAIN_HEIGHT * ((255 / 255.0f) - 0.5f) =
//				   = TERRAIN_HEIGHT * 0.5f
#define LIGHT_SOURCE_TRANSLATION_TO_TERRAIN_PEAK (TERRAIN_HEIGHT * 0.5f)
#define LIGHT_SOURCE_TRANSLATION_ABOVE_PEAK 0.2f
#define LIGHT_SOURCE_TRANSLATION_FULL (LIGHT_SOURCE_TRANSLATION_TO_TERRAIN_PEAK + LIGHT_SOURCE_TRANSLATION_ABOVE_PEAK)

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <cstdio>
#include <math.h>

#include "../include/Terrain.h"
#include "../include/Lighting.h"
#include "../include/Colouring.h"

// OpenGL and terrain
int glutWindowHandle = 0;	// used for closing GLUT windows
Terrain *_terrain;	// stores generated terrain that we'll display in our scene
float angleX = 0.0f;	// for rotation of the terrain around the X-axis of origin
float angleY = 0.0f;	// for rotation of the terrain around the Y-axis of origin
float angleZ = 0.0f;	// for rotation of the terrain around the Z-axis of origin
float angleLightSource = 60.0f;	// for the sun that brings light into our lives :P
float zoomView = -5.0f;	// zoom in/out along the Z-axis using gluLookAt()
bool toggleWireframe = false;	// turns on/off wireframe rendering for our terrain
bool invertTerrain = false; // turns on/off inversion of our terrain; if == true then grayscale colour 255 becomes 0 and vice versa
bool toggleColouring = false; // turns on/off colouring of terrain based on its height
bool toggleOpenCvOutput = false; // turns on/off rendering of captured image
bool toggleAmbientLight = false;	// turns on/off ambient light; if == true then sun stops rotating and emitting light
// TODO add trigger for enabling/disabling ambient light (if enabled -> exclude rendering of sun all together)
// FIXME toggling light works with a delay or it's because of the fixed rotation trajectory of the sun; make the sun rotate at the same angle around the same axis of our terrain
//		  so that rotation of terrain does not interfere with the lighting
// TODO add trigger for procedurally generated little trees on top of our terrain
// TODO add generator (in Terrain.c !) for trees (L-system) on the surface of the terrain; use the vertex data HERE (in drawScene()) to place them on random triangles
//		use the edges (vertices) of a triangle to calculate its center point. From there make those trees grow :-3 In addition observe the slope of a vertex - if it's
//		semi-steep, then create a tree that goes mostly horizontally (as if it's hanging from a cliff). If it's too steep - we have a cliff so don't add trees (maybe some
//		that are completely horizontal or bushes. Type of tree/bush should depend not only on slope but also on terrain's height where it grows!

// OpenCV
cv::VideoCapture *cap = NULL;
int width = 400;
int height = 400;
cv::Mat image;

// misc
std::string keyLayout = "Controls:\n?\tshow controls\n+/-\tzoom in/out with factor +/-1\n8/2\trotate around X with factor +/-2 DEG\n4/6\trotate around Y with factor +/-2 DEG\n7/9\trotate around Z with factor +/-2 DEG\nc\ttoggle colouring\nw\ttoggle wireframe\ns\ttoggle ambient light\ni\tinvert (255->0, 0->255 and vice versa)\no\ttoggle display of OpenCV image\nESC\tquit";

// a useful function for displaying your coordinate system
void drawAxes(float length)
{
    glDisable(GL_DEPTH_TEST);	//glPolygonMode() requires that we turn depth buffer off
    glDisable(GL_LIGHTING);
    glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT) ;

    // http://www.opengl.org/sdk/docs/man/html/glPolygonMode.xhtml
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) ;

    glPushMatrix();
    // position the coordinate system bottom, left (user's perspective)
    glTranslatef(2.6, -2.7, 0);
    /*GLfloat m[16] = {
            1,0,0,2.6,
            0,1,0,-2.7,
            0,0,1,0,
            0,0,0,1
    };*/
    //glMultMatrixf(m);	// multiply last state matrix of OpenGL with our matrix

    // x
    glBegin(GL_LINES) ;
    glColor3f(1,0,0) ;
    glVertex3f(0,0,0) ;
    glVertex3f(length,0,0);

    // y
    glColor3f(0,1,0) ;
    glVertex3f(0,0,0) ;
    glVertex3f(0,length,0);

    // z
    glColor3f(0,0,1) ;
    glVertex3f(0,0,0) ;
    glVertex3f(0,0,length);
    glEnd() ;

    glPopMatrix();
    glPopAttrib() ;

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void display()
{
    // clear the window
    glClear( GL_COLOR_BUFFER_BIT );

#if defined(DRAW_WORLD_AXES)
    drawAxes(1.0);
#endif

    // show the current camera frame

    // based on the way cv::Mat stores data, you need to flip it before displaying it
    cv::Mat tempimage;
    cv::flip(image, tempimage, 0);

    if(toggleOpenCvOutput)
    {
        // changed GL_BGR_EXT to GL_LUMINANCE in order to handle the grayscale images from OpenCV
        // glDrawPixels() requires that we turn depth buffer off
        // sleep(100);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDrawPixels( tempimage.size().width, tempimage.size().height, GL_LUMINANCE, GL_UNSIGNED_BYTE, tempimage.ptr() );
        glEnable(GL_DEPTH_TEST);
    }

    // here, set up new parameters to render a scene viewed from the camera.

    // set viewport
    glClear(GL_DEPTH_BUFFER_BIT);

    // set projection matrix using intrinsic camera params
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // gluPerspective is arbitrarily set, you will have to determine these values based
    // on the intrinsic camera parameters
    // http://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml
    // gluPerspective(60, tempimage.size().width*1.0/tempimage.size().height, 1, 50);
    gluPerspective(60, image.size().width*1.0/image.size().height, 1, 50);

    // you will have to set modelview matrix using extrinsic camera params
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // http://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
    //gluLookAt(0, 0, -5, 0, 0, 0, 0, 1, 0);
    gluLookAt(0, 0, zoomView, 0, 0, 0, 0, 1, 0);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
    glEnable(GL_LIGHTING);

    if(!toggleAmbientLight)
    {
        // rotating light source
        glPushMatrix();
        // glTranslatef(O.x,O.y,O.z);
        glRotatef(angleLightSource,0,0,1);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        // glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
        glColor3f(1.0,0.0,0.0);
        glTranslatef(-0.5f, LIGHT_SOURCE_TRANSLATION_FULL, 0.1f);
    #if defined(SHOW_LIGHT_SOURCE)
        glutSolidSphere(0.1, 100.0, 100.0);
    #endif
        glPopMatrix();
    }

    // now that the camera params have been set, draw your 3D shapes
    // first, save the current matrix
    glPushMatrix();
    // move to the position where you want the 3D object to go
    glTranslatef(0, 0, 0); // this is an arbitrary position for demonstration
    // you will need to adjust your transformations to match the positions where
    // you want to draw your objects(i.e. chessboard center, chessboard corners)

    // here we render our terrain
    // rotate the whole terrain to make it better visible for the user
    glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    glRotatef(angleZ, 0.0f, 0.0f, 1.0f);
    float scale = 5.0f / std::max(_terrain->width() - 1, _terrain->length() - 1);
    glScalef(scale, scale, scale);
    glTranslatef(-(float)(_terrain->width() - 1) / 2,
                 0.0f,
                 -(float)(_terrain->length() - 1) / 2);

    // colouring according to height
    float* colour;
    ColourModel* colourModel;

    if(toggleColouring)
    {
        colourModel = new ColourModel();
        float minHeight = 1000;
        float maxHeight = -1000;
        for(int z = 0; z < _terrain->length() - 1; z++){
            for(int x = 0; x < _terrain->width(); x++){
                if(_terrain->getHeight(x, z) < minHeight){
                    minHeight = _terrain->getHeight(x, z);
                }
                if(_terrain->getHeight(x, z) > maxHeight){
                    maxHeight = _terrain->getHeight(x, z);
                }
            }
        }
        colourModel->setMaxHeight(maxHeight);
        colourModel->setMinHeight(minHeight);
    }

    for(int z = 0; z < _terrain->length() - 1; z++) {
        // makes OpenGL draw a triangle at every three consecutive vertices
        glBegin(GL_TRIANGLE_STRIP);
        for(int x = 0; x < _terrain->width(); x++) {
            Vec3f normal = _terrain->getNormal(x, z);
            glNormal3f(normal[0], normal[1], normal[2]);

            if(toggleColouring)
            {
                // apply colour based on height of vertex
                colourModel->setHeight(_terrain->getHeight(x,z));
                colour = colourModel->getColourForHeight17();
                glColor3f(colour[0], colour[1], colour[2]);
            }
            else
                glColor3f(0.0f, 0.6f, 0.8f);

            glVertex3f(x, _terrain->getHeight(x, z), z);
            normal = _terrain->getNormal(x, z + 1);
            glNormal3f(normal[0], normal[1], normal[2]);
            glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
        }
        glEnd();
    }

    if(toggleColouring)
        delete colourModel;
    glPopMatrix();

    //delete _terrain's allocated memory (idle() allocates a new block for the newly generated terrain)
    Terrain::cleanup(_terrain);
    tempimage.release();

    // show the rendering on the screen
    glutSwapBuffers();

    // post the next redisplay
    glutPostRedisplay();
}

void reshape( int w, int h )
{
    // set OpenGL viewport (drawable area)
    glViewport( 0, 0, w, h );

    // from terrain generator - resizing doesn't work here for some reason :-/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / (double)h, 1.0, 200.0);
}

void mouse( int button, int state, int x, int y )
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {

    }
}

void keyboard( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case '?':
        std::cout << keyLayout << std::endl;
        break;
    case 27:
        std::cout << "Exiting..." << std::endl;
        // quit when ESC is pressed
        std::cout << "Cleaning up terrain...";
        Terrain::cleanup(_terrain);
        std::cout << "Done" << std::endl;
        std::cout << "Closing GLUT window...";
        glutDestroyWindow(glutWindowHandle);
        std::cout << "Done" << std::endl;
        exit(0);
        break;
    case 'w':
        if(!toggleWireframe)
        {
            std::cout << "Wireframe display ON" << std::endl;
            glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            toggleWireframe = true;
        }
        else
        {
            std::cout << "Wireframe display OFF" << std::endl;
            glPopAttrib();
            toggleWireframe = false;
        }
        break;
    case 'i':
        if(!invertTerrain)
        {
            std::cout << "Inverted terrain ON" << std::endl;
            invertTerrain = true;
        }
        else
        {
            std::cout << "Inverted terrain OFF" << std::endl;
            invertTerrain = false;
        }
        break;
    case '+':
        std::cout << "Zooming in : " << zoomView << std::endl;
        zoomView+=0.1f;
        break;
    case '-':
        std::cout << "Zooming out : " << zoomView << std::endl;
        zoomView-=0.1f;
        break;
    case '8':
        std::cout << "Rotating around X axis : " << angleX << "DEG" << std::endl;
        angleX+=2.0f;
        if(angleX == 360)
            angleX = 0;
        break;
    case '2':
        std::cout << "Rotating around X axis : " << angleX << "DEG" << std::endl;
        angleX-=2.0f;
        if(angleX == -360)
            angleX = 0;
        break;
    case '4':
        std::cout << "Rotating around Y axis : " << angleY << "DEG" << std::endl;
        angleY+=2.0f;
        if(angleY == 360)
            angleY = 0;
        break;
    case '6':
        std::cout << "Rotating around Y axis : " << angleY << "DEG" << std::endl;
        angleY-=2.0f;
        if(angleY == -360)
            angleY = 0;
        break;
    case '7':
        std::cout << "Rotating around Z axis : " << angleZ << "DEG" << std::endl;
        angleZ+=2.0f;
        if(angleZ == 360)
            angleZ = 0;
        break;
    case '9':
        std::cout << "Rotating around Z axis : " << angleZ << "DEG" << std::endl;
        angleZ-=2.0f;
        if(angleZ == -360)
            angleZ = 0;
        break;
    case 'c':
        if(!toggleColouring)
        {
            std::cout << "Height colouring ON" << std::endl;
            toggleColouring = true;
        }
        else
        {
            std::cout << "Height colouring OFF" << std::endl;
            toggleColouring = false;
        }
        break;
    case 'o':
        if(!toggleOpenCvOutput)
        {
            std::cout << "Display OpenCV captured image ON" << std::endl;
            toggleOpenCvOutput = true;
        }
        else
        {
            std::cout << "Display OpenCV captured image OFF" << std::endl;
            toggleOpenCvOutput = false;
        }
        break;
    case 's':
        if(!toggleAmbientLight)
        {
            std::cout << "Ambient light ON" << std::endl;
            toggleAmbientLight = true;
        }
        else
        {
            std::cout << "Ambient light OFF" << std::endl;
            toggleAmbientLight = false;
        }
        break;
    default:
        std::cout << "Unknown key pressed!" << std::endl;
        break;
    }
}

void idle()
{
    //sleep(100);
    cv::Mat tempimage;
    // grab a frame from the camera
    (*cap) >> tempimage;

    // convert to grayscale
    cv::cvtColor(tempimage, image, CV_RGBA2GRAY);

    tempimage.release();

    // from terrain generator - generate data for _terrain from current frame
    //loadTerrain(frame.data, frame.cols (aka width), frame.rows (aka height), height (for scaling))
    _terrain = Terrain::loadTerrain(image.data, image.cols, image.rows, TERRAIN_HEIGHT, invertTerrain);
}

void initRendering()
{
    // taken from terrain generator -> makes things dark
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
}

void update(int value) {
#if defined(ENABLE_TERRAIN_ROTATION)
    angleY += 1.0f;
    if (angleY > 360) {
        angleY -= 360;
    }
#elif defined(ENABLE_LIGHT_SOURCE_ROTATION)
    angleLightSource += 1.0f;
    if (angleLightSource > 360) {
        angleLightSource -= 360;
    }
#endif
    glutPostRedisplay();
    glutTimerFunc(30, update, 0);
}

int main( int argc, char **argv )
{
#if defined(OPENGL_OPENCV_TEST)
    //STATUS: PARTIALLY WORKING
    //FIXME: image is multiplied 3 times and 1/2 of the generated terrain is garbage
    //		 this might be due to the color space conversion (it's not due to the usage of SRC=DST in cvtColor()!)

    std::cout << "OpenCVGL WebCam 2 1/2 Terrain Generator" << std::endl;
    std::cout << keyLayout << std::endl;

    //std::cout << "TEST: Load Terrain object from OpenCV-frame and render on screen using OpenGL" << std::endl;
    int w,h;

    if ( argc == 1 ) {
        // start video capture from camera
        cap = new cv::VideoCapture(1);
    } else if ( argc == 2 ) {
        // start video capture from file
        cap = new cv::VideoCapture(argv[1]);
    } else {
        fprintf( stderr, "usage: %s [<filename>]\n", argv[0] );
        return 1;
    }

    // check that video is opened
    if ( cap == NULL || !cap->isOpened() ) {
        fprintf( stderr, "could not start video capture\n" );
        return 1;
    }

    // get width and height
    w = (int) cap->get( CV_CAP_PROP_FRAME_WIDTH );
    h = (int) cap->get( CV_CAP_PROP_FRAME_HEIGHT );
    // on Linux, there is currently a bug in OpenCV that returns
    // zero for both width and height here (at least for video from file)
    // hence the following override to global variable defaults:
    width = w ? w : width;
    height = h ? h : height;

    // initialize GLUT
    glutInit( &argc, argv );
    //glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition( 20, 20 );
    glutInitWindowSize( width, height );
    glutWindowHandle = glutCreateWindow( "3D Terrain Generator using OpenCV and OpenGL" );
    initRendering();

    // set up GUI callback functions
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutMouseFunc( mouse );
    glutKeyboardFunc( keyboard );
    glutTimerFunc(30, update, 0);
    glutIdleFunc( idle );

    // start GUI loop
    glutMainLoop();

#elif defined(OPENCV_WEBCAM)
    //STATUS: WORKING
    std::cout << "TEST: Capture image data from webcam" << std::endl;
    cap = new cv::VideoCapture(0);

    if(!cap->isOpened())
    {
        std::cout << "Cannot open device 0!" << std::endl;
        return -1;
    }

    cv::Mat image;
    cv::namedWindow("WEBCAM", CV_WINDOW_AUTOSIZE);

    while(cap->read(image))
    {
        cv::cvtColor(image, image, CV_RGBA2GRAY);
        cv::imshow("WEBCAM", image);

        if(cv::waitKey(30) == 27)
            break;
    }

    cv::destroyAllWindows();

#elif defined(TERRAIN_CLASS_TEST)
    //STATUS: WORKING
    //NOTE: testing if terrain generator still works after transfering the Terrain-things to a seperate header and source file
    std::cout << "TEST: Create a terrain from BMP file (after transfer of Terrain to an extra HPP and CPP)" << std::endl;
    Terrain *t = Terrain::loadTerrain("heightmap.bmp", 20);
    Terrain::cleanup(t);

#elif defined(TERRAIN_FROM_OPENCV)
    //STATUS: WORKING
    //STEP 1: http://www.cs.ucsb.edu/~holl/CS290I/opengl_cv.cpp - worked like a charm but needed some modifications (adding
    //		  terrain-generator for example but not only ;))
    //STEP 2: put Terrain-relevant things in corresponding header and footer to make things more easier to maintain later on
    //STEP 3: added an overloaded method loadTerrain(), which takes the pixel- and dimensions-data from a cv::Mat
    //STEP 4: converted camera input to grayscale (not needed for our sensor since it returns grayscale already); adjusted OpenGL
    //		  configuration to be able to show grayscale images
    std::cout << "TEST: Load Terrain object from OpenCV-frame" << std::endl;
    cap = new cv::VideoCapture(0);

    if(!cap->isOpened())
    {
        std::cout << "Cannot open device 0!" << std::endl;
        return -1;
    }

    cv::Mat image;
    cv::namedWindow("WEBCAM", CV_WINDOW_AUTOSIZE);
    Terrain *t;

    while(cap->read(image))
    {
        std::cout << "Image-type: " << image.type() << std::endl;
        cv::cvtColor(image, image, CV_RGBA2GRAY);
        std::cout << "Image-type: " << image.type() << std::endl;
        t = Terrain::loadTerrain( image.data, image.cols, image.rows , 20);
        cv::imshow("WEBCAM", image);

        image.release();
        Terrain::cleanup(t);

        if(cv::waitKey(30) == 27)
            break;
    }

    cv::destroyAllWindows();

#else
    std::cout << "Not test active";

#endif

    std::cout << "Bye";
    return 0;
}
