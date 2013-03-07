//Rajiev Timal -0220052- CS 6533 - Interactive Computer Graphics-Homework # 3

//#define WINDOWS     1 /* Set to 1 for Windows, 0 else */
//#define UNIX_LINUX  0 /* Set to 1 for Unix/Linux, 0 else */
#define _USE_MATH_DEFINES
//#if WINDOWS
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <GL/glut.h>
#include <fstream>
#include <istream>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <math.h>
#include <cmath>

using std::vector;
using namespace std;

//Specify rotation velocity of sphere;
#define anglevelocity 2 //angle increment for every frame
#define ImageWidth  64
#define ImageHeight 64
GLubyte Image[ImageHeight][ImageWidth][4];
#define	stripeImageWidth 32
GLubyte stripeImage[4*stripeImageWidth];
GLuint quadtexture=1;
GLuint spheretexture=2;

//initialize variables
int windowwidth=600;//Width of window
int windowheight=600;//Height of window
int numtriangles;
float angle =0;//total rotation angle of circlue for each segment, used to calculate d.
float anglespeed=0;//angle increment for every frame. Starts off at 0, until it is set to the anglevelocity. They are the same thing.
float d = 0;//scalar distance calculated based on angle
float pointbeginx;//start point x
float pointbeginz;//start point z
float pointendx;//end point x
float pointendz;//end point z
float newx=0; // translate x, after calculation d
float newz=0;//translate z, after calculating  d
float rad = 57.2957795;
int mode = 0;//flag for mode 0
int endflag = 0;//flag for the ending of a mode
int angleflag = 0;//flag to initialize angle to 0
int vec[3];//vector for line segment
int vecnorm[3];//vector for normal to line segment
int rotaxis[3];//rotation axis calculate from cross sproduct of the 2 above vectors
GLfloat M[16];//accumulation matrix
int showshadow=1;//flag for showing the shadow, default on
int showwireframe=0;//flag for showing wireframe , default off
//int showlighting=1;//flag for showing lighting, default on
int smoothshading = 0; // for showing smooth shading, default on
int lighting=0;//flag for show
int spotlight=0;
//fog option flags
int showfog=0;
int linear=0;
int exponential=0;
int exponentialsquared=0;
int blendshadow=0;
int texmapground=0;
int viewx=7;//initial VRP x
int viewy=3;//initial VRP y
int viewz=-10;//initial VRP z
int startflag =0;//Flag to indicate animation hasnt started yet


//Triangle struct holding 3 vertices or 9 coordinates
struct Triangle{
	GLfloat a[3];
	GLfloat b[3];
	GLfloat c[3];
	//each one of the arrays above represents a vertex
	//below are the individual vertices, not really needed but just have them here because i used them in homework2
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat x1;
	GLfloat y1;
	GLfloat z1;
	GLfloat x2;
	GLfloat y2;
	GLfloat z2;
	//The three vertex normals
	GLfloat vnormala[3];
	GLfloat vnormalb[3];
	GLfloat vnormalc[3];
	// The face normal for flat shading
	GLfloat fnormal[3];
};

//pointer to array of triangles
Triangle* triangles;

//Function prototypes
void display(void);//display function
void my_init(void);//initialize function
void reshape(int w, int h);//reshape function
void idle(void);//idle function
void readSphere();//read contents of sphere into array of triangles
float getAngle(float beginx,float beginy, float endx, float endy);//get the angle between a line segment and its x component using arc tan
void setStartPoint(float x, float y, float z);//set the start point of movement
void setEndPoint(float x, float y, float z);//set the endpoint of movement
void myKey(unsigned char key, int x, int y);//keyboard callback function
void myMouse(int button,int state,int x, int y);//mouse callback function
void leftmenu(int id);//menu callback function
void cross(GLfloat a[], GLfloat b[], GLfloat c[] , GLfloat d[]);//calculate cross product
void normalize(GLfloat d[]);//Normalize face vector for flat shading
void image_set_up(void);//set up texturing images

