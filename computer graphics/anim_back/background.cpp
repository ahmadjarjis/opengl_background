

#include <GL/glut.h>    // Header File For The GLUT Library 
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <unistd.h>     // Header file for sleeping.
#include <stdio.h>      // Header file for standard file i/o.
#include <stdlib.h>     // Header file for malloc/free.
#include <math.h>       // Header file for trigonometric functions.
#include <time.h>
#include <cmath>


/* ascii codes for various special keys */
#define ESCAPE 27
#define PAGE_UP 73
#define PAGE_DOWN 81
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77

/* The number of our GLUT window */
int window; 

GLuint loop;             // general loop variable
GLuint texture[9];       // storage for 8 textures;

int light = 0;           // lighting on/off
int blend = 0;        // blending on/off

GLfloat xrot;            // x rotation
GLfloat yrot;            // y rotation
GLfloat xspeed;          // x rotation speed
GLfloat yspeed;          // y rotation speed

GLfloat walkbias = 0;
GLfloat walkbiasangle = 0;

GLfloat lookupdown = 0.0;
const float piover180 = 0.0174532925f;

float heading, xpos, zpos, ypos;

GLfloat camx = 0, camy = 0, camz = 0; // camera location.
GLfloat therotate;

GLfloat z=0.0f;                       // depth into the screen.



GLuint filter = 0;       // texture filtering method to use (nearest, linear, linear + mipmaps)


////////for smoke
float limits = .8;
float X=0, Y=0;
const int MAX_PARTICLES = 1000;
const int MIN_PARTICLES = 10;
int currentParticle = 1;
float posX[MAX_PARTICLES], posY[MAX_PARTICLES];
void moveParticles(int amount_of_particles) {
	srand (time (NULL) );
	float myX, myY;
	
	for (int i = 0; i < amount_of_particles; i++) {		
		myX = rand() % 3 + 1;
		if(myX==1 && posX[i]<=limits ){
			int mytemp = rand() % 100 + 1;
			int temp = rand() % 5 + 1;
			posX[i]+=temp*.001;
			posY[i]+=mytemp*.0004;
		}
		if(myX==2){posX[i]+=.00;posY[i]+=.01;}
		if(myX==3 && posX[i]>=-limits){
			int temp = rand() % 5 + 1;
			int mytemp = rand() % 100 + 1;
			posX[i]-=temp*.001;
			posY[i]+=mytemp*.0004;
		}
		///////////////////////////////////////////
		if(posY[i]>=limits){	
			posY[i]=0;
			posX[i]=0;
		}
	}
}
////////////////////////////////////////////////////






typedef struct {         // vertex coordinates - 3d and texture
    GLfloat x, y, z;     // 3d coords.
    GLfloat u, v;        // texture coords.
} VERTEX;

typedef struct {         // triangle
    VERTEX vertex[3];    // 3 vertices array
} TRIANGLE;

typedef struct {         // sector of a 3d environment
    int numtriangles;    // number of triangles in the sector
    TRIANGLE* triangle;  // pointer to array of triangles.
} SECTOR;

SECTOR sector1;
SECTOR sector2;
SECTOR sector3;

/* Image type - contains height, width, and data */
typedef struct {
    int sizeX;
    int sizeY;
    char *data;
} Image;

class Setup {

public: 
  void SetupWorld();
  void SetupFloor();
  void SetupGrass();
  
} ;

// degrees to radians...2 PI radians = 360 degrees
float rad(float angle)
{
    return angle * piover180;
}

// helper for SetupWorld.  reads a file into a string until a nonblank, non-comment line
// is found ("/" at the start indicating a comment); assumes lines < 255 characters long.
void readstr(FILE *f, char *string)
{
    do {
	fgets(string, 255, f); // read the line
    } while ((string[0] == '/') || (string[0] == '\n'));
    return;
}

