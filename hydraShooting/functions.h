#ifndef FUNCTIONS_H
#define FUNCTIONS_H

GLfloat getDist(Vec3f P1,Vec3f P2);
GLfloat getDistFromLine(Vec3f P1,Vec3f P2,Vec3f P3);
GLfloat absg(GLfloat x);
Vec3 normColor(Vec3 ColorV);
GLuint GetTexture(std::string Filename);
void Lighting( void );
void skybox();
void box(GLfloat size,int txID);
void _glutBitmapString(void *font, char *s);
void showMessage(GLfloat x, GLfloat y,int white);//draw text at x, y
void message(GLfloat x, GLfloat y, GLfloat z, String msg,GLfloat scale = 0.0001);
String intToStr(String s = "a", int n =0 );

struct TPlayer{
    bool isReloading;
    bool laser;
    bool lvlOver;
    bool gameOver;
    int level;
    int score;
    int helth;
    int ammo;
    int maxAmmo;
    Vec3f bulitPos,bulitStep,headPos,gunPos,laserPos;
    GLfloat maxDist;
    TPlayer();
    void reset();
    void drawGun();
};

TPlayer::TPlayer(){
    reset();
}
void TPlayer::reset(){
    maxDist = 30.0;
    lvlOver = true;
    gameOver = false;
    level = 0;
    isReloading = false;
    laser = false;
    score = 0;
    helth = 100;
    ammo = 6;
    maxAmmo = 6;
    bulitStep[1]=-100;
}
void TPlayer::drawGun(){
    float s=0.03;
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, tex2d[7] );
    glColor3f(1,1,1);
    glPushMatrix();
    box(s,7);
    glTranslatef(0,0,-s);
    box(s,7);
    glTranslatef(0,0,-s);
    box(s,7);
    glTranslatef(0,0,-s);
    glutSolidCone(s*0.5, -s, 9, 4);
    box(s,7);
    glTranslatef(0,-s,3*s);
    box(s,7);
    glTranslatef(0,-s,0);
    box(s,7);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glColor3f(1,0,0);
    glBegin(GL_LINES);
    glVertex3f(bulitPos[0],bulitPos[1], bulitPos[2]);
    glVertex3f(bulitPos[0]+bulitStep[0],bulitPos[1]+bulitStep[1], bulitPos[2]+bulitStep[2]);
    glEnd();

}

struct TSound{
    int id;
    Vec3f position;
    GLfloat pitch;
    GLfloat gain;
    void reset();
};
void TSound::reset(){
    id=0;
    pitch=1.0;
    gain=1.0;
    position[0]=0;
    position[1]=0;
    position[2]=0;
}

//--------------------------------------------------------------------------------------------------------------
#define NUM_PARTICLES    1000          /* Number of particles  */
#define NUM_DEBRIS       200            /* Number of debris     */
#define NORMALIZE_SPEED    0
/* A particle */
struct particleData{
  float   position[3];
  float   speed[3];
  float   color[3];
};
//typedef struct particleData    particleData;

/* A piece of debris */
struct debrisData{
  float   position[3];
  float   speed[3];
  float   orientation[3];        /* Rotation angles around x, y, and z axes */
  float   orientationSpeed[3];
  float   color[3];
  float   scale[3];
};
//typedef struct debrisData    debrisData;
struct Object3D{ //a struct to store a 3d object
    Vec3f color,position, step;
    float size;
    int level;
    int type;
    int txID;
    int explode;
    bool enabled,armed;
    Object3D();
    void Draw();
    void Update(GLfloat dt);
    void resetExplosion ();
    void newSpeed (float dest[3]);
    particleData     particles[NUM_PARTICLES];
    debrisData       debris[NUM_DEBRIS];
    int wantNormalize, fuel;
};