void main(int argc, char **argv)
{//Initialization
  glutInit(&argc, argv);//Initialize
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//Window mode
  glutInitWindowSize(windowwidth, windowheight);//Window size
  glutInitWindowPosition(300, 50);  //window position
  glutCreateWindow("Sphere"); //create window named sphere
  glutDisplayFunc(display);  //display callback function
  glutReshapeFunc(reshape);  //reshape callback function
  glutIdleFunc(idle);//idle function
  glutKeyboardFunc(myKey);//keyboard call bakc
  glutMouseFunc(myMouse);//mouse callback
  //Creation of menu
  int shadowsubmenu,lightingsubmenu,wireframesubmenu,shadingsubmenu,enablelightingsubmenu,fogsubmenu,blendingshadowmenu,texmapmenu;
  shadowsubmenu=glutCreateMenu(leftmenu);
  glutAddMenuEntry("Yes",20);
  glutAddMenuEntry("No",14);
  blendingshadowmenu=glutCreateMenu(leftmenu);
  glutAddMenuEntry("Yes",29);
  glutAddMenuEntry("No",30);


  texmapmenu=glutCreateMenu(leftmenu);
  glutAddMenuEntry("Yes",31);
  glutAddMenuEntry("No",32);



  enablelightingsubmenu=glutCreateMenu(leftmenu);
  glutAddMenuEntry("Yes",23);
  glutAddMenuEntry("No",24);
  lightingsubmenu=glutCreateMenu(leftmenu);
  glutAddMenuEntry("Spot Light",15);
  glutAddMenuEntry("Point Source",16);
   wireframesubmenu=glutCreateMenu(leftmenu);
  glutAddMenuEntry("Yes",17);
  glutAddMenuEntry("No",18);
  shadingsubmenu=glutCreateMenu(leftmenu);
  glutAddMenuEntry("smooth shading",21);
  glutAddMenuEntry("flat shading",22);
  fogsubmenu=glutCreateMenu(leftmenu);
  glutAddMenuEntry("no fog",25);
  glutAddMenuEntry("linear",26);
  glutAddMenuEntry("exponential",27);
  glutAddMenuEntry("exponential square",28);
  glutCreateMenu(leftmenu);
  glutAddMenuEntry("Default View Point",1);
  glutAddSubMenu("Enable Lighting",enablelightingsubmenu);
  glutAddSubMenu("Fog Options",fogsubmenu);

  glutAddSubMenu("Texture Mapped Ground",texmapmenu);



  glutAddMenuEntry("Texture Mapped Sphere",5);
  glutAddSubMenu("Shadow",shadowsubmenu);
  glutAddSubMenu("Blending Shadow",blendingshadowmenu);
  glutAddMenuEntry("Quit",8);
  glutAddSubMenu("Wire Frame",wireframesubmenu);
  glutAddSubMenu("Shading",shadingsubmenu);
  glutAddSubMenu("Lighting",lightingsubmenu);
  glutAddMenuEntry("Firework",12);
  glutAttachMenu(GLUT_LEFT_BUTTON);
  my_init(); //initialize              
  glutMainLoop();   //enter main loop  
}

//Menu that pops up when left mouse button pressed, sets program flags
void leftmenu(int id){
	switch(id){
	case 1: viewx=7;viewy=3;viewz=-10;//go back to default camera,position.
	 break;
	case 8:
		exit(0);//exit program
		break;
	case 20:
		showshadow=1;
		break;
	case 14:
		showshadow=0;
		break;
	case 17:
		showwireframe=1;
		break;
	case 18:
		showwireframe=0;
		break;
	case 22:
		smoothshading=0;
		break;
	case 21:
		smoothshading=1;
		break;
	case 23:
		lighting=1;
		break;
	case 24:
		lighting=0;
		break;
	case 15:
		spotlight=1;
		break;
	case 16:
		spotlight=0;
		break;
	case 25:
		showfog=0;
		break;
	case 26:
		showfog=1;
		linear=1;
		exponential=0;
		exponentialsquared=0;
		break;
	case 27:
		showfog=1;
		exponential=1;
		linear=0;
		exponentialsquared=0;
		break;
	case 28:
		showfog=1;
		exponentialsquared=1;
		exponential=0;
		linear=0;
		break;
	case 29:
		blendshadow=1;
		break;
	case 30:
		blendshadow=0;
		break;
	case 31:
		texmapground=1;
		break;
	case 32:
		texmapground=0;
		break;
	}
}


