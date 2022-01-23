#ifndef FUNCTIONS_H
#define FUNCTIONS_H

GLfloat getDist(Vec3f P1,Vec3f P2);
GLfloat absg(GLfloat x);
Vec3 normColor(Vec3 ColorV);
GLuint GetTexture(std::string Filename);
void Lighting( void );
void _glutBitmapString(void *font, char *s);
void showMessage(GLfloat x, GLfloat y,int white);//draw text at x, y
///---------------------------------PADDLE------------------------------------------------------------
struct TPaddle{
    Vec3f color,position,lastPosition;
    GLfloat size,maxStep;
    GLfloat maxLeft,maxRight,maxUP,maxDown,goalFar,goalNear;
    int id,score;
    int txID;
    TPaddle();
    void Draw();
    void AIupdate(Vec3f b1,GLfloat st2);
};


TPaddle::TPaddle(){
    maxLeft = -1;
    maxRight = 1;
    maxUP = 2;
    maxDown = 0;
    goalFar = -3;
    goalNear = 0;
    maxStep = 0.06;
    position[0]=0;
    position[1]=0;
    position[2]=0;
    color[0]=1;
    color[1]=1;
    color[2]=1;
    score=0;
    size=0.5;
    id=1;
    txID=4;
}

void TPaddle::Draw(){
    glPushMatrix();
    glMultMatrixd(transform.matrix().v);
    glTranslatef(position[0],position[1],position[2]);
    glColor3f(color[0],color[1],color[2]);
    glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
    GLfloat half=size/2.0;
    glBegin(GL_QUADS);
    glNormal3f( 0.0f, 0.0f, 1.0f);      // Normal Facing Forward
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-half,  -half,  0.0f);   // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( half,  -half,  0.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( half,  half,  0.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-half,  half,  0.0f); // Top Left Of The Texture and Quad
    glEnd();

    if (id==1){
        glBegin(GL_QUADS);
        for (int i = 1; i<=5; i++){
            half= size/2.0-(i/150.0);
            glNormal3f( 0.0f, 0.0f, 1.0f);      // Normal Facing Forward
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-half,  -half,  i/150.0);  // Bottom Left Of The Texture and Quad
            glTexCoord2f(1.0f, 0.0f); glVertex3f( half,  -half,  i/150.0);  // Bottom Right Of The Texture and Quad
            glTexCoord2f(1.0f, 1.0f); glVertex3f( half,  half,  i/150.0);  // Top Right Of The Texture and Quad
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-half,  half,  i/150.0);  // Top Left Of The Texture and Quad
        }
        glEnd();
    }
    glPopMatrix();
}
void TPaddle::AIupdate(Vec3f b1,GLfloat st2){
    if (id!=2) return;

    if( (b1[2]< goalFar/2) && (st2<0) ){ //move toward the ball
        if ( position[0]+0.2<b1[0] )position[0]+=maxStep/3;
        else position[0]-=maxStep/3;
        if ( position[1]+0.2<b1[1] )position[1]+=maxStep/3;
        else position[1]-=maxStep/3;
        if ( position[2]-0.2<b1[2] )position[2]+=maxStep/3;
        else position[2]-=maxStep/3;

    }else{ //move to center
        if ( position[0]<-0.1 )position[0]+=maxStep/5;
        else if (position[0]>0.1)  position[0]-=maxStep/5;
        if ( position[1]+0.1<maxUP/2 )position[1]+=maxStep/5;
        else if (position[1]-0.1>maxUP/2) position[1]-=maxStep/5;
        if ( position[2]-0.1<goalFar )position[2]+=maxStep/5;
        else position[2]-=maxStep/5;
    }
    if ( position[0]>maxRight-size/2 )position[0]=maxRight-size/2;
    if ( position[0]<maxLeft+size/2 )position[0]=maxLeft+size/2;
    if ( position[1]>maxUP-size/2 )position[1]=maxUP-size/2;
    if ( position[1]<maxDown+size/2 )position[1]=maxDown+size/2;
    if ( position[2]>goalFar/2-size/2 )position[2]=goalFar-size/2;

}

//--------------------------------BALL--------------------------------------------------------------
struct TBall{
    Vec3f color,position,step,rotate;
    GLfloat size,maxStep,wallBounce;
    GLfloat maxLeft,maxRight,maxUP,maxDown,goalFar,goalNear;
    bool hit,visible;
    int txID,hitSndID,isOut;
    TBall();
    void Reset();
    void Draw();
    void DrawWalls();
    void Update(TPaddle &p1,TPaddle &p2);
};


TBall::TBall(){
    maxLeft = -1;
    maxRight = 1;
    maxUP = 2;
    maxDown = 0;
    goalFar = -3;
    goalNear = 0;
    maxStep = 0.1;
    wallBounce = 0.9;
    Reset();

}
void TBall::Reset(){
    position[0]=0;
    position[1]=1.0;
    position[2]=-1.0;
    rotate[0]=0;
    rotate[1]=0;
    rotate[2]=0;
    step[0]=+0.0;
    step[1]=-0.0;
    step[2]=0.005;
    color[0]=1;
    color[1]=1;
    color[2]=1;
    size=0.08;
    hit = false;
    hitSndID=0;
    txID=3;
    visible=true;
    isOut=0;
}