Object3D::Object3D(){
    enabled=true;
    explode=0;
    armed=true;
    level = 0;
    fuel=800;
    wantNormalize=NORMALIZE_SPEED;
    position[0]=0;
    position[1]=0;
    position[2]=0;
    color[0]=1;
    color[1]=1;
    color[2]=1;
    size=0.3;
    type=0;
    txID=6;
}
void Object3D::newSpeed (float dest[3]){
    float    x,y,z,len;
    x = (4.0 * ((float) rand ()) / ((float) RAND_MAX)) - 2.0;
    y = (4.0 * ((float) rand ()) / ((float) RAND_MAX)) - 2.0;
    z = (4.0 * ((float) rand ()) / ((float) RAND_MAX)) - 2.0;
    if (wantNormalize){
        len = sqrt (x * x + y * y + z * z);
        if (len){
            x = x / len;
            y = y / len;
            z = z / len;
        }
    }
	
    dest[0] = x;
    dest[1] = y;
    dest[2] = z;
}

void Object3D::resetExplosion (){
    int i;
    for (i = 0; i < NUM_PARTICLES; i++){
        particles[i].position[0] = 0.0;
        particles[i].position[1] = 0.0;
        particles[i].position[2] = 0.0;
        particles[i].color[0] = 1.0;
        particles[i].color[1] = 1.0;
        particles[i].color[2] = 0.5;
        newSpeed (particles[i].speed);
    }

    for (i = 0; i < NUM_DEBRIS; i++){
        debris[i].position[0] = 0.0;
        debris[i].position[1] = 0.0;
        debris[i].position[2] = 0.0;
        debris[i].orientation[0] = 0.0;
        debris[i].orientation[1] = 0.0;
        debris[i].orientation[2] = 0.0;
        debris[i].color[0] = color[0];
        debris[i].color[1] = color[1];
        debris[i].color[2] = color[2];
        debris[i].scale[0] = ((2.0 *((float) rand ()) / ((float) RAND_MAX)) - 1.0)*0.8;
        debris[i].scale[1] = ((2.0 *((float) rand ()) / ((float) RAND_MAX)) - 1.0)*0.8;
        debris[i].scale[2] = ((2.0 *((float) rand ()) / ((float) RAND_MAX)) - 1.0)*0.8;
        newSpeed (debris[i].speed);
        newSpeed (debris[i].orientationSpeed);
    }
    fuel=1400;
    explode=false;
}

