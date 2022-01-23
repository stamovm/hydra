#include <hydra/System.h>
#include <hydra/Utility.h>
#include <hydra/Transform.h>
#include <hydra/gfx/gfx.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <AL/alut.h>
#include <AL/al.h>
#include <iostream>
#include <SOIL/SOIL.h>

using namespace hydra;
float rotation = 0.0f;
std::vector<Vec3> cubePositions;
Lock lock;
hydra::Transform transform;
Vec3f soundPosition,tmpPosition;

GLuint tex2d[6];
int nmbrOfTextures = 6;
#include "functions.h"
void LoadData();
void SaveData();
bool chColor = false;
bool chSize = false;
bool playSound = false;
int currentSound = -1;

std::vector<Object3D> Objects;
Object3D obj;


void update()
{
    const Button* button = Input::getButton("exit");
    if (button->delta() == Button::Pressed)
    {
        System::shutdown();
        return;
    }
    float move = Input::getValuator("yAxis")->state();
    if (fabs(move) > 0.125)
    {
        transform.translate(Input::getSixDOF("wand")->forward() * System::dt() * move * 10.0);

    }
    float brotation = Input::getValuator("xAxis")->state();
    if (fabs(brotation) > 0.125)
    {
        transform.rotate(Vec3f(0,1,0), -brotation * System::dt());
    }
//    if (Input::getButton("button0")->state())
//    {
//        rotation += System::dt();
//    }

    const SixDOF* s = Input::getSixDOF("wand");
    Vec3f p = s->position();
    float lightPos2[ ] = { p[0], p[1], p[2], 1.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);

    if (Input::getButton("button1")->delta() == Button::Pressed)    {//change texture(1)

        obj.txID++;
        if ( obj.txID > nmbrOfTextures ) obj.txID=0;

    }
    if (Input::getButton("button2")->delta() == Button::Pressed)    {//play sound(2)
        if(playSound){
            playSound = false;
        }else{
//            tmpPosition=p;
            playSound = true;
            currentSound = -1;
        }
    }
    if (Input::getButton("button3")->delta() == Button::Pressed)    { //save(3)
        SaveData();
    }
    if (Input::getButton("button4")->delta() == Button::Pressed)    { //load(4)
        LoadData();
    }
    if (Input::getButton("button5")->delta() == Button::Pressed)    { //reset(5)
        Objects.clear();
    }
    if (Input::getButton("button6")->delta() == Button::Pressed)    {//change size(6)
        if(chSize){
            chSize = false;
        }else{
            tmpPosition=p;
            chSize = true;
        }
    }
    if (Input::getButton("button7")->delta() == Button::Pressed)    { //change color(7)

        if(chColor){
            chColor = false;
        }else{
            tmpPosition=p;
            chColor = true;
        }
    }

    fflush(stdout);

    lock.writeLock();
    {

        obj.position=transform.toVirtualPoint(p);
        if (Input::getButton("button0")->delta() == Button::Pressed){//delta() == Button::Pressed)
            Objects.push_back(obj);
            chColor=false;
            chSize=false;


        }

    }
    lock.writeUnlock();
}
//----------------Render------------------------------------------------------------------
void render()
{
    glClearColor(0,0,0,1);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const SixDOF* s = Input::getSixDOF("wand");
    Vec3f p = s->position();
    glPointSize(10.0);
    glColor3f(1,0,0);
    glBegin(GL_POINTS);
        glVertex3fv(p.v);
    glEnd();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(s->matrix().v);
    //glScalef(0.7,0.7,0.9);
    glRotatef(rotation * 180.0f, 0, 0, 1);
    glutWireCone(0.04, -0.2, 9, 4);
    glPopMatrix();

    lock.readLock();   //forall cubes............
    { //draw cube at point p

        glEnable(GL_TEXTURE_2D);
/*      glBindTexture( GL_TEXTURE_2D, tex2d[1] );
        glPushMatrix();
        glMultMatrixd(transform.matrix().v);
        glTranslatef(0,0,-3);
        //glRotatef(90,0,1,0);
        glColor3f(1,1,1);
        glBegin(GL_QUADS);
        glNormal3f( 0.0f, 0.0f, 1.0f);      // Normal Facing Forward
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f,  0.0f,  1.0f);  // Bottom Left Of The Texture and Quad
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 2.0f,  0.0f,  1.0f);  // Bottom Right Of The Texture and Quad
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 2.0f,  2.0f,  1.0f);  // Top Right Of The Texture and Quad
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f,  2.0f,  1.0f);  // Top Left Of The Texture and Quad
        glEnd();
        glPopMatrix();//*/

        for(int i=Objects.size()-1;i>=0;i--){ //draw all objects from the vector
            Objects[i].Draw();
        }
        glDisable(GL_TEXTURE_2D);

        if(chColor){
            obj.color[0]=fabs(p[0]-tmpPosition[0])*2;
            obj.color[1]=fabs(p[1]-tmpPosition[1])*2;
            obj.color[2]=fabs(p[2]-tmpPosition[2])*2;
            obj.color=normColor(obj.color); //make sure that rgb values are between 0-1

            obj.type=1;
            obj.Draw();
            obj.type=0;
            glPushMatrix();
            glMultMatrixf(s->matrix().v);
            glBegin(GL_LINES);
            glColor3f(1,0,0);
            glVertex3f(-obj.color[0]/2,0,0);
            glVertex3f(obj.color[0]/2,0,0);
            glColor3f(0,1,0);
            glVertex3f(0,-obj.color[1]/2,0);
            glVertex3f(0,obj.color[1]/2,0);
            glColor3f(0,0,1);
            glVertex3f(0,0,+obj.color[2]/2);
            glVertex3f(0,0,-obj.color[2]/2);
            glEnd();
            glPopMatrix();


        }
        if(chSize){
            obj.size=getDist(p,tmpPosition)+0.05;

            obj.type=1;
            obj.Draw();
            obj.type=0;
        }


    }
    lock.unlock();
}
//-----------------------sound-------------------------------------------------------------------------------
struct ALData
{
    ALuint buffer;
    ALuint source;
    int play;

};
ALData* snd = new ALData[8];

