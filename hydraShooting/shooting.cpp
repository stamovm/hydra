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
Lock lock;
hydra::Transform transform;
Vec3f soundPosition,tmpPosition;

GLuint tex2d[8];//number Of Textures
#include "functions.h"

std::vector<Object3D> Objects,Projectiles;
std::vector<TSound> sounds;
TPlayer player;
Object3D obj;
TSound sound;
void setLevel(){
    if(player.lvlOver){
        player.level++;
        player.maxAmmo=5+player.level;
        Objects.clear();
        Projectiles.clear();
        obj.level=player.level;
        obj.enabled=true;
        obj.position[1]=-50;
        obj.resetExplosion();
        for (int i = 0;i<(2+player.level*10);i++){
            if (i>(2+player.level*10)/2)
                obj.level=player.level+1;
            obj.enabled=true;
            obj.type=0;
            Objects.push_back(obj);
            obj.enabled=false;
            obj.type=1;
            Projectiles.push_back(obj);
        }
        player.lvlOver=false;
    }
}
void checkLvl(){
    if (player.gameOver){
        message(player.headPos[0]-0.04,player.headPos[1]+0.01,player.headPos[2]-0.2,"GAME OVER!");
        message(player.headPos[0]-0.038,player.headPos[1]-0.005,player.headPos[2]-0.2,intToStr("Your score is: ",player.score),0.00006);
        message(player.headPos[0]-0.034,player.headPos[1]-0.02,player.headPos[2]-0.2,"(Press button 0 to restart)",0.00004);
    }
    if (player.lvlOver){
        message(player.headPos[0]-0.035,player.headPos[1]+0.005,player.headPos[2]-0.2,intToStr("Start level:",player.level+1));
        message(player.headPos[0]-0.02,player.headPos[1]-0.02,player.headPos[2]-0.2,"(Press button 0)",0.00004);
    }
}

//-----------------------------------------------------------------------------------
void update(){
    const Button* button = Input::getButton("exit");
    if (button->delta() == Button::Pressed){
        System::shutdown();
        return;
    }
    GLfloat move = Input::getValuator("yAxis")->state();
    if (fabs(move) > 0.125)  {
        transform.translate(Input::getSixDOF("wand")->forward() * System::dt() * move * 10.0);

    }
    GLfloat brotation = Input::getValuator("xAxis")->state();
    if (fabs(brotation) > 0.125) {
        transform.rotate(Vec3f(0,1,0), -brotation * System::dt());
    }

    if (Input::getButton("button2")->delta() == Button::Pressed)    {//up left hand

    }
    if (Input::getButton("button3")->delta() == Button::Pressed)    { //down left hand
    }
    if (Input::getButton("button5")->delta() == Button::Pressed)    { //(reset) Laser
        if(player.laser)
            player.laser=false;
        else
            player.laser=true;
        sound.id=8;
        sounds.push_back(sound);
    }
    fflush(stdout);
    GLfloat dt = System::dt();
    if(player.helth<=0) player.gameOver=true;

    lock.writeLock(); {
        if (Input::getButton("button0")->delta() == Button::Pressed)    {//B 0
            if(player.gameOver)  player.reset();
            player.ammo=player.maxAmmo;
            sound.id=2;
            sound.position=player.gunPos;
            sounds.push_back(sound);
            sound.reset();
            setLevel();
        }
        if (Input::getButton("button1")->delta() == Button::Pressed)    {//Trigger
            if ((player.ammo>0)&&(!player.isReloading)){
                if (getDist(player.gunPos,player.bulitPos) > player.maxDist){
                    player.ammo--;
                    player.bulitPos=player.gunPos;
                    sound.id=7;
                    sound.position[0]=(rand() % 300 / 300.0);
                    sound.position[1]=-(rand() % 800 /80.0 +1.0);
                    sounds.push_back(sound);
                    sound.reset();
                    sound.id=0;
                    sound.position=player.gunPos;
                    sounds.push_back(sound);
                    sound.reset();

                }
            }else{
                sound.id=1;
                sound.position=player.gunPos;
                sounds.push_back(sound);
                sound.reset();
            }
        }
        bool lvlOvr=true;
        for(int i=Objects.size()-1;i>=0;i--){
            if(!player.gameOver){
                Objects[i].Update(dt);
                Projectiles[i].Update(dt);
            }
            if((Objects[i].position[1]<2.8)&&(Objects[i].position[1]>-1)){
                if(Objects[i].armed){
                    if((rand() % 300)==1){ ////fire projectile at  chance
                        Projectiles[i].position=Objects[i].position;
                        Projectiles[i].size=Objects[i].size/4.0;
                        Projectiles[i].color[0]=1.0;
                        Projectiles[i].color[1]=0;
                        Projectiles[i].color[2]=0;
                        Projectiles[i].enabled=true;
                        GLfloat d1=getDist(player.headPos,Projectiles[i].position)/3.0;
                        Projectiles[i].step[0]= ( player.headPos[0]-Projectiles[i].position[0] )/d1;
                        Projectiles[i].step[1]= ( player.headPos[1]-Projectiles[i].position[1] )/d1;
                        Projectiles[i].step[2]= ( player.headPos[2]-Projectiles[i].position[2] )/d1;
                        Objects[i].armed=false;
                        sound.position=Objects[i].position;
                        sound.position[0]=sound.position[0]/8.0;
                        sound.position[1]=sound.position[1]/8.0;
                        sound.position[2]=sound.position[2]/8.0;
                        sound.id=4;
                        sounds.push_back(sound);
                        sound.reset();
                    }
                }
            }

            if (Objects[i].enabled)
                lvlOvr=false;
        }
        player.lvlOver=lvlOvr;
    }
    lock.writeUnlock();
}