void Object3D::Draw(){
    if(enabled){
        glPushMatrix();
        glMultMatrixd(transform.matrix().v);
        glTranslatef(position[0],position[1],position[2]);
        glColor3f(color[0],color[1],color[2]);
        if(type){
            glDisable(GL_TEXTURE_2D);
            glColor3f(1,0,0);
            glutSolidSphere(size,20,20);
            glEnable(GL_TEXTURE_2D);
            wantNormalize=1;
        }
        else {
            box(size,txID);
        }

        glPopMatrix();
    }
    int    i;
    //    wantNormalize = 0;   // Speed vector normalization flag - 1 makes expolsion rounded
    if (explode){
        if (fuel > 0){
            glPushMatrix ();
            glMultMatrixd(transform.matrix().v);
            glTranslatef(position[0],position[1],position[2]);
	glDisable(GL_TEXTURE_2D);
            glBegin (GL_POINTS);
            for (i = 0; i < NUM_PARTICLES; i++) {
                glColor3fv (particles[i].color);
                glVertex3fv (particles[i].position);
            }
            glEnd ();
            glPopMatrix ();
            glNormal3f (0.0, 0.0, 1.0);
            for (i = 0; i < NUM_DEBRIS; i++){
//                glColor3fv (debris[i].color);
                glColor3f(color[0],color[1],color[2]);
                glPushMatrix ();
                glTranslatef(position[0],position[1],position[2]);
                glTranslatef (debris[i].position[0],debris[i].position[1],debris[i].position[2]);
                glRotatef (debris[i].orientation[0], 1.0, 0.0, 0.0);
                glRotatef (debris[i].orientation[1], 0.0, 1.0, 0.0);
                glRotatef (debris[i].orientation[2], 0.0, 0.0, 1.0);
                glScalef (debris[i].scale[0],debris[i].scale[1],debris[i].scale[2]);
                glBegin (GL_TRIANGLES);
                glVertex3f (0.0, 0.5, 0.0);
                glVertex3f (-0.25, 0.0, 0.0);
                glVertex3f (0.25, 0.0, 0.0);
                glEnd ();

                glPopMatrix ();
            }
            glEnable(GL_TEXTURE_2D);
        }

    }

}
void Object3D::Update(GLfloat dt){
    if(enabled){
        position[0]+=step[0]*dt;
        position[1]+=step[1]*dt;
        position[2]+=step[2]*dt;
        if(position[1]<-16){
            position[0]=rand() % 500 /100.0 -2.5;
            position[1]=rand() % 16 +level*2.0;
            position[2]=-(rand() % 20 +2.0);
            step[1]=-(rand() % 100 /1000.0 + level/10.0);
            color[0]=rand() % 100 / 100.0;
            color[1]=rand() % 100 / 100.0;
            color[2]=rand() % 100 / 100.0;
            size=rand() % 100 / 100.0 +0.3;
        }
    }
    if ((fuel > 0) && explode) {
        int i;
        for (i = 0; i < NUM_PARTICLES; i++){
            particles[i].position[0] += particles[i].speed[0] * 0.2;
            particles[i].position[1] += particles[i].speed[1] * 0.2;
            particles[i].position[2] += particles[i].speed[2] * 0.2;
            particles[i].color[0] -= 1.0 / 500.0;
            if (particles[i].color[0] < 0.0) {
                particles[i].color[0] = 0.0;
            }
            particles[i].color[1] -= 1.0 / 100.0;
            if (particles[i].color[1] < 0.0)  {
                particles[i].color[1] = 0.0;
            }
            particles[i].color[2] -= 1.0 / 50.0;
            if (particles[i].color[2] < 0.0)
            {
                particles[i].color[2] = 0.0;
            }
        }
        for (i = 0; i < NUM_DEBRIS; i++) {
            debris[i].position[0] += debris[i].speed[0] * 2 *dt;
            debris[i].position[1] += debris[i].speed[1] * 2 *dt;
            debris[i].position[2] += debris[i].speed[2] * 2 *dt;
            debris[i].orientation[0] += debris[i].orientationSpeed[0] * 200*dt;
            debris[i].orientation[1] += debris[i].orientationSpeed[1] * 200*dt;
            debris[i].orientation[2] += debris[i].orientationSpeed[2] * 200*dt;
        }
        --fuel;
    }
}

//-------------Load----Texture---------------------------------------------------------
GLuint GetTexture(std::string Filename){
    GLuint tex_ID;
    tex_ID = SOIL_load_OGL_texture(
                Filename.c_str(),
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_POWER_OF_TWO
                | SOIL_FLAG_MIPMAPS
//                | SOIL_FLAG_MULTIPLY_ALPHA
//                | SOIL_FLAG_COMPRESS_TO_DXT
                | SOIL_FLAG_DDS_LOAD_DIRECT
//                | SOIL_FLAG_INVERT_Y
                | SOIL_FLAG_NTSC_SAFE_RGB
                );
    if( 0 == tex_ID ) { /* check for an error during the load process */
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }
    if( tex_ID > 0 ){
        //glEnable( GL_TEXTURE_GEN_S );
        //glEnable( GL_TEXTURE_GEN_T );
        glEnable( GL_DEPTH_TEST );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
//        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ); // TRILINEAR
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
        glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
        return tex_ID;
    }
    else
        return 0;
}

GLfloat getDist(Vec3f P1,Vec3f P2){
    return sqrt( (P1[0]-P2[0])*(P1[0]-P2[0])+(P1[1]-P2[1])*(P1[1]-P2[1])+(P1[2]-P2[2])*(P1[2]-P2[2]) );
}
GLfloat getDistFromLine(Vec3f P1,Vec3f P2,Vec3f P3){
    GLfloat aa = (P2[0]-P1[0]);
    if (aa==0) aa = 0.000001;
    GLfloat A = (P2[1]-P1[1])/aa;
    GLfloat D = ( A*(P3[0]-P1[0]) + P1[1] - P3[1] ) / (sqrt(A*A+1));
    if (D<0) D=-D;
    return D;
}
GLfloat absg(GLfloat x){
    if (x<0) x=-x;
    return x;
}

