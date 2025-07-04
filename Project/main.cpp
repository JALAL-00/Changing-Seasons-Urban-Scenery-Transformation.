#include <iostream>
#include <GLUT/glut.h>
#include <math.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <cmath>
#define PI 3.14159265358979323846
using namespace std;


// --- ENUMS & STRUCTS ---
enum ActiveScene { SCENE_CITY, SCENE_SEA, SCENE_ROCKET, SCENE_FUTURE };
ActiveScene currentSceneView = SCENE_CITY;

enum Season { EVENING, NIGHT, MORNING, WINTER, AUTUMN, SPRING };
Season currentSeason = EVENING;


// --- GLOBAL VARIABLES --- //  // --- Timing and Speed Control ---

int lastTime = 0; // For delta time calculation

// Speeds for City Scene (units per second)
const float CLOUD_SPEED = 2.0f;
const float SUN_SPEED = -1.0f;
const float BOAT_SPEED = -17.5f;
const float WAVE_SPEED = -10.0f;
const float CAR1_SPEED = -40.0f;
const float CAR2_SPEED = 40.0f;
const float HELI_BLADE_SPEED = 2500.0f; // Degrees per second
const float RAIN_SPEED_MULTIPLIER = 50.0f;
const float LEAF_SPEED_MULTIPLIER = 20.0f;
const float SNOW_SPEED_MULTIPLIER = 20.0f;

// Speeds for Sea Scene (units per second)
const float SEA_SHIP_SPEED = 50.0f;
const float SEA_CLOUD_SPEED = 20.0f;
const float SEA_WINDMILL_SPEED = 100.0f; // Degrees per second
const float SEA_RAIN_SPEED = 200.0f;

// Speeds for Rocket Scene (units/degrees per second)
const float ROCKET_SUN_SPEED_X = 5.0f;
const float ROCKET_SUN_SPEED_Y = 50.0f;
const float ROCKET_BOAT_SPEED_X = 200.0f;
const float ROCKET_BOAT_SPEED_Y = 2.5f;
const float ROCKET_WATER_CURRENT_SPEED = 60.0f;
const float ROCKET_SPEED_Y = 100.0f;
const float ROCKET_WHEEL_ROTATION_SPEED = 500.0f;
const float ROCKET_CAR_SPEED_X = 300.0f;

// Speeds for Future City Scene (units per second)
const float FUTURE_UFO_SPEED_1 = 1.0f; // Speed for 't'
const float FUTURE_UFO_SPEED_2 = 0.5f; // Speed for 't1'
const float FUTURE_UFO_SPEED_3 = 0.75f;// Speed for 't2'
const float FUTURE_MAIN_UFO_VERTICAL_SPEED = 0.5f;
const float FUTURE_MAIN_UFO_SCALE_SPEED = 0.5f;

// --- Global state variables for the City Scene ---
float _move3 = 0.0f; // cloud
float _move4 = 0.0f; // sun
float _move5 = 0.0f; // boat
float _move8 = 0.0f; // wave
float _moveM = 0.0f; // boat up/down
float _movecar = 0.0f; // Car
float _movecar2 = 0.0f;
float heliX = 10.0f, heliY = 65.0f, heliSpeedX = 0.2f, heliSpeedY = 0.0f, bladeRotate = 0.0f;

// --- Global state variables for the Sea Scene ---
float sea_shipX = 0.0f;
bool sea_shipMoving = false;
bool sea_isNightMode = false;
bool sea_startRotation = false;
float sea_rotateAngle = 0.0f;
float sea_cloudX = 0.0f;
bool sea_cloudMoving = false;
const int sea_numDrops = 100;
float sea_rainX[sea_numDrops];
float sea_rainY[sea_numDrops];
bool sea_isRaining = false;

// <<< --- Global state variables for the Rocket Scene --- >>>
float rocket_moveSuntX = 0;
float rocket_moveSunY = 0;
float rocket_moveSunAngle = 0;
float rocket_moveboatX = 0;
float rocket_moveboatY = 0;
float rocket_movewatercurrentX = 0;
float rocket_moveRocketY = 0;
float rocket_rotateWheel=0;
float rocket_moveCarsX =0 ;


// <<< --- Global state variables for the Alien Space/Future City Scene --- >>>
float future_t=0, future_t1=-5.9, future_t2=0.7;
GLfloat future_speed=0.02f, future_speed1=0.01f, future_speed2=0.015f;
float future_sc=1, future_h=0;
int future_sz=7;
int future_starc1=204, future_starc2=206, future_starc3=240;
int future_sky1=53, future_sky2=67, future_sky3=184;
int future_back1=24, future_back2=28, future_back3=80;
int future_fly=-1; // Mouse control variable


// <<< --- END OF GLOBAL VARIABLES --- >>>

struct Raindrop {
    float x, y;
    float speed;
};
const int NUM_RAINDROPS = 300;
vector<Raindrop> raindrops(NUM_RAINDROPS);

struct Leaf {
    float x, y;
    float speed;
    float size;
    float flutter;
    float r, g, b;
};
const int NUM_LEAVES = 100;
vector<Leaf> leaves(NUM_LEAVES);

struct Snowflake {
    float x, y;
    float speed;
    float size;
};
const int NUM_SNOWFLAKES = 250;
vector<Snowflake> snowflakes(NUM_SNOWFLAKES);


// <<< ----- Function For Cuty View ------ >>>
//This function initializes the raindrops' random positions.
void initRain() {
    for (int i = 0; i < NUM_RAINDROPS; i++) {
        raindrops[i].x = static_cast<float>(rand() % 1000) / 10.0f;
        raindrops[i].y = 80.0f + (static_cast<float>(rand() % 800) / 10.0f);
        raindrops[i].speed = 0.8f + (static_cast<float>(rand() % 6) / 10.0f); // Keep individual speed variation
    }
}


//This function initializes the snowflakes' random positions and speeds.
void initSnow() {
    for (int i = 0; i < NUM_SNOWFLAKES; i++) {
        snowflakes[i].x = static_cast<float>(rand() % 1000) / 10.0f;
        snowflakes[i].y = 80.0f + (static_cast<float>(rand() % 600) / 10.0f);
        snowflakes[i].speed = 0.05f + (static_cast<float>(rand() % 10) / 100.0f); // Keep individual speed variation
        snowflakes[i].size = 0.1f + (static_cast<float>(rand() % 4) / 10.0f);
    }
}

// This function initializes the leaves' random properties.
void initLeaves() {
    for (int i = 0; i < NUM_LEAVES; i++) {
        leaves[i].x = static_cast<float>(rand() % 1000) / 10.0f;
        leaves[i].y = 80.0f + (static_cast<float>(rand() % 600) / 10.0f);
        leaves[i].speed = 0.04f + (static_cast<float>(rand() % 8) / 100.0f); // Keep individual speed variation
        leaves[i].size = 0.3f + (static_cast<float>(rand() % 5) / 10.0f);
        leaves[i].flutter = 0.0f;

        int colorType = rand() % 3;
        if (colorType == 0) {
            leaves[i].r = 0.8f; leaves[i].g = 0.2f; leaves[i].b = 0.1f;
        } else if (colorType == 1) {
            leaves[i].r = 0.9f; leaves[i].g = 0.5f; leaves[i].b = 0.1f;
        } else {
            leaves[i].r = 0.9f; leaves[i].g = 0.8f; leaves[i].b = 0.1f;
        }
    }
}

//This function draws the falling snow Add the leaf drawing logic.
void drawWeatherEffects() {
    if (currentSeason == WINTER) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, 0.8f);  // White color for snowflakes
        glBegin(GL_QUADS);
        for (int i = 0; i < NUM_SNOWFLAKES; i++) {
            float size = snowflakes[i].size;
            glVertex2f(snowflakes[i].x - size, snowflakes[i].y - size);
            glVertex2f(snowflakes[i].x + size, snowflakes[i].y - size);
            glVertex2f(snowflakes[i].x + size, snowflakes[i].y + size);
            glVertex2f(snowflakes[i].x - size, snowflakes[i].y + size);
        }
        glEnd();
        glDisable(GL_BLEND);
    }
    else if (currentSeason == AUTUMN) {  // Falling Leaves for AUTUMN
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        for (int i = 0; i < NUM_LEAVES; i++) {
            glColor4f(leaves[i].r, leaves[i].g, leaves[i].b, 0.85f);
            float size = leaves[i].size;
            float x_pos = leaves[i].x + sin(leaves[i].flutter) * 2.0;
            glVertex2f(x_pos - size, leaves[i].y - size);
            glVertex2f(x_pos + size, leaves[i].y - size);
            glVertex2f(x_pos + size, leaves[i].y + size);
            glVertex2f(x_pos - size, leaves[i].y + size);
            // Flutter is handled in the update function
        }
        glEnd();
        glDisable(GL_BLEND);
    }
    else if (currentSeason == SPRING) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.7f, 0.8f, 1.0f, 0.5f);
        glLineWidth(1.5f);
        glBegin(GL_LINES);
        for (int i = 0; i < NUM_RAINDROPS; i++) {
            glVertex2f(raindrops[i].x, raindrops[i].y);
            glVertex2f(raindrops[i].x + 0.2, raindrops[i].y - 1.0);
        }
        glEnd();
        glLineWidth(1.0f);
        glDisable(GL_BLEND);
    }
}


