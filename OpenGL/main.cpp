#include <GL/freeglut.h>    // Imports GLUT and OpenGL functions for rendering graphics and handling window events
#include <cmath>            // Provides math functions like sin(), cos(), etc.
#include <cstdlib>          // Provides rand(), srand() for random values
#include <ctime>            // For time(), used to seed random number generator


float turtleX = -1.2f;       // X-position of the turtle (starts off-screen left)
float waveAngle = 0.0f;      // Arm waving angle (legacy, unused here)
bool wavingUp = true;        // Direction of waving (unused here)
bool isWalking = true;       // Boolean to control whether the turtle is moving
float sunY = -0.8f;          // Y-position of the sun for sunrise effect
float skyBright = 0.2f;      // Brightness level of the sky
bool showMessage = false;    // Whether the turtle’s speech bubble is visible
bool flash = false;          // Whether a lightning flash is happening
float flashTimer = 0;        // Timer for controlling lightning flash duration


const int maxDrops = 100;        // Total number of rain drops
float rainX[maxDrops], rainY[maxDrops];  // Arrays to store positions of rain drops


void initRain() {
    for (int i = 0; i < maxDrops; ++i) {
        rainX[i] = -1.0f + static_cast<float>(rand()) / RAND_MAX * 3.0f;
        // Random X between -1.5 and +1.5

        rainY[i] = 10.0f + static_cast<float>(rand()) / RAND_MAX * 2.0f;
        // Random Y between -1.0 and +1.0
    }
}


void drawRain() {
    glColor3f(0.6f, 0.8f, 1.0f);      // Light blue rain color
    glLineWidth(0.5f);                // Thin lines
    glBegin(GL_LINES);               // Start drawing lines
    for (int i = 0; i < maxDrops; ++i) {
        glVertex2f(rainX[i], rainY[i]);            // Start of raindrop
        glVertex2f(rainX[i], rainY[i] - 0.05f);     // End of raindrop slightly below
    }
    glEnd();                         // End drawing lines
}


void init() {
    glClearColor(0.2f, 0.4f, 0.6f, 1.0f);       // Set initial background color
    glEnable(GL_BLEND);                        // Enable transparency blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blending mode
    srand(static_cast<unsigned>(time(0)));     // Seed random generator
    initRain();                                // Call function to generate rain
}


void drawCircle(float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);                 // Circle is a fan of triangles
    glVertex2f(0.0f, 0.0f);                   // Center of the circle
    for (int i = 0; i <= segments; i++) {
        float angle = i * 2.0f * 3.14159265f / segments;
        // Angle around the circle (0 to 2π)
        glVertex2f(radius * cosf(angle), radius * sinf(angle));
    }
    glEnd();                                  // Finish drawing
}


void drawText(const char* text, float x, float y) {
    glColor3f(0, 0, 0);               // Set text color to black
    glRasterPos2f(x, y);             // Position where text begins
    while (*text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *(text++));
    // Draw each character in the string
}


void drawSkyGradient() {
    if (flash) glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // Bright sky during lightening
    else glClearColor(0.2f, 0.4f, 0.6f, 1.0f);        // Normal sky color

    glBegin(GL_QUADS);  // Draw background rectangle
    glColor3f(skyBright, skyBright + 0.2f, skyBright + 0.4f); // Top gradient color
    glVertex2f(-1.5f, 1.0f); 
    glVertex2f(1.5f, 1.0f);
    glColor3f(skyBright, skyBright * 0.9f, skyBright * 0.6f); // Bottom gradient
    glVertex2f(1.5f, -1.0f); 
    glVertex2f(-1.5f, -1.0f);
    glEnd();
}

void drawGround() {
    glColor3f(0.2f, 0.6f, 0.2f);       // Green color
    glBegin(GL_QUADS);                // Draw a rectangle
    glVertex2f(-2.0f, -1.0f); 
    glVertex2f(2.0f, -1.0f);
    glVertex2f(2.0f, -0.7f); 
    glVertex2f(-2.0f, -0.7f);
    glEnd();
}


void drawSun() {
    glPushMatrix();                   // Save current transformation
    glTranslatef(0.0f, sunY, 0.0f);   // Move sun vertically based on sunrise
    glColor3f(1.0f, 0.9f, 0.3f);      // Yellowish sun
    drawCircle(0.1f, 50);            // Draw sun circle
    glPopMatrix();                    // Restore transformation
}


void drawTrees() {
    for (float i = -1.5f; i < 1.6f; i += 0.6f) {     // Loop to draw trees
        glPushMatrix();
        glTranslatef(i - turtleX * 0.3f, -0.4f, 0.0f); // Parallax effect

        // Tree trunk
        glColor3f(0.4f, 0.2f, 0.1f);
        glBegin(GL_QUADS);
        glVertex2f(-0.02f, -0.3f); 
        glVertex2f(0.02f, -0.3f);
        glVertex2f(0.02f, 0.0f); 
        glVertex2f(-0.02f, 0.0f);
        glEnd();

        // Leaves
        glColor3f(0.1f, 0.6f, 0.1f);
        glTranslatef(0.0f, 0.05f, 0.0f);
        drawCircle(0.1f, 30);
        glPopMatrix();
    }
}