//right mouse callback function
void myMouse(int button,int state,int x, int y){
if (button ==GLUT_RIGHT_BUTTON && state ==GLUT_DOWN && startflag==1)
{
	//stop the animation and set the startflag to 0
	anglespeed=0;
	startflag=0;
}else if (button ==GLUT_RIGHT_BUTTON && state == GLUT_DOWN && startflag==0)
{
	anglespeed=anglevelocity;
	startflag=1;
	glutIdleFunc(idle);
	return;
}
}

//keyboard callback function
void myKey(unsigned char key, int x, int y){
	//start the animation
	if (key =='B' || key =='b'){
	startflag=1;
	anglespeed=anglevelocity;//set the angle speed to the value we specified above 
	glutIdleFunc(idle);
}
	//adjust model view matrix based on keys
	//decrease view x
	if (key=='x'){
		viewx--;
		//account for fact that ball may be moving while doing this. 
		if (startflag==0){
		anglespeed=0;
		}
		if (startflag==1){
			anglespeed=anglevelocity;
		}
	glutIdleFunc(idle);
	}
	//increase view sx
	else if(key =='X'){
		viewx++;
			if (startflag==0){
			anglespeed=0;;
			}
			if (startflag==1){
			anglespeed=anglevelocity;
		}
	glutIdleFunc(idle);
}
	//decrease view y
	else if (key =='y'){
	viewy--;
		if (startflag==0){
		anglespeed=0;
	}
	if (startflag==1){
		anglespeed=anglevelocity;
	}
		glutIdleFunc(idle);
}
	//increase view y
	else if (key =='Y')
	{
	    viewy++;
		if (startflag==0){
		anglespeed=0;;
		}
			if (startflag==1){
			anglespeed=anglevelocity;
		}
	glutIdleFunc(idle);
	}
	//decrease view z
	else if (key=='z'){
		viewz--;
		if (startflag==0){
		anglespeed=0;
		}
			if (startflag==1){
			anglespeed=anglevelocity;
		}
	glutIdleFunc(idle);
	}
	//increase view z
	else if (key=='Z'){
		viewz++;
		if (startflag==0){
		anglespeed=0;
		}
				if (startflag==1){
			anglespeed=anglevelocity;
		}
	glutIdleFunc(idle);
	}
}

