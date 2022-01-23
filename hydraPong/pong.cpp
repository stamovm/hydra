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
GLfloat rotation = 0.0f;
//std::vector<Vec3> cubePositions;
Lock lock;
hydra::Transform transform;


GLuint tex2d[6];
char message[46] = "Start";
#include "functions.h"
int frame = 0;

TBall ball;
TPaddle pad1, pad2;


void update(){
    const Button* button = Input::getButton("exit");
    if (button->delta() == Button::Pressed)
    {
        System::shutdown();
        return;
    }
    GLfloat move = Input::getValuator("yAxis")->state();
    if (fabs(move) > 0.125)
    {
//        transform.translate(Input::getSixDOF("wand")->forward() * System::dt() * move * 10.0);

    }
    GLfloat brotation = Input::getValuator("xAxis")->state();
    if (fabs(brotation) > 0.125)
    {
     //   transform.rotate(Vec3f(0,1,0), -brotation * System::dt());
    }

    const SixDOF* s = Input::getSixDOF("wand");
    Vec3f p = s->position();

    if (Input::getButton("button0")->delta() == Button::Pressed)    {//B 0
        ball.size+=0.01;


    }
    if (Input::getButton("button1")->delta() == Button::Pressed)    {//Tr
        ball.size-=0.01;


    }
    if (Input::getButton("button2")->delta() == Button::Pressed)    {//up left hand
        pad1.size+=0.02;

    }
    if (Input::getButton("button3")->delta() == Button::Pressed)    { //down left hand
        pad1.size-=0.02;
    }

    if (Input::getButton("button5")->delta() == Button::Pressed)    { //(reset)
        ball.Reset();
    }
    pad1.position=p;
    ball.Update(pad1,pad2);
    pad1.lastPosition=p;
    if(!ball.isOut)
        pad2.AIupdate(ball.position,ball.step[2]);
    fflush(stdout);

    lock.writeLock(); {



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
    pad1.position = p;
    GLfloat lightPos[ ] = { ball.position[0]+0.3, ball.position[1]+0.3, ball.position[2], 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    GLfloat lightPos2[ ] = { p[0], p[1], p[2], 1.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
    glMatrixMode(GL_MODELVIEW);
//    glPushMatrix();
//    glMultMatrixf(s->matrix().v);
//    //glScalef(0.7,0.7,0.9);
//    glRotatef(rotation * 180.0f, 0, 0, 1);
//    glutWireCone(0.04, -0.2, 9, 4);
//    glPopMatrix();


    lock.readLock();  {

        glEnable(GL_TEXTURE_2D);

        pad1.Draw();
        pad2.Draw();
        ball.Draw();
        ball.DrawWalls();
        glDisable(GL_TEXTURE_2D);


    }
    sprintf(message, "P1:%i       AI:%i", pad1.score,pad2.score);
    showMessage(-0.6,0.15);

    lock.unlock();
}
//-----------------------sound-------------------------------------------------------------------------------
struct ALData{
    ALuint buffer;
    ALuint source;
};
ALData* snd = new ALData[8];

int LoadAndAssignWAV(const char* _fileName, ALuint _buffer,int sndID){
    int			error;
    ALenum		format;
    ALsizei		size;
    ALsizei		freq;
    ALboolean           loop;
    ALvoid		*data;
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

void alrender(){
    ALData* snd = (ALData*)System::contextData();
    Vec3f sp;
    //sp = transform.toRealPoint(Objects[currentSound].position);
    if (ball.hit){
        alSourcefv(snd[ball.hitSndID].source, AL_POSITION, ball.position.v);
        if (ball.hitSndID==0)
            alSourcef(snd[ball.hitSndID].source, AL_PITCH, ball.step[0]+ball.step[1]+ball.step[2]+0.3);

        if (ball.hitSndID == 2)
            alSourcef(snd[ball.hitSndID].source, AL_GAIN, 0.3);
        else
            alSourcef(snd[ball.hitSndID].source, AL_GAIN, 1);
        alSourcePlay(snd[ball.hitSndID].source);
        ball.hit=false;
        ball.hitSndID=0;
    }
    if (ball.isOut>5){
        ALint val;
        alGetSourcei(snd[2].source, AL_SOURCE_STATE, &val);
        if (val!=AL_PLAYING){
            if(ball.position[2]<-1){
                pad1.score++;
            }else{
                pad2.score++;
            }
            ball.Reset();
        }
    }

}

void alinit(){
    Vec3f zero = Vec3f::zero();
    Vec3f one = Vec3f::one();
    alListenerfv(AL_POSITION, zero.v);
    alListenerfv(AL_VELOCITY, zero.v);
    for (int i = 0; i<8; i++){
        alGenBuffers(i+1, &snd[i].buffer);
        alGenSources(i+1, &snd[i].source);
    }
    LoadAndAssignWAV("wallHit.wav",snd[0].buffer,0);
    LoadAndAssignWAV("paddle.wav",snd[1].buffer,1);
    LoadAndAssignWAV("goal.wav",snd[2].buffer,2);
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
void initContext(){
    tex2d[0] = GetTexture("pavement.jpg");
    tex2d[1] = GetTexture("marble.jpeg");
    tex2d[2] = GetTexture("celing.jpg");
    tex2d[3] = GetTexture("ball1.png");
    tex2d[4] = GetTexture("wood.jpeg");
    //tex2d[5] = GetTexture("water.jpeg");

    Lighting();
    pad2.id=2;
    pad2.position[2]=ball.goalFar;
    pad2.position[1]=ball.maxUP/2;

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