// <<< DRAWING FUNCTIONS FOR CITY SCENE >>>
void drawSky() {
    if (currentSeason == EVENING) {
        // Evening: Yellow-orange gradient
        glColor3ub(249, 226, 79);
        glBegin(GL_QUADS);
        glVertex2f(0, 35);
        glVertex2f(100, 35);
        glColor3ub(255, 255, 51);
        glVertex2f(100, 80);
        glColor3ub(255, 178, 102);
        glVertex2f(0, 80);
        glEnd();
    } else if (currentSeason == NIGHT) {
        // Night: Dark blue gradient with stars
        glColor3ub(102, 102, 255);
        glBegin(GL_QUADS);
        glVertex2f(0, 35);
        glVertex2f(100, 35);
        glColor3ub(0, 0, 102);
        glVertex2f(100, 80);
        glColor3ub(43, 57, 182);
        glVertex2f(0, 80);
        glEnd();
    } else if (currentSeason == MORNING) {
        // Morning: Light blue gradient
        glColor3ub(200, 210, 220);
        glBegin(GL_QUADS);
        glColor3ub(255, 255, 255);
        glVertex2f(0, 35);
        glColor3ub(150, 180, 200);
        glVertex2f(100, 35);
        glColor3ub(0, 153, 153);
        glVertex2f(100, 80);
        glVertex2f(0, 80);
        glEnd();
    } else if (currentSeason == WINTER) {
        // Winter: Grey, cloudy sky
        glColor3ub(200, 210, 220);
        glBegin(GL_QUADS);
        glVertex2f(0, 35);
        glVertex2f(100, 35);
        glColor3ub(180, 190, 200);
        glVertex2f(100, 80);
        glVertex2f(0, 80);
        glEnd();
    } else if (currentSeason == AUTUMN) {
        // Autumn: Golden-yellowish gradient with pale blue
        glColor3ub(255, 180, 100);
        glBegin(GL_QUADS);
        glVertex2f(0, 35);
        glVertex2f(100, 35);
        glColor3ub(135, 206, 235);
        glVertex2f(100, 80);
        glVertex2f(0, 80);
        glEnd();
    } else if (currentSeason == SPRING) {
        glColor3ub(200, 225, 255);
        glBegin(GL_QUADS);
        glVertex2f(0, 35);
        glVertex2f(100, 35);
        glColor3ub(170, 190, 240);
        glVertex2f(100, 80);
        glVertex2f(0, 80);
        glEnd();
    }
}
void drawSunAndMoon() {
    if (currentSeason == WINTER) return;
    
    if (currentSeason == EVENING) {
        glLoadIdentity();
        glPushMatrix();
        glTranslatef(0.0f, _move4, 0.0f);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 200; i++) {
            glColor3ub(245, 240, 180);
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 4.0;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 90, y + 70);
        }
        glEnd();
        glPopMatrix();
    } else if (currentSeason == NIGHT) {
        // Moon
        glBegin(GL_POLYGON);
        for (int i = 0; i < 200; i++) {
            glColor3f(1, 1, 1);
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 4.0;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 90, y + 70);
        }
        glEnd();
    } else if (currentSeason == MORNING) {
        // Sun is high
        glBegin(GL_POLYGON);
        for (int i = 0; i < 200; i++) {
            glColor3ub(255, 240, 200);
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 4.0;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 90, y + 70);
        }
        glEnd();
    }
}
void drawStars() {
    if (currentSeason != NIGHT) return;

    // Star drawing code
    float pi = 3.1416;
    for (int i = 0; i < 200; i++) {
        float A = (i * 2 * pi) / 200;
        float r = 0.2;
        float x = r * cos(A);
        float y = r * sin(A);

        // First Star
        glColor3ub(255, 255, 255);
        glBegin(GL_POLYGON);
        glVertex2f(x + 10, y + 70);
        glEnd();

        // Second Star
        glColor3ub(255, 255, 255);
        glBegin(GL_POLYGON);
        glVertex2f(x + 30, y + 74);
        glEnd();

        // Third Star
        glColor3ub(255, 255, 255);
        glBegin(GL_POLYGON);
        glVertex2f(x + 40, y + 61);
        glEnd();

        // Fourth Star
        glColor3ub(255, 255, 255);
        glBegin(GL_POLYGON);
        glVertex2f(x + 50, y + 55);
        glEnd();

        // Fifth Star
        glColor3ub(255, 255, 255);
        glBegin(GL_POLYGON);
        glVertex2f(x + 80, y + 75);
        glEnd();

        // Sixth Star
        glColor3ub(255, 255, 255);
        glBegin(GL_POLYGON);
        glVertex2f(x + 65, y + 70);
        glEnd();
    }

}
void drawClouds() {
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(_move3, 0.0f, 0.0f);

    // Set cloud color based on time of day
    if (currentSeason == MORNING) {
        glColor3ub(220, 220, 230);
    } else {
        glColor3ub(255, 255, 255);
    }

    // Cloud 1
    glBegin(GL_POLYGON);
        for(int i = 0; i < 200; i++) {
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 1.9;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 15, y + 70);
        }
    glEnd();

    glBegin(GL_POLYGON);
        for(int i = 0; i < 200; i++) {
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 1.9;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 18.5, y + 70);
        }
    glEnd();

    glBegin(GL_POLYGON);
        for(int i = 0; i < 200; i++) {
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 1.9;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 16.5, y + 72);
        }
    glEnd();

    // Cloud 2
    glBegin(GL_POLYGON);
        for(int i = 0; i < 200; i++) {
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 2;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 35, y + 65);
        }
    glEnd();

    glBegin(GL_POLYGON);
        for(int i = 0; i < 200; i++) {
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 2;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 38, y + 65);
        }
    glEnd();

    glBegin(GL_POLYGON);
        for(int i = 0; i < 200; i++) {
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 2;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 36.4, y + 67);
        }
    glEnd();

    // Cloud 3
    glBegin(GL_POLYGON);
        for(int i = 0; i < 200; i++) {
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 2;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 5, y + 70);
        }
    glEnd();

    glBegin(GL_POLYGON);
        for(int i = 0; i < 200; i++) {
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 2;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 2, y + 70);
        }
    glEnd();

    glBegin(GL_POLYGON);
        for(int i = 0; i < 200; i++) {
            float pi = 3.1416;
            float A = (i * 2 * pi) / 200;
            float r = 2;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x + 3.5, y + 72);
        }
    glEnd();
    glPopMatrix();
}
void drawBuildings() {
       if (currentSeason == WINTER) {
           glColor3f(1.0, 1.0, 1.0);
           // Snow for building1
           glBegin(GL_QUADS); glVertex2f(1, 65); glVertex2f(12, 65); glVertex2f(12, 66); glVertex2f(1, 66); glEnd();
           // Snow for building2
           glBegin(GL_QUADS); glVertex2f(9, 60); glVertex2f(22, 60); glVertex2f(25, 58); glVertex2f(25, 59); glVertex2f(22, 61); glVertex2f(9, 61); glEnd();
           // Snow for mosque
           glBegin(GL_QUADS); glVertex2f(36, 51); glVertex2f(37, 51); glVertex2f(37, 51.5); glVertex2f(36, 51.5); glEnd();
           glBegin(GL_QUADS); glVertex2f(53, 51); glVertex2f(54, 51); glVertex2f(54, 51.5); glVertex2f(53, 51.5); glEnd();
       }
    
    ///buildings
    ///mosque
        //base
        glColor3ub(153.0f,57.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(35,40);
        glVertex2f(55,40);
        glVertex2f(54,41);
        glVertex2f(36,41);
        glEnd();

        //Left pillar
        glColor3ub(179.0f,89.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(36,41);
        glVertex2f(37,41);
        glVertex2f(37,49);
        glVertex2f(36,49);
        glEnd();

        glColor3ub(179.0f,89.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(36,49);
        glVertex2f(37,49);
        glVertex2f(37.5,49.5);
        glVertex2f(35.5,49.5);
        glEnd();

        glColor3ub(153.0f,57.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(36 , 49.5);
        glVertex2f(37,49.5);
        glVertex2f(37,51);
        glVertex2f(36,51);
        glEnd();

        glColor3ub(0.0f,204.0f,204.0f);
        glBegin(GL_POLYGON);
        glVertex2f(36.5,51);
        glVertex2f(37,51.5);
        glVertex2f(36.5,52);
        glVertex2f(36,51.5);
        glEnd();

        //second left pillar
        glColor3ub(179.0f,89.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(38,45);
        glVertex2f(39,45);
        glVertex2f(39,48);
        glVertex2f(38,48);
        glEnd();

        glColor3ub(0.0f,204.0f,204.0f);
        glBegin(GL_POLYGON);
        glVertex2f(38.5,48);
        glVertex2f(39,48.5);
        glVertex2f(38.5,49);
        glVertex2f(38,48.5);
        glEnd();

        //right pillar
        glColor3ub(179.0f,89.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(53,41);
        glVertex2f(54,41);
        glVertex2f(54,49);
        glVertex2f(53,49);
        glEnd();

        glColor3ub(179.0f,89.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(53,49);
        glVertex2f(54,49);
        glVertex2f(54.5,49.5);
        glVertex2f(52.5,49.5);
        glEnd();

        glColor3ub(153.0f,57.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(53 , 49.5);
        glVertex2f(54,49.5);
        glVertex2f(54,51);
        glVertex2f(53,51);
        glEnd();

        glColor3ub(0.0f,204.0f,204.0f);
        glBegin(GL_POLYGON);
        glVertex2f(53.5,51);
        glVertex2f(54,51.5);
        glVertex2f(53.5,52);
        glVertex2f(53,51.5);
        glEnd();
    
        //second right pillar
        glColor3ub(179.0f,89.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(51,45);
        glVertex2f(52,45);
        glVertex2f(52,48);
        glVertex2f(51,48);
        glEnd();

        glColor3ub(0.0f,204.0f,204.0f);
        glBegin(GL_POLYGON);
        glVertex2f(51.5,48);
        glVertex2f(52,48.5);
        glVertex2f(51.5,49);
        glVertex2f(51,48.5);
        glEnd();

        //body
        glColor3ub(204.0f,105.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(37,41);
        glVertex2f(43.5,41);
        glVertex2f(43.5,45);
        glVertex2f(37,45);
        glEnd();

        glColor3ub(204.0f,105.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(46,41);
        glVertex2f(53,41);
        glVertex2f(53,45);
        glVertex2f(46,45);
        glEnd();

        //front door
        glColor3ub(153.0f,57.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(43.5,41);
        glVertex2f(46.5,41);
        glVertex2f(46.5,46);
        glVertex2f(43.5,46);
        glEnd();

        glColor3ub(179.0f,89.0f,0.0f);
        glBegin(GL_QUADS);
        glVertex2f(44,41);
        glVertex2f(46,41);
        glVertex2f(46,44.5);
        glVertex2f(44,44.5);
        glEnd();

        glColor3ub(179.0f,89.0f,0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(44,44.5);
        glVertex2f(46,44.5);
        glVertex2f(45,45.5);
        glEnd();

        //left doors
        glColor3ub(102.0f,0.0f,0.0f);
        glBegin(GL_QUADS);
        glVertex2f(38,41);
        glVertex2f(39,41);
        glVertex2f(39,43);
        glVertex2f(38,43);
        glEnd();

        glColor3ub(255.0f,170.0f,0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(38,43);
        glVertex2f(39,43);
        glVertex2f(38.5,43.5);
        glEnd();

        //2
        glColor3ub(102.0f,0.0f,0.0f);
        glBegin(GL_QUADS);
        glVertex2f(40,41);
        glVertex2f(41,41);
        glVertex2f(41,43);
        glVertex2f(40,43);
        glEnd();

        glColor3ub(255.0f,170.0f,0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(40,43);
        glVertex2f(41,43);
        glVertex2f(40.5,43.5);
        glEnd();

        //right doors
         glColor3ub(102.0f,0.0f,0.0f);
        glBegin(GL_QUADS);
        glVertex2f(49,41);
        glVertex2f(50,41);
        glVertex2f(50,43);
        glVertex2f(49,43);
        glEnd();

        glColor3ub(255.0f,170.0f,0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(49,43);
        glVertex2f(50,43);
        glVertex2f(49.5,43.5);
        glEnd();

        //2
        glColor3ub(102.0f,0.0f,0.0f);
        glBegin(GL_QUADS);
        glVertex2f(51,41);
        glVertex2f(52,41);
        glVertex2f(52,43);
        glVertex2f(51,43);
        glEnd();

        glColor3ub(255.0f,170.0f,0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(51,43);
        glVertex2f(52,43);
        glVertex2f(51.5,43.5);
        glEnd();

    //middle miner
        glColor3ub(0.0f,204.0f,204.0f);
        glBegin(GL_POLYGON);
        glVertex2f(43.5,46);
        glVertex2f(46.5,46);
        glVertex2f(47.5,47);
        glVertex2f(45,49);
        glVertex2f(42.5,47);
        glEnd();

        glColor3ub(255.0f,170.0f,0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(45,49);
        glVertex2f(45.5,49.5);
        glVertex2f(45,50);
        glVertex2f(44.5,49.5);
        glEnd();

    //building1
     glColor3ub(183,149,11);
        glBegin(GL_POLYGON);
        glVertex2f(1,40);
        glVertex2f(1,65);
        glVertex2f(12,65);
        glVertex2f(12,40);
        glEnd();
        glColor3ub(135,54,11);
        glBegin(GL_POLYGON);
        glVertex2f(2,42);
        glVertex2f(2,62);
        glVertex2f(11,62);
        glVertex2f(11,42);
        glEnd();
         glColor3ub(183,149,11);
        glBegin(GL_POLYGON);
        glVertex2f(2,60);
        glVertex2f(2,59);
        glVertex2f(11,59);
        glVertex2f(11,60);
        glEnd();
         glColor3ub(183,149,11);
        glBegin(GL_POLYGON);
        glVertex2f(2,57);
        glVertex2f(2,56);
        glVertex2f(11,56);
        glVertex2f(11,57);
        glEnd();
         glColor3ub(183,149,11);
        glBegin(GL_POLYGON);
        glVertex2f(2,54);
        glVertex2f(2,53);
        glVertex2f(11,53);
        glVertex2f(11,54);
        glEnd();
         glColor3ub(183,149,11);
        glBegin(GL_POLYGON);
        glVertex2f(2,51);
        glVertex2f(2,50);
        glVertex2f(11,50);
        glVertex2f(11,51);
        glEnd();
         glColor3ub(183,149,11);
        glBegin(GL_POLYGON);
        glVertex2f(2,48);
        glVertex2f(2,47);
        glVertex2f(11,47);
        glVertex2f(11,48);
        glEnd();
        glColor3ub(183,149,11);
        glBegin(GL_POLYGON);
        glVertex2f(2,45);
        glVertex2f(2,44);
        glVertex2f(11,44);
        glVertex2f(11,45);
        glEnd();
        glColor3ub(183,149,11);
        glBegin(GL_POLYGON);
        glVertex2f(4,42);
        glVertex2f(4,62);
        glVertex2f(5,62);
        glVertex2f(5,42);
        glEnd();
        glColor3ub(183,149,11);
        glBegin(GL_POLYGON);
        glVertex2f(7,42);
        glVertex2f(7,62);
        glVertex2f(8,62);
        glVertex2f(8,42);
        glEnd();

    //building3 (First one)
        glColor3ub(0,51,102);
        glBegin(GL_POLYGON);
        glVertex2f(20,40);
        glVertex2f(20,70);
        glVertex2f(34.5,70);
        glVertex2f(34.5,40);
        glEnd();
        glColor3ub(0,123,123);
        glBegin(GL_POLYGON);
        glVertex2f(21,40);
        glVertex2f(21,68);
        glVertex2f(22,68);
        glVertex2f(22,40);
        glEnd();
        glColor3ub(0,123,123);
        glBegin(GL_POLYGON);
        glVertex2f(21,40);
        glVertex2f(21,68);
        glVertex2f(22,68);
        glVertex2f(22,40);
        glEnd();
        glColor3ub(0,123,123);
        glBegin(GL_POLYGON);
        glVertex2f(23,40);
        glVertex2f(23,68);
        glVertex2f(24,68);
        glVertex2f(24,40);
        glEnd();
        glColor3ub(0,123,123);
        glBegin(GL_POLYGON);
        glVertex2f(25,40);
        glVertex2f(25,68);
        glVertex2f(26,68);
        glVertex2f(26,40);
        glEnd();
        glColor3ub(0,123,123);
        glBegin(GL_POLYGON);
        glVertex2f(27,40);
        glVertex2f(27,68);
        glVertex2f(28,68);
        glVertex2f(28,40);
        glEnd();
        glColor3ub(0,123,123);
        glBegin(GL_POLYGON);
        glVertex2f(29,40);
        glVertex2f(29,68);
        glVertex2f(30,68);
        glVertex2f(30,40);
        glEnd();
        glColor3ub(0,123,123);
        glBegin(GL_POLYGON);
        glVertex2f(31,40);
        glVertex2f(31,68);
        glVertex2f(32,68);
        glVertex2f(32,40);
        glEnd();

    //building2
    glColor3ub(0,204,204);
        glBegin(GL_POLYGON);
        glVertex2f(9,40);
        glVertex2f(9,60);
        glVertex2f(22,60);
        glVertex2f(25,58);
        glVertex2f(25,40);
        glEnd();
        glColor3ub(0,153,153);
        glBegin(GL_POLYGON);
        glVertex2f(22,40);
        glVertex2f(22,60);
        glVertex2f(25,58);
        glVertex2f(25,40);
        glEnd();
         glColor3ub(0,183,183);
        glBegin(GL_POLYGON);
        glVertex2f(9,40);
        glVertex2f(9,58);
        glVertex2f(22,58);
        glVertex2f(22,40);
        glEnd();
        glColor3ub(0,102,102);
        glBegin(GL_POLYGON);
        glVertex2f(22,40);
        glVertex2f(22,59);
        glVertex2f(24,57.5);
        glVertex2f(24,40);
        glEnd();
        glColor3ub(0,80,80);
        glBegin(GL_POLYGON);
        glVertex2f(10,57);
        glVertex2f(10,56);
        glVertex2f(24,56);
        glVertex2f(24,57);
        glEnd();
         glColor3ub(0,80,80);
        glBegin(GL_POLYGON);
        glVertex2f(10,55);
        glVertex2f(10,54);
        glVertex2f(24,54);
        glVertex2f(24,55);
        glEnd();
        glColor3ub(0,80,80);
        glBegin(GL_POLYGON);
        glVertex2f(10,53);
        glVertex2f(10,52);
        glVertex2f(24,52);
        glVertex2f(24,53);
        glEnd();
        glColor3ub(0,80,80);
        glBegin(GL_POLYGON);
        glVertex2f(10,51);
        glVertex2f(10,50);
        glVertex2f(24,50);
        glVertex2f(24,51);
        glEnd();
        glColor3ub(0,80,80);
        glBegin(GL_POLYGON);
        glVertex2f(10,49);
        glVertex2f(10,48);
        glVertex2f(24,48);
        glVertex2f(24,49);
        glEnd();
         glColor3ub(0,80,80);
        glBegin(GL_POLYGON);
        glVertex2f(10,47);
        glVertex2f(10,46);
        glVertex2f(24,46);
        glVertex2f(24,47);
        glEnd();
        glColor3ub(0,80,80);
        glBegin(GL_POLYGON);
        glVertex2f(10,45);
        glVertex2f(10,44);
        glVertex2f(24,44);
        glVertex2f(24,45);
        glEnd();
        glColor3ub(0,80,80);
        glBegin(GL_POLYGON);
        glVertex2f(10,43);
        glVertex2f(10,42);
        glVertex2f(24,42);
        glVertex2f(24,43);
        glEnd();
        glColor3ub(0,183,183);
        glBegin(GL_POLYGON);
        glVertex2f(11,40);
        glVertex2f(11,58);
        glVertex2f(12,58);
        glVertex2f(12,40);
        glEnd();
         glColor3ub(0,183,183);
        glBegin(GL_POLYGON);
        glVertex2f(13,40);
        glVertex2f(13,58);
        glVertex2f(14,58);
        glVertex2f(14,40);
        glEnd();
         glColor3ub(0,183,183);
        glBegin(GL_POLYGON);
        glVertex2f(15,40);
        glVertex2f(15,58);
        glVertex2f(16,58);
        glVertex2f(16,40);
        glEnd();
        glColor3ub(0,183,183);
        glBegin(GL_POLYGON);
        glVertex2f(17,40);
        glVertex2f(17,58);
        glVertex2f(18,58);
        glVertex2f(18,40);
        glEnd();
        glColor3ub(0,183,183);
        glBegin(GL_POLYGON);
        glVertex2f(19,40);
        glVertex2f(19,58);
        glVertex2f(20,58);
        glVertex2f(20,40);
        glEnd();
         glColor3ub(0,183,183);
        glBegin(GL_POLYGON);
        glVertex2f(21,40);
        glVertex2f(21,58);
        glVertex2f(22,58);
        glVertex2f(22,40);
        glEnd();

    //building 3 (second one)
    glColor3ub(211,41,90);
        glBegin(GL_POLYGON);
        glVertex2f(55,40);
        glVertex2f(55,65);
        glVertex2f(72,65);
        glVertex2f(72,40);
        glEnd();
        glColor3ub(0,0,102);
        glBegin(GL_POLYGON);
        glVertex2f(57,43);
        glVertex2f(57,62);
        glVertex2f(72,62);
        glVertex2f(72,43);
        glEnd();
        glColor3ub(211,41,90);
        glBegin(GL_POLYGON);
        glVertex2f(59,43);
        glVertex2f(59,62);
        glVertex2f(60,62);
        glVertex2f(60,43);
        glEnd();
         glColor3ub(211,41,90);
        glBegin(GL_POLYGON);
        glVertex2f(62,43);
        glVertex2f(62,62);
        glVertex2f(63,62);
        glVertex2f(63,43);
        glEnd();
        glColor3ub(211,41,90);
        glBegin(GL_POLYGON);
        glVertex2f(65,43);
        glVertex2f(65,62);
        glVertex2f(66,62);
        glVertex2f(66,43);
        glEnd();
        glColor3ub(211,41,90);
        glBegin(GL_POLYGON);
        glVertex2f(68,43);
        glVertex2f(68,62);
        glVertex2f(69,62);
        glVertex2f(69,43);
        glEnd();
         glColor3ub(211,41,90);
        glBegin(GL_POLYGON);
        glVertex2f(71,43);
        glVertex2f(71,62);
        glVertex2f(72,62);
        glVertex2f(72,43);
        glEnd();
       glColor3ub(211,41,90);
        glBegin(GL_POLYGON);
        glVertex2f(57,58);
        glVertex2f(57,60);
        glVertex2f(72,60);
        glVertex2f(72,58);
        glEnd();
         glColor3ub(211,41,90);
        glBegin(GL_POLYGON);
        glVertex2f(57,56);
        glVertex2f(57,54);
        glVertex2f(72,54);
        glVertex2f(72,56);
        glEnd();
          glColor3ub(211,41,90);
        glBegin(GL_POLYGON);
        glVertex2f(57,52);
        glVertex2f(57,50);
        glVertex2f(72,50);
        glVertex2f(72,52);
        glEnd();
         glColor3ub(211,41,90);
        glBegin(GL_POLYGON);
        glVertex2f(57,48);
        glVertex2f(57,46);
        glVertex2f(72,46);
        glVertex2f(72,48);
        glEnd();
        glColor3ub(211,41,90);
        glBegin(GL_POLYGON);
        glVertex2f(57,44);
        glVertex2f(57,42);
        glVertex2f(72,42);
        glVertex2f(72,44);
        glEnd();

    // building5
        glColor3ub(25,111,61);
        glBegin(GL_POLYGON);
        glVertex2f(77,40);
        glVertex2f(77,68);
        glVertex2f(86.5,68);
        glVertex2f(86.5,40);
        glEnd();
        glColor3ub(130,224,170);
        glBegin(GL_POLYGON);
        glVertex2f(78,44);
        glVertex2f(78,66);
        glVertex2f(85.5,66);
        glVertex2f(85.5,44);
        glEnd();
       glColor3ub(25,111,61);
        glBegin(GL_POLYGON);
        glVertex2f(80.3,44);
        glVertex2f(80.3,66);
        glVertex2f(80.5,66);
        glVertex2f(80.5,44);
        glEnd();
         glColor3ub(25,111,61);
        glBegin(GL_POLYGON);
        glVertex2f(82.8,44);
        glVertex2f(82.8,66);
        glVertex2f(83,66);
        glVertex2f(83,44);
        glEnd();
        glColor3ub(25,111,61);
        glBegin(GL_POLYGON);
        glVertex2f(78,63.5);
        glVertex2f(78,63.7);
        glVertex2f(86,63.7);
        glVertex2f(86,63.5);
        glEnd();
        glColor3ub(25,111,61);
        glBegin(GL_POLYGON);
        glVertex2f(78,61.1);
        glVertex2f(78,61.3);
        glVertex2f(86,61.3);
        glVertex2f(86,61.1);
        glEnd();
        glColor3ub(25,111,61);
        glBegin(GL_POLYGON);
        glVertex2f(78,58.6);
        glVertex2f(78,58.8);
        glVertex2f(86,58.8);
        glVertex2f(86,58.6);
        glEnd();
         glColor3ub(25,111,61);
        glBegin(GL_POLYGON);
        glVertex2f(78,56.1);
        glVertex2f(78,56.3);
        glVertex2f(86,56.3);
        glVertex2f(86,56.1);
        glEnd();
        glColor3ub(25,111,61);
        glBegin(GL_POLYGON);
        glVertex2f(78,53.6);
        glVertex2f(78,53.8);
        glVertex2f(86,53.8);
        glVertex2f(86,53.6);
        glEnd();
        glColor3ub(25,111,61);
        glBegin(GL_POLYGON);
        glVertex2f(78,51.1);
        glVertex2f(78,51.3);
        glVertex2f(86,51.3);
        glVertex2f(86,51.1);
        glEnd();
        glColor3ub(25,111,61);
        glBegin(GL_POLYGON);
        glVertex2f(78,48.6);
        glVertex2f(78,48.8);
        glVertex2f(86,48.8);
        glVertex2f(86,48.6);
        glEnd();
        glColor3ub(25,111,61);
        glBegin(GL_POLYGON);
        glVertex2f(78,46.1);
        glVertex2f(78,46.3);
        glVertex2f(86,46.3);
        glVertex2f(86,46.1);
        glEnd();

    //building4
    glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(71,40);
        glVertex2f(71,58);
        glVertex2f(81,58);
        glVertex2f(81,40);
        glEnd();
       glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(71,58);
        glVertex2f(71,61);
        glVertex2f(81,61);
        glVertex2f(81,58);
        glEnd();
        glColor3ub(148,119,64);
        glBegin(GL_POLYGON);
        glVertex2f(72,43);
        glVertex2f(72,58);
        glVertex2f(80,58);
        glVertex2f(80,43);
        glEnd();
        glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(72,55);
        glVertex2f(72,56);
        glVertex2f(80,56);
        glVertex2f(80,55);
        glEnd();
         glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(72,53.5);
        glVertex2f(72,54);
        glVertex2f(80,54);
        glVertex2f(80,53.5);
        glEnd();
         glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(72,52);
        glVertex2f(72,52.5);
        glVertex2f(80,52.5);
        glVertex2f(80,52);
        glEnd();
        glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(72,50.5);
        glVertex2f(72,51);
        glVertex2f(80,51);
        glVertex2f(80,50.5);
        glEnd();
        glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(72,49);
        glVertex2f(72,49.5);
        glVertex2f(80,49.5);
        glVertex2f(80,49);
        glEnd();
         glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(72,47.5);
        glVertex2f(72,48);
        glVertex2f(80,48);
        glVertex2f(80,47.5);
        glEnd();
        glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(72,46);
        glVertex2f(72,46.5);
        glVertex2f(80,46.5);
        glVertex2f(80,46);
        glEnd();
         glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(72,44.5);
        glVertex2f(72,45);
        glVertex2f(80,45);
        glVertex2f(80,44.5);
        glEnd();
         glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(74,43);
        glVertex2f(74,58);
        glVertex2f(74.5,58);
        glVertex2f(74.5,43);
        glEnd();
       glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(77,43);
        glVertex2f(77,58);
        glVertex2f(77.5,58);
        glVertex2f(77.5,43);
        glEnd();
        glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(72,59);
        glVertex2f(72,60);
        glVertex2f(80,60);
        glVertex2f(80,59);
        glEnd();
         glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(73,58);
        glVertex2f(73,61);
        glVertex2f(73.5,61);
        glVertex2f(73.5,58);
        glEnd();
       glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(74.5,58);
        glVertex2f(74.5,61);
        glVertex2f(75,61);
        glVertex2f(75,58);
        glEnd();
          glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(76,58);
        glVertex2f(76,61);
        glVertex2f(76.5,61);
        glVertex2f(76.5,58);
        glEnd();
        glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(77.5,58);
        glVertex2f(77.5,61);
        glVertex2f(78,61);
        glVertex2f(78,58);
        glEnd();
         glColor3ub(247,220,111);
        glBegin(GL_POLYGON);
        glVertex2f(78.9,58);
        glVertex2f(78.9,61);
        glVertex2f(79.1,61);
        glVertex2f(79.1,58);
        glEnd();

    //building6
        glColor3ub(178,90,0);
        glBegin(GL_POLYGON);
        glVertex2f(85,40);
        glVertex2f(85,61);
        glVertex2f(99,61);
        glVertex2f(99,40);
        glEnd();
         glColor3ub(108,0,0);
        glBegin(GL_POLYGON);
        glVertex2f(86,42);
        glVertex2f(86,59);
        glVertex2f(98,59);
        glVertex2f(98,42);
        glEnd();
        glColor3ub(178,90,0);
        glBegin(GL_POLYGON);
        glVertex2f(86,56.5);
        glVertex2f(86,57);
        glVertex2f(98,57);
        glVertex2f(98,56.5);
        glEnd();
        glColor3ub(178,90,0);
        glBegin(GL_POLYGON);
        glVertex2f(86,54);
        glVertex2f(86,54.5);
        glVertex2f(98,54.5);
        glVertex2f(98,54);
        glEnd();
        glColor3ub(178,90,0);
        glBegin(GL_POLYGON);
        glVertex2f(86,51.5);
        glVertex2f(86,52);
        glVertex2f(98,52);
        glVertex2f(98,51.5);
        glEnd();
         glColor3ub(178,90,0);
        glBegin(GL_POLYGON);
        glVertex2f(86,49);
        glVertex2f(86,49.5);
        glVertex2f(98,49.5);
        glVertex2f(98,49);
        glEnd();
        glColor3ub(178,90,0);
        glBegin(GL_POLYGON);
        glVertex2f(86,46.5);
        glVertex2f(86,47);
        glVertex2f(98,47);
        glVertex2f(98,46.5);
        glEnd();
        glColor3ub(178,90,0);
        glBegin(GL_POLYGON);
        glVertex2f(86,44);
        glVertex2f(86,44.5);
        glVertex2f(98,44.5);
        glVertex2f(98,44);
        glEnd();
        glColor3ub(178,90,0);
        glBegin(GL_POLYGON);
        glVertex2f(88,42);
        glVertex2f(88,59);
        glVertex2f(88.5,59);
        glVertex2f(88.5,42);
        glEnd();
         glColor3ub(178,90,0);
        glBegin(GL_POLYGON);
        glVertex2f(90.5,42);
        glVertex2f(90.5,59);
        glVertex2f(91,59);
        glVertex2f(91,42);
        glEnd();
        glColor3ub(178,90,0);
        glBegin(GL_POLYGON);
        glVertex2f(93,42);
        glVertex2f(93,59);
        glVertex2f(93.5,59);
        glVertex2f(93.5,42);
        glEnd();
        glColor3ub(178,90,0);
        glBegin(GL_POLYGON);
        glVertex2f(95.5,42);
        glVertex2f(95.5,59);
        glVertex2f(96,59);
        glVertex2f(96,42);
        glEnd();

    glFlush();
}
void drawTrees() {
    if (currentSeason == WINTER) {
        // Base of Tree 1
        glColor3ub(83, 53, 10); // Darker brown for tree trunk
        glBegin(GL_POLYGON);
        glVertex2f(15,40);
        glVertex2f(15,42);
        glVertex2f(16,42);
        glVertex2f(16,40);
        glEnd();

        // Snow on Tree 1
        glColor3f(1.0, 1.0, 1.0); // White for snow
        glBegin(GL_POLYGON);
        glVertex2f(12,44);
        glVertex2f(15.5,47);
        glVertex2f(19,44);
        glEnd();

        // Base of Tree 2
        glColor3ub(83, 53, 10); // Darker brown for tree trunk
        glBegin(GL_POLYGON);
        glVertex2f(27,40);
        glVertex2f(27,42);
        glVertex2f(28,42);
        glVertex2f(28,40);
        glEnd();

        // Snow on Tree 2
        glColor3f(1.0, 1.0, 1.0); // White for snow
        glBegin(GL_POLYGON);
        glVertex2f(24,44);
        glVertex2f(27.5,47);
        glVertex2f(31,44);
        glEnd();

        // Base of Tree 3
        glColor3ub(83, 53, 10); // Darker brown for tree trunk
        glBegin(GL_POLYGON);
        glVertex2f(61,40);
        glVertex2f(61,42);
        glVertex2f(62,42);
        glVertex2f(62,40);
        glEnd();

        // Snow on Tree 3
        glColor3f(1.0, 1.0, 1.0); // White for snow
        glBegin(GL_POLYGON);
        glVertex2f(58,44);
        glVertex2f(61.5,47);
        glVertex2f(65,44);
        glEnd();

        // Base of Tree 4
        glColor3ub(83, 53, 10); // Darker brown for tree trunk
        glBegin(GL_POLYGON);
        glVertex2f(75,40);
        glVertex2f(75,42);
        glVertex2f(76,42);
        glVertex2f(76,40);
        glEnd();

        // Snow on Tree 4
        glColor3f(1.0, 1.0, 1.0); // White for snow
        glBegin(GL_POLYGON);
        glVertex2f(72,44);
        glVertex2f(75.5,47);
        glVertex2f(79,44);
        glEnd();

        // Base of Tree 5
        glColor3ub(83, 53, 10); // Darker brown for tree trunk
        glBegin(GL_POLYGON);
        glVertex2f(90,40);
        glVertex2f(90,42);
        glVertex2f(91,42);
        glVertex2f(91,40);
        glEnd();

        // Snow on Tree 5
        glColor3f(1.0, 1.0, 1.0); // White for snow
        glBegin(GL_POLYGON);
        glVertex2f(87,44);
        glVertex2f(90.5,47);
        glVertex2f(94,44);
        glEnd();

    }
    else if (currentSeason == AUTUMN) {
        // Trunk for Tree 1
        glColor3ub(123, 36, 28);
        glBegin(GL_POLYGON);
        glVertex2f(15,40);
        glVertex2f(15,42);
        glVertex2f(16,42);
        glVertex2f(16,40);
        glEnd();

        // Autumn Leaves for Tree 1 (Orange)
        glColor3ub(210, 105, 30);
        glBegin(GL_POLYGON);
        glVertex2f(12,42);
        glVertex2f(15.5,45);
        glVertex2f(19,42);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(12,43);
        glVertex2f(15.5,46);
        glVertex2f(19,43);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(12,44);
        glVertex2f(15.5,47);
        glVertex2f(19,44);
        glEnd();

        // Trunk for Tree 2
        glColor3ub(123, 36, 28);
        glBegin(GL_POLYGON);
        glVertex2f(27,40);
        glVertex2f(27,42);
        glVertex2f(28,42);
        glVertex2f(28,40);
        glEnd();

        // Autumn Leaves for Tree 2 (Red)
        glColor3ub(180, 40, 40);
        glBegin(GL_POLYGON);
        glVertex2f(24,42);
        glVertex2f(27.5,44);
        glVertex2f(31,42);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(24,43);
        glVertex2f(27.5,46);
        glVertex2f(31,43);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(24,44);
        glVertex2f(27.5,47);
        glVertex2f(31,44);
        glEnd();

        // Trunk for Tree 3
        glColor3ub(123, 36, 28);
        glBegin(GL_POLYGON);
        glVertex2f(61,40);
        glVertex2f(61,42);
        glVertex2f(62,42);
        glVertex2f(62,40);
        glEnd();

        // Autumn Leaves for Tree 3 (Orange)
        glColor3ub(210, 105, 30);
        glBegin(GL_POLYGON);
        glVertex2f(58,42);
        glVertex2f(61.5,45);
        glVertex2f(65,42);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(58,43);
        glVertex2f(61.5,46);
        glVertex2f(65,43);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(58,44);
        glVertex2f(61.5,47);
        glVertex2f(65,44);
        glEnd();

        // Trunk for Tree 4
        glColor3ub(123, 36, 28);
        glBegin(GL_POLYGON);
        glVertex2f(75,40);
        glVertex2f(75,42);
        glVertex2f(76,42);
        glVertex2f(76,40);
        glEnd();

        // Autumn Leaves for Tree 4 (Red)
        glColor3ub(180, 40, 40);
        glBegin(GL_POLYGON);
        glVertex2f(72,42);
        glVertex2f(75.5,45);
        glVertex2f(79,42);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(72,43);
        glVertex2f(75.5,46);
        glVertex2f(79,43);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(72,44);
        glVertex2f(75.5,47);
        glVertex2f(79,44);
        glEnd();

        // Trunk for Tree 5
        glColor3ub(123, 36, 28);
        glBegin(GL_POLYGON);
        glVertex2f(90,40);
        glVertex2f(90,42);
        glVertex2f(91,42);
        glVertex2f(91,40);
        glEnd();

        // Autumn Leaves for Tree 5 (Orange)
        glColor3ub(210, 105, 30);
        glBegin(GL_POLYGON);
        glVertex2f(87,42);
        glVertex2f(90.5,45);
        glVertex2f(94,42);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(87,43);
        glVertex2f(90.5,46);
        glVertex2f(94,43);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(87,44);
        glVertex2f(90.5,47);
        glVertex2f(94,44);
        glEnd();

    }
    else { // This block is for EVENING, NIGHT, MORNING, and SPRING
    ///tree1
        glColor3ub(123, 36, 28);
        glBegin(GL_POLYGON);
        glVertex2f(15,40);
        glVertex2f(15,42);
        glVertex2f(16,42);
        glVertex2f(16,40);
        glEnd();

        if (currentSeason == SPRING) {
            glColor3ub(100,220,100);
        } else {
            glColor3ub(30,132,73); // Brighter green for spring
        }
        
        glBegin(GL_POLYGON);
        glVertex2f(12,42);
        glVertex2f(15.5,45);
        glVertex2f(19,42);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(12,43);
        glVertex2f(15.5,46);
        glVertex2f(19,43);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(12,44);
        glVertex2f(15.5,47);
        glVertex2f(19,44);
        glEnd();
        
        if (currentSeason == SPRING) {
            glColor3ub(255, 182, 193);
            glPointSize(5.0);
            glBegin(GL_POINTS);
            glVertex2f(14, 44);
            glVertex2f(16, 45);
            glVertex2f(17, 43);
            glEnd();
            glPointSize(1.0);
        }

    ///tree2
        glColor3ub(123,36,28);
        glBegin(GL_POLYGON);
        glVertex2f(27,40);
        glVertex2f(27,42);
        glVertex2f(28,42);
        glVertex2f(28,40);
        glEnd();
        
        if (currentSeason == SPRING) {
            glColor3ub(100,220,100);
        } else {
            glColor3ub(30,132,73);
        }

        glBegin(GL_POLYGON);
        glVertex2f(24,44);
        glVertex2f(27.5,47);
        glVertex2f(31,44);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(24,43);
        glVertex2f(27.5,46);
        glVertex2f(31,43);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(24,42);
        glVertex2f(27.5,44);
        glVertex2f(31,42);
        glEnd();

    ///tree 3
        glColor3ub(123,36,28);
        glBegin(GL_POLYGON);
        glVertex2f(61,40);
        glVertex2f(61,42);
        glVertex2f(62,42);
        glVertex2f(62,40);
        glEnd();

        if (currentSeason == SPRING) {
            glColor3ub(100,220,100);
        } else {
            glColor3ub(30,132,73);
        }

        glBegin(GL_POLYGON);
        glVertex2f(58,44);
        glVertex2f(61.5,47);
        glVertex2f(65,44);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(58,43);
        glVertex2f(61.5,46);
        glVertex2f(65,43);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(58,42);
        glVertex2f(61.5,45);
        glVertex2f(65,42);
        glEnd();

        if (currentSeason == SPRING) {
            glColor3ub(255, 182, 193);
            glPointSize(5.0);
            glBegin(GL_POINTS);
            glVertex2f(60, 45);
            glVertex2f(63, 46);
            glVertex2f(64, 44);
            glEnd();
            glPointSize(1.0);
        }

    ///tree 4
        glColor3ub(123,36,28);
        glBegin(GL_POLYGON);
        glVertex2f(75,40);
        glVertex2f(75,42);
        glVertex2f(76,42);
        glVertex2f(76,40);
        glEnd();

        if (currentSeason == SPRING) {
            glColor3ub(100,220,100);
        } else {
            glColor3ub(30,132,73);
        }

        glBegin(GL_POLYGON);
        glVertex2f(72,44);
        glVertex2f(75.5,47);
        glVertex2f(79,44);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(72,43);
        glVertex2f(75.5,46);
        glVertex2f(79,43);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(72,42);
        glVertex2f(75.5,45);
        glVertex2f(79,42);
        glEnd();

    ///tree5
        glColor3ub(123,36,28);
        glBegin(GL_POLYGON);
        glVertex2f(90,40);
        glVertex2f(90,42);
        glVertex2f(91,42);
        glVertex2f(91,40);
        glEnd();

        if (currentSeason == SPRING) {
            glColor3ub(100,220,100);
        } else {
            glColor3ub(30,132,73);
        }

        glBegin(GL_POLYGON);
        glVertex2f(87,44);
        glVertex2f(90.5,47);
        glVertex2f(94,44);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(87,43);
        glVertex2f(90.5,46);
        glVertex2f(94,43);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(87,42);
        glVertex2f(90.5,45);
        glVertex2f(94,42);
        glEnd();

        if (currentSeason == SPRING) {
            glColor3ub(255, 182, 193);
            glPointSize(5.0);
            glBegin(GL_POINTS);
            glVertex2f(88, 45);
            glVertex2f(91, 46);
            glVertex2f(93, 44);
            glEnd();
            glPointSize(1.0);
        }

    }
}
void drawRoad() {
    if (currentSeason == WINTER) {
        glColor3ub(80, 80, 90);
    } else {
        glColor3ub(50, 50, 50);
    }
    glBegin(GL_QUADS);
    glVertex2f(0, 34);
    glVertex2f(100, 34);
    glVertex2f(100, 40);
    glVertex2f(0, 40);
    glEnd();
    
    /// Road Code (Main Road)
    glColor3ub(50, 50, 50);
    glBegin(GL_QUADS);
    glVertex2f(0, 35);
    glVertex2f(100, 35);
    glVertex2f(100, 40);
    glVertex2f(0, 40);
    glEnd();

    /// Road Divider
    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(-2, 37.35);
    glVertex2f(3, 37.35);
    glVertex2f(3, 37.65);
    glVertex2f(-2, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(5, 37.35);
    glVertex2f(10, 37.35);
    glVertex2f(10, 37.65);
    glVertex2f(5, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(12, 37.35);
    glVertex2f(17, 37.35);
    glVertex2f(17, 37.65);
    glVertex2f(12, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(19, 37.35);
    glVertex2f(24, 37.35);
    glVertex2f(24, 37.65);
    glVertex2f(19, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(26, 37.35);
    glVertex2f(31, 37.35);
    glVertex2f(31, 37.65);
    glVertex2f(26, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(33, 37.35);
    glVertex2f(38, 37.35);
    glVertex2f(38, 37.65);
    glVertex2f(33, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(40, 37.35);
    glVertex2f(45, 37.35);
    glVertex2f(45, 37.65);
    glVertex2f(40, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(47, 37.35);
    glVertex2f(52, 37.35);
    glVertex2f(52, 37.65);
    glVertex2f(47, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(54, 37.35);
    glVertex2f(59, 37.35);
    glVertex2f(59, 37.65);
    glVertex2f(54, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(61, 37.35);
    glVertex2f(66, 37.35);
    glVertex2f(66, 37.65);
    glVertex2f(61, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(68, 37.35);
    glVertex2f(73, 37.35);
    glVertex2f(73, 37.65);
    glVertex2f(68, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(75, 37.35);
    glVertex2f(80, 37.35);
    glVertex2f(80, 37.65);
    glVertex2f(75, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(82, 37.35);
    glVertex2f(87, 37.35);
    glVertex2f(87, 37.65);
    glVertex2f(82, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(89, 37.35);
    glVertex2f(94, 37.35);
    glVertex2f(94, 37.65);
    glVertex2f(89, 37.65);
    glEnd();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(96, 37.35);
    glVertex2f(101, 37.35);
    glVertex2f(101, 37.65);
    glVertex2f(96, 37.65);
    glEnd();

    /// Road Border
    glColor3ub(100, 100, 100);
    glBegin(GL_QUADS);
    glVertex2f(0, 33);
    glVertex2f(100, 33);
    glVertex2f(100, 34);
    glVertex2f(0, 34);
    glEnd();

    /// Pole Lines (Additional Road Markers)
    glColor3ub(100, 100, 100);
    glBegin(GL_QUADS);
    glVertex2f(0, 35.25);
    glVertex2f(100, 35.25);
    glVertex2f(100, 35.5);
    glVertex2f(0, 35.5);
    glEnd();

    for (float x = 2.0; x < 100; x += 5) {
        glColor3ub(100, 100, 100);
        glBegin(GL_QUADS);
        glVertex2f(x, 34);
        glVertex2f(x + 0.25, 34);
        glVertex2f(x + 0.25, 35.25);
        glVertex2f(x, 35.25);
        glEnd();
    }
    
}
void drawCars() {
    ///cars
    ///right to left
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(_movecar, 0.0f, 0.0f);
    glColor3f(0.1373, 0.5725, 0.5529);
    glBegin(GL_QUADS);
    glVertex2f(60, 38.5);
    glVertex2f(68, 38.5);
    glVertex2f(68, 40);
    glVertex2f(60, 40);
    glEnd();

    glColor3f(0.1373, 0.5725, 0.5529);
    glBegin(GL_QUADS);
    glVertex2f(61, 40);
    glVertex2f(68, 40);
    glVertex2f(67, 41.5);
    glVertex2f(63, 41.5);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.7f, 0.8f, 1.0f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(61.5, 40);
    glVertex2f(67.7, 40);
    glVertex2f(66.7, 41.2);
    glVertex2f(63.2, 41.2);
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(0.1373, 0.5725, 0.5529);
    glBegin(GL_QUADS);
    glVertex2f(65.5, 40);
    glVertex2f(65.8, 40);
    glVertex2f(65.8, 41.5);
    glVertex2f(65.5, 41.5);
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i < 200; i++)
    {
        glColor3f(0.4, 0.4, 0.4);
        float pi = 3.1416;
        float A = (i * 2 * pi) / 200;
        float r = .75;
        float x = r * cos(A);
        float y = r * sin(A);
        glVertex2f(x + 62.25, y + 38.5);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i < 200; i++)
    {
        glColor3f(0.4, 0.4, 0.4);
        float pi = 3.1416;
        float A = (i * 2 * pi) / 200;
        float r = .75;
        float x = r * cos(A);
        float y = r * sin(A);
        glVertex2f(x + 66.2, y + 38.5);
    }
    glEnd();
    
    if (currentSeason == NIGHT) {
        // Draw Headlights for car 1
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4ub(255, 255, 200, 150);
        glBegin(GL_TRIANGLES);
        glVertex2f(60, 39.5);
        glVertex2f(55, 38.5);
        glVertex2f(55, 40.5);
        glEnd();
        glDisable(GL_BLEND);
    }
    glPopMatrix();
    
    ///left to right
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(_movecar2, 0.0f, 0.0f);
    glColor3ub(196, 30, 58);
    glBegin(GL_QUADS);
    glVertex2f(9, 36);
    glVertex2f(17, 36);
    glVertex2f(16.6, 37.5);
    glVertex2f(9, 37.5);
    glEnd();

    glColor3ub(196, 30, 58);
    glBegin(GL_QUADS);
    glVertex2f(10, 37.5);
    glVertex2f(15.5, 37.5);
    glVertex2f(14, 38.6);
    glVertex2f(11.2, 38.6);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.7f, 0.8f, 1.0f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(10.3, 37.5);
    glVertex2f(15.1, 37.5);
    glVertex2f(13.7, 38.4);
    glVertex2f(11.4, 38.4);
    glEnd();
    glDisable(GL_BLEND);

    glBegin(GL_POLYGON);
    for (int i = 0; i < 200; i++)
    {
        glColor3f(0.4, 0.4, 0.4);
        float pi = 3.1416;
        float A = (i * 2 * pi) / 200;
        float r = .75;
        float x = r * cos(A);
        float y = r * sin(A);
        glVertex2f(x + 11, y + 36);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i < 200; i++)
    {
        glColor3f(0.4, 0.4, 0.4);
        float pi = 3.1416;
        float A = (i * 2 * pi) / 200;
        float r = .75;
        float x = r * cos(A);
        float y = r * sin(A);
        glVertex2f(x + 15, y + 36);
    }
    glEnd();

    if (currentSeason == NIGHT) {
        // Draw Headlights for car 2
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4ub(255, 255, 200, 150);
        glBegin(GL_TRIANGLES);
        glVertex2f(17, 37);
        glVertex2f(22, 36);
        glVertex2f(22, 38);
        glEnd();
        glDisable(GL_BLEND);
    }
    glPopMatrix();
}
void drawRiverAndWaves() {
    ///river
    // Make the river look colder in winter
    if (currentSeason == WINTER) {
        glColor3ub(100, 110, 130);
        glBegin(GL_QUADS);
        glVertex2f(0.0, 0.0);
        glVertex2f(100, 0);
        glVertex2f(100, 33);
        glVertex2f(0, 33);
        glEnd();
        
    } else {
        ///shade1
        if(currentSeason == NIGHT) {
            glColor3ub(153, 153, 255);
            glBegin(GL_QUADS);
            glVertex2f(0.0, 0.0);
            glColor3ub(102, 153, 255);
            glVertex2f(100, 0);
            glVertex2f(100, 33);
            glColor3ub(102, 153, 255);
            glVertex2f(0, 33);
            glEnd();
        } else if (currentSeason == MORNING) {
            glColor3ub(113, 218, 218);
            glBegin(GL_QUADS);
            glVertex2f(0.0, 0.0);
            glVertex2f(100, 0);
            glVertex2f(100, 33);
            glVertex2f(0, 33);
            glEnd();
        } else { // Evening
            glColor3ub(128.0f, 204.0f, 255.0f);
            glBegin(GL_QUADS);
            glVertex2f(0.0, 0.0);
            glColor3ub(26.0f, 163.0f, 255.0f);
            glVertex2f(100, 0);
            glVertex2f(100, 33);
            glColor3ub(220.0f, 220.0f, 220.0f);
            glVertex2f(0, 33);
            glEnd();
        }
        
        ///wave
        glLoadIdentity();
        glPushMatrix();
        glTranslatef(_move8, 0.0f, 0.0f);
        glColor3ub(243.0f, 235.0f, 208.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(5, 25);
        glVertex2f(19, 25);
        glVertex2f(12, 26);
        glEnd();
        
        glColor3ub(243.0f, 235.0f, 208.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(30, 25);
        glVertex2f(40, 25);
        glVertex2f(35, 26);
        glEnd();
        
        glColor3ub(243.0f, 235.0f, 208.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(70, 25);
        glVertex2f(85, 25);
        glVertex2f(78, 26);
        glEnd();
        glPopMatrix();
        
        ///wave2
        glLoadIdentity();
        glPushMatrix();
        glTranslatef(_move8, 0.0f, 0.0f);
        glColor3ub(243.0f, 235.0f, 208.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(50, 20);
        glVertex2f(60, 20);
        glVertex2f(55, 21);
        glEnd();
        
        glColor3ub(243.0f, 235.0f, 208.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(30.0f, 20);
        glVertex2f(37.0f, 20);
        glVertex2f(34, 21);
        glEnd();
        
        glColor3ub(243.0f, 235.0f, 208.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(80, 20);
        glVertex2f(90, 20);
        glVertex2f(85, 21);
        glEnd();
        glPopMatrix();
        
        ///wave3
        glLoadIdentity();
        glPushMatrix();
        glTranslatef(_move8, 0.0f, 0.0f);
        glColor3ub(243.0f, 235.0f, 208.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(25, 30);
        glVertex2f(35, 30);
        glVertex2f(30, 31);
        glEnd();
        
        glColor3ub(243.0f, 235.0f, 208.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(75, 30);
        glVertex2f(85, 30);
        glVertex2f(80, 31);
        glEnd();
        glPopMatrix();
    }
}
void drawBoat() {
    ///boat
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(_move5, _moveM, 0.0f);

    glColor3ub(27.0f, 27.0f, 60.0f);
    glBegin(GL_POLYGON);
    glVertex2f(60, 22);
    glVertex2f(62, 19);
    glVertex2f(73, 19);
    glVertex2f(75, 22);
    glEnd();

    glColor3ub(180.0f, 100.0f, 30.0f);
    glBegin(GL_POLYGON);
    glVertex2f(68, 22);
    glVertex2f(69, 22.5);
    glVertex2f(68.5, 28.25);
    glEnd();

    glColor3ub(245.0f, 240.0f, 180.0f);
    glBegin(GL_POLYGON);
    glVertex2f(68.5, 22.25);
    glVertex2f(68.5, 28.25);
    glVertex2f(75, 25);
    glEnd();

    glPopMatrix();
}
void drawHelicopter() {
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(heliX, heliY, 0.0f);

    glScalef(12.0, 12.0, 1.0);
    glScalef(-1.0, 1.0, 1.0);
    // --- Helicopter Body  ---
    glColor3f(0.2f, 0.2f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(-0.15f, -0.05f);
        glVertex2f(0.15f, -0.05f);
        glVertex2f(0.2f, 0.05f);
        glVertex2f(-0.2f, 0.05f);
    glEnd();

    // --- Window ---
    glColor3f(0.5f, 0.8f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(-0.18f, 0.0f);
        glVertex2f(-0.05f, 0.0f);
        glVertex2f(-0.03f, 0.04f);
        glVertex2f(-0.18f, 0.04f);
    glEnd();

    // --- Tail ---
    glColor3f(0.3f, 0.3f, 0.6f);
    glBegin(GL_QUADS);
        glVertex2f(0.15f, -0.02f);
        glVertex2f(0.4f, -0.01f);
        glVertex2f(0.4f, 0.01f);
        glVertex2f(0.15f, 0.02f);
    glEnd();

    // --- Tail Rotor  ---
    glPushMatrix();
    glTranslatef(0.4f, 0.0f, 0.0f);
    glRotatef(bladeRotate * 2.0f, 0.0f, 0.0f, 1.0f);
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(2.0);
    glBegin(GL_LINES);
        glVertex2f(0.0f, -0.04f); glVertex2f(0.0f, 0.04f);
        glVertex2f(-0.04f, 0.0f); glVertex2f(0.04f, 0.0f);
    glEnd();
    glPopMatrix();

    // --- Landing Skids ---
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
        glVertex2f(-0.1f, -0.1f);
        glVertex2f(0.1f, -0.1f);
        glVertex2f(0.1f, -0.08f);
        glVertex2f(-0.1f, -0.08f);
    glEnd();
    glLineWidth(2.0);
    glBegin(GL_LINES);
        glVertex2f(-0.08f, -0.05f); glVertex2f(-0.08f, -0.08f);
        glVertex2f(0.08f, -0.05f);  glVertex2f(0.08f, -0.08f);
    glEnd();

    // --- Top Rotor ---
    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.0f);
    glRotatef(bladeRotate, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.6f, 0.6f, 0.6f, 0.6f);
    glBegin(GL_QUADS);
        glVertex2f(-0.3f, -0.01f);
        glVertex2f(0.3f, -0.01f);
        glVertex2f(0.3f, 0.01f);
        glVertex2f(-0.3f, 0.01f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(-0.01f, -0.3f);
        glVertex2f(0.01f, -0.3f);
        glVertex2f(0.01f, 0.3f);
        glVertex2f(-0.01f, 0.3f);
    glEnd();
    glDisable(GL_BLEND);
    glPopMatrix();

    glPopMatrix();
}
void drawParkElements() {
    //lower railing
    glColor3ub(1, 1, 1);
    glBegin(GL_POLYGON);
    glVertex2f(0, 15);
    glVertex2f(100, 15);
    glVertex2f(100, 15.5);
    glVertex2f(0, 15.5);
    glEnd();

    glColor3ub(1, 1, 1);
    glBegin(GL_POLYGON);
    glVertex2f(0, 14);
    glVertex2f(100, 14);
    glVertex2f(100, 14.5);
    glVertex2f(0, 14.5);
    glEnd();

    glColor3ub(1, 1, 1);
    glBegin(GL_POLYGON);
    glVertex2f(0, 13);
    glVertex2f(100, 13);
    glVertex2f(100, 13.5);
    glVertex2f(0, 13.5);
    glEnd();

    glColor3ub(1, 1, 1);
    glBegin(GL_POLYGON);
    glVertex2f(0, 12);
    glVertex2f(100, 12);
    glVertex2f(100, 12.5);
    glVertex2f(0, 12.5);
    glEnd();
    // Stand
    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(0.5,12);
        glVertex2f(1,12);
        glVertex2f(1,16);
        glVertex2f(0.5,16);
    glEnd();

    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(10,12);
        glVertex2f(10.5,12);
        glVertex2f(10.5,16);
        glVertex2f(10,16);
    glEnd();

    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(20,12);
        glVertex2f(20.5,12);
        glVertex2f(20.5,16);
        glVertex2f(20,16);
    glEnd();

    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(30,12);
        glVertex2f(30.5,12);
        glVertex2f(30.5,16);
        glVertex2f(30,16);
    glEnd();

    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(40,12);
        glVertex2f(40.5,12);
        glVertex2f(40.5,16);
        glVertex2f(40,16);
    glEnd();

    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(50,12);
        glVertex2f(50.5,12);
        glVertex2f(50.5,16);
        glVertex2f(50,16);
    glEnd();

    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(60,12);
        glVertex2f(60.5,12);
        glVertex2f(60.5,16);
        glVertex2f(60,16);
    glEnd();

    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(70,12);
        glVertex2f(70.5,12);
        glVertex2f(70.5,16);
        glVertex2f(70,16);
    glEnd();

    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(80,12);
        glVertex2f(80.5,12);
        glVertex2f(80.5,16);
        glVertex2f(80,16);
    glEnd();

    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(90,12);
        glVertex2f(90.5,12);
        glVertex2f(90.5,16);
        glVertex2f(90,16);
    glEnd();

    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(99,12);
        glVertex2f(99.5,12);
        glVertex2f(99.5,16);
        glVertex2f(99,16);
    glEnd();

    
    //grass
    if (currentSeason == NIGHT) {
        glColor3ub(0, 150, 0);
    } else if (currentSeason == SPRING) {
        glColor3ub(60, 220, 60);
    } else {
        glColor3ub(50, 205, 50);
    }

    glBegin(GL_POLYGON);
    glVertex2f(0, 0);
    glVertex2f(100, 0);
    glVertex2f(100, 12);
    glVertex2f(0, 12);
    glEnd();

    // Bench
    glColor3ub(238,118,33);
    glBegin(GL_POLYGON);
        glVertex2f(5,12);
        glVertex2f(18,12);
        glVertex2f(18,13);
        glVertex2f(5,13);
    glEnd();

    glColor3ub(139,69,16);
    glBegin(GL_POLYGON);
        glVertex2f(5,13);
        glVertex2f(18,13);
        glVertex2f(18,13.25);
        glVertex2f(5,13.25);
    glEnd();

    glColor3ub(238,118,33);
    glBegin(GL_POLYGON);
        glVertex2f(5,13.25);
        glVertex2f(18,13.25);
        glVertex2f(18,14.25);
        glVertex2f(5,14.25);
    glEnd();

    glColor3ub(139,69,16);
    glBegin(GL_POLYGON);
        glVertex2f(5,14.25);
        glVertex2f(18,14.25);
        glVertex2f(18,14.5);
        glVertex2f(5,14.5);
    glEnd();

    glColor3ub(238,118,33);
    glBegin(GL_POLYGON);
        glVertex2f(5,14.5);
        glVertex2f(18,14.5);
        glVertex2f(18,15.5);
        glVertex2f(5,15.5);
    glEnd();

    glColor3ub(139,69,16);
    glBegin(GL_POLYGON);
        glVertex2f(5,15.5);
        glVertex2f(18,15.5);
        glVertex2f(18,15.75);
        glVertex2f(5,15.75);
    glEnd();

    glColor3ub(238,118,33);
    glBegin(GL_POLYGON);
        glVertex2f(5,15.75);
        glVertex2f(18,15.75);
        glVertex2f(18,16.75);
        glVertex2f(5,16.75);
    glEnd();

    glColor3ub(139,69,16);
    glBegin(GL_POLYGON);
        glVertex2f(5,12);
        glVertex2f(18,12);
        glVertex2f(18,12.25);
        glVertex2f(5,12.25);
    glEnd();

    // Sitting
    glColor3ub(238,118,33);
    glBegin(GL_POLYGON);
        glVertex2f(4.5,9.5);
        glVertex2f(18.5,9.5);
        glVertex2f(18,12);
        glVertex2f(5,12);
    glEnd();

    // Legs
    glColor3ub(56, 56, 56);
    glBegin(GL_POLYGON);
        glVertex2f(4.5,9);
        glVertex2f(18.5,9);
        glVertex2f(18.5,9.5);
        glVertex2f(4.5,9.5);
    glEnd();

    glColor3ub(56, 56, 56);
    glBegin(GL_POLYGON);
        glVertex2f(4.5,7.25);
        glVertex2f(5,7.25);
        glVertex2f(5,9.5);
        glVertex2f(4.5,9.5);
    glEnd();

    glColor3ub(56, 56, 56);
    glBegin(GL_POLYGON);
        glVertex2f(18,7.25);
        glVertex2f(18.5,7.25);
        glVertex2f(18.5,9.5);
        glVertex2f(18,9.5);
    glEnd();

    // Food Cart
    glColor3ub(139,69,19);
    glBegin(GL_POLYGON);
        glVertex2f(25,9);
        glVertex2f(45,9);
        glVertex2f(45,13);
        glVertex2f(25,13);
    glEnd();

    glColor3ub(204,102,0);
    glBegin(GL_POLYGON);
        glVertex2f(25,13);
        glVertex2f(45,13);
        glVertex2f(45,14);
        glVertex2f(25,14);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.7f, 0.8f, 1.0f, 0.85f);
    glBegin(GL_POLYGON);
    glVertex2f(25,14);
    glVertex2f(45,14);
    glVertex2f(45,20);
    glVertex2f(25,20);
    glEnd();
    glDisable(GL_BLEND);
    
    // Triangles
    glColor3ub(255, 101, 24);
    glBegin(GL_TRIANGLES);
        glVertex2f(25, 20);
        glVertex2f(27.5, 17.5);
        glVertex2f(30, 20);
    glEnd();

    glColor3ub(255, 255, 0);
    glBegin(GL_TRIANGLES);
        glVertex2f(30, 20);
        glVertex2f(32.5, 17.5);
        glVertex2f(35, 20);
    glEnd();

    glColor3ub(153, 51, 255);
    glBegin(GL_TRIANGLES);
        glVertex2f(35, 20);
        glVertex2f(37.5, 17.5);
        glVertex2f(40, 20);
    glEnd();

    glColor3ub(0, 255, 0);
    glBegin(GL_TRIANGLES);
        glVertex2f(40, 20);
        glVertex2f(42.5, 17.5);
        glVertex2f(45, 20);
    glEnd();

    // Upper Boxes
    glColor3ub(204, 204, 0);
    glBegin(GL_POLYGON);
        glVertex2f(30, 20);
        glVertex2f(35, 20);
        glVertex2f(35, 23);
        glVertex2f(30, 23);
    glEnd();

    glColor3ub(139, 0, 139);
    glBegin(GL_POLYGON);
        glVertex2f(35, 20);
        glVertex2f(40, 20);
        glVertex2f(40, 23);
        glVertex2f(35, 23);
    glEnd();

    glColor3ub(0, 100, 0);
    glBegin(GL_POLYGON);
        glVertex2f(40, 20);
        glVertex2f(45, 20);
        glVertex2f(45, 23);
        glVertex2f(40, 23);
    glEnd();

    glColor3ub(219, 73, 0);
    glBegin(GL_POLYGON);
        glVertex2f(25, 20);
        glVertex2f(30, 20);
        glVertex2f(30, 23);
        glVertex2f(25, 23);
    glEnd();

    ///left wheel
    glBegin(GL_POLYGON); for(int i=0;i<200;i++) { glColor3ub(32,32,32); float pi=3.1416; float A=(i*2*pi)/200; float r=2; float x=r*cos(A); float y=r*sin(A); glVertex2f(x+30,y+9); } glEnd();
    glBegin(GL_POLYGON); for(int i=0;i<200;i++) { glColor3ub(192, 192, 192); float pi=3.1416; float A=(i*2*pi)/200; float r=0.75; float x=r*cos(A); float y=r*sin(A); glVertex2f(x+30,y+9); } glEnd();
    ///right wheel
    glBegin(GL_POLYGON); for(int i=0;i<200;i++) { glColor3ub(32, 32, 32); float pi=3.1416; float A=(i*2*pi)/200; float r=2; float x=r*cos(A); float y=r*sin(A); glVertex2f(x+40,y+9); } glEnd();
    glBegin(GL_POLYGON); for(int i=0;i<200;i++) { glColor3ub(192,192,192); float pi=3.1416; float A=(i*2*pi)/200; float r=0.75; float x=r*cos(A); float y=r*sin(A); glVertex2f(x+40,y+9); } glEnd();
}











// <<< --- SEA JOURNEY SCENE  --- >>>

// --- Drawing Functions for the Sea Scene ---
void sea_drawCircle(float radius, float x, float y, int r, int g, int b) {
    glColor3ub(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int angle = 0; angle <= 360; angle += 10) {
        float rad = angle * 3.1415926535 / 180.0f;
        glVertex2f(x + cos(rad) * radius, y + sin(rad) * radius);
    }
    glEnd();
}
void sea_drawSky() {
    if (sea_isNightMode) {
        glColor3f(0.1f, 0.1f, 0.3f);
    } else {
        glColor3f(0.5f, 0.8f, 0.95f);
    }
    glBegin(GL_QUADS);
    glVertex2f(-50.0f, 0.0f);
    glVertex2f(50.0f, 0.0f);
    glVertex2f(50.0f, 40.0f);
    glVertex2f(-50.0f, 40.0f);
    glEnd();
}
void sea_drawCloud(float x, float y, float scale = 1.0f) {
    glColor3f(1.0f, 1.0f, 1.0f);
    float radius = 3.0f * scale;
    for (int i = 0; i < 3; i++) {
        float cx = x + i * radius * 0.8f;
        float cy = y;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int angle = 0; angle <= 360; angle += 30) {
            float rad = angle * 3.14159f / 180.0f;
            glVertex2f(cx + cos(rad) * radius, cy + sin(rad) * radius * 0.6f);
        }
        glEnd();
    }
}
void sea_drawSun() {
    if (sea_isNightMode) {
        GLfloat x = -40.0f;
        GLfloat y = 30.0f;
        GLfloat radius = 4.0f;
        glColor3f(1.0f, 1.0f, 1.0f);
        int triangleAmount = 100;
        GLfloat twicePi = 2.0f * PI;

        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y); // Center
            for (int i = 0; i <= triangleAmount; i++) {
                glVertex2f(
                    x + (radius * cos(i * twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount))
                );
            }
        glEnd();
    } else {
        GLfloat x = -40.0f;
        GLfloat y = 30.0f;
        GLfloat radius = 4.0f;
        int triangleAmount = 100;
        GLfloat twicePi = 2.0f * PI;

        glColor3f(1.0f, 1.0f, 0.0f);

        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y);
            for (int i = 0; i <= triangleAmount; i++) {
                glVertex2f(
                    x + (radius * cos(i * twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount))
                );
            }
        glEnd();
    }
}
void sea_drawSea() {
    if (sea_isNightMode) {
        glColor3f(0.0f, 0.0f, 0.5f);
    } else {
        glColor3f(0.1f, 0.3f, 0.8f);
    }
    glBegin(GL_QUADS);
    glVertex2f(-50.0f, -40.0f);
    glVertex2f(50.0f, -40.0f);
    glVertex2f(50.0f, -10.0f);
    glVertex2f(-50.0f, -10.0f);
    glEnd();
}
void sea_drawLand() {
    glColor3f(0.6f, 0.4f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-50.0f, -10.0f);
    glVertex2f(50.0f, -10.0f);
    glVertex2f(50.0f, 0.0f);
    glVertex2f(-50.0f, 0.0f);
    glEnd();
}
void sea_drawGrass() {
    if (sea_isNightMode) {
        glColor3f(0.0f, 0.3f, 0.0f);
    } else {
        glColor3f(0.0f, 0.6f, 0.0f);
    }

    for (float x = -50.0f; x < 50.0f; x += 1.0f) {
        glPushMatrix();
        glTranslatef(x, 0.0f, 0.0f);

        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, -10.0f);
        glVertex2f(0.5f, -8.5f);
        glVertex2f(1.0f, -10.0f);
        glEnd();

        glPopMatrix();
    }
}
void sea_drawMountains() {
    glColor3f(0.5f, 0.4f, 0.3f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-40.0f, 0.0f);
    glVertex2f(-30.0f, 20.0f);
    glVertex2f(-20.0f, 30.0f);
    glVertex2f(-10.0f, 10.0f);
    glVertex2f(-5.0f, 0.0f);
    glEnd();

    glColor3f(0.45f, 0.35f, 0.25f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-20.0f, 0.0f);
    glVertex2f(-10.0f, 25.0f);
    glVertex2f(0.0f, 35.0f);
    glVertex2f(10.0f, 20.0f);
    glVertex2f(20.0f, 0.0f);
    glEnd();

    glColor3f(0.4f, 0.3f, 0.2f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(5.0f, 0.0f);
    glVertex2f(15.0f, 15.0f);
    glVertex2f(30.0f, 28.0f);
    glVertex2f(40.0f, 10.0f);
    glVertex2f(45.0f, 0.0f);
    glEnd();
}
void sea_drawTrees() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3f(0.76f, 0.60f, 0.42f);
    glVertex2f(-45.0f, -10.0f);
    glVertex2f(-45.0f, -1.0f);
    glEnd();
    if (sea_isNightMode)
        glColor3f(0.0f, 0.2f, 0.0f);
    else
        glColor3f(0.0f, 0.6f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-47.0f, -3.0f);
    glVertex2f(-45.0f, 0.0f);
    glVertex2f(-43.0f, -3.0f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(-47.0f, -1.0f);
    glVertex2f(-45.0f, 2.0f);
    glVertex2f(-43.0f, -1.0f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(-46.5f, 0.5f);
    glVertex2f(-45.0f, 3.0f);
    glVertex2f(-43.5f, 0.5f);
    glEnd();

    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3f(0.76f, 0.60f, 0.42f);
    glVertex2f(-40.0f, -10.0f);
    glVertex2f(-40.0f, -1.0f);
    glEnd();
    if (sea_isNightMode)
        glColor3f(0.0f, 0.2f, 0.0f);
    else
        glColor3f(0.0f, 0.6f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-42.0f, -3.0f);
    glVertex2f(-40.0f, 0.0f);
    glVertex2f(-38.0f, -3.0f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(-42.0f, -1.0f);
    glVertex2f(-40.0f, 2.0f);
    glVertex2f(-38.0f, -1.0f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(-41.5f, 0.5f);
    glVertex2f(-40.0f, 3.0f);
    glVertex2f(-38.5f, 0.5f);
    glEnd();

}
void sea_drawBush(float centerX, float centerY) {
    // Trunk (Line)
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3f(0.76f, 0.60f, 0.42f);
    glVertex2f(centerX, centerY);
    glVertex2f(centerX, centerY + 2.0f);
    glEnd();

    // Leaves (Triangles)
    if (sea_isNightMode) {
        glColor3f(0.0f, 0.2f, 0.0f);
    } else {
        glColor3f(0.0f, 0.6f, 0.0f);
    }
    
    // Leaf layers using triangles
    glBegin(GL_TRIANGLES);
    glVertex2f(centerX - 2.0f, centerY + 2.0f);
    glVertex2f(centerX, centerY + 4.0f);
    glVertex2f(centerX + 2.0f, centerY + 2.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(centerX - 1.5f, centerY + 3.0f);
    glVertex2f(centerX, centerY + 5.0f);
    glVertex2f(centerX + 1.5f, centerY + 3.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(centerX - 1.0f, centerY + 4.0f);
    glVertex2f(centerX, centerY + 6.0f);
    glVertex2f(centerX + 1.0f, centerY + 4.0f);
    glEnd();
}
void sea_drawBackgroundBushes() {
    for (float x = -48.0f; x < 50.0f; x += 6.0f) {
        sea_drawBush(x, -9.0f);
    }
}
void sea_drawWindmillBlades(float translationX, float translationY) {
    glPushMatrix();
    
    // Translate to the windmill center
    glTranslatef(-14.9f + translationX, 34.0f + translationY, 0.0f);
    
    // Rotate the blades if the rotation flag is set
    if (sea_startRotation) {
        glRotatef(sea_rotateAngle, 0.0f, 0.0f, 1.0f);
    }

    // Reset translation to place the blades at the right position
    glTranslatef(-(-14.9f + translationX), -(34.0f + translationY), 0.0f);

    // Set color based on the night mode flag
    if (sea_isNightMode)
        glColor3ub(80, 80, 80);
    else
        glColor3ub(151, 153, 151);

    // First blade (top left)
    glBegin(GL_TRIANGLES);
    glVertex2f(-14.9f + translationX, 34.0f + translationY);
    glVertex2f(-18.0f + translationX, 39.0f + translationY);
    glVertex2f(-18.7f + translationX, 38.0f + translationY);
    glEnd();

    // Second blade (top right)
    glBegin(GL_TRIANGLES);
    glVertex2f(-14.9f + translationX, 34.0f + translationY);
    glVertex2f(-11.0f + translationX, 39.0f + translationY);
    glVertex2f(-11.5f + translationX, 39.8f + translationY);
    glEnd();

    // Third blade (bottom)
    glBegin(GL_TRIANGLES);
    glVertex2f(-14.9f + translationX, 34.0f + translationY);
    glVertex2f(-14.0f + translationX, 28.0f + translationY);
    glVertex2f(-13.2f + translationX, 29.0f + translationY);
    glEnd();
    
    glPopMatrix();
}
void sea_drawWindmill(float translationX, float translationY) {
    translationY -= 30.0f;
    translationX += 10.0f;

    if (sea_isNightMode)
        glColor3ub(80, 80, 80);
    else
        glColor3ub(151, 153, 151);

    glBegin(GL_POLYGON);
    glVertex2f(-15.0f + translationX, 25.0f + translationY);
    glVertex2f(-15.0f + translationX, 34.0f + translationY);
    glVertex2f(-14.7f + translationX, 34.0f + translationY);
    glVertex2f(-14.7f + translationX, 25.0f + translationY);
    glEnd();

    if (sea_isNightMode)
        sea_drawCircle(0.4, -14.85f + translationX, 34.0f + translationY, 120, 120, 120);
    else
        sea_drawCircle(0.4, -14.85f + translationX, 34.0f + translationY, 184, 186, 184);

    sea_drawWindmillBlades(translationX, translationY);
}
void sea_drawCustomShip() {
    glPushMatrix();
    glTranslatef(sea_shipX, 0.0f, 0.0f);

    // Ship body (dark blue)
    glBegin(GL_POLYGON);
    glColor3f(0.1f, 0.1f, 0.4f);
    glVertex2f(-20.0f, -29.5f);
    glVertex2f(20.0f, -29.5f);
    glVertex2f(18.0f, -32.5f);
    glVertex2f(-18.0f, -32.5f);
    glEnd();

    // White upper part
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(-16.0f, -29.5f);
    glVertex2f(16.0f, -29.5f);
    glVertex2f(14.0f, -22.0f);
    glVertex2f(-14.0f, -22.0f);
    glEnd();

    // Gray top
    glBegin(GL_POLYGON);
    glColor3f(0.8f, 0.8f, 0.8f);
    glVertex2f(-12.0f, -22.0f);
    glVertex2f(12.0f, -22.0f);
    glVertex2f(10.0f, -19.5f);
    glVertex2f(-10.0f, -19.5f);
    glEnd();

    // Windows (light blue)
    glColor3f(0.3f, 0.6f, 0.9f);
    for (float x = -13.0f; x <= 10.0f; x += 4.0f) {
        glBegin(GL_QUADS);
        glVertex2f(x, -27.0f);
        glVertex2f(x + 2.0f, -27.0f);
        glVertex2f(x + 2.0f, -24.0f);
        glVertex2f(x, -24.0f);
        glEnd();
    }

    // White cabin top
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(-3.0f, -19.5f);
    glVertex2f(3.0f, -19.5f);
    glVertex2f(2.5f, -16.5f);
    glVertex2f(-2.5f, -16.5f);
    glEnd();

    // Mast
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, -16.5f);
    glVertex2f(0.0f, -15.0f);
    glEnd();

    glPopMatrix();
}
void sea_drawMiniBoat() {
    // Left part of the mini boat (red base)
    glBegin(GL_POLYGON);
    glColor3f(0.8f, 0.1f, 0.1f);
    glVertex2f(-17.0f, -9.5f);
    glVertex2f(-13.0f, -9.5f);
    glVertex2f(-13.5f, -10.5f);
    glVertex2f(-16.5f, -10.5f);
    glEnd();

    // Left boat upper part (gray)
    glBegin(GL_QUADS);
    glColor3f(0.9f, 0.9f, 0.9f);
    glVertex2f(-16.0f, -9.5f);
    glVertex2f(-14.0f, -9.5f);
    glVertex2f(-14.0f, -8.7f);
    glVertex2f(-16.0f, -8.7f);
    glEnd();

    // Left boat mast (brown line)
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.3f, 0.2f, 0.1f);
    glVertex2f(-15.0f, -8.7f);
    glVertex2f(-15.0f, -7.5f);
    glEnd();

    // Left boat sail (white triangle)
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(-15.0f, -7.5f);
    glVertex2f(-15.0f, -8.5f);
    glVertex2f(-13.8f, -8.0f);
    glEnd();

    // Right part of the mini boat (red base)
    glBegin(GL_POLYGON);
    glColor3f(0.8f, 0.1f, 0.1f);
    glVertex2f(16.0f, -9.5f);
    glVertex2f(20.0f, -9.5f);
    glVertex2f(19.5f, -10.5f);
    glVertex2f(16.5f, -10.5f);
    glEnd();

    // Right boat upper part (gray)
    glBegin(GL_QUADS);
    glColor3f(0.9f, 0.9f, 0.9f);
    glVertex2f(17.0f, -9.5f);
    glVertex2f(19.0f, -9.5f);
    glVertex2f(19.0f, -8.7f);
    glVertex2f(17.0f, -8.7f);
    glEnd();

    // Right boat mast (brown line)
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.3f, 0.2f, 0.1f);
    glVertex2f(18.0f, -8.7f);
    glVertex2f(18.0f, -7.5f);
    glEnd();

    // Right boat sail (white triangle)
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(18.0f, -7.5f);
    glVertex2f(18.0f, -8.5f);
    glVertex2f(19.2f, -8.0f);
    glEnd();
}
void sea_initRain() {
    for (int i = 0; i < sea_numDrops; i++) {
        sea_rainX[i] = rand() % 100 - 50;
        sea_rainY[i] = rand() % 100 - 60;
    }
}
void sea_drawRain() {
    glColor3f(0.7f, 0.7f, 1.0f);
    glLineWidth(1.0f);
    for (int i = 0; i < sea_numDrops; i++) {
        glBegin(GL_LINES);
        glVertex2f(sea_rainX[i], sea_rainY[i]);
        glVertex2f(sea_rainX[i], sea_rainY[i] - 2.0f);
        glEnd();
    }
}











// <<< --- ROCKET SCENE DRAWING & ANIMATION FUNCTIONS --- >>>

struct Custom_Color
{
    int r;
    int g;
    int b;
};

struct Scene
{
    string T_scene;
    Custom_Color skyColor;
    Custom_Color riverColor;
    Custom_Color sunColor;
    Custom_Color moonLayers;
    //boat
    Custom_Color boatColor;
    Custom_Color boatbottomColor;
    Custom_Color boatwindowColor;
    //river
    Custom_Color rivershade1Color;
    Custom_Color rivershade2Color;
    Custom_Color rivershade3Color;
    Custom_Color rivershade4Color;
    //stars
    Custom_Color starsColor;

    //Road Color
    Custom_Color roadShoulder;
    Custom_Color roadShoulder2;
    Custom_Color roadShoulder3;
    Custom_Color roadColor;
    Custom_Color roadDivider;
    Custom_Color roadPier1;
    Custom_Color roadPier2;
    Custom_Color roadShadow;
    Custom_Color landColor;
    Custom_Color landColor2;
    Custom_Color groundColor1;
    Custom_Color groundColor2;
    Custom_Color groundColor3;

    // Rocket Color
    Custom_Color rocketGround;
    Custom_Color rocketGround2;
    Custom_Color rocketTower1;
    Custom_Color rocketTower2;
    Custom_Color rocketBody;
    Custom_Color rocketBody2;
    Custom_Color rocketHead;
    Custom_Color rocketHead2;

    //hill
    Custom_Color hillColor;
    Custom_Color hillColor1;
    Custom_Color hillShadow;
    Custom_Color hillShadow1;
    Custom_Color hillShadow2;
    Custom_Color hillShadow3;
    Custom_Color hillShadow4;

    //house
    Custom_Color houseWindow;
    Custom_Color house11;
    Custom_Color house1;
    Custom_Color houseDoor;
    Custom_Color houseTop1;
    Custom_Color houseTop11;
    Custom_Color houseSide1;
    Custom_Color houseLines;
    Custom_Color house2;
    Custom_Color house2Side;
    Custom_Color house2WindowBorder;
    Custom_Color house2Door;
    Custom_Color house3;
    Custom_Color house33;
    Custom_Color house3Side;
    Custom_Color house3Top1;
    Custom_Color house3Top2;
    Custom_Color house3DoorParking;
    Custom_Color house4;
    Custom_Color house44;
    Custom_Color house4Side;
    Custom_Color house4ParkingDoor;
    Custom_Color house4Door;
    Custom_Color house4top;
    Custom_Color house4TopSide;
    Custom_Color house5;
    Custom_Color house55;
    Custom_Color house5top;
    Custom_Color houseLand;
    Custom_Color houseLandShadow;
    Custom_Color houseRoad;

    //cars
    Custom_Color car1Body;
    Custom_Color car1Window;
    Custom_Color car1WindowLine;
    Custom_Color car1Tyre;
    Custom_Color car1TyreUp;
    Custom_Color car1Light;

    Custom_Color car2Body;
    Custom_Color car2Body2;
    Custom_Color car2Window;
    Custom_Color car2Tyre;
    Custom_Color car2Light;

    Custom_Color car3Body;
    Custom_Color car3seat;
    Custom_Color car3Light;


};

Scene Day =
{
    "Day",
    {133, 193, 233},      //skycolor
    {68,121,207},       //rivercolor
    {241, 196, 15},    //sunColor
    {241, 196, 15},      //moonLayers
    //boat
    {255, 255, 255},    //boatcolor
    {39,30,73},         //boatbottomcolor
    {107,111,116},      //boatwindowcolor
    //river
    {61, 109, 186},     //rivershade1color
    {54,102,192},       //rivershade2color
    {33, 59, 122},      //rivershade3color
    {110,161,233},      //rivershade4color
    //stars
    {133, 193, 233},    //starscolor

    //Road Color
    {183, 183, 183},     //roadShoulder
    {200, 200, 200},     //roadShoulder2
    {160, 160, 160},     //roadShoulder3
    {102, 104, 103},     //roadColor
    {255, 230, 81},      //roadBorder;
    {183, 183, 183},     //roadPier1
    {148, 148, 146},     //roadPier2
    {84, 84, 84},        //roadShadow
    //land
    {114, 109, 0},     //landColor
    {151, 105, 71},      //landColor2
    {180, 122, 79},       // groundColor1;
    {73, 26, 10},       //groundColor2;
    {115, 66, 35},         //groundColor3;

    //Rocket Color
    {0, 0, 0},  //rockeGround;
    {117, 133, 88},  //rockeGround2;
    {64, 64, 66}, //rockeTower1;
    {148, 149, 153}, //rockeTower2;
    {255, 255, 255}, //rockeBody;
    {177, 188, 189}, //rockeBody2;
    {178, 76, 34}, //rocketHead;
    {247, 177, 129}, //rocketHead2;

    //hill
    {129, 136, 118}, //hillColor
    {120, 127, 49},//hillColor1
    {145, 192, 164},  //hillShadow;
    {163, 219, 192},   //hillShadow1;
    {86, 84, 61},   //hillShado2;
    {55, 63, 22},   //hillShado3;
    {111, 117, 47},   //hillShado4fronthill;

    //house
    {133, 97, 176},    //houseWindows
    {247, 183, 171},  //house1
    {251, 218, 203},  //house11
    {165, 177, 245},   //housedor
    {112, 35, 53},   //houseTop1
    {154, 64, 89},   ////houseTop11
    {197, 147, 174},  //houseSide1
    {230, 234, 231},  //houseLines

    {113, 176, 160},  //house2
    {32, 71, 100},  //house2Side
    {213, 214, 244},  //house2windowborder
    {164, 62, 49},  //house2door
    {225, 130, 48}, //house3;
    {248, 165, 79}, //house33;
    {165, 50, 59}, //house3Side;
    {64, 66, 1},   //houseTop3;
    {131, 135, 51},   //houseTop33;
    {240, 231, 226},  //house3DoorParking;
    {237, 71, 70},   //house4
    {245, 99, 86}, //house44;
    {126, 30, 56}, //house4side;
    {255, 179, 162}, //house4parkingdoor;
    {100, 28, 28}, //house4door
    {63, 39, 65},   //house4top
    {51, 27, 53},  //house4TopSide
    {119, 177, 237}, //house5;
    {141, 201, 235}, //house55;
    {63, 45, 97}, //house5top;
    {167, 146, 0}, //houseLand;
    {67, 16, 43},  //houseLandshadow;
    {230, 182, 136}, //houseRoad;

    //cars
    {210, 214, 223}, //car1Body;
    {66, 90, 90}, //car1Window;
    {46, 48, 47}, //carWindowLine
    {73, 79, 79}, //car1Tyre;
    {25, 2, 3}, //car1TyreUp;
    {189, 68, 85}, //car1Light;

    {255, 131, 59},  //car2Body;
    {115, 110, 104},  //car2Body2;
    {125, 117, 128},  //car2Window;
    {86, 86, 86},  //car2Tyre;
    {195, 245, 244}, //car2Light;

    {195, 32, 38}, //car3Body;
    {2, 2, 2}, //car3seat;
    {254, 254, 254}, //car3Light;

};

Scene Night =
{
    "Night",
    {48, 89, 124},      //skycolor
    {21, 67, 96},       //rivercolor
    {211, 212, 222},    //planetcolor
    {179, 196, 204},      //planetlayers
    //boat
    {23, 32, 42},    //boatcolor
    {28, 40, 51},         //boatbottomcolor
    {241, 196, 15},      //boatwindowcolor
    //river
    {36, 113, 163},     //rivershade1color
    {26, 82, 118},       //rivershade2color
    {33, 59, 122},      //rivershade3color
    {84, 153, 199},      //rivershade4color
    //stars
    {255, 255, 255},    //starscolor

    // Road Color
    {43, 43, 43},     //roadShoulder
    {112, 35, 53},     //roadShoulder2
    {43, 43, 43},     //roadShoulder3
    {32, 24, 20},     //roadColor
    {255, 230, 81},      //roadBorder;
    {43, 43, 43},     //roadPier1
    {32, 24, 20},     //roadPier2
    {32, 24, 20},     //roadShadow

    //land
    {36,83,16},     //landColor
    {172, 98, 58},     //landColor2
    {122, 84, 83},       // groundColor1;
    {73, 26, 10},       //groundColor2;
    {131, 86, 80},         //groundColor3;

    // Rocket Color
    {0, 0, 0},  //rockeGround;
    {117, 133, 88},  //rockeGround2;
    {64, 64, 66}, //rockeTower1;
    {148, 149, 153}, //rockeTower2;
    {28, 40, 51}, //rockeBody;
    {112, 35, 53}, //rockeBody2;
    {178, 76, 34}, //rocketHead;
    {241, 196, 15}, //rocketHead2;

    //hill
    {32, 24, 20}, //hillColor
    {37, 31, 67}, //hillColor1
    {0, 0, 0},  //hillShadow;
    {0, 0, 0},   //hillShadow1;
    {0, 0, 0},   //hillShado2;
    {0, 0, 0},   //hillShado3;
    {32, 24, 20},   //hillShado4fronthill;

    //house
    {241, 196, 15},    //houseWindows
    {112, 35, 53},  //house1
    {112, 35, 53},  //house11
    {165, 177, 245},   //housedor
    {112, 35, 53},   //houseTop1
    {154, 64, 89},   ////houseTop11
    {154, 64, 89},  //houseSide1
    {43, 43, 43},  //houseLines
    {154, 64, 89},  //house2
    {112, 35, 53},  //house2Side
    {213, 214, 244},  //house2windowborder
    {164, 62, 49},  //house2door

    {154, 64, 89}, //house3;
    {112, 35, 53},//house33;
    {165, 50, 59}, //house3Side;
    {64, 66, 1},   //houseTop3;
    {112, 35, 53},   //houseTop33;
    {112, 35, 53},  //house3DoorParking;

    {112, 35, 53},   //house4
    {154, 64, 89}, //house44;
    {126, 30, 56}, //house4side;
    {154, 64, 89}, //house4parkingdoor;
    {100, 28, 28}, //house4door
    {63, 39, 65},   //house4top
    {51, 27, 53},  //house4TopSide

    {119, 177, 237}, //house5;
    {141, 201, 235}, //house55;
    {63, 45, 97}, //house5top;

    {167, 146, 0}, //houseLand;
    {67, 16, 43},  //houseLandshadow;
    {230, 182, 136}, //houseRoad;


    //cars
    {210, 214, 223}, //car1Body;
    {66, 90, 90}, //car1Window;
    {46, 48, 47}, //carWindowLine
    {73, 79, 79}, //car1Tyre;
    {25, 2, 3}, //car1TyreUp;
    {241, 196, 15}, //car1Light;

    {255, 131, 59},  //car2Body;
    {115, 110, 104},  //car2Body2;
    {125, 117, 128},  //car2Window;
    {86, 86, 86},  //car2Tyre;
    {241, 196, 15}, //car2Light;

    {195, 32, 38}, //car3Body;
    {2, 2, 2}, //car3seat;
    {241, 196, 15}, //car3Light;
};

Scene scenes[2] = {Day, Night};
int daynighttracker = 0;
Scene currentScene = scenes[daynighttracker];


// --- Animation Functions ---
void rocket_updateCarWheels(float deltaTime)
{
    rocket_rotateWheel -= ROCKET_WHEEL_ROTATION_SPEED * deltaTime;
    if(rocket_rotateWheel < -360.0f)
    {
        rocket_rotateWheel += 360.0f;
    }
}
void rocket_updateCars(float deltaTime)
{
    rocket_moveCarsX += ROCKET_CAR_SPEED_X * deltaTime;
    if (rocket_moveCarsX > 2420) // Increased the reset limit to ensure it's off-screen
    {
        rocket_moveCarsX = -500;
    }
}
void rocket_updateRocket(float deltaTime)
{
    rocket_moveRocketY += ROCKET_SPEED_Y * deltaTime;
    if (rocket_moveRocketY > 800) // Increased limit for full launch
    {
        rocket_moveRocketY = 0;
    }
}
void rocket_updateSun(float deltaTime)
{
    rocket_moveSuntX += ROCKET_SUN_SPEED_X * deltaTime;
    rocket_moveSunY -= ROCKET_SUN_SPEED_Y * deltaTime;

    if (rocket_moveSunY < -1800) // Adjusted reset limit
    {
        rocket_moveSuntX = 5;
        rocket_moveSunY = 1.0;
    }
}
void rocket_updateBoat(float deltaTime)
{
    rocket_moveboatX -= ROCKET_BOAT_SPEED_X * deltaTime;
    rocket_moveboatY = sin(glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * ROCKET_BOAT_SPEED_Y;

    if (rocket_moveboatX < -3000) // Increased reset limit
    {
        rocket_moveboatX = 1920;
    }
}
void rocket_updateWaterCurrent(float deltaTime)
{
    rocket_movewatercurrentX += ROCKET_WATER_CURRENT_SPEED * deltaTime;

    if (rocket_movewatercurrentX > 400)
    {
        rocket_movewatercurrentX = 0;
    }
}


// --- Helper Drawing Functions ---
void Custom_Points1(vector<pair<float, float>> coord, Custom_Color Custom_Color = {255,255,255}, float x = 0, float y = 0, float s = 1)
{
    glPointSize(2);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
    glBegin(GL_POINTS);
    for (int i = 0; i < coord.size(); i++)
    {
        glVertex2f(x + s * coord[i].first, y + s * coord[i].second);
    }
    glEnd();
}
void Custom_Points2(vector<pair<float, float>> coord, Custom_Color Custom_Color = {255,255,255}, float x = 0, float y = 0, float s = 1)
{
    glPointSize(4);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
    glBegin(GL_POINTS);
    for (int i = 0; i < coord.size(); i++)
    {
        glVertex2f(x + s * coord[i].first, y + s * coord[i].second);
    }
    glEnd();
}
void Custom_Points3(vector<pair<float, float>> coord, Custom_Color Custom_Color = {255,255,255}, float x = 0, float y = 0, float s = 1)
{
    glPointSize(10);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
    glBegin(GL_POINTS);
    for (int i = 0; i < coord.size(); i++)
    {
        glVertex2f(x + s * coord[i].first, y + s * coord[i].second);
    }
    glEnd();
}
void Custom_Polygon(vector<pair<float, float>> coord, Custom_Color Custom_Color = {255,255,255}, float x = 0, float y = 0)
{
    glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < coord.size(); i++)
    {
        glVertex2f(x +  coord[i].first, y +  coord[i].second);
    }
    glEnd();
}
void Custom_Polygon(vector<pair<float, float>> coord, float XSize, float YSize, Custom_Color Custom_Color = {255,255,255}, float x = 0, float y = 0)
{
    glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
    glBegin(GL_POLYGON);
    glVertex2f(coord[0].first+x,coord[0].second+y);
    for (int i = 0; i < 3; i++)
    {
        if(i==0)
            glVertex2f(XSize +  coord[0].first+x,coord[0].second+y);
        else if(i==1)
            glVertex2f(XSize +  coord[0].first+x, YSize +  coord[0].second+y);
        else
            glVertex2f(coord[0].first+x, YSize +  coord[0].second+y);
    }
    glEnd();
}
void Halfcircle(float radius, float xC, float yC, Custom_Color Custom_Color = {255,255,255})
{
    glBegin(GL_POLYGON);
    for(int i=0; i<250; i++)
    {
        glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
        float pi = 1.5708;
        float A = (i*2*pi)/250;
        float r = radius;
        float x = r * cos(A);
        float y = r * sin(A);
        glVertex2f(x+xC, y+yC);
    }
    glEnd();
}
void Halfcircle(float radius, float xC, float yC,float xS,float yS, Custom_Color Custom_Color = {255,255,255})
{
    glBegin(GL_POLYGON);
    for(int i=0; i<250; i++)
    {
        glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
        float pi = 1.5708;
        float A = (i*2*pi)/250;
        float r = radius;
        float x = xS*r * cos(A);
        float y = yS*r * sin(A);
        glVertex2f(x+xC, y+yC);
    }
    glEnd();
}
void HalfcircleWithLines(float radius, float xC, float yC, Custom_Color Custom_Color = {255,255,255})
{
    glPointSize(0.5);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POINTS);
    for(int i=0; i<250; i++)
    {
        glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
        float pi = 1.5708;
        float A = (i*2*pi)/250;
        float r = radius;
        float x = r * cos(A);
        float y = r * sin(A);
        glVertex2f(x+xC, y+yC);
    }
    glEnd();
}
void FullcircleWithLines(float radius, float xC, float yC, Custom_Color Custom_Color = {255,255,255})
{
    glPointSize(0.5);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POINTS);
    for(int i=0; i<250; i++)
    {
        glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
        float pi = 3.1416;
        float A = (i*2*pi)/250;
        float r = radius;
        float x = r * cos(A);
        float y = r * sin(A);
        glVertex2f(x+xC, y+yC);
    }
    glEnd();
}
void HalfcircleBorder(float radius, float xC, float yC, Custom_Color Custom_Color = {255,255,255})
{
    glPointSize(0.1);
    glBegin(GL_POINTS);
    for(int i=0; i<250; i++)
    {
        glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
        float pi = 1.5708;
        float A = (i*2*pi)/250;
        float r = radius;
        float x = r * cos(A);
        float y = r * sin(A);
        glVertex2f(x+xC, y+yC);
    }
    glEnd();
}
void Fullcircle(float radius, float xC, float yC, Custom_Color Custom_Color = {255,255,255})
{
    glBegin(GL_POLYGON);
    for(int i=0; i<250; i++)
    {
        glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
        float pi = 3.1416;
        float A = (i*2*pi)/250;
        float r = radius;
        float x = r * cos(A);
        float y = r * sin(A);
        glVertex2f(x+xC, y+yC);
    }
    glEnd();
}
void FullcircleBorder(float radius, float xC, float yC, Custom_Color Custom_Color = {255,255,255})
{
    glLineWidth(3);
    glBegin(GL_LINES);
    for(int i=0; i<250; i++)
    {
        glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
        float pi = 3.1416;
        float A = (i*2*pi)/250;
        float r = radius;
        float x = r * cos(A);
        float y = r * sin(A);
        glVertex2f(x+xC, y+yC);
    }
    glEnd();
}
void Linestrip(vector<pair<float, float>> coord, Custom_Color Custom_Color = {255,255,255}, float x = 0, float y = 0, float s = 1)
{
    glLineWidth(1);
    glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < coord.size(); i++)
    {
        glVertex2f(x + s * coord[i].first, y + s * coord[i].second);
    }
    glEnd();
}
void Linestrip(vector<pair<float, float>> coord,float XSize, float YSize, Custom_Color Custom_Color = {255,255,255},float lineWidth=1, float x = 0, float y = 0)
{
    glLineWidth(lineWidth);
    glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
    glBegin(GL_LINE_STRIP);
    glVertex2f(coord[0].first+x,coord[0].second+y);
    for (int i = 0; i < 5; i++)
    {
        if(i==0)
            glVertex2f(coord[0].first+x,coord[0].second+y);
        else if(i==1)
            glVertex2f(XSize +  coord[0].first+x,coord[0].second+y);
        else if(i==2)
            glVertex2f(XSize +  coord[0].first+x, YSize +  coord[0].second+y);
        else if(i==3)
            glVertex2f(coord[0].first+x, YSize +  coord[0].second+y);
        else
            glVertex2f(coord[0].first+x,coord[0].second+y);
    }
    glEnd();
}
void Linestrip2(vector<pair<float, float>> coord, Custom_Color Custom_Color = {255,255,255},float lineWidth=1, float x = 0, float y = 0, float s = 1)
{
    glLineWidth(lineWidth);
    glColor3ub(Custom_Color.r, Custom_Color.g, Custom_Color.b);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < coord.size(); i++)
    {
        glVertex2f(x + s * coord[i].first, y + s * coord[i].second);
    }
    glEnd();
}

// --- High-level Drawing Functions ---
void Cars()
{

    glMatrixMode(GL_MODELVIEW);
    //first car
    glPushMatrix();
    glTranslatef(rocket_moveCarsX,0,0);
    Custom_Polygon({{24.5, 406.5},{12, 408},{11.5, 413},{12, 416.5},{13.2, 419},{16, 438},{22, 438},{47, 453},{50, 454},{82, 454},{87, 453},{111, 440},{115, 438},
                   {123, 436},{142, 431},{148, 422},{149, 419},{147.5, 415},{147.5, 406.5},{24.5, 406.5}},currentScene.car1Body);//car1body
    Custom_Polygon({{108, 438},{28, 438},{48, 450},{54, 452},{82, 452},{88, 450}},currentScene.car1Window);//window
    Custom_Polygon({{69, 452},{70, 438},{74, 438},{71, 452}},currentScene.car1WindowLine);
    Custom_Polygon({{14, 436},{28, 436},{22, 430},{16, 428},{14, 428.5},{13, 430}},currentScene.car1Light);
    Custom_Polygon({{141, 430.5},{130, 432},{134, 428},{140, 426},{145, 426}},currentScene.car1Light);

    glPushMatrix();
    Halfcircle(16,42.5,406.5,currentScene.car1TyreUp);
    glTranslatef(42.5,406.5,0);
    glRotatef(rocket_rotateWheel,0,0,1);
    glTranslatef(-42.5,-406.5,0);
    Fullcircle(12,42.5,406.5,currentScene.car1Tyre);
    FullcircleWithLines(7,42.5,406.5,currentScene.car1Body);
    Linestrip2({{36, 406},{49, 406}},currentScene.car1Body,2);
    Linestrip2({{42, 400},{42, 413}},currentScene.car1Body,2);
    Fullcircle(3.5,42.5,406.5,currentScene.car1Body);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(70,0,0);
    Halfcircle(16,42.5,406.5,currentScene.car1TyreUp);
    glTranslatef(42.5,406.5,0);
    glRotatef(rocket_rotateWheel,0,0,1);
    glTranslatef(-42.5,-406.5,0);
    Fullcircle(12,42.5,406.5,currentScene.car1Tyre);
    FullcircleWithLines(7,42.5,406.5,currentScene.car1Body);
    Linestrip2({{36, 406},{49, 406}},currentScene.car1Body,2);
    Linestrip2({{42, 400},{42, 413}},currentScene.car1Body,2);
    Fullcircle(3.5,42.5,406.5,currentScene.car1Body);
    glPopMatrix();


    //second cars
    glPushMatrix();
    glTranslated(20,55,0);
    Custom_Polygon({{184.5, 351.5},{165, 354},{163, 365},{162, 384},{164, 387.5},{246, 386},{245, 350}},currentScene.car2Body);//body

    Custom_Polygon({{245, 350},{246, 386},{248, 400},{249, 403.5},{251, 405},{266.5, 405},{270, 404.5},{274, 403.5},{278, 402},{282, 400},
                  {284.5, 398.5},{299.5, 386.5},{307, 386},{315, 385},{321, 384},{326, 383},{332, 381},{336, 379},{337, 377},{337, 374},
                  {336, 367},{332, 349},{325, 348.5}},currentScene.car2Body);//body
    Custom_Polygon({{255, 385},{257, 402},{270, 401},{277, 398},{282, 394},{288, 389},{292, 383}},currentScene.car2Window);//window
    Linestrip2({{162, 387.5},{246, 386}},currentScene.car2Tyre,2);//lines
    Linestrip2({{246, 386},{245, 350}},currentScene.car2Tyre,2);//lines
    Linestrip2({{332, 349},{198, 351}},currentScene.car2Tyre,2);//lines
    Custom_Polygon({{162, 368},{166, 368},{168, 372},{168, 378},{166, 382},{162, 382}},currentScene.car1Light);
    Custom_Polygon({{164, 366},{159, 362},{160, 358},{161, 356},{164, 354},{171, 354},{169, 364},{167, 365}},currentScene.car2Tyre);
    Custom_Polygon({{337, 374},{331, 374},{331, 367},{336, 367}},currentScene.car2Light);

    glPushMatrix();
    Halfcircle(17,198,351,currentScene.car1TyreUp);
    glTranslatef(198,351,0);
    glRotatef(rocket_rotateWheel,0,0,1);
    glTranslatef(-198,-351,0);
    Fullcircle(13,198,351,currentScene.car1Tyre);
    FullcircleWithLines(8,198,351,currentScene.car1Body);
    Linestrip2({{194, 358},{202, 344}},currentScene.car1Body,2);
    Linestrip2({{194, 344},{202, 358}},currentScene.car1Body,2);
    Linestrip2({{191, 351},{205, 351}},currentScene.car1Body,2);
    Fullcircle(3.5,198,351,currentScene.car2Tyre);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(100,-2,0);
    Halfcircle(17,198,351,currentScene.car1TyreUp);
    glTranslatef(198,351,0);
    glRotatef(rocket_rotateWheel,0,0,1);
    glTranslatef(-198,-351,0);
    Fullcircle(13,198,351,currentScene.car1Tyre);
    FullcircleWithLines(8,198,351,currentScene.car1Body);
    Linestrip2({{194, 358},{202, 344}},currentScene.car1Body,2);
    Linestrip2({{194, 344},{202, 358}},currentScene.car1Body,2);
    Linestrip2({{191, 351},{205, 351}},currentScene.car1Body,2);
    Fullcircle(3.5,198,351,currentScene.car2Tyre);
    glPopMatrix();

    glPopMatrix();


    //Thired car
     glPushMatrix();
     glTranslatef(50,5,0);
    Custom_Polygon({{375.5, 438},{375, 444},{376, 446},{378, 446},{380, 444},{382, 442},{384, 437}},currentScene.car3seat);
    Custom_Polygon({{402, 436},{402, 443},{402, 445},{402, 445},{406, 445},{408, 442},{409, 440},{411, 435}},currentScene.car3seat);
    Custom_Polygon({{363, 402},{352, 408},{352, 420},{354, 422},{352, 428},{354, 434},{358, 438},{436, 434},
                   {448, 434},{455, 433},{461, 432},{468, 430},{472, 427},{475, 423},{479, 416},{479, 413},{477, 407},
                   {474, 402},{461, 402}},currentScene.car3Body);//body
    Custom_Polygon({{436, 434},{418, 452},{417, 450},{416, 452},{419, 451},{430, 446},{437, 442},{448, 434}},currentScene.car3Body);
    Custom_Polygon({{354, 434},{352, 428},{353, 426},{358, 426},{361, 431},{359, 434},{354, 434}},currentScene.car3Light);
    Custom_Polygon({{477, 419},{469, 419},{464, 425},{471, 425},{475, 423}},currentScene.car3Light);
    Custom_Polygon({{479, 413},{479, 414},{473, 414},{471, 410},{471, 409},{472, 408},{477, 408}},currentScene.car3seat);

    glPushMatrix();
    glTranslatef(179,52,0);
    Halfcircle(15,198,351,currentScene.car1TyreUp);
    glTranslatef(198,351,0);
    glRotatef(rocket_rotateWheel,0,0,1);
    glTranslatef(-198,-351,0);
    Fullcircle(13,198,351,currentScene.car1Tyre);
    FullcircleWithLines(8,198,351,currentScene.car1Body);
    Linestrip2({{194, 358},{202, 344}},currentScene.car1Body,2);
    Linestrip2({{194, 344},{202, 358}},currentScene.car1Body,2);
    Linestrip2({{191, 351},{205, 351}},currentScene.car1Body,2);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(250,52,0);
    Halfcircle(15,198,351,currentScene.car1TyreUp);
    glTranslatef(198,351,0);
    glRotatef(rocket_rotateWheel,0,0,1);
    glTranslatef(-198,-351,0);
    Fullcircle(13,198,351,currentScene.car1Tyre);
    FullcircleWithLines(8,198,351,currentScene.car1Body);
    Linestrip2({{194, 358},{202, 344}},currentScene.car1Body,2);
    Linestrip2({{194, 344},{202, 358}},currentScene.car1Body,2);
    Linestrip2({{191, 351},{205, 351}},currentScene.car1Body,2);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
}
void House()
{
    //house font land
    Custom_Polygon({{750, 490}},1170,30,currentScene.houseLand);
    Linestrip2({{750, 490},{1920, 490}},currentScene.houseLandShadow,2);
    Linestrip2({{750, 500},{1920, 500}},currentScene.houseRoad,4);
    Custom_Polygon({{750, 520},{750, 488},{780,488}},currentScene.landColor);

    Custom_Polygon({{822, 520},{852, 500},{874, 500},{840, 520}},currentScene.houseRoad);//house1
    Custom_Polygon({{990, 520},{1038, 490},{1100, 490},{1045, 520}},currentScene.houseRoad);//house1
    Linestrip2({{1100, 490},{1045, 520}},currentScene.landColor,2);

    Custom_Polygon({{1095, 520},{1124, 500},{1144, 500},{1110, 520}},currentScene.houseRoad);//house2
    Custom_Polygon({{1164, 520},{1212, 490},{1258, 490},{1200, 520}},currentScene.houseRoad);//house2
    Linestrip2({{1258, 490},{1200, 520}},currentScene.landColor,2);

    Custom_Polygon({{1235, 520},{1282, 490},{1350, 490},{1295, 520}},currentScene.houseRoad);//house3
    Linestrip2({{1350, 490},{1295, 520}},currentScene.landColor,2);
    Custom_Polygon({{1356, 520},{1388, 500},{1406, 500},{1374, 520}},currentScene.houseRoad);//house3

    Custom_Polygon({{1538, 520},{1568, 500},{1590, 500},{1558, 520}},currentScene.houseRoad);//house4
    Custom_Polygon({{1626, 520},{1674, 490},{1742, 490},{1685, 520}},currentScene.houseRoad);//house4
    Linestrip2({{1742, 490},{1685, 520}},currentScene.landColor,2);

    Custom_Polygon({{1786, 520},{1812, 500},{1832, 500},{1798, 520}},currentScene.houseRoad);//house5
    Custom_Polygon({{1870, 520},{1920, 490},{1920, 514},{1915, 520}},currentScene.houseRoad);//house5


    //Left to Right

    //House-5

     Linestrip2({{1710, 519},{1861, 519}},currentScene.houseLines,4);
     Custom_Polygon({{1858, 520},{1858, 542},{1896, 578},{1920, 554},{1920,520}},currentScene.house5);//parkingbody
      Linestrip2({{1896, 578},{1858, 542},{1896, 578},{1920, 554}},currentScene.houseLines,3);
     Custom_Polygon({{1854, 578},{1854, 542},{1858, 542},{1896, 578}},currentScene.house5top);//parkingtop
     Custom_Polygon({{1870, 520}},45,18,currentScene.houseLines);

     Custom_Polygon({{1888, 550}},12,11,currentScene.houseLines);
     Custom_Polygon({{1890, 551}},8,9,currentScene.houseWindow);



    Custom_Polygon({{1770, 520},{1770, 606},{1812, 652},{1856, 606},{1856,520}},currentScene.house55);//body
    Custom_Polygon({{1690, 520}},30,82,currentScene.house2Side);//side
    Custom_Polygon({{1686, 588},{1690, 586},{1695, 602},{1693, 607}},currentScene.house4top);//shadowlefttop
    Custom_Polygon({{1746, 520}},24,90,currentScene.house2Side);//sideMiddle
    Custom_Polygon({{1710, 520}},56,44,currentScene.house5);//bodyleft
    Custom_Polygon({{1706, 566},{1774, 566},{1748, 610},{1692, 610}},currentScene.house5top);//topLeft
    Linestrip2({{1706, 565},{1774, 565}},currentScene.houseLines,2);//linesleft
    Linestrip2({{1744, 607},{1768, 607},{1818, 660}},currentScene.houseLines,3);//topleftlines
    Custom_Polygon({{1744, 608},{1768, 608},{1816, 660},{1792, 660}},currentScene.house5top);//righttop
    Custom_Polygon({{1812, 652},{1816, 656},{1862, 606},{1856, 606}},currentScene.house2Side);//rightshadow
    Linestrip2({{1864, 606},{1814, 659}},currentScene.houseLines,4);//toprightlines
    Linestrip2({{1778, 565},{1860, 565}},currentScene.houseLines,3);//linesMiddle
    Custom_Polygon({{1772, 576},{1778, 566},{1860, 566},{1854, 576}},currentScene.house5top);
    Linestrip2({{1779,520},{1779, 565}},currentScene.houseLines,3);//fontlines
    Linestrip2({{1779,520},{1779, 565}},currentScene.houseLines,3,27);//fontlines
    Linestrip2({{1779,520},{1779, 565}},currentScene.houseLines,3,52);//fontlines
    Linestrip2({{1779,520},{1779, 565}},currentScene.houseLines,3,82);//fontlines


    Custom_Polygon({{1786, 578}},14,32,currentScene.houseLines);//topwindow
    Custom_Polygon({{1788, 580}},10,26,currentScene.houseWindow);//topwindow
    Linestrip2({{1788, 594},{1798, 594}},currentScene.houseLines,1.5);//topwindow

    Custom_Polygon({{1786, 578}},14,32,currentScene.houseLines,20);//top2window
    Custom_Polygon({{1788, 580}},10,26,currentScene.houseWindow,20);//top2window
    Linestrip2({{1788, 594},{1798, 594}},currentScene.houseLines,1.5,20);//top2window

    Custom_Polygon({{1786, 578}},14,32,currentScene.houseLines,40);//top3window
    Custom_Polygon({{1788, 580}},10,26,currentScene.houseWindow,40);//top3window
    Linestrip2({{1788, 594},{1798, 594}},currentScene.houseLines,1.5,40);//top3window

    Custom_Polygon({{1718, 529}},14,32,currentScene.houseLines);//Leftwindow
    Custom_Polygon({{1720, 531}},10,26,currentScene.houseWindow);//Leftwindow
    Linestrip2({{1720, 545},{1732, 545}},currentScene.houseLines,1.5);//Leftwindow

    Custom_Polygon({{1718, 529}},14,32,currentScene.houseLines,25);//Left2window
    Custom_Polygon({{1720, 531}},10,26,currentScene.houseWindow,25);//Left2window
    Linestrip2({{1720, 545},{1732, 545}},currentScene.houseLines,1.5,25);//Left2window

    Custom_Polygon({{1784,520}},16,38,currentScene.houseLines);
    Custom_Polygon({{1786,520}},12,32,currentScene.house2Door);
    Custom_Polygon({{1788, 538}},9,11,currentScene.houseWindow);
    Linestrip2({{1786, 555},{1798, 555}},currentScene.houseWindow,2);



    Custom_Polygon({{1718, 529}},14,32,currentScene.houseLines,92);//rightwindow
    Custom_Polygon({{1720, 531}},10,26,currentScene.houseWindow,92);//rightwindow
    Linestrip2({{1720, 545},{1732, 545}},currentScene.houseLines,1.5,92);//rightwindow

    Custom_Polygon({{1718, 529}},14,32,currentScene.houseLines,116);//rightwindow
    Custom_Polygon({{1720, 531}},10,26,currentScene.houseWindow,116);//rightwindow
    Linestrip2({{1720, 545},{1732, 545}},currentScene.houseLines,1.5,116);//rightwindow

    Fullcircle(8,1810,630,currentScene.houseLines);
    Fullcircle(6,1810,630,currentScene.houseWindow);

    //House-4
    Custom_Polygon({{1440, 520}},22,100,currentScene.house4Side);//side
    Custom_Polygon({{1462, 520}},238,43,currentScene.house4);//side
    Linestrip2({{1518.5, 520},{1518.5, 562}},currentScene.house4Side,5);//windowLeftUp
    Custom_Polygon({{1440, 622},{1458, 562},{1704, 562},{1670, 622}},currentScene.house4top);//top
    Custom_Polygon({{1522, 520},{1574,520},{1574, 562},{1549, 597},{1522, 562}},currentScene.house44);//top
    Custom_Polygon({{1510, 562},{1522, 562},{1549, 597},{1537, 597}},currentScene.house4TopSide);//topSideMiddle
    Linestrip2({{1458, 561},{1522, 561},{1549, 596},{1574, 561},{1704, 561}},currentScene.house4ParkingDoor,2.5);//lines
    Custom_Polygon({{1474, 578},{1474, 594},{1496, 620},{1518, 596},{1518, 578}},currentScene.house4);//topwindowleft
    Linestrip2({{1464, 593.5},{1472, 593.5},{1496, 619},{1522, 593}},currentScene.house4ParkingDoor,3);//topWindowLeftLines
    Custom_Polygon({{1464, 594},{1472, 594},{1496, 620},{1475, 620}},currentScene.house4TopSide);//topwindowleftSide
    Custom_Polygon({{1484, 582}},22,16,currentScene.house4ParkingDoor);//windowLeftUp
    Custom_Polygon({{1486, 584}},18,12,currentScene.houseWindow);//windowLeftUp
    Linestrip2({{1486, 592},{1504, 592}},currentScene.house4ParkingDoor,2);//windowLeftUp
    Linestrip2({{1492, 584},{1492, 596}},currentScene.house4ParkingDoor,2);//windowLeftUp
    Linestrip2({{1498, 584},{1498, 596}},currentScene.house4ParkingDoor,2);//windowLeftUp
    Linestrip2({{1482, 599},{1508, 599}},currentScene.house4ParkingDoor,2);//windowLeftUp
    Linestrip2({{1482, 582},{1508, 582}},currentScene.house4ParkingDoor,2);//windowLeftUp

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(110, 0, 0);
    Custom_Polygon({{1474, 578},{1474, 594},{1496, 620},{1518, 596},{1518, 578}},currentScene.house4);//topwindowleft
    Linestrip2({{1464, 593.5},{1472, 593.5},{1496, 619},{1522, 593}},currentScene.house4ParkingDoor,3);//topWindowLeftLines
    Custom_Polygon({{1464, 594},{1472, 594},{1496, 620},{1475, 620}},currentScene.house4TopSide);//topwindowleftSide
    Custom_Polygon({{1484, 582}},22,16,currentScene.house4ParkingDoor);//windowLeftUp
    Custom_Polygon({{1486, 584}},18,12,currentScene.houseWindow);//windowLeftUp
    Linestrip2({{1486, 592},{1504, 592}},currentScene.house4ParkingDoor,2);//windowLeftUp
    Linestrip2({{1492, 584},{1492, 596}},currentScene.house4ParkingDoor,2);//windowLeftUp
    Linestrip2({{1498, 584},{1498, 596}},currentScene.house4ParkingDoor,2);//windowLeftUp
    Linestrip2({{1482, 599},{1508, 599}},currentScene.house4ParkingDoor,2);//windowLeftUp
    Linestrip2({{1482, 582},{1508, 582}},currentScene.house4ParkingDoor,2);//windowLeftUp
    glPopMatrix();

    Custom_Polygon({{1470, 528}},34,28,currentScene.house4ParkingDoor);//windowLeftDown
    Custom_Polygon({{1472, 530}},30,24,currentScene.houseWindow);//windowLeftDown
    Linestrip2({{1468, 528},{1506, 528}},currentScene.house4ParkingDoor,2);//windowLeftDown
    Linestrip2({{1468, 556},{1506, 556}},currentScene.house4ParkingDoor,2);//windowLeftDown
    Linestrip2({{1472, 548},{1502, 548}},currentScene.house4ParkingDoor,2);//windowLeftDown
    Linestrip2({{1482, 530},{1482, 554}},currentScene.house4ParkingDoor,2);//windowLeftDown
    Linestrip2({{1482, 530},{1482, 554}},currentScene.house4ParkingDoor,2,10);//windowLeftDown

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(115, 0, 0);
    Custom_Polygon({{1470, 528}},34,28,currentScene.house4ParkingDoor);//windowLeftDown
    Custom_Polygon({{1472, 530}},30,24,currentScene.houseWindow);//windowLeftDown
    Linestrip2({{1468, 528},{1506, 528}},currentScene.house4ParkingDoor,2);//windowLeftDown
    Linestrip2({{1468, 556},{1506, 556}},currentScene.house4ParkingDoor,2);//windowLeftDown
    Linestrip2({{1472, 548},{1502, 548}},currentScene.house4ParkingDoor,2);//windowLeftDown
    Linestrip2({{1482, 530},{1482, 554}},currentScene.house4ParkingDoor,2);//windowLeftDown
    Linestrip2({{1482, 530},{1482, 554}},currentScene.house4ParkingDoor,2,10);//windowLeftDown
    glPopMatrix();
    Custom_Polygon({{1536, 520}},24,34,currentScene.house4ParkingDoor);//doorSide
    Custom_Polygon({{1538, 520}},20,32,currentScene.house2Door);//door
    Custom_Polygon({{1627, 520}},58,33,currentScene.house4ParkingDoor);//doorParking
    Linestrip2({{1687, 520},{1687, 553}},currentScene.house4Door,2);//doorParking

    Fullcircle(8,1548,570,currentScene.house4ParkingDoor);
    Fullcircle(6,1548,570,currentScene.houseWindow);


    //House-3
    Custom_Polygon({{1315,520}},125,115,currentScene.house3);//body
    Custom_Polygon({{1270, 635},{1315, 635},{1335, 668}},currentScene.house3Top1);//topleft
    Custom_Polygon({{1315, 635},{1450, 635},{1395, 668},{1335, 668}},currentScene.house3Top2);//topright
    Custom_Polygon({{1340, 635},{1347, 635},{1370, 662},{1354, 662}},currentScene.house3Top1);//topmiddle
    Custom_Polygon({{1326, 588},{1338, 578},{1452, 578},{1440, 588}},currentScene.house3Top2);//middle
    Custom_Polygon({{1340, 588},{1350, 582},{1350, 636},{1340, 636}},currentScene.house3Side);//sideMIddle
    Custom_Polygon({{1350, 640},{1350, 582},{1386, 582},{1386, 636},{1368, 660}},currentScene.house33);//middlebody
    Linestrip2({{1386, 636},{1368, 658}},currentScene.house3Top2,3);
    Custom_Polygon({{1275,520}},40,115,currentScene.house3Side);//sidemiddle
    Linestrip2({{1270, 634},{1315, 634}},currentScene.house2Door,3);//door
    Linestrip2({{1220,520},{1220, 575}},currentScene.house3Side,10);//sideleft
    Custom_Polygon({{1225,520}},85,45,currentScene.house3);//parking
    Linestrip2({{1220, 564},{1310, 564}},currentScene.houseLines,3);//topparkin
    Custom_Polygon({{1200, 595},{1215, 580},{1205, 600}},currentScene.houseTop11);//house2house3gap
    Custom_Polygon({{1220, 565},{1310, 565},{1295, 600},{1205, 600}},currentScene.house3Top2);//topParking
    Custom_Polygon({{1200, 595},{1210, 585},{1205, 600}},currentScene.houseTop11);
    Custom_Polygon({{1235,520}},60,35,currentScene.house3DoorParking);//parkingdoor
    Linestrip2({{1238, 550},{1253, 550}},currentScene.houseWindow,3);//doorwindow
    Linestrip2({{1238, 550},{1253, 550}},currentScene.houseWindow,3,20);//doorwindow
    Linestrip2({{1238, 550},{1253, 550}},currentScene.houseWindow,3,40);//doorwindow
    Linestrip2({{1297,520},{1297, 555}},currentScene.house3Top2,3);
    Linestrip2({{1315, 634},{1348, 634},{1368, 662},{1390, 634},{1450, 634}},currentScene.houseLines,3);//lines

    //window up left
    Custom_Polygon({{1323, 608}},14,14,currentScene.houseLines);
    Custom_Polygon({{1325, 610}},10,10,currentScene.houseWindow);

    //window up middle
    Custom_Polygon({{1358, 592}},20,34,currentScene.houseLines);
    Custom_Polygon({{1360, 594}},16,26,currentScene.houseWindow);
    Linestrip2({{1365, 592},{1365,623}},currentScene.houseLines,1);
    Linestrip2({{1372, 592},{1372,623}},currentScene.houseLines,1);
    Linestrip2({{1360,607},{1376,607}},currentScene.houseLines,2);

    //window up right
    Custom_Polygon({{1394, 591}},28,36,currentScene.houseLines);
    Custom_Polygon({{1396, 593}},24,32,currentScene.houseWindow);
    Linestrip2({{1392, 627},{1424, 627}},currentScene.houseLines,2);
    Linestrip2({{1396, 610},{1422, 610}},currentScene.houseLines,2);
    Linestrip2({{1408,591},{1408,626}},currentScene.houseLines,2);

    //window  down left
    Custom_Polygon({{1324, 550}},12,12,currentScene.houseLines);
    Custom_Polygon({{1326, 552}},8,8,currentScene.houseWindow);

    //window  down right
    Custom_Polygon({{1398, 526}},30,38,currentScene.houseLines);
    Custom_Polygon({{1400, 528}},26,34,currentScene.houseWindow);
    Linestrip2({{1400, 546},{1426, 546}},currentScene.houseLines,2);
    Linestrip2({{1413, 528},{1413, 562}},currentScene.houseLines,2);

    //door
    Custom_Polygon({{1348, 520}},34,46,currentScene.houseLines);
    Custom_Polygon({{1356, 520}},18,38,currentScene.house2Door);
    Linestrip2({{1352, 524},{1352, 554}},currentScene.houseWindow,3);
    Linestrip2({{1378, 524},{1378, 554}},currentScene.houseWindow,3);
    Linestrip2({{1350, 562},{1380, 562}},currentScene.houseWindow,4);


    Linestrip2({{1341, 520},{1341, 578}},currentScene.houseLines,3);
    Linestrip2({{1390, 520},{1390, 578}},currentScene.houseLines,3);
    Linestrip2({{1450, 520},{1450, 578}},currentScene.houseLines,4);
    Linestrip2({{1339, 577},{1450, 577}},currentScene.houseLines,2);

    Linestrip2({{1315,519},{1452, 519}},currentScene.houseLines,3);



    //House-2
    Custom_Polygon({{1070,520},{1070, 595},{1132, 658},{1215, 575},{1215,520}}, currentScene.house2);//body
    Custom_Polygon({{1040,520}},30,75,currentScene.house2Side);//sideleft
    Custom_Polygon({{1098, 580}},10,50,currentScene.house2Side);//sideright
    Custom_Polygon({{1145, 528}},5,57,currentScene.house2Side);//sideright
    Custom_Polygon({{1070, 590},{1080, 565},{1150, 565},{1145, 580},{1108, 580},{1098, 590}}, currentScene.houseTop11);//middleBlack
    Linestrip2({{1070, 590},{1080, 565},{1150, 565}}, currentScene.houseLines,2);//middleline
    Custom_Polygon({{1117, 590}},30,32,currentScene.house2WindowBorder);//windowtop
    Custom_Polygon({{1117, 622},{1147, 622},{1132, 629}},currentScene.house2WindowBorder);//windowtop
    Custom_Polygon({{1121, 594}},22,24,currentScene.houseWindow);//windowMiddle
    Linestrip2({{1132, 594},{1132,620}}, currentScene.house2WindowBorder,2);//windowMiddleborder
    Linestrip2({{1171, 615},{1132, 655}}, currentScene.houseTop1,5);//lineblack

    Linestrip2({{1031, 590},{1065, 590},{1132, 658},{1176, 615}}, currentScene.houseLines,4);//lines

    Custom_Polygon({{1030, 590},{1065, 590},{1132, 659},{1100, 659}}, currentScene.houseTop1);//top
    Linestrip2({{1218, 573},{1178, 614}}, currentScene.houseTop1,3);//lineblack
    Linestrip2({{1130, 575},{1141, 575},{1180, 615},{1219, 575}}, currentScene.houseLines,3);//line middle
    Custom_Polygon({{1130, 575},{1140, 575},{1180, 615},{1170, 615}}, currentScene.houseTop1);//top2middle
    Linestrip2({{1155, 549},{1220, 549}}, currentScene.houseLines,3);//lineblack
    Custom_Polygon({{1150, 560},{1155, 550},{1220, 550},{1215, 560}}, currentScene.houseTop11);//top2middleright

    Custom_Polygon({{1170, 564}},24,24,currentScene.house2WindowBorder);//windowRight
    Custom_Polygon({{1173, 566}},18,20,currentScene.houseWindow);//windowMiddleRight
    Linestrip2({{1183, 564},{1183, 588}}, currentScene.house2WindowBorder,2);//windowRightborder

    Custom_Polygon({{1164, 520}},35,25,currentScene.house2WindowBorder);//parkingDoor
    Linestrip2({{1200, 520},{1200, 546}}, currentScene.houseTop1,2);//parkingDoor
    Custom_Polygon({{1086, 528}},20,30,currentScene.house2Door);//Door
    Linestrip2({{1080, 528},{1080, 565}}, currentScene.houseLines,3);//Door
    Linestrip2({{1110, 528},{1110, 565}}, currentScene.houseLines,3);//Door
    Custom_Polygon({{1070, 520}},80,8,currentScene.house2WindowBorder);//ground


    Custom_Polygon({{1116, 536}},24,24,currentScene.house2WindowBorder);//windowLeftbottom
    Custom_Polygon({{1119, 539}},18,18,currentScene.houseWindow);//windowLeftbottom
    Linestrip2({{1128, 539},{1128, 557}}, currentScene.house2WindowBorder,2);//windowLeftbottom




    //House-1
    Custom_Polygon({{750,520}},150,95, currentScene.house1);//body
    //Custom_Polygon({{750, 520},{900,520},{900, 615},{750,615}}, currentScene.house1);//body
    Custom_Polygon({{900, 520},{900, 615},{935, 670},{965, 615},{965,520}}, currentScene.house11);//body
    Custom_Polygon({{750, 520}},20,95, currentScene.houseSide1);//houseside
    Custom_Polygon({{885, 520}},15,95, currentScene.houseSide1);//houseside
    Linestrip2({{740, 614},{900, 614}}, currentScene.houseLines,4);//
    Custom_Polygon({{740, 615},{900, 615},{870, 650},{800, 650}}, currentScene.houseTop11);//housetop
    Custom_Polygon({{885, 615},{900, 615},{935, 670},{890, 670},{870, 650}}, currentScene.houseTop1);//housetop
    Custom_Polygon({{785, 565},{900, 565},{886, 582},{770, 582}}, currentScene.houseTop11);//middle
    Linestrip2({{770, 582},{785, 565},{900, 565}}, currentScene.houseLines,2);//
    Custom_Polygon({{798, 582},{804, 575},{842, 640},{834, 640}}, currentScene.houseTop1);//middleTriangle
    Custom_Polygon({{804, 575},{842, 640},{880, 575}}, currentScene.house1);//middleTriangle
    Linestrip2({{875, 575},{842, 634}}, currentScene.houseTop11,5);//middleTriangle
    Linestrip2({{804, 576},{842, 640},{880, 576}}, currentScene.houseLines,4);//middleTriangle
    Linestrip2({{966, 615},{935, 665}}, currentScene.houseTop11,3);//rightTriangle
    Linestrip2({{970, 615},{935, 670},{900, 615}}, currentScene.houseLines,5);//rightTriangle
    //window right down
    Linestrip({{915, 525}},36,35, currentScene.houseLines,5);//window
    Custom_Polygon({{915, 525}},35,35, currentScene.houseWindow);//window
    Linestrip2({{927, 525},{927, 560}}, currentScene.houseLines,2);//windowline
    Linestrip2({{927, 525},{927, 560}}, currentScene.houseLines,2,12);//windowline
    //window right up
    Linestrip({{920, 585}},26,25, currentScene.houseLines,5);//window
    Custom_Polygon({{920, 585}},25,25, currentScene.houseWindow);//window
    Linestrip2({{932, 586},{932, 610}}, currentScene.houseLines,2);//windowline

    //window right up
    Linestrip({{926, 625}},13,13, currentScene.houseLines,5);//window
    Custom_Polygon({{926, 625}},13,13, currentScene.houseWindow);//window
    //window left-1
    Linestrip({{785, 525}},21,35, currentScene.houseLines,3);//window
    Custom_Polygon({{785, 525}},20,35, currentScene.houseWindow);//window
    Linestrip2({{785, 545},{805, 545}}, currentScene.houseLines,2);//windowline
    //window left-2
    Linestrip({{855, 525}},21,35, currentScene.houseLines,3);//window
    Custom_Polygon({{855, 525}},20,35, currentScene.houseWindow);//window
    Linestrip2({{855, 545},{875, 545}}, currentScene.houseLines,2);//windowline

    //window left up
    Linestrip({{830, 584}},16,22, currentScene.houseLines,2);//window
    Custom_Polygon({{830, 584}},16,22, currentScene.houseWindow);//window
    Linestrip2({{838, 606},{838, 584}}, currentScene.houseLines,2);//windowline

    Linestrip2({{820, 555},{820, 520},{841, 520},{841, 555},{820, 555}}, currentScene.houseLines,3);//door
    Custom_Polygon({{820, 555},{820, 520},{840, 520},{840, 555}}, currentScene.houseDoor);//door

    Linestrip2({{790, 565},{790, 520},{825, 520},{825,565}}, currentScene.houseLines,3);
    Linestrip2({{825, 520},{860, 520},{860, 565}}, currentScene.houseLines,3);
    Linestrip2({{860, 520},{900, 520},{900, 565}}, currentScene.houseLines,3);

    //car parking
    Custom_Polygon({{965, 520},{975, 520},{975, 545},{965, 545}}, currentScene.houseSide1);//side
    Custom_Polygon({{975, 520},{975, 545},{1020, 590},{1055, 550},{1055,520}}, currentScene.house11);//body
    Linestrip2({{965, 543},{975, 543},{1021, 590},{1058, 550}}, currentScene.houseLines,3);//lines
    Linestrip2({{1055, 550},{1020, 585}}, currentScene.houseTop11,2);//lines
    Linestrip2({{860, 520},{900, 520},{900, 565}}, currentScene.houseLines,3);
    Custom_Polygon({{965, 545},{975, 545},{1020, 590},{965, 590}}, currentScene.houseTop1);//top

    //window parking
    Linestrip2({{1010, 556},{1025, 556},{1025, 568},{1010, 568},{1010, 556}}, currentScene.houseLines,2);//window
    Custom_Polygon({{1010, 556},{1025, 556},{1025, 568},{1010, 568}}, currentScene.houseWindow);//window
    //Linestrip2({{838, 606},{838, 584}}, currentScene.houseLines,2);//windowline

    //door parking
    //Linestrip({{990, 520}},55,30, currentScene.houseLines,3);//window
    Custom_Polygon({{990, 520}},55,20, currentScene.houseDoor);//window
    Linestrip2({{990, 520},{990, 540},{1045, 540},{1045,520}}, currentScene.houseLines,4);//windowline
}
void Sky()
{
    // upper limits of the River to top
    Custom_Polygon({{0, 470},{1920, 470},{1920, 1080},{0, 1080}}, currentScene.skyColor, 0, 0);
}
void SunCoordinate()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(rocket_moveSuntX+100, rocket_moveSunY, 0);
    Fullcircle(80, 800, 1200, currentScene.sunColor);
    Custom_Polygon({{1099, 697},{1102, 696},{1106, 699},{1107, 707},{1115, 717},{1109, 719},{1107, 725},{1094, 725}}, currentScene.moonLayers, -296, 460);
    Custom_Polygon({{1092, 725},{1090, 735},{1086, 734},{1080, 741},{1074, 739},{1066, 742},{1060, 737},{1060, 731}}, currentScene.moonLayers, -296, 460);
    Custom_Polygon({{1060, 730},{1055, 727},{1054, 719},{1058, 711},{1060, 707},{1068, 705},{1090, 706},{1107, 719},{1082, 734},{1064, 732}}, currentScene.moonLayers, -296, 460);
    Custom_Polygon({{1070, 708},{1070, 688},{1072, 686},{1070, 679},{1074, 675},{1073, 668},{1078, 657},{1088, 656},{1084, 662},{1086, 666},{1082, 668},{1082, 672},{1091, 676},{1097, 687}}, currentScene.moonLayers, -296, 480);
    Custom_Polygon({{1165, 765},{1156, 768},{1155, 773},{1143, 772},{1143, 771},{1134, 770},{1130, 768},{1131, 763},{1121, 757},{1124, 753},{1121, 743},{1130, 733},{1141, 732}}, currentScene.moonLayers, -296, 460);
    Custom_Polygon({{1155, 740},{1154, 726},{1161, 718},{1173, 720},{1175, 730},{1181, 741}}, currentScene.moonLayers, -310, 460);
    Custom_Polygon({{1156, 742},{1162, 735},{1173, 735},{1182, 739},{1181, 748},{1184, 750},{1178, 756}}, currentScene.moonLayers, -310, 460);
    Custom_Polygon({{1042, 764},{1056, 764},{1059, 761},{1061, 765},{1068, 770},{1069, 775},{1082, 785},{1088, 786},{1077, 793},{1069, 791}}, currentScene.moonLayers, -296, 460);
    Custom_Polygon({{1066, 791},{1064, 799},{1074, 807},{1068, 810},{1061, 815},{1056, 810},{1049, 809}}, currentScene.moonLayers, -296, 450);
    Custom_Polygon({{1049, 807},{1036, 801},{1033, 795},{1034, 785},{1046, 785},{1057, 779}}, currentScene.moonLayers, -286, 450);
    Custom_Polygon({{1049, 807},{1036, 801},{1033, 795},{1034, 785},{1046, 785},{1057, 779}}, currentScene.moonLayers, -296, 420);
    Custom_Polygon({{1049, 807},{1036, 801},{1033, 795},{1034, 785},{1046, 785},{1057, 779}}, currentScene.moonLayers, -296, 430);
    glPopMatrix();
}
void Hill()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(0,40 , 0);

    //back hill right side
    Custom_Polygon({{1320, 760},{1470, 810},{1510, 790},{1510, 640},{1320, 640}},currentScene.hillColor1);//m1
    Custom_Polygon({{1510, 790},{1560, 820},{1560, 640},{1510, 640}},currentScene.hillColor1);//m1
    Custom_Polygon({{1590, 830},{1610, 860},{1645, 870},{1680, 860},{1680, 640},{1560,640},{1560, 820}},currentScene.hillColor1);//q1
    Custom_Polygon({{1680, 860},{1750, 850},{1800, 820},{1800,640},{1680,640}},currentScene.hillColor1);
    Custom_Polygon({{1800, 820},{1840, 835},{1880, 805},{1922, 785},{1922,640},{1800,640}},currentScene.hillColor1);//q1

    //hiddShadow
    Custom_Polygon({{1320, 760},{1470, 810},{1480, 790}},currentScene.hillShadow);
    Custom_Polygon({{1380, 760},{1470, 810},{1480, 790}},currentScene.hillShadow);
    Custom_Polygon({{1510, 790},{1470, 810},{1480, 790},{1580, 680}},currentScene.hillShadow1);
    Custom_Polygon({{1510, 790},{1580, 680},{1530, 802}},currentScene.hillShadow1);

    Custom_Polygon({{1600, 820},{1610, 860},{1645, 870}},currentScene.hillShadow);
    Custom_Polygon({{1560, 820},{1610, 810},{1590, 830}},currentScene.hillShadow);
    Custom_Polygon({{1590, 830},{1600, 820},{1640, 810},{1610, 860}},currentScene.hillShadow);

    Custom_Polygon({{1820, 810},{1825, 820},{1880, 760},{1860, 800},{1880, 790},{1880, 805},{1840, 835},{1800, 820}},currentScene.hillShadow2);
    Custom_Polygon({{1800, 820},{1820, 800},{1825, 820}},currentScene.hillShadow2);
    Custom_Polygon({{1820, 810},{1825, 820},{1800, 820}},currentScene.hillShadow1);
    Custom_Polygon({{1825, 820},{1840, 810},{1840, 835}},currentScene.hillShadow1);
    Custom_Polygon({{1800, 820},{1825, 820},{1840, 835}},currentScene.hillShadow1);

    Custom_Polygon({{1820, 640},{1640, 780},{1680, 770},{1730, 760},{1700, 800},{1780, 750},{1770, 780},{1790, 770},{1780, 800},{1920, 720},{1920,640}},currentScene.hillShadow2);
    Custom_Polygon({{1680, 770},{1670, 790},{1730, 760}},currentScene.hillShadow2);
    Custom_Polygon({{1860, 640},{1730, 760},{1700, 800},{1780, 750},{1900, 640}},currentScene.hillShadow3);
    Custom_Polygon({{1730, 760},{1720, 760},{1770, 730}},currentScene.hillShadow3);
    Custom_Polygon({{1790, 740},{1780, 760},{1860, 670}},currentScene.hillShadow3);


    //back hill right side
    Custom_Polygon({{0,760},{130, 800},{200, 790},{260, 640},{0,640}},currentScene.hillColor);//i
    Custom_Polygon({{200, 790},{260, 820},{330, 830},{330, 640},{260, 640}},currentScene.hillColor);//
    Custom_Polygon({{330, 830},{390, 870},{460, 895},{500, 890},{500, 640},{260, 640}},currentScene.hillColor);//p
    Custom_Polygon({{500, 890},{620, 940},{700, 970},{740, 960},{800, 940},{840, 900},{840,640},{500, 640}},currentScene.hillColor);//w
    Custom_Polygon({{840, 900},{880, 890},{920, 860},{940, 840},{940,640},{840,640}},currentScene.hillColor);//b1
    Custom_Polygon({{940, 840},{960, 840},{1000, 880},{1040, 880},{1080, 840},{1220, 820},{1380, 740},{1380,640},{940,640}},currentScene.hillColor);//j1
    Custom_Polygon({{1380, 740},{1435, 750},{1470, 770},{1660, 640},{1380,640}},currentScene.hillColor);//i1

    //hiddShadow
    Custom_Polygon({{640, 920},{640, 910},{700, 970},{620, 940},{500, 890}},currentScene.hillShadow);
    Custom_Polygon({{700, 970},{660, 940},{670, 890},{700, 940}},currentScene.hillShadow);
    Custom_Polygon({{700, 970},{740, 960},{800, 940},{780, 900},{700, 940}},currentScene.hillShadow1);
    Custom_Polygon({{800, 940},{720, 860},{840, 900}},currentScene.hillShadow1);
    Custom_Polygon({{500, 890},{460, 870},{480, 895}},currentScene.hillShadow1);
    Custom_Polygon({{480, 895},{460, 880},{460, 895}},currentScene.hillShadow1);
    Custom_Polygon({{390, 870},{330, 800},{330, 830}},currentScene.hillShadow2);
    Custom_Polygon({{310, 815},{320, 810},{330, 800},{330, 830},{295, 810}},currentScene.hillShadow2);
    Custom_Polygon({{240, 780},{220, 800},{260, 820},{330, 830},{295, 810}},currentScene.hillShadow2);
    Custom_Polygon({{180, 750},{150, 760},{200, 790},{260, 820}},currentScene.hillShadow2);
    Custom_Polygon({{60, 740},{90, 730},{120, 750},{150, 760},{200, 790},{130, 800},{60, 740}},currentScene.hillShadow2);
    Custom_Polygon({{860, 750},{920, 860},{940, 840},{900, 740}},currentScene.hillShadow3);
    Custom_Polygon({{1080, 840},{1000, 880},{1040, 880},{1220, 820},{1320, 770}},currentScene.hillShadow1);
    Custom_Polygon({{1380, 740},{1460, 720},{1435, 760}},currentScene.hillShadow3);
    Custom_Polygon({{1420, 750},{1440, 760},{1460, 760},{1480, 740},{1470, 770}},currentScene.hillShadow1);
    Custom_Polygon({{1460, 760},{1520, 735},{1470, 770}},currentScene.hillShadow1);
    Custom_Polygon({{1100, 800},{1080, 840},{1000, 880}},currentScene.hillShadow1);
    Custom_Polygon({{900, 740},{1000, 810},{1180, 740},{1180, 640},{900, 640}},currentScene.hillShadow2);
    Custom_Polygon({{1180, 740},{1160, 780},{1300, 720},{1300, 640},{1180, 640}},currentScene.hillShadow2);
    Custom_Polygon({{1300, 720},{1280, 750},{1600, 640},{1300, 640}},currentScene.hillShadow2);

    Custom_Polygon({{230, 730},{280, 760},{260, 730}},currentScene.hillShadow2);
    Custom_Polygon({{260, 730},{290, 750},{280, 710}},currentScene.hillShadow2);
    Custom_Polygon({{270, 720},{440, 820},{460, 780},{520, 800},{500, 760},{440, 640}},currentScene.hillShadow2);
    Custom_Polygon({{460, 680},{600, 780},{740, 840},{700, 780},{800, 800},{680, 700}},currentScene.hillShadow2);
    Custom_Polygon({{340, 680},{500, 780},{480, 740},{600, 820},{560, 760},{600, 780},{500, 680}},currentScene.hillShadow3);



    //fronthill
    Custom_Polygon({{0, 720},{20, 730},{60, 740},{90, 730},{90,640},{0,640}},currentScene.hillColor1);//d2
    Custom_Polygon({{90, 730},{120, 750},{150, 760},{150, 760},{180, 750},{180,640},{90,640}},currentScene.hillColor1);//g2
    Custom_Polygon({{180, 750},{210, 750},{230, 730},{230,640},{180,640}},currentScene.hillColor1);//i1
    Custom_Polygon({{230, 730},{260, 730},{320, 700},{320,640},{230,640}},currentScene.hillColor1);//l2
    Custom_Polygon({{400, 680},{480, 720},{560, 700},{580, 740},{620, 730},{620,640},{320,640},{320, 700}},currentScene.hillColor1);//l2
    Custom_Polygon({{620, 730},{660, 760},{700, 750},{780, 770},{820, 760},{900, 740},{960, 720},{1020, 700},{1160, 680},{1220, 640},{620,640}},currentScene.hillColor1);//q2
    //fronthill hiddShadow
    Custom_Polygon({{240, 640},{120, 720},{120, 735},{135, 735},{130, 745},{145, 740},{150, 760},{210, 750},{230, 730},{260, 730},{320, 700},{460, 680},{700,640}},currentScene.hillShadow4);
    Custom_Polygon({{120, 735},{130, 720},{120, 720}},currentScene.hillColor1);
    Custom_Polygon({{820, 640},{820, 760},{800, 765},{770, 760},{780, 750},{760, 730},{810, 720},{770, 700},{790, 690},{700, 640}},currentScene.hillShadow4);
    Custom_Polygon({{820, 640},{820, 760},{860, 750},{860, 730},{890, 700},{870, 700},{910, 680},{870, 690},{890, 670},{970,640}},currentScene.hillShadow4);
    Custom_Polygon({{860, 730},{890, 720},{860, 750}},currentScene.hillShadow4);

    glPopMatrix();
}
void LandRiverSide()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0, 0, 0);
    Custom_Polygon({{0,250},{1930,250},{1930,235},{0,235}},currentScene.landColor2);

    Custom_Polygon({{12,235},{8,250},{16,250},{18,240}},currentScene.groundColor1);
    Custom_Polygon({{27,237},{22,250},{35,250},{40,245}},currentScene.groundColor2);
    Custom_Polygon({{39,237},{51,250},{58,250}},currentScene.groundColor3);
    Custom_Polygon({{46,235},{64,250},{70,235}},currentScene.groundColor3);
    Custom_Polygon({{82,235},{78,244},{102,235}},currentScene.groundColor2);
    Custom_Polygon({{108,235},{89,250},{118,250},{113,235}},currentScene.groundColor1);
    Custom_Polygon({{117,235},{134,250},{142,250},{130,235}},currentScene.groundColor3);
    Custom_Polygon({{144,235},{155,246},{157,235}},currentScene.groundColor2);
    Custom_Polygon({{178,235},{160,250},{173,250},{214,235}},currentScene.groundColor1);
    Custom_Polygon({{192,250},{214,250},{223,236}},currentScene.groundColor3);
    Custom_Polygon({{244,235},{230,250},{269,250},{254,235}},currentScene.groundColor3);
    Custom_Polygon({{260,235},{275,250},{281,250},{277,235}},currentScene.groundColor2);
    glPopMatrix();

    glPushMatrix();
    glScalef(1.5,1,0);
    glTranslatef(200, 0, 0);

    Custom_Polygon({{12,235},{8,250},{16,250},{18,240}},currentScene.groundColor1);
    Custom_Polygon({{27,237},{22,250},{35,250},{40,245}},currentScene.groundColor2);
    Custom_Polygon({{39,237},{51,250},{58,250}},currentScene.groundColor3);
    Custom_Polygon({{46,235},{64,250},{70,235}},currentScene.groundColor3);
    Custom_Polygon({{82,235},{78,244},{102,235}},currentScene.groundColor2);
    Custom_Polygon({{108,235},{89,250},{118,250},{113,235}},currentScene.groundColor1);
    Custom_Polygon({{117,235},{134,250},{142,250},{130,235}},currentScene.groundColor3);
    Custom_Polygon({{144,235},{155,246},{157,235}},currentScene.groundColor2);
    Custom_Polygon({{178,235},{160,250},{173,250},{214,235}},currentScene.groundColor1);
    Custom_Polygon({{192,250},{214,250},{223,236}},currentScene.groundColor3);
    Custom_Polygon({{244,235},{230,250},{269,250},{254,235}},currentScene.groundColor3);
    Custom_Polygon({{260,235},{275,250},{281,250},{277,235}},currentScene.groundColor2);
    glPopMatrix();

    glPushMatrix();
    glScalef(1.7,1,0);
    glTranslatef(450, 0, 0);

    Custom_Polygon({{12,235},{8,250},{16,250},{18,240}},currentScene.groundColor1);
    Custom_Polygon({{27,237},{22,250},{35,250},{40,245}},currentScene.groundColor2);
    Custom_Polygon({{39,237},{51,250},{58,250}},currentScene.groundColor3);
    Custom_Polygon({{46,235},{64,250},{70,235}},currentScene.groundColor3);
    Custom_Polygon({{82,235},{78,244},{102,235}},currentScene.groundColor2);
    Custom_Polygon({{108,235},{89,250},{118,250},{113,235}},currentScene.groundColor1);
    Custom_Polygon({{117,235},{134,250},{142,250},{130,235}},currentScene.groundColor3);
    Custom_Polygon({{144,235},{155,246},{157,235}},currentScene.groundColor2);
    Custom_Polygon({{178,235},{160,250},{173,250},{214,235}},currentScene.groundColor1);
    Custom_Polygon({{192,250},{214,250},{223,236}},currentScene.groundColor3);
    Custom_Polygon({{244,235},{230,250},{269,250},{254,235}},currentScene.groundColor3);
    Custom_Polygon({{260,235},{275,250},{281,250},{277,235}},currentScene.groundColor2);
    glPopMatrix();

    glPushMatrix();
    glScalef(1.5,1,0);
    glTranslatef(850, 0, 0);

    Custom_Polygon({{12,235},{8,250},{16,250},{18,240}},currentScene.groundColor1);
    Custom_Polygon({{27,237},{22,250},{35,250},{40,245}},currentScene.groundColor2);
    Custom_Polygon({{39,237},{51,250},{58,250}},currentScene.groundColor3);
    Custom_Polygon({{46,235},{64,250},{70,235}},currentScene.groundColor3);
    Custom_Polygon({{82,235},{78,244},{102,235}},currentScene.groundColor2);
    Custom_Polygon({{108,235},{89,250},{118,250},{113,235}},currentScene.groundColor1);
    Custom_Polygon({{117,235},{134,250},{142,250},{130,235}},currentScene.groundColor3);
    Custom_Polygon({{144,235},{155,246},{157,235}},currentScene.groundColor2);
    Custom_Polygon({{178,235},{160,250},{173,250},{214,235}},currentScene.groundColor1);
    Custom_Polygon({{192,250},{214,250},{223,236}},currentScene.groundColor3);
    Custom_Polygon({{244,235},{230,250},{269,250},{254,235}},currentScene.groundColor3);
    Custom_Polygon({{260,235},{275,250},{281,250},{277,235}},currentScene.groundColor2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1700, 0, 0);

    Custom_Polygon({{12,235},{8,250},{16,250},{18,240}},currentScene.groundColor1);
    Custom_Polygon({{27,237},{22,250},{35,250},{40,245}},currentScene.groundColor2);
    Custom_Polygon({{39,237},{51,250},{58,250}},currentScene.groundColor3);
    Custom_Polygon({{46,235},{64,250},{70,235}},currentScene.groundColor3);
    Custom_Polygon({{82,235},{78,244},{102,235}},currentScene.groundColor2);
    Custom_Polygon({{108,235},{89,250},{118,250},{113,235}},currentScene.groundColor1);
    Custom_Polygon({{117,235},{134,250},{142,250},{130,235}},currentScene.groundColor3);
    Custom_Polygon({{144,235},{155,246},{157,235}},currentScene.groundColor2);
    Custom_Polygon({{178,235},{160,250},{173,250},{214,235}},currentScene.groundColor1);
    Custom_Polygon({{192,250},{214,250},{223,236}},currentScene.groundColor3);
    Custom_Polygon({{244,235},{230,250},{269,250},{254,235}},currentScene.groundColor3);
    Custom_Polygon({{260,235},{275,250},{281,250},{277,235}},currentScene.groundColor2);
    glPopMatrix();
}
void Land()
{
    Custom_Polygon({{0,680},{1930,680},{1930,250},{0,250}},currentScene.landColor);
}
void Flyover()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0, 18, 0);
    //Horizontal flyover
    Custom_Polygon({{0,460},{1930,460},{1930,440},{0,440}},currentScene.roadShoulder);
    Linestrip2({{0,460},{1930,460}},currentScene.roadShoulder2,4);
    Linestrip2({{0,444},{1930,444}},currentScene.roadColor,1.5);
    Custom_Polygon({{0,440},{1930,440},{1930,330},{0,330}},currentScene.roadColor);
    Linestrip2({{0,436},{1930,436}},currentScene.roadShadow,6);
    Custom_Polygon({{0,330},{1930,330},{1930,300},{0,300}},currentScene.roadShoulder);
    Linestrip2({{0,330},{1930,330}},currentScene.roadShoulder2,6);

    //flyover pier
    Custom_Polygon({{0,300},{80,300},{40,280},{40,240},{0,240}},currentScene.roadPier1);//body
    Linestrip2({{0,297},{80,297}},currentScene.roadShoulder3,4.5);
    Custom_Polygon({{40,280},{80,300},{105,300},{55,275},{55,250},{40,240}},currentScene.roadPier2);//shade 1


    //flyover pier 2
    Custom_Polygon({{560,280},{520,300},{640,300},{600,280},{600,240},{560,240}},currentScene.roadPier1);//body
    Linestrip2({{520,297},{640,297}},currentScene.roadShoulder3,4.5);
    Custom_Polygon({{560,280},{520,300},{500,300},{545,275},{545,240},{560,240}},currentScene.roadPier2);//shade 1
    Custom_Polygon({{600,280},{640,300},{660,300},{615,275},{615,250},{600,240}},currentScene.roadPier2);//shade 2



    //flyover pier 3
    Custom_Polygon({{1320,280},{1280,300},{1400,300},{1360,280},{1360,240},{1320,240}},currentScene.roadPier1);//body
    Linestrip2({{1280,297},{1400,297}},currentScene.roadShoulder3,4.5);
    Custom_Polygon({{1320,280},{1280,300},{1265,300},{1305,275},{1305,240},{1320,240}},currentScene.roadPier2);//shade 1
    Custom_Polygon({{1360,280},{1400,300},{1415,300},{1375,275},{1375,250},{1360,240}},currentScene.roadPier2);//shade 2



    //flyover pier 4
    Custom_Polygon({{1860,280},{1820,300},{1920,300},{1920,240},{1860,240}},currentScene.roadPier1);//body
    Linestrip2({{1820,297},{1920,297}},currentScene.roadShoulder3,4.5);
    Custom_Polygon({{1860,280},{1820,300},{1805,300},{1845,275},{1845,240},{1860,240}},currentScene.roadPier2);//shade 1

    //road divider line
    Linestrip2({{0,390},{80,390}},currentScene.roadDivider,4);
    Linestrip2({{160,390},{240,390}},currentScene.roadDivider,4);
    Linestrip2({{320,390},{400,390}},currentScene.roadDivider,4);
    Linestrip2({{480,390},{560,390}},currentScene.roadDivider,4);
    Linestrip2({{640,390},{720,390}},currentScene.roadDivider,4);
    Linestrip2({{800,390},{880,390}},currentScene.roadDivider,4);
    Linestrip2({{960,390},{1040,390}},currentScene.roadDivider,4);
    Linestrip2({{1120,390},{1200,390}},currentScene.roadDivider,4);
    Linestrip2({{1280,390},{1360,390}},currentScene.roadDivider,4);
    Linestrip2({{1440,390},{1520,390}},currentScene.roadDivider,4);
    Linestrip2({{1600,390},{1680,390}},currentScene.roadDivider,4);
    Linestrip2({{1760,390},{1840,390}},currentScene.roadDivider,4);
    Linestrip2({{1900,390},{1920,390}},currentScene.roadDivider,4);

    Cars();
    glPopMatrix();
}
void RocketStation()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glScalef(1.4,1.4,0);
    glTranslatef(0, -150, 0);

    //Controm Senter
    Custom_Polygon({{22, 616},{46, 616},{46, 510},{22, 510}},currentScene.rocketGround);//black center
    Custom_Polygon({{40, 608},{54, 608},{54, 510},{40, 510}},currentScene.rocketTower1);
    Linestrip2({{55, 565},{12, 565}},currentScene.rocketBody2,10);//middle
    Linestrip2({{33.5, 570},{33.5, 510}},currentScene.rocketTower2,10);//middle
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,10);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,20);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,30);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,40);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,50);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,60);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,70);
    Linestrip2({{37, 600},{55, 600}},currentScene.rocketBody,2);
    Linestrip2({{37, 600},{37, 510}},currentScene.rocketBody,2);
    Linestrip2({{55, 600},{55, 510}},currentScene.rocketBody,2);

    glPushMatrix();
    glTranslatef(-25, 0, 0);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,10);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,20);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,30);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,40);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,50);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,60);
    Linestrip2({{55, 520},{37, 520},{55,530}},currentScene.rocketBody,1,0,70);
    Linestrip2({{37, 600},{55, 600}},currentScene.rocketBody,2);
    Linestrip2({{37, 600},{37, 510}},currentScene.rocketBody,2);
    Linestrip2({{55, 600},{55, 510}},currentScene.rocketBody,2);
    glPopMatrix();

    Custom_Polygon({{5, 532},{31, 532},{31, 510},{5, 510}},currentScene.rocketBody2);
    //tower antina
    Linestrip2({{35, 624},{35, 638}},currentScene.rocketTower2,2);
    Custom_Polygon({{28, 616},{35, 624},{35, 616}},currentScene.rocketBody2);
    Custom_Polygon({{42, 616},{35, 624},{35, 616}},currentScene.rocketTower2);

    //Rocket Ground
    Custom_Polygon({{75, 525},{205, 525},{235, 510},{45, 510}},currentScene.rocketGround);
    //Rocket stand black
    Linestrip2({{96, 640},{105, 648},{176, 648},{186, 640},{186, 525},{96, 525},{96, 640}},currentScene.rocketGround,2.5);

    //Rocket stand left
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,10);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,10);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,20);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,30);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,40);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,50);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,60);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,70);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,80);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,90);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,100);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,110);
    Linestrip2({{110,530},{90, 530},{110, 540}},currentScene.rocketBody,1,0,120);
    Linestrip2({{90, 525},{90, 658}},currentScene.rocketBody,2);
    Linestrip2({{110, 525},{110, 670}},currentScene.rocketBody,2);

    //Rocket stand right
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,10);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,20);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,30);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,40);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,50);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,60);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,70);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,80);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,90);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,100);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,110);
    Linestrip2({{175, 530},{195, 530},{175, 540}},currentScene.rocketBody,1,0,120);
    Linestrip2({{175, 525},{175, 670}},currentScene.rocketBody,2);
    Linestrip2({{195, 525},{195, 658}},currentScene.rocketBody,2);

    Linestrip2({{96, 640},{104, 648},{176, 648},{186, 640},{186, 525},{96, 525},{96, 640}},currentScene.rocketGround,1);
    glPopMatrix();
}
void Rocket()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glScalef(1.4,1.4,0);
    glTranslatef(3, -150 + rocket_moveRocketY, 0);
    Custom_Polygon({{120, 592},{140, 608},{160, 592},{165,530},{115,530}},currentScene.rocketBody);//groun body down side
    Custom_Polygon({{132, 612},{149, 612},{150, 600},{131,601}},currentScene.rocketBody);//groun body left
    Custom_Polygon({{126, 598},{131,601},{128, 530},{121,530}},currentScene.rocketBody2);//groun body left shadow
    Custom_Polygon({{153, 597},{160, 592},{165,530},{158,530}},currentScene.rocketBody2);//groun body right shadow
    Custom_Polygon({{132, 624},{132, 694},{149, 694},{149, 624}},currentScene.rocketBody); // upper body
    Linestrip2({{140, 705},{140, 720}},currentScene.rocketBody,1); //antina
    Custom_Polygon({{134, 702},{140, 710},{147, 702},{149,694},{132, 694}},currentScene.rocketHead);//head
    Custom_Polygon({{136.665, 702.418},{140, 710},{138.845, 700.846},{139.092,694},{135.143, 694}},currentScene.rocketHead2);// head shadow

    Custom_Polygon({{134, 621},{147, 621},{147, 615},{134, 615}},currentScene.rocketBody);//groun body middle white
    Custom_Polygon({{134, 621},{147, 621},{149, 624},{132, 624}},currentScene.rocketHead);//groun body middle upper
    Custom_Polygon({{132, 612},{134, 615},{147, 615},{149, 612}},currentScene.rocketHead);//groun body middle down

    //rock fire
    Custom_Polygon({{134, 530},{148, 530},{146, 526},{136, 526}},currentScene.rocketHead);
    Linestrip2({{135, 528},{147, 528}},currentScene.rocketBody,1);
    Custom_Polygon({{134, 530},{148, 530},{146, 526},{136, 526}},currentScene.rocketHead,15);
    Linestrip2({{135, 528},{147, 528}},currentScene.rocketBody,1,15);
    Custom_Polygon({{134, 530},{148, 530},{146, 526},{136, 526}},currentScene.rocketHead,-15);
    Linestrip2({{135, 528},{147, 528}},currentScene.rocketBody,1,-15);
    glPopMatrix();
}
void Road()
{
    Custom_Polygon({{1130, 90},{1120,106},{1203,61},{1185, 50},{1185, 15},{1130, 50}},currentScene.roadPier1);//body
    Linestrip2({{1125,105},{1203,61}},currentScene.roadShoulder3,8);
    Custom_Polygon({{1185, 50},{1203,61},{1214,54},{1200, 45},{1200, 25},{1185, 15}},currentScene.roadPier2);//shade 1

    Custom_Polygon({{1380,0},{340,520},{440,520},{1660,0}},currentScene.roadColor);//road
    Custom_Polygon({{1310,0},{840,262},{840,273},{1380,0}},currentScene.roadShoulder);//roadshoulderrever
    Custom_Polygon({{1360,0},{840,270},{840,273},{1380,0}},currentScene.roadShoulder2);//roadshoulderrever
    Custom_Polygon({{840,262},{330,520},{340,520},{840,270}},currentScene.roadShoulder2);//roadsider

    Custom_Polygon({{1660,0},{1030,268},{1030,278},{1730,0}},currentScene.roadShoulder);//roadshoulderrever
    Custom_Polygon({{1730,0},{1030,278},{1030,280},{1748,0}},currentScene.roadShoulder2);//roadshoulderrever
    Custom_Polygon({{1030,268},{440,520},{451,520},{1030,274}},currentScene.roadShoulder2);//roadsider

    Custom_Polygon({{340,520},{700,680},{750,680},{440,520}},currentScene.roadColor);
    Custom_Polygon({{330,520},{695,680},{700,680},{340,520}},currentScene.roadShoulder2);//roadsider
    Custom_Polygon({{440,520},{750,680},{753,680},{451,520}},currentScene.roadShoulder2);//roadsider

    //roadBorder
    Custom_Polygon({{1510,0},{393.5,520},{396.5,520},{1530,0}},currentScene.roadDivider);//roadsider
    Linestrip2({{396.5,520},{725, 680}},currentScene.roadDivider,0.5);
}
void Boat()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(rocket_moveboatX-1000, rocket_moveboatY-100, 0);
    Custom_Polygon({{1679, 268},{1719, 237},{1753, 232},{1837, 229},{1858, 248},{1847, 257},{1760, 271}}, currentScene.boatColor, 300, -60);
    Custom_Polygon({{1807, 282},{1805, 286},{1828, 288},{1820, 290},{1784, 291}}, currentScene.boatColor, 300, -65);
    Custom_Polygon({{1679, 268},{1719, 237},{1753, 232},{1837, 229},{1854, 245},{1796, 251},{1755, 248}}, currentScene.boatbottomColor, 300, -60);
    Custom_Polygon({{1822, 261},{1803, 286},{1751, 291},{1729, 271},{1764, 271}}, currentScene.boatColor, 300, -60);
    Custom_Polygon({{1751, 289},{1735, 273},{1762, 272},{1766, 288}}, currentScene.boatwindowColor, 300, -60);
    Custom_Polygon({{1772, 287},{1766, 273},{1786, 269},{1787, 285}}, currentScene.boatwindowColor, 300, -60);
    Custom_Polygon({{1790, 286},{1790, 267},{1816, 262},{1801, 285}}, currentScene.boatwindowColor, 300, -60);
    Linestrip({{1681, 265},{1732, 264},{1780, 258},{1851, 246}}, currentScene.boatbottomColor, 300, -57);
    glPopMatrix();
}