//display function
void display(void)
{
	float light[4]={-14.0,12.0,-3.0,1.0};//coordinates of light0, used in shadow matrix
	//shadow projection matrix m2. First light is translated to origin, then projected, then translated back, for flattening of sphere
	GLfloat m2[16];
	for(int i =0;i<=15;i++){m2[i]=0.0;}
	m2[0]=m2[5]=m2[10]=1.0;
	m2[7]=-1.0/light[1];
	//****************************
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);  //clear frame buffer
	glMatrixMode(GL_MODELVIEW);//choose modelview matrix
	glLoadIdentity();//load identity matrix
	if (lighting==1){glEnable(GL_LIGHTING);}else {glDisable(GL_LIGHTING);};//set lighting based on options
	glEnable(GL_LIGHT0);//enable first light with default values
	GLfloat global_ambient[] = {1, 1, 1,1.0};//global ambient light values
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);//set global ambient
	
	gluLookAt((GLdouble)viewx,(GLdouble)viewy,(GLdouble)viewz,(GLdouble)0,(GLdouble)0,(GLdouble)0,(GLdouble)0,(GLdouble)1,(GLdouble)0);//set camera position and orientation
	glPushMatrix();//save matrix before transformations, this is done because when we have to draw the shadow, we have to transform the shadow also to the position of the sphere, so we pop the matrix then
	GLfloat light1_pos[]={-14.0,12.0,-3.0,1};//first light position
	GLfloat ambient1[]={0.0,0.0,0.0,1.0};//ambient light values for light 1
	GLfloat diffuse1[]={1.0,1.0,1.0,1.0};//light one diffuse values
	GLfloat specular1[]={1.0,1.0,1.0,1.0};//light one specular values
	GLfloat spot_direction[]={8.0,-12.0,1.5,0};//light one(spot light and point source) direction vector
	glLightfv(GL_LIGHT1,GL_POSITION,light1_pos);//set light 1 position
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);//set light one directoin
	glLightfv(GL_LIGHT1,GL_AMBIENT,ambient1);//set light one ambient
	glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuse1);//set light one diffuse
	glLightfv(GL_LIGHT1,GL_SPECULAR,specular1);//set light one specular
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 2.0);//set light one constant attentuation
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.01);//set light one linear attentuation
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.001);//set light one quadratic attenuation
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 5.0);//set light one spot exponent
	glEnable(GL_LIGHT1);//enable light one (spot light and point light)

	///fog
  float fogCol[4]={0.7f,0.7f,0.7f,.5f}; // Define a nice light grey
  glFogfv(GL_FOG_COLOR,fogCol);     // Set the fog color
  glHint (GL_FOG_HINT, GL_NICEST);
  GLfloat density = .09;
  if (linear==1){
  glFogi(GL_FOG_MODE, GL_LINEAR); // Note the 'i' after glFog - the GL_LINEAR constant is an integer.
  glFogf(GL_FOG_START, 0.8f);
  glFogf(GL_FOG_END, 18.f);}
  else if (exponential==1){
	  glFogi (GL_FOG_MODE, GL_EXP);
	  glFogf (GL_FOG_DENSITY, density);
  }
  else if (exponentialsquared==1){
	  glFogi (GL_FOG_MODE, GL_EXP2);
	  glFogf (GL_FOG_DENSITY, density);
  }
  if (showfog==1){glEnable(GL_FOG);}
  if (showfog==0){glDisable(GL_FOG);}
  //end fog


	//set light one spot cutoff based on if spotlight or point source is selected
	if (spotlight==1){
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF,20.f);
	}
	else{
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF,180.f);}

	glPolygonMode(GL_FRONT, GL_FILL);//fill the quadrilateral
	//color of quadrilateral


	glColor3f(0,1,0);
	//draw quadrilateral
	//Set material ambient,diffuse and specular properties
	GLfloat diffusequad[]={0.0,1.0,0.0,1.0};
	GLfloat ambientquad[]={0.2,0.2,0.2,1.0};
	GLfloat specularquad[]={0.0,0.0,0.0,1.0};
	glMaterialfv(GL_FRONT,GL_DIFFUSE,diffusequad);
	glMaterialfv(GL_FRONT,GL_AMBIENT,ambientquad);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specularquad);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,diffusequad);
	glDepthMask(GL_FALSE);//**FOR DECALING, we do not write the quadrilateral to the frame buffer.
	

	if(texmapground==1){
	glBindTexture(GL_TEXTURE_2D,quadtexture);
	glEnable(GL_TEXTURE_2D);
	}else if (texmapground==0)
	{
			glDisable(GL_TEXTURE_2D);
	}



	glBegin(GL_QUADS);
	{
		glNormal3f(0,1,0);
		glTexCoord2f(0.0,0.0);glVertex4f(-5,0,-4,1);
		glTexCoord2f(0.0,1.5);glVertex4f(-5,0,8,1);
		glTexCoord2f(1.25,1.5);glVertex4f(5,0,8,1);
		glTexCoord2f(1.25,0.0);glVertex4f(5,0,-4,1);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);


	glDepthMask(GL_TRUE);//enable writing to depth buffer again

	//Width of axes
	glLineWidth(2.0);
	glDisable(GL_LIGHTING);//Have to disable lighting so glcolor3f can work for drawing axis with right colors

	//draw z axis
	glColor3f(0,0,1);
	glBegin(GL_LINES);
	{

		glVertex3f(0,0,0);
		glVertex3f(0,0,8);
	}
	glEnd();

		//draw y axis
	glColor3f(1, 0, 1);  
	glBegin(GL_LINES);
	{
		glVertex3f(0,0,0);
		glVertex3f(0,8,0);
	}
	glEnd();

		//draw x axis
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	{
		glVertex3f(0,0,0);
		glVertex3f(5,0,0);
	}
	glEnd();

	//Enable or disable lighting of sphere
	if (lighting==1){glEnable(GL_LIGHTING);}else {glDisable(GL_LIGHTING);};

		//sets triangles to draw as wireframe or solid based on selection
		if (showwireframe==1){
				glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glutPostRedisplay();
		}else
		{
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glutPostRedisplay();
		}



	//displace by position calculated based on rotation
	glTranslatef(newx,0,newz);
	//set to start position depending on mode
	if (mode == 0){
	glTranslatef((GLfloat)3,(GLfloat)1,(GLfloat)5);
	}
		//set to start position depending on mode
	else if (mode == 1){
		glTranslatef((GLfloat)-2,(GLfloat)1,(GLfloat)-2.5);
	
	}
		//set to start position depending on mode
	else if (mode == 2){
			glTranslatef((GLfloat)2,(GLfloat)1,(GLfloat)-4);
	}

	//This is the part that accumulates the matrix. Does not include translation in the matrix, adds the new rotation to the previous rotation and saves as M.
	glPushMatrix();
	glLoadIdentity();//load identity matrjx
	glRotatef(anglespeed,rotaxis[0],rotaxis[1],rotaxis[2]);//rotate by current angle and around current rotation axis
	glMultMatrixf(M);//multiple current rotation increment with last rotation matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, M);//save this
	glPopMatrix();//retrieve translations

	glMultMatrixf(M);	//multiply by current rotation matrix that includes the new increment and all the past rotations
	//glPushMatrix();//1

	glColor3f(1.0, 0.84, 0);//Set sphere color, this is only used if lighting is disable d
	glLineWidth(1.0);//width of triangles
	//draw triangles  of sphere
	//diffuse color of sphere
	GLfloat diffusesphere[]={1.0,.84,0.0};
	//glMaterialfv only works if GL_COLOR_MATERIAL is disabled
	glDisable(GL_COLOR_MATERIAL);
	//set diffuse material color of sphere
	glMaterialfv(GL_FRONT,GL_DIFFUSE,diffusesphere);
		//set shininess of sphere
	glMaterialf(GL_FRONT,GL_SHININESS,125.0);
	//draw sphere, the first part of the if statement uses one face normal if flat shading is enabled
	//the else parts draws a vertex normal for each vertex of smooth shading is enabled

	glBindTexture(GL_TEXTURE_1D,spheretexture);
	GLfloat planes[]={2.5,0,0,0};
	GLfloat planes2[]={1,1,1,0};
	GLfloat planes3[]={50,50,50,50};
	glEnable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S,GL_EYE_PLANE,planes2);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T,GL_EYE_PLANE,planes2);


	glBegin(GL_TRIANGLES);  
	for (int i = 0;i<numtriangles;i++)
	{	
		if (smoothshading==0){
			if(smoothshading==0){glShadeModel(GL_FLAT);}
		cross(triangles[i].a,triangles[i].b,triangles[i].c,triangles[i].fnormal);//the cross product is calcualted for each triangle here and stored as the face normal. This is then used below
		//face normal
		glNormal3fv(triangles[i].fnormal);
		glVertex3fv(triangles[i].a);
		glVertex3fv(triangles[i].b);
		glVertex3fv(triangles[i].c);}
		else{
			//in this case, the vector normal is equal to the vertex coordinate because of the properties of the sphere
		glNormal3fv(triangles[i].vnormala);
		glVertex3fv(triangles[i].a);
		glNormal3fv(triangles[i].vnormalb);
		glVertex3fv(triangles[i].b);
		glNormal3fv(triangles[i].vnormalc);
		glVertex3fv(triangles[i].c);}
	}
		glEnd();
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);

		//Draw shadow if the users chooses shadow to be drawn
		glPopMatrix(); //restore matrix that was saved before transformations were done on the sphere
		if(showshadow==1){
			//translate light to origin, project using shadow matrix, then translate back to original position. THis creates shadow effect
		glTranslatef(light[0],light[1],light[2]);
		glMultMatrixf(m2);//m2 is the shadow projection matrix
		glTranslatef(-light[0],-light[1],-light[2]);
			glTranslatef(newx,0,newz);
//now all the transformations the circle went through has to be applied to the shadow, even the rotation that can be seen in wireframe mode. This all happens before its flattened in the lines above.
	//set to start position depending on mode
	if (mode == 0){
	glTranslatef((GLfloat)3,(GLfloat)1,(GLfloat)5);
	}
		//set to start position depending on mode
	else if (mode == 1){
		glTranslatef((GLfloat)-2,(GLfloat)1,(GLfloat)-2.5);
	
	}
		//set to start position depending on mode
	else if (mode == 2){
			glTranslatef((GLfloat)2,(GLfloat)1,(GLfloat)-4);
	}
	//	glMultMatrixf(shadow);
		
	glMultMatrixf(M);//multiply by rotation matrix
	glDisable(GL_LIGHTING);//shadow doesnt need to be lighted

	//Blending
	if(blendshadow==1){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	}
	else{
		glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	}
	glColor4f(.25,.25,.25,.65);//shadow color
	//specify shadow vertices
	//for Decaling of the shadow, disable writing to the frame buffer
	glBegin(GL_TRIANGLES);  
	for (int i = 0;i<numtriangles;i++)
	{	
		glVertex3fv(triangles[i].a);
		glVertex3fv(triangles[i].b);
		glVertex3fv(triangles[i].c);}
glEnd();
glDepthMask(GL_TRUE);//
	if(lighting ==1){glEnable(GL_LIGHTING);} // re-enable lighting if it was enabled
		}
 glDisable(GL_BLEND);//Disable blending




		//end of drawing shadow

 //write the triangles back to the depth buffer, but not the color buffer, this is part of decaling 
 glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
 glBegin(GL_TRIANGLES);  
	for (int i = 0;i<numtriangles;i++)
	{	
		glVertex3fv(triangles[i].a);
		glVertex3fv(triangles[i].b);
		glVertex3fv(triangles[i].c);}
