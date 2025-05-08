#ifndef GLUT_COMPAT_HPP
#define GLUT_COMPAT_HPP

#ifdef __APPLE__
    #ifndef GL_SILENCE_DEPRECATION
        #define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
    #endif
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <GLUT/glut.h>
    
    // Add our own implementation of glutSolidCylinder for macOS
    inline void glutSolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks) {
        GLUquadricObj *quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);
        gluQuadricNormals(quadric, GLU_SMOOTH);
        gluQuadricTexture(quadric, GL_TRUE);
        gluCylinder(quadric, radius, radius, height, slices, stacks);
        
        // Draw the top and bottom caps to make a solid cylinder
        glPushMatrix();
        gluDisk(quadric, 0.0, radius, slices, 1);
        glTranslatef(0.0, 0.0, height);
        gluDisk(quadric, 0.0, radius, slices, 1);
        glPopMatrix();
        
        gluDeleteQuadric(quadric);
    }
#else
    #include <GL/glut.h>
    #include <GL/freeglut_ext.h>
#endif

#endif // GLUT_COMPAT_HPP