void Riverflow()
{
    //water
    Custom_Polygon({{1920, 0},{1920, 470},{0, 470},{0, 0}}, currentScene.riverColor, 0, 0);

    //moving water
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(rocket_movewatercurrentX, -200, 0);

    // manual shades
    //type 1
    Custom_Polygon({{1234, 403},{1402, 397},{1611, 413},{1414, 381}}, currentScene.rivershade1Color, -50);
    Custom_Polygon({{1234, 403},{1402, 397},{1611, 413},{1414, 381}}, currentScene.rivershade1Color, -150, 50);
    Custom_Polygon({{1234, 403},{1402, 397},{1611, 413},{1414, 381}}, currentScene.rivershade1Color, -270, 50);
    //type 2
    Custom_Polygon({{1328, 326},{1593, 337},{1897, 319},{1407, 307}}, currentScene.rivershade1Color);
    Custom_Polygon({{1328, 326},{1593, 337},{1897, 319},{1407, 307}}, currentScene.rivershade1Color, -750);

    //type 3
    Custom_Polygon({{568, 401},{829, 395},{1086, 410},{790, 374}}, currentScene.rivershade3Color);
    Custom_Polygon({{568, 401},{829, 395},{1086, 410},{790, 374}}, currentScene.rivershade3Color, -700, -50);
    Custom_Polygon({{568, 401},{829, 395},{1086, 410},{790, 374}}, currentScene.rivershade3Color, 100, -100);
    Custom_Polygon({{568, 401},{829, 395},{1086, 410},{790, 374}}, currentScene.rivershade3Color, -1200, -100);

    //type 4
    Custom_Polygon({{633, 399},{895, 433},{1216, 417},{778, 399}}, currentScene.rivershade2Color, -500, 20);
    Custom_Polygon({{633, 399},{895, 433},{1216, 417},{778, 399}}, currentScene.rivershade2Color, 200, -180);
    Custom_Polygon({{633, 399},{895, 433},{1216, 417},{778, 399}}, currentScene.rivershade2Color, 800, -200);
    Custom_Polygon({{633, 399},{895, 433},{1216, 417},{778, 399}}, currentScene.rivershade2Color, -800, -150);
    Custom_Polygon({{633, 399},{895, 433},{1216, 417},{778, 399}}, currentScene.rivershade2Color, -1200, -150);
    Custom_Polygon({{633, 399},{895, 433},{1216, 417},{778, 399}}, currentScene.rivershade2Color, -1200, -40);

    //type 5
    Custom_Polygon({{1248, 397},{1446, 388},{1676, 400},{1425, 399}}, currentScene.rivershade4Color, 0, 50);
    Custom_Polygon({{1248, 397},{1446, 388},{1676, 400},{1425, 399}}, currentScene.rivershade4Color, -700, 50);
    Custom_Polygon({{1248, 397},{1446, 388},{1676, 400},{1425, 399}}, currentScene.rivershade4Color, -1200, 20);
    Custom_Polygon({{1248, 397},{1446, 388},{1676, 400},{1425, 399}}, currentScene.rivershade4Color, -500, -10);

    glPopMatrix();
}
void StarsCoordinate()
{
    Custom_Points2({{1174, 970},{1348, 989},{1336, 945},{1435, 921},{1582, 927}}, currentScene.starsColor);
    Custom_Points2({{1174, 970},{1348, 989},{1336, 945},{1435, 921},{1582, 927}}, currentScene.starsColor, 400);
    Custom_Points2({{1174, 970},{1348, 989},{1336, 945},{1435, 921},{1582, 927}}, currentScene.starsColor, -200);
    Custom_Points2({{1174, 970},{1348, 989},{1336, 945},{1435, 921},{1582, 927}}, currentScene.starsColor, -600, -100);
    Custom_Points2({{1174, 970},{1348, 989},{1336, 945},{1435, 921},{1582, 927}}, currentScene.starsColor, -600, -150);
    Custom_Points2({{1174, 970},{1348, 989},{1336, 945},{1435, 921},{1582, 927}}, currentScene.starsColor, -1000, -150);

    Custom_Points1({{256, 1004},{349, 962},{279, 953},{283, 872},{406, 936},{406, 857},{370, 814},{341, 723},{321, 757},{584, 1021},{514, 983},{618, 970},{735, 962},
        {828, 893},{784, 1011},{915, 989},{932, 874},{835, 727},{780, 791},{854, 802},{990, 712},{918, 650},{765, 586},{1246, 561},{1374, 753},{1212, 893},
        {1306, 1030},{1369, 970},{1476, 913},{1480, 849},{1325, 870},{1240, 774},{1077, 1026},{1043, 942},{1204, 983},{1792, 761},{1779, 866},{1679, 810},
        {1603, 868},{1616, 893},{1537, 759},{1607, 744},{1514, 684},{1467, 537},{1376, 631},{1276, 667},{1249, 603},{1344, 535},{1823, 1011},{1872, 927},
        {1887, 1028},{1450, 1075}}, currentScene.starsColor, -150);
}