glEnd();


 	//writing is already disabled to the color buffer from before with the triangles.Only write to the depth buffer. This is part of decaling.The quadrilateral info is put back into the depth
		//buffer, it was removed before during decaling
		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);//this disables writing to the color buffer
	glBegin(GL_QUADS);
	{
		glVertex3f(5,0,8);
		glVertex3f(5,0,-4);
		glVertex3f(-5,0,-4);
		glVertex3f(-5,0,8);
	}
	glEnd();
		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);


		glFlush();         // Render (draw) the object
	glutSwapBuffers(); // Swap buffers in double buffering.
}

//initialize funciton. 
void my_init()
{
	readSphere(); 
	  glMatrixMode(GL_MODELVIEW);
	  glLoadIdentity();
	  	glGetFloatv(GL_MODELVIEW_MATRIX, M);//set our own matrix to the modelview matrix
		//set our own matrix to the modelview matrix, which is the identity matrix right now
	  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	  glClearColor(0.529, 0.807, 0.92, 0.0);                     
	  glClear(GL_COLOR_BUFFER_BIT);  
	glEnable(GL_DEPTH_TEST);//enable z buffer
	  glutIdleFunc(NULL); //start out with ball not moving
	 image_set_up();	
	glBindTexture(GL_TEXTURE_2D,quadtexture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,ImageWidth,ImageHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,Image);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	
	glBindTexture(GL_TEXTURE_1D,spheretexture);
	glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,stripeImageWidth,0,GL_RGBA,GL_UNSIGNED_BYTE,stripeImage);
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	//Quad texture initializatin
}// end my_init

