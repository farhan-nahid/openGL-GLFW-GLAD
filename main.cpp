#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <GL/glu.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <math.h>

using namespace std;

// Window dimensions
const int width = 700;
const int height = 700;

// Camera variables
GLfloat eyeX = 28;
GLfloat eyeY = 7;
GLfloat eyeZ = -38;

GLfloat centerX = 7;
GLfloat centerY = 6;
GLfloat centerZ = 25;

double angle = 0;
bool l_on1 = true;
bool l_on2 = true;
bool l_on3 = true;
bool l_on4 = false;
float rot = -8;
int stop = 1;
float door_angle = 0.5;
float l_height = 0.5;
float spt_cutoff = 30;
unsigned int ID;
vector<int> v;

static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3) {
    GLfloat Ux = x2 - x1, Uy = y2 - y1, Uz = z2 - z1;
    GLfloat Vx = x3 - x1, Vy = y3 - y1, Vz = z3 - z1;
    GLfloat Nx = Uy * Vz - Uz * Vy;
    GLfloat Ny = Uz * Vx - Ux * Vz;
    GLfloat Nz = Ux * Vy - Uy * Vx;
    glNormal3f(Nx, Ny, Nz);
}

static GLfloat v_cube[8][3] = {
    {0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1},
    {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}
};

static GLubyte c_ind[6][4] = {
    {3, 5, 5, 7}, {6, 4, 0, 2}, {2, 3, 7, 6},
    {1, 0, 4, 5}, {7, 5, 4, 6}, {2, 0, 1, 3}
};

void cube(float R = 0.5, float G = 0.5, float B = 0.5, int type = 0, float val = 1) {
    GLfloat m_no[] = {0, 0, 0, 1.0};
    GLfloat m_amb[] = {R, G, B, 1};
    GLfloat m_diff[] = {R, G, B, 1};
    GLfloat m_spec[] = {0, 0, 0, 0};
    GLfloat m_sh[] = {30};
    GLfloat m_em[] = {0, 1, 1, 0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    if (type == 1) {
        glMaterialfv(GL_FRONT, GL_EMISSION, l_on1 ? m_em : m_no);
    } else if (type == 2) {
        glMaterialfv(GL_FRONT, GL_EMISSION, l_on3 ? m_em : m_no);
    } else {
        glMaterialfv(GL_FRONT, GL_EMISSION, m_no);
    }

    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++) {
        getNormal3p(
            v_cube[c_ind[i][0]][0], v_cube[c_ind[i][0]][1], v_cube[c_ind[i][0]][2],
            v_cube[c_ind[i][1]][0], v_cube[c_ind[i][1]][1], v_cube[c_ind[i][1]][2],
            v_cube[c_ind[i][2]][0], v_cube[c_ind[i][2]][1], v_cube[c_ind[i][2]][2]);
        glTexCoord2f(0, val); glVertex3fv(&v_cube[c_ind[i][0]][0]);
        glTexCoord2f(0, 0); glVertex3fv(&v_cube[c_ind[i][1]][0]);
        glTexCoord2f(val, 0); glVertex3fv(&v_cube[c_ind[i][2]][0]);
        glTexCoord2f(val, val); glVertex3fv(&v_cube[c_ind[i][3]][0]);
    }
    glEnd();
}

class BmpLoader {
public:
    unsigned char* textureData;
    int iWidth, iHeight;

    BmpLoader(const char* filename) {
        FILE* file = fopen(filename, "rb");
        if (!file) {
            cout << "File not found: " << filename << endl;
            return;
        }
        BITMAPFILEHEADER bfh;
        BITMAPINFOHEADER bih;
        fread(&bfh, sizeof(BITMAPFILEHEADER), 1, file);
        if (bfh.bfType != 0x4D42) {
            cout << "Not a valid bitmap: " << filename << endl;
            fclose(file);
            return;
        }
        fread(&bih, sizeof(BITMAPINFOHEADER), 1, file);
        if (bih.biSizeImage == 0) bih.biSizeImage = bih.biHeight * bih.biWidth * 3;
        textureData = new unsigned char[bih.biSizeImage];
        fseek(file, bfh.bfOffBits, SEEK_SET);
        fread(textureData, 1, bih.biSizeImage, file);
        for (int i = 0; i < bih.biSizeImage; i += 3) {
            unsigned char temp = textureData[i];
            textureData[i] = textureData[i + 2];
            textureData[i + 2] = temp;
        }
        iWidth = bih.biWidth;
        iHeight = bih.biHeight;
        fclose(file);
    }