// <<< --- END OF Rocket DRAWING FUNCTIONS --- >>>










// <<< --- FUTURE CITY SCENE DRAWING & HELPER FUNCTIONS --- >>>

void midPointEllipse(float xradius, float yradius, float q)
{
    glBegin(GL_POLYGON);

    for(int i=0; i < 360; i++)
    {
        //convert degrees into radians
        float degInRad = i*PI/180.0/q;
        glVertex2f(cos(degInRad)*xradius,sin(degInRad)*yradius);
    }
    glEnd();
}

void drawFutureCityScene() {
    
    glColor3ub(future_back1, future_back2, future_back3);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f,1.0f);
    glVertex2f(-1.0f,-1.0f);
    glVertex2f(1.0f,-1.0f);
    glVertex2f(1.0f,1.0f);
    glEnd();

    // Horizon line
    glColor3ub(53, 67, 184); // C2
    glLineWidth(8);
    glBegin(GL_LINES);
    glVertex2f(-1.0f,-0.6f);
    glVertex2f(1.0f,-0.6f);
    glEnd();

    glPointSize(2);
    glBegin(GL_POINTS);

    // Use global variable for star color
    glColor3ub(future_starc1, future_starc2, future_starc3);
    glVertex2f(-0.0f,0.79f);
    glVertex2f(-0.0f,0.9f);
    glVertex2f(-0.07f,0.9f);
    glVertex2f(-0.07f,0.7f);
    glVertex2f(-0.03f,0.75f);
    glVertex2f(-0.03f,0.85f);
    glVertex2f(-0.03f,0.95f);
    glVertex2f(-0.13f,0.79f);
    glVertex2f(-0.13f,0.9f);
    glVertex2f(-0.23f,0.75f);
    glVertex2f(-0.23f,0.85f);
    glVertex2f(-0.33f,0.65f);
    glVertex2f(-0.33f,0.785f);
    glVertex2f(-0.33f,0.958f);
    glVertex2f(-0.43f,0.55f);
    glVertex2f(-0.43f,0.75f);
    glVertex2f(-0.43f,0.89f);
    glVertex2f(-0.55f,0.47f);
    glVertex2f(-0.55f,0.67f);
    glVertex2f(-0.55f,0.87f);
    glVertex2f(-0.5f,0.42f);
    glVertex2f(-0.58f,0.45f);
    glVertex2f(-0.58f,0.65f);
    glVertex2f(-0.58f,0.95f);
    glVertex2f(-0.49f,0.51f);
    glVertex2f(-0.63f,0.525f);
    glVertex2f(-0.63f,0.725f);
    glVertex2f(-0.63f,0.925f);
    glVertex2f(-0.73f,0.425f);
    glVertex2f(-0.73f,0.525f);
    glVertex2f(-0.73f,0.725f);
    glVertex2f(-0.83f,0.35f);
    glVertex2f(-0.83f,0.46f);
    glVertex2f(-0.83f,0.56f);
    glVertex2f(-0.83f,0.76f);
    glVertex2f(-0.83f,0.96f);
    glVertex2f(-0.93f,0.485f);
    glVertex2f(-0.93f,0.585f);
    glVertex2f(-0.93f,0.785f);
    glVertex2f(-0.98f,0.885f);
    glVertex2f(-0.98f,0.285f);
    glVertex2f(0.0f,0.79f);
    glVertex2f(0.0f,0.9f);
    glVertex2f(0.07f,0.9f);
    glVertex2f(0.07f,0.7f);
    glVertex2f(0.03f,0.75f);
    glVertex2f(0.03f,0.85f);
    glVertex2f(0.03f,0.95f);
    glVertex2f(0.13f,0.79f);
    glVertex2f(0.13f,0.9f);
    glVertex2f(0.23f,0.75f);
    glVertex2f(0.23f,0.85f);
    glVertex2f(0.33f,0.65f);
    glVertex2f(0.33f,0.785f);
    glVertex2f(0.33f,0.958f);
    glVertex2f(0.43f,0.55f);
    glVertex2f(0.43f,0.75f);
    glVertex2f(0.43f,0.89f);
    glVertex2f(0.55f,0.47f);
    glVertex2f(0.55f,0.67f);
    glVertex2f(0.55f,0.87f);
    glVertex2f(0.5f,0.42f);
    glVertex2f(0.58f,0.45f);
    glVertex2f(0.58f,0.65f);
    glVertex2f(0.58f,0.95f);
    glVertex2f(0.49f,0.51f);
    glVertex2f(0.63f,0.525f);
    glVertex2f(0.63f,0.725f);
    glVertex2f(0.63f,0.925f);
    glVertex2f(0.73f,0.425f);
    glVertex2f(0.73f,0.525f);
    glVertex2f(0.73f,0.725f);
    glVertex2f(0.83f,0.35f);
    glVertex2f(0.83f,0.46f);
    glVertex2f(0.83f,0.56f);
    glVertex2f(0.83f,0.76f);
    glVertex2f(0.83f,0.96f);
    glVertex2f(0.93f,0.485f);
    glVertex2f(0.93f,0.585f);
    glVertex2f(0.93f,0.785f);
    glVertex2f(0.98f,0.885f);
    glVertex2f(0.98f,0.285f);
    glEnd();

    // Use global variable for building silhouettes
    glColor3ub(future_sky1, future_sky2, future_sky3);
    glBegin(GL_POLYGON);//..........................................building
    glVertex2f(-1.0f,-0.59f);
    glVertex2f(-1.0f,0.2f);
    glVertex2f(-0.98f,0.2f);
    glVertex2f(-0.98f,0.15f);
    glVertex2f(-0.95f,0.18f);
    glVertex2f(-0.92f,0.15f);
    glVertex2f(-0.92f,0.3f);
    glVertex2f(-0.89f,0.3f);
    glVertex2f(-0.89f,0.27f);
    glVertex2f(-0.86f,0.27f);
    glVertex2f(-0.86f,0.25f);
    glVertex2f(-0.83f,0.25f);
    glVertex2f(-0.83f,0.2f);
    glVertex2f(-0.8f,0.23f);
    glVertex2f(-0.77f,0.2f);
    glVertex2f(-0.74f,0.2f);
    glVertex2f(-0.74f,0.35f);
    glVertex2f(-0.7f,0.35f);
    glVertex2f(-0.7f,0.2f);
    glVertex2f(-0.67f,0.2f);
    glVertex2f(1.0f,-0.59f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.67f,-0.59f);
    glVertex2f(-0.67f,0.4f);
    glVertex2f(-0.61f,0.4f);
    glVertex2f(-0.61f,0.3f);
    glVertex2f(-0.55f,0.3f);
    glVertex2f(-0.55f,0.25f);
    glVertex2f(-0.53f,0.25f);
    glVertex2f(-0.51f,0.2f);
    glVertex2f(-0.46f,0.2f);
    glVertex2f(1.0f,-0.59f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.46f,-0.59f);
    glVertex2f(-0.46f,0.5f);
    glVertex2f(-0.35f,0.5f);
    glVertex2f(-0.35f,0.25f);
    glVertex2f(-0.3f,0.25f);
    glVertex2f(1.0f,-0.59f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.3f,-0.59f);
    glVertex2f(-0.3f,0.5f);
    glVertex2f(-0.25f,0.5f);
    glVertex2f(-0.25f,0.53f);
    glVertex2f(-0.23f,0.53f);
    glVertex2f(-0.23f,0.25f);
    glVertex2f(-0.23f,0.4f);
    glVertex2f(-0.14f,0.4f);
    glVertex2f(-0.14f,0.25f);
    glVertex2f(-0.1f,0.25f);
    glVertex2f(-0.1f,0.2f);
    glVertex2f(-0.05f,0.2f);
    glVertex2f(1.0f,-0.59f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.05f,-0.59f);
    glVertex2f(-0.05f,0.5f);
    glVertex2f(0.05f,0.5f);
    glVertex2f(0.05f,0.25f);
    glVertex2f(1.0f,-0.59f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(0.3f,-0.59f);
    glVertex2f(0.3f,0.5f);
    glVertex2f(0.25f,0.5f);
    glVertex2f(0.25f,0.53f);
    glVertex2f(0.23f,0.53f);
    glVertex2f(0.23f,0.25f);
    glVertex2f(0.23f,0.4f);
    glVertex2f(0.14f,0.4f);
    glVertex2f(0.14f,0.25f);
    glVertex2f(0.1f,0.25f);
    glVertex2f(0.1f,0.2f);
    glVertex2f(0.05f,0.2f);
    glVertex2f(-1.0f,-0.59f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(0.46f,-0.59f);
    glVertex2f(0.46f,0.5f);
    glVertex2f(0.35f,0.5f);
    glVertex2f(0.35f,0.25f);
    glVertex2f(0.3f,0.25f);
    glVertex2f(-1.0f,-0.59f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(0.67f,-0.59f);
    glVertex2f(0.67f,0.4f);
    glVertex2f(0.61f,0.4f);
    glVertex2f(0.61f,0.3f);
    glVertex2f(0.55f,0.3f);
    glVertex2f(0.55f,0.25f);
    glVertex2f(0.53f,0.25f);
    glVertex2f(0.51f,0.2f);
    glVertex2f(0.46f,0.2f);
    glVertex2f(-1.0f,-0.59f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(1.0f,-0.59f);
    glVertex2f(1.0f,0.2f);
    glVertex2f(0.98f,0.2f);
    glVertex2f(0.98f,0.15f);
    glVertex2f(0.95f,0.18f);
    glVertex2f(0.92f,0.15f);
    glVertex2f(0.92f,0.3f);
    glVertex2f(0.89f,0.3f);
    glVertex2f(0.89f,0.27f);
    glVertex2f(0.86f,0.27f);
    glVertex2f(0.86f,0.25f);
    glVertex2f(0.83f,0.25f);
    glVertex2f(0.83f,0.2f);
    glVertex2f(0.8f,0.23f);
    glVertex2f(0.77f,0.2f);
    glVertex2f(0.74f,0.2f);
    glVertex2f(0.74f,0.35f);
    glVertex2f(0.7f,0.35f);
    glVertex2f(0.7f,0.2f);
    glVertex2f(0.67f,0.2f);
    glVertex2f(-1.0f,-0.59f);
    glEnd();

    glColor3ub(89, 114, 222); // C3
    glBegin(GL_QUADS);
    glVertex2f(-0.75f,-0.59f);
    glVertex2f(-0.5f,-0.59f);
    glVertex2f(-0.51f,-0.4f);
    glVertex2f(-0.74f,-0.4f);

    glVertex2f(-0.72f,0.3f);
    glVertex2f(-0.52f,0.3f);
    glVertex2f(-0.52f,0.4f);
    glVertex2f(-0.72f,0.4f);

    glColor3ub(142, 155, 232); // C4
    glVertex2f(-0.74f,-0.4f);
    glVertex2f(-0.51f,-0.4f);
    glVertex2f(-0.52f,0.3f);
    glVertex2f(-0.72f,0.3f);
    glEnd();

glPushMatrix();
    glTranslatef(-0.62,0.45,0);
    glScalef(0.08,0.08,1);
    midPointEllipse(4, 1, 1);
glPopMatrix();

    glColor3ub(241, 239, 249); // C6 (Highlight color)
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.6f,0.55f);
    glVertex2f(-0.64f,0.55f);
    glVertex2f(-0.62f,0.7f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(-0.95f,0.43f);
    glVertex2f(-0.3f,0.43f);
    glVertex2f(-0.3f,0.48f);
    glVertex2f(-0.95f,0.48f);

    glColor3ub(142, 155, 232); // C4
    glVertex2f(-0.66f,0.55f);
    glVertex2f(-0.58f,0.55f);
    glVertex2f(-0.58f,0.5f);
    glVertex2f(-0.66f,0.5f);

    glColor3ub(241, 239, 249); // C6 (Lights)
    glVertex2f(-0.93f,0.44f);
    glVertex2f(-0.83f,0.44f);
    glVertex2f(-0.83f,0.47f);
    glVertex2f(-0.93f,0.47f);

    glVertex2f(-0.58f,0.44f);
    glVertex2f(-0.68f,0.44f);
    glVertex2f(-0.68f,0.47f);
    glVertex2f(-0.58f,0.47f);

    glVertex2f(-0.43f,0.44f);
    glVertex2f(-0.33f,0.44f);
    glVertex2f(-0.33f,0.47f);
    glVertex2f(-0.43f,0.47f);

    glVertex2f(-0.6f,0.31f);
    glVertex2f(-0.63f,0.31f);
    glVertex2f(-0.63f,0.34f);
    glVertex2f(-0.6f,0.34f);

    glVertex2f(-0.65f,0.31f);
    glVertex2f(-0.68f,0.31f);
    glVertex2f(-0.68f,0.34f);
    glVertex2f(-0.65f,0.34f);

    glVertex2f(-0.7f,0.31f);
    glVertex2f(-0.72f,0.31f);
    glVertex2f(-0.72f,0.34f);
    glVertex2f(-0.7f,0.34f);

    glVertex2f(-0.55f,0.31f);
    glVertex2f(-0.58f,0.31f);
    glVertex2f(-0.58f,0.34f);
    glVertex2f(-0.55f,0.34f);

    glVertex2f(-0.52f,0.31f);
    glVertex2f(-0.53f,0.31f);
    glVertex2f(-0.53f,0.34f);
    glVertex2f(-0.52f,0.34f);

    glVertex2f(-0.6f,0.21f);
    glVertex2f(-0.63f,0.21f);
    glVertex2f(-0.63f,0.24f);
    glVertex2f(-0.6f,0.24f);

    glVertex2f(-0.6f,0.11f);
    glVertex2f(-0.63f,0.11f);
    glVertex2f(-0.63f,0.14f);
    glVertex2f(-0.6f,0.14f);

    glVertex2f(-0.6f,0.01f);
    glVertex2f(-0.63f,0.01f);
    glVertex2f(-0.63f,0.04f);
    glVertex2f(-0.6f,0.04f);

    glVertex2f(-0.6f,-0.05f);
    glVertex2f(-0.63f,-0.05f);
    glVertex2f(-0.63f,-0.08f);
    glVertex2f(-0.6f,-0.08f);

    glVertex2f(-0.6f,-0.15f);
    glVertex2f(-0.63f,-0.15f);
    glVertex2f(-0.63f,-0.18f);
    glVertex2f(-0.6f,-0.18f);

    glVertex2f(-0.6f,-0.25f);
    glVertex2f(-0.63f,-0.25f);
    glVertex2f(-0.63f,-0.28f);
    glVertex2f(-0.6f,-0.28f);

    glVertex2f(-0.6f,-0.35f);
    glVertex2f(-0.63f,-0.35f);
    glVertex2f(-0.63f,-0.38f);
    glVertex2f(-0.6f,-0.38f);

    glVertex2f(-0.51f,-0.45f);
    glVertex2f(-0.6f,-0.45f);
    glVertex2f(-0.6f,-0.59f);
    glVertex2f(-0.51f,-0.59f);

    glEnd();
glPushMatrix();
    glTranslatef(-0.95,-0.59,0);
    glBegin(GL_POLYGON);
    for(int i=0;i<200;i++)
        {
            glColor3ub(142, 155, 232); // C4
            float pi=3.1416/2;
            float A=(i*2*pi)/200;
            float r=0.4;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x,y);
        }

    glEnd();
    glBegin(GL_POLYGON);
    for(int i=0;i<200;i++)
        {
            glColor3ub(241, 239, 249); // C6
            float pi=3.1416/4;
            float A=(i*2*pi)/200;
            float r=0.4;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x,y);
        }

    glEnd();
glPopMatrix();

    glColor3ub(241, 239, 249); // C6
    glBegin(GL_QUADS);
    glVertex2f(-0.65f,-0.33f);
    glVertex2f(-1.0f,-0.33f);
    glVertex2f(-1.0f,-0.38f);
    glVertex2f(-0.6f,-0.38f);

    glVertex2f(-0.65f,-0.33f);
    glVertex2f(-1.0f,-0.33f);
    glVertex2f(-1.0f,-0.38f);
    glVertex2f(-0.6f,-0.38f);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(-1.0f,-0.55f);
    glVertex2f(-0.95f,-0.55f);
    glVertex2f(-0.95f,-0.5f);
    glVertex2f(-1.0f,-0.5f);

    glVertex2f(-0.8f,-0.55f);
    glVertex2f(-0.65f,-0.55f);
    glVertex2f(-0.65f,-0.5f);
    glVertex2f(-0.8f,-0.5f);

    glEnd();

    glColor3ub(241, 239, 249); // C6
glPushMatrix();
    glTranslatef(-0.28,-0.59,0);
    glScalef(0.08,0.08,1);
    midPointEllipse(3, 12, 2);
glPopMatrix();
    glColor3ub(142, 155, 232); // C4
glPushMatrix();
    glTranslatef(-0.28,-0.59,0);
    glScalef(0.05,0.07,1);
    midPointEllipse(3, 12, 2);
glPopMatrix();
    glColor3ub(241, 239, 249); // C6

    glBegin(GL_TRIANGLES);
    glVertex2f(-0.34f,-0.18f);
    glVertex2f(-0.22f,-0.18f);
    glVertex2f(-0.285f,0.18f);

    glEnd();

    glBegin(GL_QUADS);

    glVertex2f(-0.20f,-0.23f);
    glVertex2f(-0.36f,-0.23f);
    glVertex2f(-0.36f,-0.28f);
    glVertex2f(-0.20f,-0.28f);

    glVertex2f(-0.18f,-0.33f);
    glVertex2f(-0.38f,-0.33f);
    glVertex2f(-0.38f,-0.38f);
    glVertex2f(-0.18f,-0.38f);

    glVertex2f(-0.16f,-0.43f);
    glVertex2f(-0.40f,-0.43f);
    glVertex2f(-0.40f,-0.48f);
    glVertex2f(-0.16f,-0.48f);

    glVertex2f(-0.15f,-0.53f);
    glVertex2f(-0.41f,-0.53f);
    glVertex2f(-0.41f,-0.58f);
    glVertex2f(-0.15f,-0.58f);

    glEnd();

    glBegin(GL_QUADS);

    glVertex2f(-0.20f,-0.23f);
    glVertex2f(-0.36f,-0.23f);
    glVertex2f(-0.36f,-0.28f);
    glVertex2f(-0.20f,-0.28f);

    glVertex2f(-0.18f,-0.33f);
    glVertex2f(-0.38f,-0.33f);
    glVertex2f(-0.38f,-0.38f);
    glVertex2f(-0.18f,-0.38f);

    glVertex2f(-0.16f,-0.43f);
    glVertex2f(-0.40f,-0.43f);
    glVertex2f(-0.40f,-0.48f);
    glVertex2f(-0.16f,-0.48f);

    glVertex2f(-0.15f,-0.53f);
    glVertex2f(-0.41f,-0.53f);
    glVertex2f(-0.41f,-0.58f);
    glVertex2f(-0.15f,-0.58f);

    glEnd();

glPushMatrix();
    glTranslatef(0.105,0.51,0);
    glBegin(GL_POLYGON);
    for(int i=0;i<200;i++)
        {
            glColor3ub(142, 155, 232); // C4
            float pi=3.1416;
            float A=(i*2*pi)/200;
            float r=0.2;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x,y);
        }

    glEnd();
    glBegin(GL_POLYGON);
    for(int i=0;i<200;i++)
        {
            glColor3ub(241, 239, 249); // C6
            float pi=3.1416/2;
            float A=(i*2*pi)/200;
            float r=0.17;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x,y);
        }

    glEnd();