void reshape(int w, int h)
{
  glViewport(0, 0, w, h);           // Viewport within the graphics window.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50,w/h,1,2000);
  glMatrixMode(GL_MODELVIEW);
}

void idle(void)
{
	float theta;//angle that is arctan of x and y components
	float d;//distance
	float pi = 3.14159265;
//from a to b
	if (mode ==0){
		setStartPoint(3,1,5);
		setEndPoint(-2,1,-2.5);
		//setting vector values used to calculate cross product and axis of rotation
		vec[0]=(-2-3);
		vec[1]=(1-1);
		vec[2]=(-2.5-5);
	}
	
	//from b to c
	else if (mode == 1){
		setStartPoint(-2,1,-2.5);
		setEndPoint(2,1,-4);
		//setting vector values used to calculate cross product and axis of rotation
		vec[0]=(2-(-2));
		vec[1]=(1-1);
		vec[2]=(-4-(-2.5));
	}
	//from c to d
	else if (mode == 2){
		setStartPoint(2,1,-4);
		setEndPoint(3,1,5);
		//setting vector values used to calculate cross product and axis of rotation
		vec[0]=(3-2);
		vec[1]=(1-1);
		vec[2]=(5-(-4));
	}
	//set normal vector values used to calculate cross product and axis of  rotation
	vecnorm[0]=0;
	vecnorm[1]=1;
	vecnorm[2]=0;
	//Cross product to find rotation axes
	rotaxis[0]=vecnorm[1]*vec[2]-vecnorm[2]*vec[1];
	rotaxis[1]=vecnorm[2]*vec[0]-vecnorm[1]*vec[2];
	rotaxis[2]=vecnorm[0]*vec[1]-vecnorm[1]*vec[0];
	//increment angle, anglespeed will be 0 if the ball is in a state of not mocing
		angle+=anglespeed;
		//reset angle if a new mode is beginning
		if (angleflag==1){
			angle=0;
		}
		//calculate distance
		d = angle*(2*pi*1)/360;
		if (endflag==1){
			mode+=1;
			endflag=0;
			if (mode>2){
			mode=0;
			}
		}
		//if we are going from a to b
		if (mode==0){
			angleflag=0;
			theta = getAngle(pointbeginx,pointbeginz,pointendx,pointendz);
			newz=-d*sin(theta*pi/180);
			newx=-d*cos(theta*pi/180);
			//end case
			if (newx<=-5){
				newx = -5;
				newz = -7.5;
				endflag =1;
				angleflag =1;
			}
		}
		//if we are going from b to c
		else if (mode == 1){
			angleflag=0;
			theta = getAngle(pointbeginx,pointbeginz,pointendx,pointendz);
			newz=d*sin(theta*pi/180);
			newx=d*cos(theta*pi/180);
				//end case
			if (newx>=4){
				newx = 4;
				newz = -1.5;
				endflag =1;
				angleflag =1;
			}
		}
		//if we are going from c to d
		else if (mode == 2){
			angleflag=0;
			theta = getAngle(pointbeginx,pointbeginz,pointendx,pointendz);
			newz=d*sin(theta*pi/180);
			newx=d*cos(theta*pi/180);
			//end case
			if (newx>=1)
			{
				newx=1;
				newz=9;
				endflag=1;
				angleflag=1;
			}
		}
	glutPostRedisplay(); //redisplay
}