int LoadAndAssignWAV(const char* _fileName, ALuint _buffer,int sndID)
{
    int			error;
    ALenum		format;
    ALsizei		size;
    ALsizei		freq;
    ALboolean           loop;
    ALvoid		*data;

    // Load in the WAV file from disk
    alutLoadWAVFile((ALbyte *)_fileName, &format, &data, &size, &freq, &loop);
    if ((error = alGetError()) != AL_NO_ERROR)  {
        std::cerr << "Error: Unable to load sound file: "<< _fileName << std::endl;
        fflush(stdout);
        System::shutdown();
    }

    alBufferData(_buffer,format,data,size,freq);
    alutUnloadWAV(format,data,size,freq);

    alSourcei(snd[sndID].source, AL_BUFFER, snd[sndID].buffer);
    alSourcef(snd[sndID].source, AL_GAIN, 1.0);
    Vec3f one = Vec3f::one();
    alSourcefv(snd[sndID].source, AL_POSITION, one.v);
    alSourcei(snd[sndID].source, AL_LOOPING, AL_FALSE);


}

void alrender()
{
    if(Objects.size()==0) return;
    if(playSound){
        ALData* snd = (ALData*)System::contextData();
        Vec3f sp;
        if (currentSound==-1){ //first obj
            currentSound++;
            sp = transform.toRealPoint(Objects[currentSound].position);
            alSourcefv(snd[Objects[currentSound].txID].source, AL_POSITION, Objects[currentSound].position.v);
            alSourcePlay(snd[Objects[currentSound].txID].source);

        }else{
//            printf("current=%i    ,  %i \n", currentSound,Objects[currentSound].txID);
            if (currentSound>=Objects.size()-1) {
                playSound = false;
                return;
            }
            ALint val;
            alGetSourcei(snd[Objects[currentSound].txID].source, AL_SOURCE_STATE, &val);
            if (val!=AL_PLAYING){
                currentSound++;
                sp = transform.toRealPoint(Objects[currentSound].position);
                alSourcefv(snd[Objects[currentSound].txID].source, AL_POSITION, sp.v);
                alSourcePlay(snd[Objects[currentSound].txID].source);
            }
        }

    }


}