    ~BmpLoader() { delete[] textureData; }
};

void LoadTexture(const char* filename) {
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    BmpLoader bl(filename);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bl.iWidth, bl.iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData);
}

void flr() {
    for (int i = -10; i <= 35; i++) {
        for (float j = -11; j <= 14; j += 2) {
            glPushMatrix();
            glScalef(1.7, 0.5, 0.92);
            glTranslatef(j, -0.999, -i);
            cube(i % 2 == 0 ? 1 : 0.2, i % 2 == 0 ? 1 : 0.2, i % 2 == 0 ? 1 : 0.2);
            glPopMatrix();
        }
    }
    for (int i = -10; i <= 35; i++) {
        for (float j = -12; j <= 14; j += 2) {
            glPushMatrix();
            glScalef(1.7, 0.5, 0.92);
            glTranslatef(j, -0.999, -i);
            cube(i % 2 != 0 ? 1 : 0.2, i % 2 != 0 ? 1 : 0.2, i % 2 != 0 ? 1 : 0.2);
            glPopMatrix();
        }
    }
}

void wall1() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPushMatrix();
    glScalef(1, 20, 55);
    glTranslatef(-19, 0, -0.5);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void wall2() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPushMatrix();
    glScalef(1, 20, 56);
    glTranslatef(23, 0, -0.5);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void wall3() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPushMatrix();
    glScalef(46, 20, 1);
    glTranslatef(-0.44, 0, 8);
    cube(0.741, 0.718, 0.420, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void door() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPushMatrix();
    glScalef(35, 20, 1);
    glTranslatef(-0.542, 0, -28);
    cube(0.741, 0.718, 0.420, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void board() {
    glPushMatrix();
    glScalef(17, 10, 0.8);
    glTranslatef(-0.6, 0.4, 9.1);
    cube(1, 1, 1);
    glPopMatrix();

    glPushMatrix();
    glScalef(17.5, 11, 0.8);
    glTranslatef(-0.601, 0.32, 9.4);
    cube(0, 0, 0);
    glPopMatrix();
}

void window() {
    glPushMatrix();
    glScalef(1, 8, 12);
    glTranslatef(22.95, 0.6, -1.5);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[1]);
    cube(0.4, 1, 4, 0, 1);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void headwall() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[2]);
    glPushMatrix();
    glScalef(56, 1, 59);
    glTranslatef(-0.5, 19, -0.5);
    cube(0.690, 0.769, 0.871, 0, 4);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

float k = 0;
float angle1 = 0;
bool markdoor = 0;
double window_val = 0;

void spin() {
    angle = (angle + k) * stop;
    k += 0.001;
    if (angle >= 360) angle = 0;
    angle1 += 0.2;
    if (angle1 >= 360) angle1 = 0;

    bool ok = false;
    if (markdoor && door_angle > -0.8) {
        door_angle -= 0.001;
        ok = true;
        if (door_angle < 0.4) window_val += 0.00085;
        window_val = min(window_val, 1.0);
        l_on4 = true;
    }
    if (!markdoor && door_angle < 0.5) {
        door_angle += 0.001;
        ok = true;
        window_val -= 0.001;
        window_val = max(window_val, 0.0);
    }
    if (!ok) l_on4 = markdoor;
}

void fan() {
    float length = 10, width = 0.3;
    glPushMatrix();
    glTranslatef(1, length / 2, -4);
    glScalef(width, length, width);
    glTranslatef(-20, 1, 20);
    cube(0.4, 0.4, 1);
    glPopMatrix();

    glPushMatrix();
    glScalef(1, 0.1, 1);
    glTranslatef(-5, 150, 2);
    cube(1, 1, 1);
    glRotatef(angle, 0, 0, 1);
    glRecti(-2, -2, 2, 2);
    cube(0.5, 0, 0.5);
    glRecti(-1, 2, 1, 10);
    glRecti(-10, -1, -2, 1);
    glRecti(-1, -10, 1, -2);
    glRecti(2, -1, 10, 1);
    glPopMatrix();
}

void Chair12() {
    float height = 2.5, width = 4, length = 2;
    float base_height = 0.5, leg_height = height - base_height, leg_width = 0.4;

    glPushMatrix();
    glTranslatef(0, leg_height, 0);

    glPushMatrix();
    glScalef(width, base_height, length);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.4f, 0.5f, 0.3f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width / 2 - leg_width / 2), 0, (length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width / 2 - leg_width / 2), 0, -(length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width / 2 - leg_width / 2), 0, (length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width / 2 - leg_width / 2), 0, -(length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glScalef(3, 2, 0.5);
    glTranslatef(-0.5, 0.5, -1);
    cube(0.7f, 0.5f, 0.3f);
    glPopMatrix();

    glPushMatrix();
    glScalef(0.2, 1, 0.2);
    glTranslatef(3, 0, -2);
    cube(0, 0, 0);
    glPopMatrix();

    glPushMatrix();
    glScalef(0.2, 1, 0.2);
    glTranslatef(-4, 0, -2);
    cube(0, 0, 0);
    glPopMatrix();
    glPopMatrix();
}

void Table12(float height = 2.4, float width = 4, float length = 2, float base_height = 0.5, float leg_width = 0.2) {
    float leg_height = height - base_height;

    glPushMatrix();
    glTranslatef(0, leg_height, 0);

    glPushMatrix();
    glScalef(width, base_height, length);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.6, 0.3, 0.3);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width / 2 - leg_width / 2), 0, (length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.8f, 0.6f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width / 2 - leg_width / 2), 0, -(length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.8f, 0.6f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width / 2 - leg_width / 2), 0, (length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.8f, 0.6f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width / 2 - leg_width / 2), 0, -(length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.8f, 0.6f, 0.0f);
    glPopMatrix();
    glPopMatrix();
}