void TBall::Draw(){
    if (!visible) return;
    glPushMatrix();
    glMultMatrixd(transform.matrix().v);
    glTranslatef(position[0],position[1],position[2]);
    rotate[0]+=step[0]*100;
    rotate[1]+=step[1]*100;
    rotate[2]+=step[2]*100;
    glRotatef(rotate[0],0,0,1);
    glRotatef(rotate[1],0,1,0);
    glRotatef(rotate[2],1,0,0);
    glColor3f(color[0],color[1],color[2]);
    glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
    glEnable( GL_TEXTURE_GEN_S );
    glEnable( GL_TEXTURE_GEN_T );

    glutSolidSphere(size,20,20);
    glPopMatrix();
    glDisable( GL_TEXTURE_GEN_S );
    glDisable( GL_TEXTURE_GEN_T );

}
void TBall::DrawWalls(){
    glBindTexture( GL_TEXTURE_2D, tex2d[1] );
    glMultMatrixd(transform.matrix().v);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glNormal3f( 1.0f, 0.0f, 0.0f);      // Normal Facing right
    glTexCoord2f(0.0f, 0.0f); glVertex3f( maxLeft,  maxDown,  goalNear);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( maxLeft,  maxDown,  goalFar);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( maxLeft,  maxUP,  goalFar);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( maxLeft,  maxUP,  goalNear);  // Top Left Of The Texture and Quad
    glNormal3f( -1.0f, 0.0f, 0.0f);      // Normal Facing left
    glTexCoord2f(0.0f, 0.0f); glVertex3f( maxRight,  maxDown,  goalNear);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( maxRight,  maxDown,  goalFar);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( maxRight,  maxUP,  goalFar);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( maxRight,  maxUP,  goalNear);  // Top Left Of The Texture and Quad
    glEnd();
    glBindTexture( GL_TEXTURE_2D, tex2d[0] );
    glBegin(GL_QUADS);
    glNormal3f( 0.0f, 1.0f, 0.0f);      // Normal Facing up
    glTexCoord2f(0.0f, 0.0f); glVertex3f( maxLeft,  maxDown,  goalNear);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( maxLeft,  maxDown,  goalFar);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( maxRight,  maxDown,  goalFar);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( maxRight,  maxDown,  goalNear);  // Top Left Of The Texture and Quad
    glEnd();
    glBindTexture( GL_TEXTURE_2D, tex2d[2] );
    glBegin(GL_QUADS);
    glNormal3f( 0.0f, -1.0f, 0.0f);      // Normal Facing down
    glTexCoord2f(0.0f, 0.0f); glVertex3f( maxLeft,  maxUP,  goalNear);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( maxLeft,  maxUP,  goalFar);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( maxRight,  maxUP,  goalFar);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( maxRight,  maxUP,  goalNear);  // Top Left Of The Texture and Quad
    glEnd();
    glPopMatrix();

}

void TBall::Update(TPaddle &p1,TPaddle &p2){
    for (int i=0;i<=2;i++){
        if (step[i]>maxStep)step[i]=maxStep;
        if (step[i]<-maxStep)step[i]=-maxStep;
    }
    position[0]+=step[0];
    position[1]+=step[1];
    position[2]+=step[2];
    if(getDist(position,p1.position) <= size+p1.size/2 ){
        if(position[2]+size > p1.position[2]){
            position[2] = p1.position[2]-size-0.03;
            hit = true;
            hitSndID=1;
            GLfloat st0 = -(p1.lastPosition[0]-p1.position[0]);
            GLfloat st1 = -(p1.lastPosition[1]-p1.position[1]);
            GLfloat st2 = (p1.lastPosition[2]-p1.position[2]);

            if(absg(st0) > 0.005 ) step[0]=st0;
            if(absg(st1) > 0.005 ) step[1]=st1;
            if(absg(st2) > 0.005 ) step[2]=st2;
            step[2]=-absg(step[2]);
        }
    }
    if(getDist(position,p2.position) <= size+p2.size/2 ){
        if(position[2] < p2.position[2]){
            position[2] = p2.position[2]+size+0.03;
            step[2]=absg(step[2]);
            hit = true;
            hitSndID=1;

        }
    }
    if (position[0]<maxLeft+size-0.02){
        hit = true;
        step[0]*=wallBounce;
        position[0]=maxLeft+size-0.02;
        step[0]=-step[0];
    }
    if (position[0]>maxRight-size+0.02){
        hit = true;
        step[0]*=wallBounce;
        position[0]=maxRight-size+0.02;
        step[0]=-step[0];
    }
    if (position[1]<maxDown+size-0.02){
        hit = true;
        step[1]*=wallBounce;
        position[1]=maxDown+size-0.02;
        step[1]=-step[1];
    }
    if (position[1]>maxUP-size+0.02){
        hit = true;
        step[1]*=wallBounce;
        position[1]=maxUP-size+0.02;
        step[1]=-step[1];
    }
    if (position[2]<goalFar-size-0.1){
        if(!isOut){
            hit = true;
            hitSndID=2;
        }
        isOut++;
    }
    if (position[2]>goalNear+size*2+0.2){
        if(!isOut){
            hit = true;
            hitSndID=2;
        }
        isOut++;
    }
}



//------------------------------------Load----Texture---------------------------------------------------------
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
                | SOIL_FLAG_INVERT_Y
                | SOIL_FLAG_NTSC_SAFE_RGB
//                | SOIL_FLAG_CoCg_Y
//                | SOIL_FLAG_TEXTURE_RECTANGLE

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
    _glutBitmapString(GLUT_BITMAP_9_BY_15, message);

}

#endif 
