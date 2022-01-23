#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//#include <cstring>
#include <iostream>
#include <fstream>

float getDist(Vec3f P1,Vec3f P2);
Vec3 normColor(Vec3 ColorV);
GLuint GetTexture(std::string Filename);
void Lighting( void );

struct Object3D{ //a struct to store a 3d object
    Vec3f color,position;
    float size;
    int type;
    int txID;
    Object3D();
    void Draw();
};


Object3D::Object3D(){
    position[0]=0;
    position[1]=0;
    position[2]=0;
    color[0]=1;
    color[1]=1;
    color[2]=1;
    size=0.3;
    type=0;
    txID=0;
}

void Object3D::Draw(){
    glPushMatrix();
    glMultMatrixd(transform.matrix().v);
    glTranslatef(position[0],position[1],position[2]);
    glColor3f(color[0],color[1],color[2]);
    if(type){
        glutWireSphere(size,20,20);

    }
    else {
        GLfloat half=size/2.0;
        if (txID < nmbrOfTextures) glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
        else  glBindTexture( GL_TEXTURE_2D, tex2d[4] );

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

        if (txID < nmbrOfTextures) glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
        else glBindTexture( GL_TEXTURE_2D, tex2d[1] );

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

        if (txID < nmbrOfTextures) glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
        else glBindTexture( GL_TEXTURE_2D, tex2d[0] );

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


        if (txID < nmbrOfTextures) glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
        else  glBindTexture( GL_TEXTURE_2D, tex2d[5] );

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

        if (txID < nmbrOfTextures) glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
        else glBindTexture( GL_TEXTURE_2D, tex2d[2] );
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

        if (txID < nmbrOfTextures) glBindTexture( GL_TEXTURE_2D, tex2d[txID] );
        else  glBindTexture( GL_TEXTURE_2D, tex2d[3] );

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



    glPopMatrix();

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

float getDist(Vec3f P1,Vec3f P2){
    return sqrt( (P1[0]-P2[0])*(P1[0]-P2[0])+(P1[1]-P2[1])*(P1[1]-P2[1])+(P1[2]-P2[2])*(P1[2]-P2[2]) );
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

void Lighting( void ){    //sets the lights
    float ambient_light[ ] = { 0.3, 0.3, 0.3, 1.0 };
    float diffuse_light[ ] = { 0.7, 0.7, 0.7, 1.0 };
    float specular_light[ ] = { 1.0, 1.0, 1.0, 1.0 };
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

#endif 