void test() {
    glPushMatrix();
    glTranslatef(10.0, 1, -14 + 9 * 2.0 + 2.2f);
    glScalef(0.8f, 0.8f, 0.8f);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    Chair12();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10.0, 1, -16 + 9 * 2.0 + 2.2f);
    glScalef(1.7f, 1.3f, 1.0f);
    Table12(2, 4, 2, 0.6, 0.3);
    glPopMatrix();

    for (int i = -6; i <= 8; i += 2) {
        for (int j = -4; j <= 4; j += 2) {
            glPushMatrix();
            glTranslatef(i * 2.0, 0.6f, -16 + j * 2.0 + 2.2f);
            glScalef(0.5f, 0.5f, 0.5f);
            glRotatef(0.0, 0.0, 1.0, 0.0);
            Chair12();
            glPopMatrix();
        }
    }

    for (int i = -6; i <= 8; i += 2) {
        for (int j = -4; j <= 4; j += 2) {
            glPushMatrix();
            glTranslatef(i * 2.0 + 0.3f, 1.2f, -14.2 + j * 2.0 + 1.2f);
            glScalef(0.75f, 0.7f, 0.6f);
            Table12();
            glPopMatrix();
        }
    }
}

void table() {
    float height = 0.5, width = 42.5, length = 7;
    float base_height = 0.5, leg_height = height - base_height, leg_width = 1;

    glPushMatrix();
    glTranslatef(2.7, 0, 5);
    glPushMatrix();
    glTranslatef(0, leg_height, 0);

    glPushMatrix();
    glScalef(width, base_height, length);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.4, 0.6, 0.6);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width / 2 - leg_width / 2), 0, (length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.53, 0.39, 0.28);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width / 2 - leg_width / 2), 0, -(length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.53, 0.39, 0.28);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width / 2 - leg_width / 2), 0, (length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.53, 0.39, 0.28);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width / 2 - leg_width / 2), 0, -(length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.53, 0.39, 0.28);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