glPopMatrix();

    glBegin(GL_QUADS);

    glColor3ub(89, 114, 222); // C3
    glVertex2f(-0.05f,-0.59f);
    glVertex2f(0.26f,-0.59f);
    glVertex2f(0.26f,-0.38f);
    glVertex2f(-0.05f,-0.38f);

    glColor3ub(204, 206, 240); // C5
    glVertex2f(-0.05f,-0.38f);
    glVertex2f(0.26f,-0.38f);
    glVertex2f(0.26f,0.38f);
    glVertex2f(-0.05f,0.38f);

    glColor3ub(142, 155, 232); // C4
    glVertex2f(-0.15f,0.4f);
    glVertex2f(0.36f,0.4f);
    glVertex2f(0.36f,0.33f);
    glVertex2f(-0.15f,0.33f);

    glEnd();

    glBegin(GL_LINES);

    glColor3ub(142, 155, 232); // C4
    glVertex2f(-0.05f,0.59f);
    glVertex2f(0.26f,0.59f);

    glVertex2f(0.0f,0.5f);
    glVertex2f(0.07f,0.68f);

    glVertex2f(0.2f,0.5f);
    glVertex2f(0.13f,0.68f);

    glEnd();

    glBegin(GL_QUADS);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(-0.15f,0.39f);
    glVertex2f(-0.1f,0.39f);
    glVertex2f(-0.1f,0.34f);
    glVertex2f(-0.15f,0.34f);

    glVertex2f(-0.05f,0.39f);
    glVertex2f(-0.0f,0.39f);
    glVertex2f(-0.0f,0.34f);
    glVertex2f(-0.05f,0.34f);

    glVertex2f(0.05f,0.39f);
    glVertex2f(0.1f,0.39f);
    glVertex2f(0.1f,0.34f);
    glVertex2f(0.05f,0.34f);

    glVertex2f(0.15f,0.39f);
    glVertex2f(0.2f,0.39f);
    glVertex2f(0.2f,0.34f);
    glVertex2f(0.15f,0.34f);

    glVertex2f(0.25f,0.39f);
    glVertex2f(0.3f,0.39f);
    glVertex2f(0.3f,0.34f);
    glVertex2f(0.25f,0.34f);

    glVertex2f(0.35f,0.39f);
    glVertex2f(0.36f,0.39f);
    glVertex2f(0.36f,0.34f);
    glVertex2f(0.35f,0.34f);

    glVertex2f(-0.0f,0.28f);
    glVertex2f(-0.05f,0.28f);
    glVertex2f(-0.05f,0.2f);
    glVertex2f(-0.0f,0.2f);

    glVertex2f(0.21f,0.28f);
    glVertex2f(0.26f,0.28f);
    glVertex2f(0.26f,0.2f);
    glVertex2f(0.21f,0.2f);

    glVertex2f(-0.0f,0.18f);
    glVertex2f(-0.05f,0.18f);
    glVertex2f(-0.05f,0.1f);
    glVertex2f(-0.0f,0.1f);

    glVertex2f(0.21f,0.18f);
    glVertex2f(0.26f,0.18f);
    glVertex2f(0.26f,0.1f);
    glVertex2f(0.21f,0.1f);

    glVertex2f(-0.0f,0.08f);
    glVertex2f(-0.05f,0.08f);
    glVertex2f(-0.05f,0.0f);
    glVertex2f(-0.0f,0.0f);

    glVertex2f(0.21f,0.08f);
    glVertex2f(0.26f,0.08f);
    glVertex2f(0.26f,0.0f);
    glVertex2f(0.21f,0.0f);

    glVertex2f(-0.0f,-0.02f);
    glVertex2f(-0.05f,-0.02f);
    glVertex2f(-0.05f,-0.1f);
    glVertex2f(-0.0f,-0.1f);

    glVertex2f(0.21f,-0.02f);
    glVertex2f(0.26f,-0.02f);
    glVertex2f(0.26f,-0.1f);
    glVertex2f(0.21f,-0.1f);

    glVertex2f(-0.0f,-0.12f);
    glVertex2f(-0.05f,-0.12f);
    glVertex2f(-0.05f,-0.2f);
    glVertex2f(-0.0f,-0.2f);

    glVertex2f(0.21f,-0.12f);
    glVertex2f(0.26f,-0.12f);
    glVertex2f(0.26f,-0.2f);
    glVertex2f(0.21f,-0.2f);

    glVertex2f(-0.0f,-0.22f);
    glVertex2f(-0.05f,-0.22f);
    glVertex2f(-0.05f,-0.3f);
    glVertex2f(-0.0f,-0.3f);

    glVertex2f(0.21f,-0.22f);
    glVertex2f(0.26f,-0.22f);
    glVertex2f(0.26f,-0.3f);
    glVertex2f(0.21f,-0.3f);

    glEnd();

    glBegin(GL_QUADS);

    glColor3ub(89, 114, 222); // C3
    glVertex2f(0.3f,-0.59f);
    glVertex2f(0.4f,-0.59f);
    glVertex2f(0.4f,0.28f);
    glVertex2f(0.3f,0.28f);

    glColor3ub(204, 206, 240); // C5
    glVertex2f(0.4f,-0.59f);
    glVertex2f(0.56f,-0.59f);
    glVertex2f(0.56f,0.28f);
    glVertex2f(0.4f,0.28f);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.4f,0.28f);
    glVertex2f(0.46f,0.28f);
    glVertex2f(0.46f,0.38f);
    glVertex2f(0.4f,0.38f);

    glVertex2f(0.49f,0.28f);
    glVertex2f(0.5f,0.28f);
    glVertex2f(0.5f,0.4f);
    glVertex2f(0.49f,0.4f);

    glColor3ub(89, 114, 222); // C3
    glVertex2f(0.43f,0.25f);
    glVertex2f(0.47f,0.25f);
    glVertex2f(0.47f,0.15f);
    glVertex2f(0.43f,0.15f);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.48f,0.25f);
    glVertex2f(0.52f,0.25f);
    glVertex2f(0.52f,0.15f);
    glVertex2f(0.48f,0.15f);

    glColor3ub(89, 114, 222); // C3
    glVertex2f(0.53f,0.25f);
    glVertex2f(0.56f,0.25f);
    glVertex2f(0.56f,0.15f);
    glVertex2f(0.53f,0.15f);

    glVertex2f(0.43f,0.14f);
    glVertex2f(0.47f,0.14f);
    glVertex2f(0.47f,0.04f);
    glVertex2f(0.43f,0.04f);

    glVertex2f(0.48f,0.14f);
    glVertex2f(0.52f,0.14f);
    glVertex2f(0.52f,0.04f);
    glVertex2f(0.48f,0.04f);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.53f,0.14f);
    glVertex2f(0.56f,0.14f);
    glVertex2f(0.56f,0.04f);
    glVertex2f(0.53f,0.04f);

    glColor3ub(89, 114, 222); // C3
    glVertex2f(0.43f,0.03f);
    glVertex2f(0.47f,0.03f);
    glVertex2f(0.47f,-0.07f);
    glVertex2f(0.43f,-0.07f);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.48f,0.03f);
    glVertex2f(0.52f,0.03f);
    glVertex2f(0.52f,-0.07f);
    glVertex2f(0.48f,-0.07f);

    glColor3ub(89, 114, 222); // C3
    glVertex2f(0.53f,0.03f);
    glVertex2f(0.56f,0.03f);
    glVertex2f(0.56f,-0.07f);
    glVertex2f(0.53f,-0.07f);

    glVertex2f(0.43f,-0.08f);
    glVertex2f(0.47f,-0.08f);
    glVertex2f(0.47f,-0.18f);
    glVertex2f(0.43f,-0.18f);

    glVertex2f(0.48f,-0.08f);
    glVertex2f(0.52f,-0.08f);
    glVertex2f(0.52f,-0.18f);
    glVertex2f(0.48f,-0.18f);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.53f,-0.08f);
    glVertex2f(0.56f,-0.08f);
    glVertex2f(0.56f,-0.18f);
    glVertex2f(0.53f,-0.18f);

    glVertex2f(0.43f,-0.19f);
    glVertex2f(0.47f,-0.19f);
    glVertex2f(0.47f,-0.29f);
    glVertex2f(0.43f,-0.29f);

    glColor3ub(89, 114, 222); // C3
    glVertex2f(0.48f,-0.19f);
    glVertex2f(0.52f,-0.19f);
    glVertex2f(0.52f,-0.29f);
    glVertex2f(0.48f,-0.19f);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.53f,-0.19f);
    glVertex2f(0.56f,-0.19f);
    glVertex2f(0.56f,-0.29f);
    glVertex2f(0.53f,-0.29f);

    glVertex2f(0.43f,-0.3f);
    glVertex2f(0.47f,-0.3f);
    glVertex2f(0.47f,-0.4f);
    glVertex2f(0.43f,-0.4f);

    glColor3ub(89, 114, 222); // C3
    glVertex2f(0.48f,-0.3f);
    glVertex2f(0.52f,-0.3f);
    glVertex2f(0.52f,-0.4f);
    glVertex2f(0.48f,-0.4f);

    glVertex2f(0.53f,-0.3f);
    glVertex2f(0.56f,-0.3f);
    glVertex2f(0.56f,-0.4f);
    glVertex2f(0.53f,-0.4f);

    glVertex2f(0.43f,-0.41f);
    glVertex2f(0.47f,-0.41f);
    glVertex2f(0.47f,-0.51f);
    glVertex2f(0.43f,-0.51f);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.48f,-0.41f);
    glVertex2f(0.52f,-0.41f);
    glVertex2f(0.52f,-0.51f);
    glVertex2f(0.48f,-0.51f);

    glColor3ub(89, 114, 222); // C3
    glVertex2f(0.53f,-0.41f);
    glVertex2f(0.56f,-0.41f);
    glVertex2f(0.56f,-0.51f);
    glVertex2f(0.53f,-0.51f);

    glEnd();