void drawSpeechBubble() {
    if (!showMessage) return;

    glColor3f(1, 1, 1);       // White box
    glBegin(GL_POLYGON);
    glVertex2f(turtleX + 0.1f, -0.3f);
    glVertex2f(turtleX + 0.5f, -0.3f);
    glVertex2f(turtleX + 0.5f, -0.1f);
    glVertex2f(turtleX + 0.1f, -0.1f);
    glEnd();

    drawText("Good morning!", turtleX + 0.15f, -0.2f); // Add text inside
}


void drawTurtle(float x) {
    glPushMatrix();
    glTranslatef(x, -0.65f, 0.0f);  // Place turtle at ground level

    // Body
    glColor3f(0.2f, 0.7f, 0.3f);
    glPushMatrix();
    glScalef(1.0f, 0.6f, 1.0f); drawCircle(0.2f, 50);
    glPopMatrix();

    // Head
    glPushMatrix();
    glTranslatef(0.25f, 0.05f, 0.0f);   // Position head
    glColor3f(0.3f, 0.9f, 0.4f); 
    drawCircle(0.06f, 30);

    // Eyes
    glColor3f(0, 0, 0);
    glPushMatrix(); 
    glTranslatef(-0.015f, 0.015f, 0.0f); 
    drawCircle(0.008f, 10); glPopMatrix();
    glPushMatrix(); 
    glTranslatef(0.015f, 0.015f, 0.0f); 
    drawCircle(0.008f, 10); glPopMatrix();

    // Mouth
    glBegin(GL_LINES); 
    glVertex2f(-0.015f, -0.015f); 
    glVertex2f(0.015f, -0.015f); 
    glEnd();

    glPopMatrix(); // End head

    // Legs
    glColor3f(0.1f, 0.5f, 0.2f);
    glPushMatrix(); glTranslatef(-0.15f, -0.12f, 0.0f); drawCircle(0.04f, 20); glPopMatrix();
    glPushMatrix(); glTranslatef(0.15f, -0.12f, 0.0f); drawCircle(0.04f, 20); glPopMatrix();

    glPopMatrix();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawSkyGradient();
    drawSun();
    drawTrees();
    drawGround();
    drawTurtle(turtleX);
    drawSpeechBubble();
    drawRain();
    glutSwapBuffers(); // Double buffering
}


void timer(int) {
    if (isWalking) {
        turtleX += 0.0015f;                     // Move turtle
        if (turtleX > 1.6f) turtleX = -1.2f;     // Reset position

 
        if (sunY < 0.3f) sunY += 0.0008f;        // Sunrise animation
        if (skyBright < 0.8f) skyBright += 0.001f;
        if (turtleX > 0.5f) showMessage = true;  // Show message after turtle reaches certain position
    }

    // Rain drop falling
    for (int i = 0; i < maxDrops; ++i) {
        rainY[i] -= 0.03f;
        if (rainY[i] < -1.0f) {
            rainX[i] = -1.5f + static_cast<float>(rand()) / RAND_MAX * 3.0f;
            rainY[i] = 1.0f;
        }
    }

    // Flash lightning logic
    flashTimer -= 0.016f;
    if (flashTimer <= 0) {
        flash = (rand() % 100 < 3);
        flashTimer = flash ? 0.1f : (rand() % 3 + 2);
    }

    glutPostRedisplay();            // Ask to redraw
    glutTimerFunc(16, timer, 0);    // Repeat every 16ms (~60 FPS)
}

void reshape(int w, int h) {
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(-1.5f, 1.5f, -1.0f, 1.0f); // 2D coordinate system aspect rtio is 1.5
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
}


void keyboard(unsigned char key, int, int) {
    if (key == 'r' || key == 'R') { // Reset
        turtleX = -1.2f; waveAngle = 0.0f; sunY = -0.8f; skyBright = 0.2f;
        isWalking = true; showMessage = false;
    }
    else if (key == 'p' || key == 'P') isWalking = false; // Pause
    else if (key == 's' || key == 'S') isWalking = true;  // Start
    else if (key == 'm' || key == 'M') showMessage = true;
    else if (key == 27) exit(0); // ESC to quit
}


void mouse(int button, int state, int, int) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) isWalking = !isWalking; // Toggle walking
        else if (button == GLUT_RIGHT_BUTTON) { // Reset
            turtleX = -1.2f; waveAngle = 0.0f; sunY = -0.8f; skyBright = 0.2f;
            isWalking = true; showMessage = false;
        }
    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);                       // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Double buffer + RGB
    glutInitWindowSize(900, 600);                // Window size
    glutCreateWindow("Turtle with Rain, Lightning, and Sunrise");

    glutDisplayFunc(display);      // Set display function
    glutReshapeFunc(reshape);      // Set reshape callback
    glutKeyboardFunc(keyboard);    // Keyboard input
    glutMouseFunc(mouse);          // Mouse input
    glutTimerFunc(0, timer, 0);    // Start timer

    init();                        // Initialize scene
    glutMainLoop();                // Enter main loop
    return 0;
}