// loads the world from a text file.
void Setup::SetupWorld() 
{
    float x, y, z, u, v;
    int vert;
    int numtriangles;
    FILE *filein;        // file to load the world from
    char oneline[255];

    filein = fopen("Data/world.txt", "rt");

    readstr(filein, oneline);
    sscanf(oneline, "NUMPOLLIES %d\n", &numtriangles);

    sector1.numtriangles = numtriangles;
    sector1.triangle = (TRIANGLE *) malloc(sizeof(TRIANGLE)*numtriangles);
    
    for (loop = 0; loop < numtriangles; loop++) {
	for (vert = 0; vert < 3; vert++) {
	    readstr(filein,oneline);
	    sscanf(oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
	    sector1.triangle[loop].vertex[vert].x = x;
	    sector1.triangle[loop].vertex[vert].y = y;
	    sector1.triangle[loop].vertex[vert].z = z;
	    sector1.triangle[loop].vertex[vert].u = u;
	    sector1.triangle[loop].vertex[vert].v = v;
	}
    }

    fclose(filein);
    return;
}
    
   
void Setup::SetupFloor() 
{
    float x, y, z, u, v;
    int vert;
    int numtriangles;
    FILE *filein;        // file to load the world from
    char oneline[255];

    filein = fopen("Data/floor.txt", "rt");

    readstr(filein, oneline);
    sscanf(oneline, "NUMPOLLIES %d\n", &numtriangles);

    sector2.numtriangles = numtriangles;
    sector2.triangle = (TRIANGLE *) malloc(sizeof(TRIANGLE)*numtriangles);
    
    for (loop = 0; loop < numtriangles; loop++) {
	for (vert = 0; vert < 3; vert++) {
	    readstr(filein,oneline);
	    sscanf(oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
	    sector2.triangle[loop].vertex[vert].x = x;
	    sector2.triangle[loop].vertex[vert].y = y;
	    sector2.triangle[loop].vertex[vert].z = z;
	    sector2.triangle[loop].vertex[vert].u = u;
	    sector2.triangle[loop].vertex[vert].v = v;
	}
    }

    fclose(filein);
    return;
}


void Setup::SetupGrass() 
{
    float x, y, z, u, v;
    int vert;
    int numtriangles;
    FILE *filein;        // file to load the world from
    char oneline[255];

    filein = fopen("Data/grass.txt", "rt");

    readstr(filein, oneline);
    sscanf(oneline, "NUMPOLLIES %d\n", &numtriangles);

    sector3.numtriangles = numtriangles;
    sector3.triangle = (TRIANGLE *) malloc(sizeof(TRIANGLE)*numtriangles);
    
    for (loop = 0; loop < numtriangles; loop++) {
	for (vert = 0; vert < 3; vert++) {
	    readstr(filein,oneline);
	    sscanf(oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
	    sector3.triangle[loop].vertex[vert].x = x;
	    sector3.triangle[loop].vertex[vert].y = y;
	    sector3.triangle[loop].vertex[vert].z = z;
	    sector3.triangle[loop].vertex[vert].u = u;
	    sector3.triangle[loop].vertex[vert].v = v;
	}
    }

    fclose(filein);
    return;
}
    
    
    FILE *file;

int ImageLoad(char *filename, Image *image) {

    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1) 
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // used to convert bgr to rgb color.

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL)
    {
	printf("File Not Found : %s\n",filename);
	return 0;
    }
    
    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
	printf("Error reading width from %s.\n", filename);
	return 0;
    }
    printf("Width of %s: %lu\n", filename, image->sizeX);
    
    // read the height 
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
	printf("Error reading height from %s.\n", filename);
	return 0;
    }
    printf("Height of %s: %lu\n", filename, image->sizeY);
    
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
	printf("Error reading planes from %s.\n", filename);
	return 0;
    }
    if (planes != 1) {
	printf("Planes from %s is not 1: %u\n", filename, planes);
	return 0;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
	printf("Error reading bpp from %s.\n", filename);
	return 0;
    }
    if (bpp != 24) {
	printf("Bpp from %s is not 24: %u\n", filename, bpp);
	return 0;
    }
	
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data. 
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
	printf("Error allocating memory for color-corrected image data");
	return 0;	
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
	printf("Error reading image data from %s.\n", filename);
	return 0;
    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
	temp = image->data[i];
	image->data[i] = image->data[i+2];
	image->data[i+2] = temp;
    }
    fclose(file);

    // we're done.
    return 1;
}
    // Load Texture
 Image *image1 , *image2 , *image3, *image4, *image5, *image6, *image7, *image8, *image9;