void drop() {
    glPushMatrix();
    cube(113 / 255.0, 74 / 255.0, 44 / 255.0);
    glBegin(GL_POLYGON);
    glVertex3f(460, 660, 0);
    glVertex3f(470, 670, 0);
    glVertex3f(330, 670, 0);
    glVertex3f(340, 660, 0);
    glVertex3f(340, 320, 0);
    glVertex3f(460, 320, 0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 0);
    glScalef(105, 95, 10);
    glPushMatrix();
    glTranslatef(3.32, 4.6, -0.49);
    cube(62 / 255.0, 43 / 255.0, 45 / 255.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(3.32, 5.7, -0.49);
    cube(62 / 255.0, 43 / 255.0, 45 / 255.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(3.32, 3.5, -0.49);
    cube(62 / 255.0, 43 / 255.0, 45 / 255.0);
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1, 0, 0);
    glScalef(20, 10, 10);
    glPushMatrix();
    glTranslatef(19.5, 38, -0.5);
    cube(1, 1, 1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(19.5, 58, -0.5);
    cube(1, 1, 1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(19.5, 48, -0.5);
    cube(1, 1, 1);
    glPopMatrix();
    glPopMatrix();

    cube(62 / 255.0, 43 / 255.0, 35 / 255.0);
    glBegin(GL_POLYGON);
    glVertex3f(470, 670, 0);
    glVertex3f(500, 667, 0);
    glVertex3f(492, 657, 0);
    glVertex3f(492, 330, 0);
    glVertex3f(460, 320, 0);
    glVertex3f(460, 660, 0);
    glEnd();
    glPopMatrix();
}

void weardrop() {
    glPushMatrix();
    glTranslatef(-8, -7, 7.5);
    glScalef(-0.02, 0.021, 0.4);
    drop();
    glPopMatrix();
}

void fan1() {
    glPushMatrix();
    glTranslatef(6, -0.85, -10);
    cube(1, 1, 1);
    fan();
    glPopMatrix();
}

float al1, dl1, sl1;
float al2, dl2, sl2;
float al3, dl3, sl3;

void light1(float a, float b, float c) {
    glEnable(GL_LIGHT0);
    GLfloat l_no[] = {0.9, 0, 0.9, 1.0};
    GLfloat l_amb[] = {0.4 + al1, 0.4 + al1, 0.4 + al1, 1.0};
    GLfloat l_dif[] = {0.4 + dl1, 0.4 + dl1, 0.4 + dl1, 1};
    GLfloat l_spec[] = {0.4 + sl1, 0.4 + sl1, 0.4 + sl1, 1};
    GLfloat l_pos[] = {a, b, c, 1.0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, l_on1 ? l_amb : l_no);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, l_on1 ? l_dif : l_no);
    glLightfv(GL_LIGHT0, GL_SPECULAR, l_on1 ? l_spec : l_no);
    glLightfv(GL_LIGHT0, GL_POSITION, l_pos);
}

void light2(float a, float b, float c) {
    glEnable(GL_LIGHT2);
    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {0.3 + al2, 0.3 + al2, 0.3 + al2, 1.0};
    GLfloat l_dif[] = {0.6 + dl2, 0.6 + dl2, 0.6 + dl2, 1};
    GLfloat l_spec[] = {5 + sl2, 5 + sl2, 5 + sl2, 1};
    GLfloat l_pos[] = {a, b, c, 1.0};

    glLightfv(GL_LIGHT2, GL_AMBIENT, l_on3 ? l_amb : l_no);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, l_on3 ? l_dif : l_no);
    glLightfv(GL_LIGHT2, GL_SPECULAR, l_on3 ? l_spec : l_no);
    glLightfv(GL_LIGHT2, GL_POSITION, l_pos);
}

void window_light(float a, float b, float c) {
    glEnable(GL_LIGHT3);
    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {0.5 * window_val, 0.5 * window_val, 0.5 * window_val, 1.0};
    GLfloat l_dif[] = {1 * window_val, 1 * window_val, 1 * window_val, 1};
    GLfloat l_spec[] = {1 * window_val, 1 * window_val, 1 * window_val, 1};
    GLfloat l_pos[] = {a, b, c, 1.0};

    glLightfv(GL_LIGHT3, GL_AMBIENT, l_on4 ? l_amb : l_no);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, l_on4 ? l_dif : l_no);
    glLightfv(GL_LIGHT3, GL_SPECULAR, l_on4 ? l_spec : l_no);
    glLightfv(GL_LIGHT3, GL_POSITION, l_pos);

    GLfloat l_spt[] = {0, -1, 0, 1};
    GLfloat spt_ct[] = {84};
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, l_spt);
    glLightfv(GL_LIGHT3, GL_SPOT_CUTOFF, spt_ct);
}