//** I also read the values into individual x,y,and z values for each vertex, in addition to an array for each vertex. This is just incase i needed to use
//individual points. Not really needed
void readSphere(){
	GLint numvertices;
	ifstream myfile;
		myfile.open ("sphere.1024");//open file
		myfile>>numtriangles;//get number of triangles
		triangles = new Triangle[numtriangles];//initialize array of triangles
		for (int i =0;i<numtriangles;i++)//populte 
		{
			myfile>>numvertices;//this value is usually 3 in this assignment
			//read verticles from file into array
			myfile>>triangles[i].x;
			triangles[i].a[0]=triangles[i].x;
			triangles[i].vnormala[0]=triangles[i].a[0];
			myfile>>triangles[i].y;
			triangles[i].a[1]=triangles[i].y;
			triangles[i].vnormala[1]=triangles[i].a[1];
			myfile>>triangles[i].z;
			triangles[i].a[2]=triangles[i].z;
			triangles[i].vnormala[2]=triangles[i].a[2];
			myfile>>triangles[i].x1;
			triangles[i].b[0]=triangles[i].x1;
			triangles[i].vnormalb[0]=triangles[i].b[0];
			myfile>>triangles[i].y1;
			triangles[i].b[1]=triangles[i].y1;
			triangles[i].vnormalb[1]=triangles[i].b[1];
			myfile>>triangles[i].z1;
			triangles[i].b[2]=triangles[i].z1;
			triangles[i].vnormalb[2]=triangles[i].b[2];
			myfile>>triangles[i].x2;
			triangles[i].c[0]=triangles[i].x2;
			triangles[i].vnormalc[0]=triangles[i].c[0];
			myfile>>triangles[i].y2;
			triangles[i].c[1]=triangles[i].y2;
			triangles[i].vnormalc[1]=triangles[i].c[1];
			myfile>>triangles[i].z2;
			triangles[i].c[2]=triangles[i].z2;
			triangles[i].vnormalc[2]=triangles[i].c[2];
		}
		myfile.close();
}