// Load Bitmaps And Convert To Textures
GLvoid LoadGLTextures() 
{	

    
    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/download.bmp", image1)) {
	exit(1);
	
    }
    
   image2 = (Image *) malloc(sizeof(Image));
    if (image2 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/floor.bmp", image2)) {
	exit(1);
      }    
      
    image3 = (Image *) malloc(sizeof(Image));
    if (image3 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/grass.bmp", image3)) {
	exit(1);  
    }
    
     image4 = (Image *) malloc(sizeof(Image));
    if (image4 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/sky.bmp", image4)) {
	exit(1);  
    }
    
      image5 = (Image *) malloc(sizeof(Image));
    if (image5 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/images.bmp", image5)) {
	exit(1);  
    }
    
      image6 = (Image *) malloc(sizeof(Image));
    if (image6 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/security.bmp", image6)) {
	exit(1);  
    }
    
     image7 = (Image *) malloc(sizeof(Image));
    if (image7 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/comm.bmp", image7)) {
	exit(1);  
    }
    
    image8 = (Image *) malloc(sizeof(Image));
    if (image8 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/laser_gun.bmp", image8)) {
	exit(1);  
    }
    
    image9 = (Image *) malloc(sizeof(Image));
    if (image9 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/tower.bmp", image9)) {
	exit(1);  
    }

    // Create Textures	
    glGenTextures(8, &texture[0]);

    // nearest filtered texture
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // scale cheaply when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // scale cheaply when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image3->sizeX, image3->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image3->data);
    
    free(image3);

    // linear filtered texture
    glBindTexture(GL_TEXTURE_2D, texture[1]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    
    free(image1);

    // mipmapped texture
    glBindTexture(GL_TEXTURE_2D, texture[2]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale mipmap when image smalled than texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image2->sizeX, image2->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image2->data);
    
    free(image2); 
    
    // mipmapped texture
    glBindTexture(GL_TEXTURE_2D, texture[3]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale mipmap when image smalled than texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image4->sizeX, image4->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image4->data);
    
    free(image4); 
    
    // mipmapped texture
    glBindTexture(GL_TEXTURE_2D, texture[4]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale mipmap when image smalled than texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image5->sizeX, image5->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image5->data);
    
    free(image5); 
    
    // mipmapped texture
    glBindTexture(GL_TEXTURE_2D, texture[5]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale mipmap when image smalled than texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image6->sizeX, image6->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image6->data);
    
    free(image6); 
    
    // mipmapped texture
    glBindTexture(GL_TEXTURE_2D, texture[6]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale mipmap when image smalled than texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image7->sizeX, image7->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image7->data);
    
    free(image7); 
    
    // mipmapped texture
    glBindTexture(GL_TEXTURE_2D, texture[7]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale mipmap when image smalled than texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image8->sizeX, image8->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image8->data);
    
    free(image8); 
    
    // mipmapped texture
    glBindTexture(GL_TEXTURE_2D, texture[8]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale mipmap when image smalled than texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image9->sizeX, image9->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image9->data);
    
    free(image9); 
};

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
GLvoid InitGL(GLsizei Width, GLsizei Height)	// We call this right after our OpenGL window is created.
{
    LoadGLTextures();                           // load the textures.
    glEnable(GL_TEXTURE_2D);                    // Enable texture mapping.

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);          // Set the blending function for translucency (note off at init time)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// This Will Clear The Background Color To Black
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);                       // type of depth test to do.
    glEnable(GL_DEPTH_TEST);                    // enables depth testing.
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                           // Reset The Projection Matrix
    
    glOrtho(-100.0, 100.0, -100.0, 100.0, -100.0, 100.0);
    
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,500.0f);	// Calculate The Aspect Ratio Of The Window
    
    glMatrixMode(GL_MODELVIEW);
    
    
    GLfloat LightAmbient[]  = {0.5f, 0.5f, 0.5f, 1.0f}; 
