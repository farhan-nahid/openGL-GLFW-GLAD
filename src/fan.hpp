#ifndef FAN_HPP
#define FAN_HPP

#include "glut_compat.hpp"
#include <math.h>

#define DEF_D 5

class Fan {
private:
    bool on;
    float fanRotation;

public:
    Fan() {
        on = false;
        fanRotation = 0.0f;
    }

    void toggleFan() {
        on = !on;
    }

    bool getState() const {
        return on;
    }

    void drawCylinder() {
        glutSolidCylinder(1.0, 1.0, 20, 20);
    }

    void drawFan() {
        glTranslatef(0, 5, -5.0);
        glScalef(5, 5, 5);

        // Ceiling mount (distinctive lighter color)
        glPushMatrix();
        glColor3f(0.7f, 0.7f, 0.7f);  // Light gray mount for contrast
        glTranslatef(0, -1.0, 0);
        glScalef(0.1, 0.3, 0.1);
        drawCylinder();
        glPopMatrix();

        // Rotating fan assembly
        glPushMatrix();
        glRotatef(fanRotation, 0.0f, 1.0f, 0.0f);

        // Center hub
        glPushMatrix();
        glColor3f(0.75f, 0.75f, 0.75f);  // Silver hub
        glTranslatef(0, -1.3, 0);
        glutSolidSphere(0.15, 20, 20);
        glPopMatrix();

        // Fan blades (longer and darker)
        for (int i = 0; i < 4; i++) {
            glPushMatrix();
            glRotatef(i * 90, 0, 1, 0);
            glColor3f(0.6f, 0.7f, 0.8f);  // Steel blue blades
            glTranslatef(0.8, -1.3, 0);  // Moved further out for longer blades
            glScalef(1.6, 0.05, 0.2);    // Increased length
            glutSolidCube(1.0);
            glPopMatrix();
        }

        glPopMatrix();
    }

    void rotateFan() {
        if (on) {
            fanRotation += 1.0f;
            if (fanRotation > 360.0f) fanRotation -= 360.0f;
        }
    }
};

#endif