float getAngle(float beginx,float beginy, float endx, float endy){
	float inter = (endy-beginy)/(endx-beginx);
	return atan(inter)* rad ;
}

void setStartPoint(float x, float y, float z){
 pointbeginx=x;
 pointbeginz=z;
}

void setEndPoint(float x, float y, float z){
 pointendx=x;
 pointendz=z;
}

void cross(GLfloat a[], GLfloat b[], GLfloat c[] , GLfloat d[]){
	GLfloat v1[3];
	GLfloat v2[3];
	v1[0]=a[0]-b[0];
	v1[1]=a[1]-b[1];
	v1[2]=a[2]-b[2];

	v2[0]=b[0]-c[0];
	v2[1]=b[1]-c[1];
	v2[2]=b[2]-c[2];


	d[0]=v1[1]*v2[2]-v1[2]*v2[1];
	d[1]=v1[2]*v2[0]-v1[0]*v2[2];
	d[2]=v1[0]*v2[1]-v1[1]*v2[0];

	normalize(d);
}


void normalize(GLfloat d[]){
	GLfloat a =0;
	int i;
	for (i=0;i<3;i++){ a+=d[i]*d[i];}
	a = sqrt(a);
	if (a>0.0) for (i=0;i<3;i++) d[i]/=a;
}





void image_set_up(void)
{
 int i, j, c; 
 
 /* --- Generate checkerboard image to the image array ---*/
  for (i = 0; i < ImageHeight; i++)
    for (j = 0; j < ImageWidth; j++)
      {
       c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));

       if (c == 1) /* white */
	{
         c = 255;  
	 Image[i][j][0] = (GLubyte) c;
         Image[i][j][1] = (GLubyte) c;
         Image[i][j][2] = (GLubyte) c;
	}
       else  /* green */
	{
         Image[i][j][0] = (GLubyte) 0;
         Image[i][j][1] = (GLubyte) 150;
         Image[i][j][2] = (GLubyte) 0;
	}

       Image[i][j][3] = (GLubyte) 255;
      }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

/*--- Generate 1D stripe image to array stripeImage[] ---*/
  for (j = 0; j < stripeImageWidth; j++) {
     /* When j <= 4, the color is (255, 0, 0),   i.e., red stripe/line.
        When j > 4,  the color is (255, 255, 0), i.e., yellow remaining texture
      */
    stripeImage[4*j] = (GLubyte)    255;
    stripeImage[4*j+1] = (GLubyte) ((j>4) ? 255 : 0);
    stripeImage[4*j+2] = (GLubyte) 0; 
    stripeImage[4*j+3] = (GLubyte) 255;
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
/*----------- End 1D stripe image ----------------*/

/*--- texture mapping set-up is to be done in 
      init() (set up texture objects)
      and in 
      display() (specify actual mapping and 
                 assign texture coordinates, etc.)
 ---*/

} /* end function */