//----------------Render---------------------------------------------------------
void render(){
    glClearColor(0,0,0,1);
    glClearDepth(1.0);
    glPointSize(2.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    skybox();
    Lighting();
    checkLvl();
    const SixDOF* s = Input::getSixDOF("wand");
    Vec3f f1 = s->forward();
    Vec3f p = s->position();
    const SixDOF* s2 = Input::getSixDOF("head");
    player.gunPos=p;
    player.headPos=s2->position();
    player.bulitStep=f1 /20.0;
    GLfloat lightPos2[ ] = { p[0]+0.3, p[1]+0.3, p[2]+0.3, 1.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
    if (getDist(player.gunPos,player.bulitPos) < player.maxDist){
        player.bulitPos+=player.bulitStep;
//        if((player.bulitPos[1]<0)&&(player.bulitPos[1]>-0.01)){
//            sound.position=player.bulitPos;
//            sound.position[0]=sound.position[0]/2.0;
//            sound.position[1]=sound.position[1]/2.0;
//            sound.position[2]=sound.position[2]/2.0;
//            sound.id=6;//ricochet
//            sounds.push_back(sound);
//            sound.reset();
//        }
    }
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(s->matrix().v);
    glColor3d(0, 0, 1);
    message(-0.005,0,0.025,intToStr("",player.ammo));
    glColor3d(0.4, 0, 0);
    message(-0.012,-0.02,0.025,intToStr("",player.helth));
    glColor3d(0.4, 0.4, 0.4);
    message(-0.012,-0.05,0.025,intToStr("L ",player.level));
    message(-0.012,-0.07,0.025,intToStr("",player.score));
    glScalef(1,1.6,1.6);
    player.drawGun();

    glPopMatrix();
    if(player.laser){//draw laser
        glColor3f(1,0,0);
	player.laserPos=player.gunPos;
        for (int i = 1;i<1200;i++){
  		player.laserPos+=player.bulitStep;
	}
        glBegin(GL_LINES);
        glVertex3fv(&player.laserPos[0]);
        glVertex3fv(&player.gunPos[0]);
        glEnd();
    }
    lock.readLock(); {
        glEnable(GL_TEXTURE_2D);
        for(int i=Objects.size()-1;i>=0;i--){ //draw all objects from the vector and check colusions
            Objects[i].Draw();
            Projectiles[i].Draw();
            if(getDist(Objects[i].position,player.bulitPos) < (Objects[i].size/2)){
                if (Objects[i].enabled){
                    Objects[i].explode=true;
                    Objects[i].enabled=false;
                    player.score+=Objects[i].level;
                    GLfloat lightPos[ ] = { Objects[i].position[0]+0.3, Objects[i].position[1]+0.3, Objects[i].position[2], 1.0 };
                    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
                    sound.position=Objects[i].position;
                    sound.position[0]=sound.position[0]/8.0;
                    sound.position[1]=sound.position[1]/8.0;
                    sound.position[2]=sound.position[2]/8.0;
                    sound.id=3;
                    sounds.push_back(sound);
                    sound.reset();
                }
            }
            if(getDist(Projectiles[i].position,player.bulitPos) < (Objects[i].size/2)){
                if (Projectiles[i].enabled){
                    Projectiles[i].explode=true;
                    Projectiles[i].enabled=false;
                    player.score+=Projectiles[i].level*2;
                    sound.position=Projectiles[i].position;
                    sound.position[0]=sound.position[0]/8.0;
                    sound.position[1]=sound.position[1]/8.0;
                    sound.position[2]=sound.position[2]/8.0;
                    sound.id=3;
                    sounds.push_back(sound);
                    sound.reset();
                }
            }
            if(Projectiles[i].position[2]>10){
                Projectiles[i].enabled=false;
            }
            if(getDist(Projectiles[i].position,player.headPos) < (Objects[i].size/2-0.2)){
                if (Projectiles[i].enabled){
//                    Projectiles[i].explode=true;
                    Projectiles[i].enabled=false;
                    player.helth-=Projectiles[i].level*2+(int)(Projectiles[i].size*5)+rand() % 16;
                    sound.position=Projectiles[i].position;
                    sound.position[0]=sound.position[0]/8.0;
                    sound.position[1]=sound.position[1]/8.0;
                    sound.position[2]=sound.position[2]/8.0;
                    sound.id=5;
                    sounds.push_back(sound);
                    sound.reset();
                }
            }

        }
        glDisable(GL_TEXTURE_2D);
    }
    lock.unlock();
}
//-----------------------sound-------------------------------------------------------------------------------
struct ALData{
    ALuint buffer;
    ALuint source;
    int play;

};
ALData* snd = new ALData[9];

int LoadAndAssignWAV(const char* _fileName, ALuint _buffer,int sndID){
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

void alrender(){
    ALint val;
    alGetSourcei(snd[2].source, AL_SOURCE_STATE, &val);
    if (val==AL_PLAYING){
        player.isReloading=true;
    }else
        player.isReloading=false;
    if(sounds.size()==0) return;
    ALData* snd = (ALData*)System::contextData();
    Vec3f sp;
    int i = sounds.size()-1;
    sp = transform.toRealPoint(sounds[i].position);
    alSourcefv(snd[sounds[i].id].source, AL_POSITION, sp.v);
    alSourcePlay(snd[sounds[i].id].source);
    sounds.pop_back();
    sound.reset();

}

void alinit(){
    Vec3f zero = Vec3f::zero();
    Vec3f one = Vec3f::one();
    alListenerfv(AL_POSITION, zero.v);
    alListenerfv(AL_VELOCITY, zero.v);
    for (int i = 0; i<9; i++){
        alGenBuffers(1, &snd[i].buffer);
        alGenSources(1, &snd[i].source);
    }
    LoadAndAssignWAV("sound/shoot.wav",snd[0].buffer,0);
    LoadAndAssignWAV("sound/click.wav",snd[1].buffer,1);
    LoadAndAssignWAV("sound/reload.wav",snd[2].buffer,2);
    LoadAndAssignWAV("sound/glass.wav",snd[3].buffer,3);
    LoadAndAssignWAV("sound/blaster.wav",snd[4].buffer,4);
    LoadAndAssignWAV("sound/bang.wav",snd[5].buffer,5);
    LoadAndAssignWAV("sound/ricochet.wav",snd[6].buffer,6);
    LoadAndAssignWAV("sound/shell_drop.wav",snd[7].buffer,7);
    LoadAndAssignWAV("sound/switch.wav",snd[8].buffer,8);
    alDistanceModel(AL_INVERSE_DISTANCE);
    if (alGetError() != AL_NO_ERROR)
        System::shutdown();
    System::contextData(snd);
}
//----------------init--and----main----------------------------------------------------------------------------------
void initContext(){
    tex2d[0] = GetTexture("texture/front.jpg");
    tex2d[1] = GetTexture("texture/right.jpg");
    tex2d[2] = GetTexture("texture/back.jpg");
    tex2d[3] = GetTexture("texture/left.jpg");
    tex2d[4] = GetTexture("texture/up.jpg");
    tex2d[5] = GetTexture("texture/down.jpg");
    tex2d[6] = GetTexture("texture/glass.jpg");
    tex2d[7] = GetTexture("texture/black.jpg");
    srand((unsigned)time(0));
    Lighting();
}

int main(int argc, char** argv){
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
