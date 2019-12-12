#include "include/chip8.hpp"

#include <iostream>

#include <GL/freeglut.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SCALE 20

#define PRESS 1
#define RELEASE 0

int display_width = SCREEN_WIDTH * SCREEN_SCALE;
int display_height = SCREEN_HEIGHT * SCREEN_SCALE;

void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void keyboardHelper(unsigned char key, uint8_t state);

void display(void);
void redisplay(void);

uint8_t screen_data[SCREEN_HEIGHT][SCREEN_WIDTH][3];
void setupTexture();
void updateTexture();
void reshape_window(GLsizei w, GLsizei h);

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "USAGE: chip8-emu /path/to/app" << std::endl;
        exit(1);
    }

    // TODO(oren): load the game here

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
    // TODO(oren): step the emulator, check drawflag?

    glClear(GL_COLOR_BUFFER_BIT);
    updateTexture();

    glutSwapBuffers();
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
void updateTexture()
{
    // populate screen data with interpreter graphix buffer

    // for (int y = 0; y < SCREEN_HEIGHT; y++) {
    //     for (int x = 0; x < SCREEN_WIDTH; x++) {
    //         screen_data[y][x][0] = screen_data[y][x][1] = screen_data[y][x][2] = c_val;
    //     }
    // }

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
    (void) key;
    (void) state;

    // if(key == '1')	myChip8.setKey(0x1, state);
    // else if(key == '2')	myChip8.setKey(0x2, state);
    // else if(key == '3')	myChip8.setKey(0x3, state);
    // else if(key == '4')	myChip8.setKey(0xC, state);

    // else if(key == 'q')	myChip8.setKey(0x4, state);
    // else if(key == 'w')	myChip8.setKey(0x5, state);
    // else if(key == 'e')	myChip8.setKey(0x6, state);
    // else if(key == 'r')	myChip8.setKey(0xD, state);

    // else if(key == 'a')	myChip8.setKey(0x7, state);
    // else if(key == 's')	myChip8.setKey(0x8, state);
    // else if(key == 'd')	myChip8.setKey(0x9, state);
    // else if(key == 'f')	myChip8.setKey(0xE, state);

    // else if(key == 'z')	myChip8.setKey(0xA, state);
    // else if(key == 'x')	myChip8.setKey(0x0, state);
    // else if(key == 'c')	myChip8.setKey(0xB, state);
    // else if(key == 'v')	myChip8.setKey(0xF, state);
}


