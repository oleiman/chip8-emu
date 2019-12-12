#include "include/chip8.hpp"

#include <iostream>

#include <GL/freeglut.h>

#define SCREEN_SCALE 20

#define PRESS 1
#define RELEASE 0

using chip8::Chip8;

int display_width = SCREEN_WIDTH * SCREEN_SCALE;
int display_height = SCREEN_HEIGHT * SCREEN_SCALE;

void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void keyboardHelper(unsigned char key, uint8_t state);

void display(void);
void redisplay(void);

uint8_t screen_data[SCREEN_HEIGHT][SCREEN_WIDTH][3];
void setupTexture();
void updateTexture(Chip8& c8);
void reshape_window(GLsizei w, GLsizei h);

Chip8 c8;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "USAGE: chip8-emu /path/to/app" << std::endl;
        exit(1);
    } else  if (!c8.loadApp(argv[1])) {
        std::cout << "BAD FILE: " << argv[1] << std::endl;
        exit(1);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowSize(display_width, display_height);
    glutInitWindowPosition(320, 320);
    glutCreateWindow("Oren's Chip8");

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape_window);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);

    setupTexture();

    glutMainLoop();

    return 0;
}

void display()
{
    c8.cycle();

    if (c8.drawFlag()) {
        glClear(GL_COLOR_BUFFER_BIT);
        updateTexture(c8);
        glutSwapBuffers();
        c8.resetDrawFlag();
    }
}

void setupTexture()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            screen_data[y][x][0] = screen_data[y][x][1] = screen_data[y][x][2] = 255;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, 3, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) screen_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glEnable(GL_TEXTURE_2D);
}

// TODO(oren): add the interpreter object as param
void updateTexture(Chip8& c8)
{
    c8.exportScreenBuf(screen_data);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) screen_data);

    glBegin(GL_TRIANGLES);
    {
        glTexCoord2d(0.0, 0.0);
        glVertex2d(0.0, 0.0);

        glTexCoord2d(1.0, 0.0);
        glVertex2d(display_width, 0.0);

        glTexCoord2d(0.0, 1.0);
        glVertex2d(0.0, display_height);
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    {
        glTexCoord2d(1.0, 1.0);
        glVertex2d(display_width, display_height);

        glTexCoord2d(0.0, 1.0);
        glVertex2d(0.0, display_height);

        glTexCoord2d(1.0, 0.0);
        glVertex2d(display_width, 0.0);
    }
    glEnd();
    
}

void reshape_window(GLsizei w, GLsizei h)
{
    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);        
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);

    // Resize quad
    display_width = w;
    display_height = h;
}

void redisplay()
{
    glutPostRedisplay();
}

void keyboardDown(unsigned char key, int x, int y)
{
    (void) x;
    (void) y;
    if (key == 27) {
        exit(0);
    }
    keyboardHelper(key, PRESS);
}

void keyboardUp(unsigned char key, int x, int y)
{
    (void) x;
    (void) y;
    keyboardHelper(key, RELEASE);
}

void keyboardHelper(unsigned char key, uint8_t state)
{

    if(key == '1')	c8.setKey(0x1, state);
    else if(key == '2')	c8.setKey(0x2, state);
    else if(key == '3')	c8.setKey(0x3, state);
    else if(key == '4')	c8.setKey(0xC, state);

    else if(key == 'q')	c8.setKey(0x4, state);
    else if(key == 'w')	c8.setKey(0x5, state);
    else if(key == 'e')	c8.setKey(0x6, state);
    else if(key == 'r')	c8.setKey(0xD, state);

    else if(key == 'a')	c8.setKey(0x7, state);
    else if(key == 's')	c8.setKey(0x8, state);
    else if(key == 'd')	c8.setKey(0x9, state);
    else if(key == 'f')	c8.setKey(0xE, state);

    else if(key == 'z')	c8.setKey(0xA, state);
    else if(key == 'x')	c8.setKey(0x0, state);
    else if(key == 'c')	c8.setKey(0xB, state);
    else if(key == 'v')	c8.setKey(0xF, state);
}