glPushMatrix();
    glTranslatef(future_t1,1.05,0);
    glScalef(0.2,0.2,1);

    glColor3ub(24, 28, 80); // C1 (Flying saucer body)
glPushMatrix();
    glTranslatef(0.75,-0.5,0);
    glScalef(0.08,0.08,1);
    midPointEllipse(4, 0.5, 1);
glPopMatrix();

glPushMatrix();
    glTranslatef(0.75,-0.5,0);
    glBegin(GL_POLYGON);
    for(int i=0;i<200;i++)
        {
            glColor3ub(142, 155, 232); // C4
            float pi=3.1416/2;
            float A=(i*2*pi)/200;
            float r=0.17;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x,y);
        }

    glEnd();
glPopMatrix();

    glBegin(GL_TRIANGLES);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.53f,-0.6f);
    glVertex2f(0.57f,-0.6f);
    glVertex2f(0.555f,-0.51f);

    glVertex2f(0.93f,-0.6f);
    glVertex2f(0.97f,-0.6f);
    glVertex2f(0.955f,-0.51f);

    glVertex2f(0.73f,-0.59f);
    glVertex2f(0.77f,-0.59f);
    glVertex2f(0.755f,-0.51f);

    glEnd();

    glPointSize(2);
    glBegin(GL_POINTS);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.53f,-0.51f);
    glVertex2f(0.63f,-0.515f);
    glVertex2f(0.73f,-0.52f);
    glVertex2f(0.83f,-0.52f);
    glVertex2f(0.93f,-0.515f);
    glVertex2f(1.03f,-0.51f);

    glEnd();