void light() {
    glPushMatrix();
    glPushMatrix();
    glRotatef(200, 0, 1, 0);
    float a = -5, b = 16, c = 9;
    light1(a, b, c);
    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-0.08, -0.85, -8);
    cube(1, 1, 1, 1);
    glPopMatrix();

    glPushMatrix();
    glPushMatrix();
    glRotatef(200, 0, 1, 0);
    a = 15, b = 17, c = -15;
    light2(a, b, c);
    glPopMatrix();
    glTranslatef(4, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1, 1);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-3, 3, -2, 3, 2.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, 0, 1, 0);

    glRotatef(rot, 0.2, 0.5, 0.5);
    table();
    flr();
    wall1();
    wall2();
    wall3();
    door();
    board();
    headwall();
    test();
    window();
    weardrop();
    fan1();
    light();
}

void texture_image() {
    LoadTexture("111.bmp");
    v.push_back(ID);
    LoadTexture("333.bmp");
    v.push_back(ID);
    LoadTexture("333.bmp");
    v.push_back(ID);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_KP_ADD: // '+'
            eyeZ++;
            break;
        case GLFW_KEY_KP_SUBTRACT: // '-'
            eyeZ--;
            break;
        case GLFW_KEY_A:
            eyeX++;
            break;
        case GLFW_KEY_S:
            eyeX--;
            break;
        case GLFW_KEY_G:
            eyeY++;
            break;
        case GLFW_KEY_H:
            eyeY--;
            break;
        case GLFW_KEY_W:
            centerY++;
            break;
        case GLFW_KEY_E:
            centerY--;
            break;
        case GLFW_KEY_O:
            centerX++;
            break;
        case GLFW_KEY_P:
            centerX--;
            break;
        case GLFW_KEY_K:
            centerZ++;
            break;
        case GLFW_KEY_L:
            centerZ--;
            break;
        case GLFW_KEY_COMMA:
            rot++;
            break;
        case GLFW_KEY_PERIOD:
            rot--;
            break;
        case GLFW_KEY_KP_MULTIPLY: // '*'
            stop = 0;
            break;
        case GLFW_KEY_SEMICOLON:
            angle = 0, stop = 1, k = 0;
            break;
        case GLFW_KEY_1:
            l_height += 0.1;
            break;
        case GLFW_KEY_2:
            l_height -= 0.1;
            break;
        case GLFW_KEY_3:
            spt_cutoff++;
            break;
        case GLFW_KEY_4:
            spt_cutoff--;
            break;
        case GLFW_KEY_T:
            l_on1 = !l_on1;
            break;
        case GLFW_KEY_U:
            l_on2 = !l_on2;
            break;
        case GLFW_KEY_Y:
            l_on3 = !l_on3;
            break;
        case GLFW_KEY_D:
            markdoor = !markdoor;
            break;
    }
}

int main() {
    if (!glfwInit()) {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(width, height, "Tonmoy_1361", NULL, NULL);
    if (!window) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glEnable(GL_LIGHTING);
    GLfloat globalAmbient[] = {0.0, 0.0, 0.0, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    texture_image();

    glfwSetKeyCallback(window, key_callback);

    cout << "1. Press ESC key to exit the program\n";

    while (!glfwWindowShouldClose(window)) {
        spin();
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}