void alinit()
{

    Vec3f zero = Vec3f::zero();
    Vec3f one = Vec3f::one();
    alListenerfv(AL_POSITION, zero.v);
    alListenerfv(AL_VELOCITY, zero.v);

    for (int i = 0; i<8; i++){
        alGenBuffers(i+1, &snd[i].buffer);
        alGenSources(i+1, &snd[i].source);
    }
    LoadAndAssignWAV("guitar.wav",snd[0].buffer,0);
    LoadAndAssignWAV("piano.wav",snd[1].buffer,1);
    LoadAndAssignWAV("guitar2.wav",snd[2].buffer,2);
    LoadAndAssignWAV("drums.wav",snd[3].buffer,3);
    LoadAndAssignWAV("guitar3.wav",snd[4].buffer,4);
    LoadAndAssignWAV("bass.wav",snd[5].buffer,5);
    LoadAndAssignWAV("shoot.wav",snd[6].buffer,6);
    LoadAndAssignWAV("doorbell.wav",snd[7].buffer,7);

    alDistanceModel(AL_INVERSE_DISTANCE);
    if (alGetError() != AL_NO_ERROR)
        System::shutdown();
    System::contextData(snd);
}
//----------------init--and----main----------------------------------------------------------------------------------
void initContext()
{
//  GLData* cd = new GLData;
//  cd->quadric = gluNewQuadric();
//  System::contextData(cd);

    tex2d[0] = GetTexture("stone.jpeg");
    tex2d[1] = GetTexture("marble.jpeg");
    tex2d[2] = GetTexture("wood.jpeg");
    tex2d[3] = GetTexture("flag.jpeg");
    tex2d[4] = GetTexture("bricks.png");
    tex2d[5] = GetTexture("water.jpeg");

    Lighting();
    float lightPos[ ] = { -1.0, 2.0f, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    LoadData();

}


int main(int argc, char** argv)
{
    if (!System::initialize(argc, argv))
        return -1;

    int numGlutArgs = 3;
    char* glutArgs[3];
    glutArgs[0] = "executable";
    glutArgs[1] = "-display";
    glutArgs[2] = ":0";
    glutInit(&numGlutArgs, glutArgs);
    alutInitWithoutContext(&argc, argv);

    System::registerCallback("update", update);
    System::registerCallback("render", render);
    System::registerCallback("initContext", initContext);
    System::registerCallback("alrender", alrender);
    System::registerCallback("alinit", alinit);

    System::run();

    return 0;
}
//=================Read==and==Write==to==file========================================================================
void LoadData(){
    hydra::Reporter::info("Loading Data from file: data.txt");
    std::ifstream indata;
    indata.open("data.txt"); // opens the file
    if( !indata ) {
        std::cerr << "Error: file could not be opened" << std::endl;
        fflush(stdout);
        return;
        //System::shutdown();
    }
    int objNmbr;
    Objects.clear();
    indata >>objNmbr;
    for (int i=0; i<objNmbr; i++){
        indata >> obj.size >> obj.txID;
        indata >> obj.position[0]>>obj.position[1]>> obj.position[2];
        indata >> obj.color[0]>>obj.color[1]>> obj.color[2];
        Objects.push_back(obj);

    }
    indata.close();
}
void SaveData(){
    hydra::Reporter::info("Saving data to file: data.txt");
    std::ofstream outdata;
    outdata.open("data.txt");
    if( !outdata ) {
        std::cerr << "Error: file could not be saved" << std::endl;
        fflush(stdout);
        System::shutdown();
    }
    outdata << Objects.size() << std::endl;
    for (int i=0; i<Objects.size(); i++){
        outdata << Objects[i].size<<" " << Objects[i].txID << std::endl;
        outdata << Objects[i].position[0]<<" "<<Objects[i].position[1]<<" "<< Objects[i].position[2] << std::endl;
        outdata << Objects[i].color[0]<<" "<<Objects[i].color[1]<<" "<< Objects[i].color[2] << std::endl;

    }
    outdata.close();
}