glPopMatrix();

glPushMatrix();
    glTranslatef(-future_t2,1.1,0);
    glScalef(0.4,0.4,1);

    glColor3ub(24, 28, 80); // C1 (Flying saucer body)
glPushMatrix();
    glTranslatef(0.75,-0.5,0);
    glScalef(0.08,0.08,1);
    midPointEllipse(4, 0.5, 1);
glPopMatrix();

glPushMatrix();
    glTranslatef(0.75,-0.5,0);
    glBegin(GL_POLYGON);
    for(int i=0;i<200;i++)
        {
            glColor3ub(142, 155, 232); // C4
            float pi=3.1416/2;
            float A=(i*2*pi)/200;
            float r=0.17;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x,y);
        }

    glEnd();
glPopMatrix();

    glBegin(GL_TRIANGLES);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.53f,-0.6f);
    glVertex2f(0.57f,-0.6f);
    glVertex2f(0.555f,-0.51f);

    glVertex2f(0.93f,-0.6f);
    glVertex2f(0.97f,-0.6f);
    glVertex2f(0.955f,-0.51f);

    glVertex2f(0.73f,-0.59f);
    glVertex2f(0.77f,-0.59f);
    glVertex2f(0.755f,-0.51f);

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.53f,-0.51f);
    glVertex2f(0.63f,-0.515f);
    glVertex2f(0.73f,-0.52f);
    glVertex2f(0.83f,-0.52f);
    glVertex2f(0.93f,-0.515f);
    glVertex2f(1.03f,-0.51f);

    glEnd();