GLfloat LightDiffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f}; 
GLfloat LightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};
    
    //For smoke
    
    const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 0.0f, 0.0f, 3.0f, 1.0f };
//const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
//const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
//const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
//const GLfloat high_shininess[] = { 100.0f };
    //////////////////////////

    // set up lights.
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
    glEnable(GL_LIGHT1);
    
     glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    
    ////For smoke
  //    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
  //  glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
  //  glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
  //  glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    ////////////////////////////////////
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
GLvoid ReSizeGLScene(GLsizei Width, GLsizei Height)
{
    if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
	Height=1;

    glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}


void sky(){
  
  
GLUquadric *sky = gluNewQuadric(); 

gluQuadricTexture(sky,GL_TRUE); 


glBindTexture(GL_TEXTURE_2D,texture[3]);

    glPushMatrix();
    glTranslatef(0.0,0.0,0.0);
	   gluSphere(sky,40.0,100,100);
    glPopMatrix();
    
gluDeleteQuadric(sky);      

}


void power(){
  
  
GLUquadric *power = gluNewQuadric(); 

gluQuadricTexture(power,GL_TRUE); 


glBindTexture(GL_TEXTURE_2D,texture[4]);

    glPushMatrix();
    glTranslatef(-20.0,1.2,3.75);
	   gluSphere(power,0.2,100,100);
    glPopMatrix();
    
gluDeleteQuadric(power);


 GLUquadric *power_tower = gluNewQuadric(); 

gluQuadricTexture(power_tower,GL_TRUE); 


glBindTexture(GL_TEXTURE_2D,texture[8]);

    glPushMatrix();
    
    glTranslatef(-20.0,0.0,3.75);
    glRotatef(-90,1,0,0);
	gluCylinder(	power_tower,
 		0.08,
 	 	0.08,
 	 	1.0,
 	 	100,
 	 	100);

    glPopMatrix();
    
gluDeleteQuadric(power_tower); 


GLUquadric *power_outlet = gluNewQuadric(); 

gluQuadricTexture(power_outlet,GL_TRUE); 


glBindTexture(GL_TEXTURE_2D,texture[8]);

    glPushMatrix();
    
    glTranslatef(-20.0,2.0,3.75);
    glRotatef(-90,1,0,0);
	gluCylinder(	power_outlet,
 		0.2,
 	 	0.2,
 	 	3.5,
 	 	100,
 	 	100);

    glPopMatrix();
    
gluDeleteQuadric(power_outlet); 


 GLUquadric *chip_tower = gluNewQuadric(); 

gluQuadricTexture(chip_tower,GL_TRUE); 

glBindTexture(GL_TEXTURE_2D,texture[8]);

    glPushMatrix();
    
    glTranslatef(-20.0,0.0,3.0);
    glRotatef(-90,1,0,0);
	gluCylinder(	chip_tower,
 		0.08,
 	 	0.08,
 	 	0.7,
 	 	100,
 	 	100);

    glPopMatrix();
    
gluDeleteQuadric(chip_tower); 


glBindTexture(GL_TEXTURE_2D,texture[6]);

    glPushMatrix();
    
    //glTranslatef(-20.0,0.0,3.0);
    //glRotatef(-90,1,0,0);
    glBegin(GL_QUADS);
    //glNormal3f( 0.0f, 0.0f, 1.0f);
    glTexCoord2f(1.0,0.0);
    glVertex3f(-20.0,0.7,2.975);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-20.0,0.8,2.975);
    glTexCoord2f(0.0,1.0);
    glVertex3f(-20.0,0.8,3.025);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-20.0,0.7,3.025);
    glEnd();

    glPopMatrix();
    

}