Vec3 normColor(Vec3 ColorV){
    if (ColorV[0]>1) ColorV[0]=1;
    if (ColorV[1]>1) ColorV[1]=1;
    if (ColorV[2]>1) ColorV[2]=1;
    if (ColorV[0]<0) ColorV[0]=0;
    if (ColorV[1]<0) ColorV[1]=0;
    if (ColorV[2]<0) ColorV[2]=0;
    return ColorV;
}
//-------------------------------------LIGHTNING----------------------------------------
void Lighting( void ){    //sets the lights
    GLfloat ambient_light[ ] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat diffuse_light[ ] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat specular_light[ ] = { 1.0, 1.0, 1.0, 1.0 };
    glEnable( GL_LIGHTING );

    // global light
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambient_light );

    glEnable( GL_LIGHT0 );
    glEnable( GL_LIGHT1 );

    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse_light );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specular_light );

    glLightfv( GL_LIGHT1, GL_DIFFUSE, diffuse_light );
    glLightfv( GL_LIGHT1, GL_SPECULAR, specular_light );

    // matirials
    glMaterialfv( GL_FRONT, GL_AMBIENT, ambient_light );
    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse_light );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular_light );
    glMateriali( GL_FRONT, GL_SHININESS, 128 );

    glEnable( GL_COLOR_MATERIAL );

    glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
}


void _glutBitmapString(void *font, char *s){
    char *p = s;
    while(*p != '\0') {
        glutBitmapCharacter(font, *p);
        p++;
    }
}