glPopMatrix();

glPushMatrix();
    glTranslatef(0,future_h,0);
    glScalef(future_sc,future_sc,1);
    glColor3ub(241, 239, 249); // C6
glPushMatrix();
    glTranslatef(0.75,-0.5,0);
    glScalef(0.08,0.08,1);
    midPointEllipse(4, 0.5, 1);
glPopMatrix();

glPushMatrix();
    glTranslatef(0.75,-0.5,0);
    glBegin(GL_POLYGON);
    for(int i=0;i<200;i++)
        {
            glColor3ub(142, 155, 232); // C4
            float pi=3.1416/2;
            float A=(i*2*pi)/200;
            float r=0.17;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x,y);
        }

    glEnd();
glPopMatrix();

    glBegin(GL_TRIANGLES);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.53f,-0.6f);
    glVertex2f(0.57f,-0.6f);
    glVertex2f(0.555f,-0.51f);

    glVertex2f(0.93f,-0.6f);
    glVertex2f(0.97f,-0.6f);
    glVertex2f(0.955f,-0.51f);

    glVertex2f(0.73f,-0.59f);
    glVertex2f(0.77f,-0.59f);
    glVertex2f(0.755f,-0.51f);

    glEnd();

    glPointSize(future_sz);
    glBegin(GL_POINTS);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.53f,-0.51f);
    glVertex2f(0.63f,-0.515f);
    glVertex2f(0.73f,-0.52f);
    glVertex2f(0.83f,-0.52f);
    glVertex2f(0.93f,-0.515f);
    glVertex2f(1.03f,-0.51f);

    glEnd();
glPopMatrix();

glPushMatrix();
    glTranslatef(future_t,1.2,0);
    glScalef(0.8,0.8,1);

    glColor3ub(241, 239, 249); // C6
glPushMatrix();
    glTranslatef(0.75,-0.5,0);
    glScalef(0.08,0.08,1);
    midPointEllipse(4, 0.5, 1);
glPopMatrix();

glPushMatrix();
    glTranslatef(0.75,-0.5,0);
    glBegin(GL_POLYGON);
    for(int i=0;i<200;i++)
        {
            glColor3ub(142, 155, 232); // C4
            float pi=3.1416/2;
            float A=(i*2*pi)/200;
            float r=0.17;
            float x = r * cos(A);
            float y = r * sin(A);
            glVertex2f(x,y);
        }

    glEnd();
glPopMatrix();

    glBegin(GL_TRIANGLES);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.53f,-0.6f);
    glVertex2f(0.57f,-0.6f);
    glVertex2f(0.555f,-0.51f);

    glVertex2f(0.93f,-0.6f);
    glVertex2f(0.97f,-0.6f);
    glVertex2f(0.955f,-0.51f);

    glVertex2f(0.73f,-0.59f);
    glVertex2f(0.77f,-0.59f);
    glVertex2f(0.755f,-0.51f);

    glEnd();

    glPointSize(6);
    glBegin(GL_POINTS);

    glColor3ub(241, 239, 249); // C6
    glVertex2f(0.53f,-0.51f);
    glVertex2f(0.63f,-0.515f);
    glVertex2f(0.73f,-0.52f);
    glVertex2f(0.83f,-0.52f);
    glVertex2f(0.93f,-0.515f);
    glVertex2f(1.03f,-0.51f);

    glEnd();
glPopMatrix();
}

// << --- All Drawing End ---- >>>









// <<< --- ANIMATION, KEYBOARD, and MAIN LOGIC --- >>>

// <<< NEW: The Single, Central Update Function >>>
void update(int value) {
    // --- Calculate Delta Time ---
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f; // Convert milliseconds to seconds
    lastTime = currentTime;

    // --- Scene-Specific Updates ---
    if (currentSceneView == SCENE_CITY) {
        // Cloud
        _move3 += CLOUD_SPEED * deltaTime;
        if (_move3 > 110.0f) _move3 = -50.0f;

        // Sun (only moves in evening)
        if (currentSeason == EVENING) {
            _move4 += SUN_SPEED * deltaTime;
        }

        // Boat
        _move5 += BOAT_SPEED * deltaTime;
        if (_move5 < -110.0f) _move5 = 110.0f;

        // Wave (doesn't move in winter)
        if (currentSeason != WINTER) {
            _move8 += WAVE_SPEED * deltaTime;
            if (_move8 < -110.0f) _move8 = 110.0f;
        }
        
        // Cars
        _movecar += CAR1_SPEED * deltaTime;
        if (_movecar < -110.0f) _movecar = 110.0f;

        _movecar2 += CAR2_SPEED * deltaTime;
        if (_movecar2 > 110.0f) _movecar2 = -20.0f;

        // Helicopter (Movement is key-based, rotation is time-based)
        heliX += heliSpeedX * 10.0f * deltaTime; // Multiply by a factor to make speed feel right
        heliY += heliSpeedY * 10.0f * deltaTime;
        bladeRotate += HELI_BLADE_SPEED * deltaTime;

        // Weather Effects
        if (currentSeason == SPRING) {
            for (int i = 0; i < NUM_RAINDROPS; i++) {
                raindrops[i].y -= raindrops[i].speed * RAIN_SPEED_MULTIPLIER * deltaTime;
                if (raindrops[i].y < 35.0f) { // Reset raindrop
                    raindrops[i].x = static_cast<float>(rand() % 1000) / 10.0f;
                    raindrops[i].y = 80.0f;
                }
            }
        } else if (currentSeason == AUTUMN) {
            for (int i = 0; i < NUM_LEAVES; i++) {
                leaves[i].y -= leaves[i].speed * LEAF_SPEED_MULTIPLIER * deltaTime;
                leaves[i].flutter += leaves[i].speed * 0.5f; // Flutter can remain frame-based for simplicity
                if (leaves[i].y < 35.0f) { // Reset leaf
                    leaves[i].x = static_cast<float>(rand() % 1000) / 10.0f;
                    leaves[i].y = 80.0f;
                }
            }
        } else if (currentSeason == WINTER) {
            for (int i = 0; i < NUM_SNOWFLAKES; i++) {
                snowflakes[i].y -= snowflakes[i].speed * SNOW_SPEED_MULTIPLIER * deltaTime;
                if (snowflakes[i].y < 35.0f) { // Reset snowflake
                    snowflakes[i].x = static_cast<float>(rand() % 1000) / 10.0f;
                    snowflakes[i].y = 80.0f;
                }
            }
        }
    } else if (currentSceneView == SCENE_SEA) {
        if (sea_shipMoving) {
            sea_shipX += SEA_SHIP_SPEED * deltaTime;
            if (sea_shipX > 75.0f) sea_shipX = -55.0f;
        }
        if (sea_startRotation) {
            sea_rotateAngle += SEA_WINDMILL_SPEED * deltaTime;
            if (sea_rotateAngle > 360.0f) sea_rotateAngle -= 360.0f;
        }
        if (sea_cloudMoving) {
            sea_cloudX += SEA_CLOUD_SPEED * deltaTime;
            if (sea_cloudX > 100.0f) sea_cloudX = -100.0f;
        }
        if (sea_isRaining) {
            for (int i = 0; i < sea_numDrops; i++) {
                sea_rainY[i] -= SEA_RAIN_SPEED * deltaTime;
                if (sea_rainY[i] < -40.0f) {
                    sea_rainY[i] = 40.0f;
                    sea_rainX[i] = rand() % 100 - 50;
                }
            }
        }
    } else if (currentSceneView == SCENE_ROCKET) {
        rocket_updateSun(deltaTime);
        rocket_updateBoat(deltaTime);
        rocket_updateWaterCurrent(deltaTime);
        rocket_updateRocket(deltaTime);
        rocket_updateCarWheels(deltaTime);
        rocket_updateCars(deltaTime);
    } else if (currentSceneView == SCENE_FUTURE) {
        // UFO 1
        future_t -= future_speed * 50.0f * deltaTime; // Multiplier to match original speed
        if (future_t < -2.5f) {
            future_t += 3.5f;
        }
        // UFO 2
        future_t1 -= future_speed1 * 50.0f * deltaTime;
        if (future_t1 < -2.5f) {
            future_t1 += 5.5f;
        }
        // UFO 3
        future_t2 -= future_speed2 * 50.0f * deltaTime;
        if (future_t2 < -2.5f) {
            future_t2 += 5.5f;
        }
        
        // Main UFO mouse-controlled flight
        if (future_fly == 0) { // Flying away
            if (future_h <= 1.0f) {
                future_h += FUTURE_MAIN_UFO_VERTICAL_SPEED * deltaTime;
            }
            if (future_sc > 0 && future_h >= 1.0) {
                future_sc -= FUTURE_MAIN_UFO_SCALE_SPEED * deltaTime;
                future_sz -= 1; // Point size change is better as an integer step
                if (future_sz < 1) future_sz = 1;
            }
        } else if (future_fly == 1) { // Coming closer
            if (future_sc <= 1.0f) { // Use 1.0 for scale
                future_sc += FUTURE_MAIN_UFO_SCALE_SPEED * deltaTime;
                future_sz += 1;
                if (future_sz > 7) future_sz = 7;
            }
            if (future_h >= 0.0) {
                future_h -= FUTURE_MAIN_UFO_VERTICAL_SPEED * deltaTime;
            }
        } else { // Reset
            future_h = 0;
            future_sc = 1;
            future_sz = 7;
        }
    }
    

    // --- Common Updates & Redisplay ---
    glutPostRedisplay(); // Request a redraw
    glutTimerFunc(16, update, 0); // Register the next update call (target ~60fps)
}


void specialKeyFunc(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            heliSpeedX -= 0.05f;
            if (heliSpeedX < -0.9f) heliSpeedX = -0.9f;
            break;
        case GLUT_KEY_RIGHT:
            heliSpeedX += 0.09f;
            if (heliSpeedX > 0.9f) heliSpeedX = 0.9f;
            break;
    }
    glutPostRedisplay();
}

// Add this function with your other input handlers like keyboard() and specialKeyFunc()
void mouse(int button, int state, int x, int y) {
    // Only handle mouse input if we are in the future scene
    if (currentSceneView == SCENE_FUTURE) {
        if (state == GLUT_DOWN) { // Action happens on click
            if (button == GLUT_LEFT_BUTTON) {
                future_fly = 1; // Come closer
            }
            if (button == GLUT_MIDDLE_BUTTON) {
                future_fly = -1; // Reset
            }
            if (button == GLUT_RIGHT_BUTTON) {
                future_fly = 0; // Fly away
            }
            glutPostRedisplay();
        }
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 's' || key == 'S') {
        currentSceneView = SCENE_SEA;
        cout << "Switched to Sea Journey Scene" << endl;
        glutPostRedisplay();
        return;
    }
    if (key == 'c' || key == 'C') {
        currentSceneView = SCENE_CITY;
        cout << "Switched to City View Scene" << endl;
        glutPostRedisplay();
        return;
    }
    if (key == 'r' || key == 'R') {
        currentSceneView = SCENE_ROCKET;
        cout << "Switched to Rocket Station Scene" << endl;
        glutPostRedisplay();
        return;
    }
    if (key == 'f' || key == 'F') { // <-- NEW
        currentSceneView = SCENE_FUTURE;
        cout << "Switched to Future City Scene" << endl;
        glutPostRedisplay();
        return;
    }

    if (currentSceneView == SCENE_CITY) {
        if (key == 'w' || key == 'W') {
            if (_moveM <= 10) _moveM += 1.0;
        }
        else if (key == 't' || key == 'T') {
            if (_moveM >= -6) _moveM -= 1.0;
        }
        if (key == 'u' || key == 'U') {
            if (heliSpeedY < 0.4f) heliSpeedY += 0.05f;
        }
        else if (key == 'd' || key == 'D') {
            if (heliSpeedY > -0.4f) heliSpeedY -= 0.05f;
        }
        switch (key) {
            case '1': currentSeason = EVENING; _move4 = 0.0f; cout << "Season changed to: EVENING" << endl; break;
            case '2': currentSeason = NIGHT; cout << "Season changed to: NIGHT" << endl; break;
            case '3': currentSeason = MORNING; cout << "Season changed to: MORNING" << endl; break;
            case '4': currentSeason = SPRING; cout << "Season changed to: SPRING" << endl; break;
            case '5': currentSeason = AUTUMN; cout << "Season changed to: AUTUMN" << endl; break;
            case '6': currentSeason = WINTER; cout << "Season changed to: WINTER" << endl; break;
        }
    }
    else if (currentSceneView == SCENE_SEA) {
        switch (key) {
            case 'm': sea_shipMoving = !sea_shipMoving; break;
            case 'q': sea_cloudMoving = !sea_cloudMoving; break;
            case 'o': sea_startRotation = !sea_startRotation; break;
            case 'a': sea_isRaining = !sea_isRaining; break;
            case 'u': case 'U': sea_isNightMode = !sea_isNightMode; break;
        }
    } else if (currentSceneView == SCENE_ROCKET) {
        switch(key)
       {
           case 'd':
           case 'D':
               currentScene = scenes[0]; // Day mode
               cout << "Rocket Scene: Day Mode" << endl;
               break;
           case 'n':
           case 'N':
               currentScene = scenes[1]; // Night mode
               cout << "Rocket Scene: Night Mode" << endl;
               break;
       }
   } else if (currentSceneView == SCENE_FUTURE) {
       switch (key) {
           case 'a': future_speed += 0.01f; break;
           case 'q': future_speed = 0.02f; break;
           case 'z': future_speed -= 0.01f; break;
           case 'v': future_speed1 += 0.01f; break;
           case 'w': future_speed1 = 0.01f; break;
           case 'x': future_speed1 -= 0.01f; break;
           case 'd': future_speed2 += 0.01f; break;
           case 'e': future_speed2 = 0.015f; break;
           case 'c': future_speed2 -= 0.01f; break;
           case 'n': // Dawn theme
               future_starc1=241; future_starc2=239; future_starc3=249;
               future_back1=204; future_back2=206; future_back3=240;
               future_sky1=142; future_sky2=155; future_sky3=232;
               cout << "Future City: Dawn Theme" << endl;
               break;
           case 'j': // Night theme
               future_starc1=204; future_starc2=206; future_starc3=240;
               future_sky1=53; future_sky2=67; future_sky3=184;
               future_back1=24; future_back2=28; future_back3=80;
               cout << "Future City: Night Theme" << endl;
               break;
       }
   }

    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (currentSceneView == SCENE_CITY) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 100, 0, 80);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        drawSky();
        drawSunAndMoon();
        drawStars();
        drawHelicopter();
        drawClouds();
        drawBuildings();
        drawTrees();
        drawRoad();
        drawRiverAndWaves();
        drawBoat();
        drawParkElements();
        drawCars();
        drawWeatherEffects();

    } else if (currentSceneView == SCENE_SEA) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(-50.0, 50.0, -40.0, 40.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        sea_drawSky();
        sea_drawCloud(-30.0f + sea_cloudX, 30.0f, 1.2f);
        sea_drawCloud(10.0f + sea_cloudX, 32.0f, 0.9f);
        sea_drawCloud(25.0f + sea_cloudX, 35.0f, 1.5f);
        sea_drawSun();
        sea_drawMountains();
        sea_drawLand();
        sea_drawGrass();
        sea_drawBackgroundBushes();
        sea_drawTrees();
        sea_drawWindmill(0.0, 0.0);
        sea_drawWindmill(20.0, 0.0);
        sea_drawSea();
        sea_drawCustomShip();
        sea_drawMiniBoat();
        if (sea_isRaining) {
            sea_drawRain();
        }

    } else if (currentSceneView == SCENE_ROCKET) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 1920, 0, 1080);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // CORRECTED: Call the functions without parameters
        Sky();
        StarsCoordinate();
        SunCoordinate();
        Hill();
        Riverflow();
        Land();
        LandRiverSide();
        House();
        RocketStation();
        Rocket();
        Boat();
        Road();
        Flyover();
        
    } else if (currentSceneView == SCENE_FUTURE) {
        // Set the unique coordinate system for the Future City Scene
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Call the main drawing function for this scene
        drawFutureCityScene();
    }

    glutSwapBuffers();
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 80);
    
    glMatrixMode(GL_MODELVIEW);

    initSnow();
    initLeaves();
    initRain();
    
    sea_initRain();
}


int main(int argc, char** argv) {
    cout << "--- Changing Seasons: Urban Scenery Transformation ---" << endl;
    cout << "Press 'C' for City Scene" << endl;
    cout << "Press 'S' for Sea Scene" << endl;
    cout << "Press 'B' for Rocket Station Scene" << endl;
    cout << "Press 'F' for Future City Scene" << endl; //
    cout << "-----------------------------------" << endl;
    cout << "City Controls: 1-6 for seasons, arrow keys & u/d for helicopter, w/t for boat." << endl;
    cout << "Sea Controls: m, q, o, a, u for animations." << endl;
    cout << "Rocket Controls: 'd' for Day, 'n' for Night." << endl;
    cout << "Future Controls: n/j for themes, mouse clicks for main UFO." << endl;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1920, 1080); // Window size
    glutInitWindowPosition(100, 100); // Window position
    glutCreateWindow("Changing Seasons: Urban Scenery Transformation"); // Window title

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyFunc);
    glutMouseFunc(mouse);
    
    lastTime = glutGet(GLUT_ELAPSED_TIME);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