void security() {

  GLUquadric *security = gluNewQuadric(); 

gluQuadricTexture(security,GL_TRUE); 


glBindTexture(GL_TEXTURE_2D,texture[5]);

    glPushMatrix();
    
    glTranslatef(0.0,0.0,2.0);
    glRotatef(-90,1,0,0);
	gluCylinder(	security,
 		0.05,
 	 	0.05,
 	 	0.5,
 	 	50,
 	 	50);

    glPopMatrix();
    
gluDeleteQuadric(security); 

glBindTexture(GL_TEXTURE_2D,texture[6]);
glPushMatrix();
    glBegin(GL_QUADS);
    glNormal3f( 0.0f, 0.0f, 1.0f);
    glTexCoord2f(1.0,0.0);
    glVertex3f(0.12,0.4,2.1);
    glTexCoord2f(0.0,0.0);
    glVertex3f(0.12,0.3,2.1);
    glTexCoord2f(0.0,1.0);
    glVertex3f(-0.12,0.3,2.1);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-0.12,0.4,2.1);
    glEnd();
glPopMatrix();  


GLUquadric *laser_gun = gluNewQuadric(); 

gluQuadricTexture(laser_gun,GL_TRUE); 
  
glBindTexture(GL_TEXTURE_2D,texture[7]);

    glPushMatrix();
    glTranslatef(0.0,0.6,2.0);
	   gluSphere(laser_gun,0.08,100,100);
    glPopMatrix();
    
gluDeleteQuadric(laser_gun);

}

/*void Timer(int iUnused)
{
    glutPostRedisplay();
    glutTimerFunc(30, Timer, 0);
}*/

/////////For smoke
GLvoid Draw_smoke(void) {
 
	int thingy = 1;
	bool check = false;
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 glPushMatrix();
	 glTranslatef(-20.0,5.5,5.75);
	 
	/*
	
	glColor3d(.3, .1, 0);
	
	glutSolidCube(.3);
	glPopMatrix();
	*/
	if (check == false) {
		float R, G, B;
		glPushMatrix();
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < MAX_PARTICLES; i++) {
			//R = rand() % 100 + 1;
			//G = rand() % 100 + 1;
			//B = rand() % 100 + 1;
			//glColor3d(R*.01, G*.01, B*.01);
			glColor3f(1.0, 1.0, 1.0);
			//glColor3d(10, 10, 0);
			//glColor3d(0, 0+posY[i], 0);
			glVertex3f(X-.01, Y, -2);
			glVertex3f(X+.01, Y, -2);
			glVertex3f(X, Y+.02, -2);
			X = posX[i];
			Y = posY[i];
		}
		glEnd();
		glPopMatrix();
		check = true;
	}
	switch(thingy){
		case 1:
			//Sleep(1);
			moveParticles(currentParticle);
			if (currentParticle != MAX_PARTICLES) {
				currentParticle++;
			}
			
			glutPostRedisplay();
			break;
			
			
		
	}
	
	//glutSwapBuffers();
}


///////////////////////////////////////////////////