void showMessage(GLfloat x, GLfloat y,int white = 0){
    if (white)glColor3f(1.0, 1.0, 0.0);
    else glColor3f(1.0, 0.0, 0.0);
    glRasterPos3f(x, y,-1.5);
    //_glutBitmapString(GLUT_BITMAP_9_BY_15, message);

}
////////////////////////////////////////////////////////////////////////////////
void skybox(){
    glPushMatrix();
    Vec3f camera = Input::getSixDOF("head")->forward();
    gluLookAt(  0,0,0, camera.x(),camera.y(),camera.z(),0,1,0);
    // Enable/Disable features
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    GLfloat size = 100.0f;
    // Just in case we set all vertices to white.
    glColor4f(1,1,1,1);
    // FRONT
    glBindTexture(GL_TEXTURE_2D, tex2d[0]);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f(  size, -size, -size );
    glTexCoord2f(0, 1); glVertex3f( -size, -size, -size );
    glTexCoord2f(0, 0); glVertex3f( -size,  size, -size );
    glTexCoord2f(1, 0); glVertex3f(  size,  size, -size );
    glEnd();

    // RIGHT
    glBindTexture(GL_TEXTURE_2D, tex2d[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f(  size, -size,  size );
    glTexCoord2f(0, 1); glVertex3f(  size, -size, -size );
    glTexCoord2f(0, 0); glVertex3f(  size,  size, -size );
    glTexCoord2f(1, 0); glVertex3f(  size,  size,  size );
    glEnd();

    // BACK
    glBindTexture(GL_TEXTURE_2D, tex2d[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f( -size, -size,  size );
    glTexCoord2f(0, 1); glVertex3f(  size, -size,  size );
    glTexCoord2f(0, 0); glVertex3f(  size,  size,  size );
    glTexCoord2f(1, 0); glVertex3f( -size,  size,  size );
    glEnd();

    // LEFT
    glBindTexture(GL_TEXTURE_2D, tex2d[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f( -size, -size, -size );
    glTexCoord2f(0, 1); glVertex3f( -size, -size,  size );
    glTexCoord2f(0, 0); glVertex3f( -size,  size,  size );
    glTexCoord2f(1, 0); glVertex3f( -size,  size, -size );
    glEnd();

    // UP
    glBindTexture(GL_TEXTURE_2D, tex2d[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f( -size,  size, -size );
    glTexCoord2f(0, 1); glVertex3f( -size,  size,  size );
    glTexCoord2f(0, 0); glVertex3f(  size,  size,  size );
    glTexCoord2f(1, 0); glVertex3f(  size,  size, -size );
    glEnd();

    // DOWN
    glBindTexture(GL_TEXTURE_2D, tex2d[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 0); glVertex3f( -size, -size, -size );
    glTexCoord2f(1, 1); glVertex3f( -size, -size,  size );
    glTexCoord2f(0, 1); glVertex3f(  size, -size,  size );
    glTexCoord2f(0, 0); glVertex3f(  size, -size, -size );
    glEnd();
    // Restore enable bits and matrix
    glPopAttrib();
    glPopMatrix();
}

void box(GLfloat size,int txID){
GLfloat half=size/2.0;
glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
glBegin(GL_QUADS);//bottom
{
    glNormal3d(0, -1, 0);
    glTexCoord2d(0.0,0.0);
    glVertex3f(half,-half,half);
    glTexCoord2d(1.0,0.0);
    glVertex3f(half,-half,-half);
    glTexCoord2d(1.0,1.0);
    glVertex3f(-half,-half,-half);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-half,-half,half);
}glEnd();

glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
glBegin(GL_QUADS);//top
{
    glNormal3d(0, 1, 0);
    glTexCoord2d(0.0,1.0);
    glVertex3f(-half,half,-half);
    glTexCoord2d(0.0,0.0);
    glVertex3f(-half,half,half);
    glTexCoord2d(1.0,0.0);
    glVertex3f(half,half,half);
    glTexCoord2d(1.0,1.0);
    glVertex3f(half,half,-half);
}glEnd();

glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
glBegin(GL_QUADS);//front
{
    glNormal3d(0, 0, 1);
    glTexCoord2d(0.0,0.0);
    glVertex3f(-half,-half,half);
    glTexCoord2d(1.0,0.0);
    glVertex3f(-half,half,half);
    glTexCoord2d(1.0,1.0);
    glVertex3f(half,half,half);
    glTexCoord2d(0.0,1.0);
    glVertex3f(half,-half,half);
}glEnd();

glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
glBegin(GL_QUADS);//back
{
    glNormal3d(0, 0, -1);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(half,-half,-half);
    glTexCoord2d(1.0,0.0);
    glVertex3f(half,half,-half);
    glTexCoord2d(1.0,1.0);
    glVertex3f(-half,half,-half);
    glTexCoord2d(0.0,1.0);
    glVertex3f(-half,-half,-half);
}glEnd();

glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
glBegin(GL_QUADS);//left
{
    glNormal3d(-1, 0, 0);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-half,-half,-half);
    glTexCoord2d(1.0,0.0);
    glVertex3f(-half,half,-half);
    glTexCoord2d(1.0,1.0);
    glVertex3f(-half,half,half);
    glTexCoord2d(0.0,1.0);
    glVertex3f(-half,-half,half);
}glEnd();

glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
glBegin(GL_QUADS);//right
{
    glNormal3d(1, 0, 0);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(half,-half,half);
    glTexCoord2d(1.0,0.0);
    glVertex3f(half,half,half);
    glTexCoord2d(1.0,1.0);
    glVertex3f(half,half,-half);
    glTexCoord2d(0.0,1.0);
    glVertex3f(half,-half,-half);
}glEnd();
}
void message(GLfloat x, GLfloat y, GLfloat z, String msg,GLfloat scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScaled(scale, scale, scale);
//    glTranslatef(x, y, z);
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
//    glRasterPos3f(x, y,z);
    char ch;
    for (int offset = 0; offset < msg.length(); offset++) {
        ch = msg[offset];
        glutStrokeCharacter(GLUT_STROKE_ROMAN, ch);
//        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ch);
    }
    glEnable(GL_LIGHTING);
    glPopMatrix();
}
String intToStr(String s, int n ){
  std::ostringstream r;
  r <<s<< n;
  return r.str();
  }


#endif 