/* The main drawing function. */
GLvoid DrawGLScene()
{
    GLfloat x_m, y_m, z_m, u_m, v_m;
    GLfloat xtrans, ztrans, ytrans;
    GLfloat sceneroty;
    int numtriangles;

    // calculate translations and rotations.
    xtrans = -xpos;
    ztrans = -zpos;
    ytrans = -walkbias-0.25f;
    sceneroty = 360.0f - yrot;
    	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();
    
    
 
    glRotatef(lookupdown, 1.0f, 0, 0);
    glRotatef(sceneroty, 0, 1.0f, 0);

    glTranslatef(xtrans, ytrans, ztrans);    

    glBindTexture(GL_TEXTURE_2D, texture[1]); // pick the texture.
    
    
    numtriangles = sector1.numtriangles;

    for (loop=0; loop<numtriangles; loop++) {        // loop through all the triangles
	glBegin(GL_TRIANGLES);		
	glNormal3f( 0.0f, 0.0f, 1.0f);
	
	x_m = sector1.triangle[loop].vertex[0].x;
	y_m = sector1.triangle[loop].vertex[0].y;
	z_m = sector1.triangle[loop].vertex[0].z;
	u_m = sector1.triangle[loop].vertex[0].u;
	v_m = sector1.triangle[loop].vertex[0].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);
	
	x_m = sector1.triangle[loop].vertex[1].x;
	y_m = sector1.triangle[loop].vertex[1].y;
	z_m = sector1.triangle[loop].vertex[1].z;
	u_m = sector1.triangle[loop].vertex[1].u;
	v_m = sector1.triangle[loop].vertex[1].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);
	
	x_m = sector1.triangle[loop].vertex[2].x;
	y_m = sector1.triangle[loop].vertex[2].y;
	z_m = sector1.triangle[loop].vertex[2].z;
	u_m = sector1.triangle[loop].vertex[2].u;
	v_m = sector1.triangle[loop].vertex[2].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);	
	
	glEnd();	
	
    }
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	numtriangles = sector2.numtriangles;

    for (loop=0; loop<numtriangles; loop++) {        // loop through all the triangles
	glBegin(GL_TRIANGLES);		
	glNormal3f( 0.0f, 0.0f, 1.0f);
	
	x_m = sector2.triangle[loop].vertex[0].x;
	y_m = sector2.triangle[loop].vertex[0].y;
	z_m = sector2.triangle[loop].vertex[0].z;
	u_m = sector2.triangle[loop].vertex[0].u;
	v_m = sector2.triangle[loop].vertex[0].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);
	
	x_m = sector2.triangle[loop].vertex[1].x;
	y_m = sector2.triangle[loop].vertex[1].y;
	z_m = sector2.triangle[loop].vertex[1].z;
	u_m = sector2.triangle[loop].vertex[1].u;
	v_m = sector2.triangle[loop].vertex[1].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);
	
	x_m = sector2.triangle[loop].vertex[2].x;
	y_m = sector2.triangle[loop].vertex[2].y;
	z_m = sector2.triangle[loop].vertex[2].z;
	u_m = sector2.triangle[loop].vertex[2].u;
	v_m = sector2.triangle[loop].vertex[2].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);	
	
	glEnd();	
    }
   
   
   glBindTexture(GL_TEXTURE_2D, texture[0]);
	numtriangles = sector3.numtriangles;

    for (loop=0; loop<numtriangles; loop++) {        // loop through all the triangles
	glBegin(GL_TRIANGLES);		
	glNormal3f( 0.0f, 0.0f, 1.0f);
	
	x_m = sector3.triangle[loop].vertex[0].x;
	y_m = sector3.triangle[loop].vertex[0].y;
	z_m = sector3.triangle[loop].vertex[0].z;
	u_m = sector3.triangle[loop].vertex[0].u;
	v_m = sector3.triangle[loop].vertex[0].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);
	
	x_m = sector3.triangle[loop].vertex[1].x;
	y_m = sector3.triangle[loop].vertex[1].y;
	z_m = sector3.triangle[loop].vertex[1].z;
	u_m = sector3.triangle[loop].vertex[1].u;
	v_m = sector3.triangle[loop].vertex[1].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);
	
	x_m = sector3.triangle[loop].vertex[2].x;
	y_m = sector3.triangle[loop].vertex[2].y;
	z_m = sector3.triangle[loop].vertex[2].z;
	u_m = sector3.triangle[loop].vertex[2].u;
	v_m = sector3.triangle[loop].vertex[2].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);	
	
	glEnd();
      
        
    }
   	sky();
	power();
	security();
	Draw_smoke();
   
    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();
}




/* The function called whenever a normal key is pressed. */
void keyPressed(unsigned char key, int x, int y) 
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key) {    
    case ESCAPE: // kill everything.
	/* exit the program...normal termination. */
	exit(1);                   	
	break; // redundant.

    case 'b': 
    case 'B': // switch the blending
	printf("B/b pressed; blending is: %d\n", blend);
	blend = blend ? 0 : 1;              // switch the current value of blend, between 0 and 1.
	if (blend) {
	    glEnable(GL_BLEND);
	    glDisable(GL_DEPTH_TEST);
	} else {
	    glDisable(GL_BLEND);
	    glEnable(GL_DEPTH_TEST);
	}
	printf("Blending is now: %d\n", blend);
	break;

    case 'f': 
    case 'F': // switch the filter
	printf("F/f pressed; filter is: %d\n", filter);
	filter++;                           // switch the current value of filter, between 0/1/2;
	if (filter > 2) {
	    filter = 0;
	}
	printf("Filter is now: %d\n", filter);
	break;

    case 'l': 
    case 'L': // switch the lighting
	printf("L/l pressed; lighting is: %d\n", light);
	light = light ? 0 : 1;              // switch the current value of light, between 0 and 1.
	if (light) {
	    glEnable(GL_LIGHTING);
	} else {
	    glDisable(GL_LIGHTING);
	}
	printf("Lighting is now: %d\n", light);
	break;

    default:
      printf ("Key %d pressed. No action there yet.\n", key);
      break;
    }	
}

/* The function called whenever a normal key is pressed. */
void specialKeyPressed(int key, int x, int y) 
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key) {    
    case GLUT_KEY_PAGE_UP: // tilt up
	z -= 0.2f;
	lookupdown -= 0.2f;
	break;
    
    case GLUT_KEY_PAGE_DOWN: // tilt down
	z += 0.2f;
	lookupdown += 1.0f;
	break;

    case GLUT_KEY_UP: // walk forward (bob head)
	xpos -= (float)sin(yrot*piover180) * 0.05f;
	zpos -= (float)cos(yrot*piover180) * 0.05f;	
	if (walkbiasangle >= 359.0f)
	    walkbiasangle = 0.0f;	
	else 
	    walkbiasangle+= 10;
	walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
	break;

    case GLUT_KEY_DOWN: // walk back (bob head)
	xpos += (float)sin(yrot*piover180) * 0.05f;
	zpos += (float)cos(yrot*piover180) * 0.05f;	
	if (walkbiasangle <= 1.0f)
	    walkbiasangle = 359.0f;	
	else 
	    walkbiasangle-= 10;
	walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
	break;

    case GLUT_KEY_LEFT: // look left
	yrot += 1.5f;
	break;
    
    case GLUT_KEY_RIGHT: // look right
	yrot -= 1.5f;
	break;

    default:
	printf ("Special key %d pressed. No action there yet.\n", key);
	break;
    }	
}

int main(int argc, char **argv) 
{  
    Setup setup;
    /* load our world from disk */
    setup.SetupWorld();
    setup.SetupFloor();
    setup.SetupGrass();
    /* Initialize GLUT state - glut will take any command line arguments that pertain to it or 
       X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */  
    glutInit(&argc, argv);  

    /* Select type of Display mode:   
     Double buffer 
     RGBA color
     Depth buffer 
     Alpha blending */  
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);  

    /* get a 640 x 480 window */
    glutInitWindowSize(640, 480);  

    /* the window starts at the upper left corner of the screen */
    glutInitWindowPosition(0, 0);  

    /* Open a window */  
    window = glutCreateWindow("Background");  

    /* Register the function to do all our OpenGL drawing. */
    glutDisplayFunc(&DrawGLScene);  
    
    //For smoke
   // glutDisplayFunc(&Draw_smoke); 

    /* Go fullscreen.  This is as soon as possible. */
    glutFullScreen();

    /* Even if there are no events, redraw our gl scene. */
    glutIdleFunc(&DrawGLScene); 
    
    

    /* Register the function called when our window is resized. */
    glutReshapeFunc(&ReSizeGLScene);

    /* Register the function called when the keyboard is pressed. */
    glutKeyboardFunc(&keyPressed);

    /* Register the function called when special keys (arrows, page down, etc) are pressed. */
    glutSpecialFunc(&specialKeyPressed);

    /* Initialize our window. */
    InitGL(640, 480);
    
    //Timer(0);
  
    /* Start Event Processing Engine */  
    glutMainLoop();  

    return 1;
}
