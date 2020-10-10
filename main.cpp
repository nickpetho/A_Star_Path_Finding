/*
 * File:   main.cpp
 * Author: Nick Petho
 *
 * Created on April 16, 2018
 */

#include <iostream>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <ctime>

#define WIDTH 24;
#define HEIGHT 24;

using namespace std;

/************* Modify this for your map *********/
/* I chose this map design based on a fictional area of Italy. It has multiple different tile types
 * and costs. There are two rivers that pass through the map that create 3 different sections. Storms
 * can also randomly happen that affect the map in a few different ways: all water tiles become impassable,
 * possibility that some bridges will be flooded over, and possibility that some trees will fall and
 * block road tiles. Tile costs vary by day and night as well. The goal of this project was to expand
 * on the A* assignment and create a much larger and dynamic map. This map is 576 total tiles compared
 * to the 64 in the assignment. I used most of the same code provided from the assignment and made
 * changes where necessary based on the size change. The heuristic is 400 lines long and was the hardest
 * part of the project. I broke the map up into 16 sectors of 6x6 tiles and multiplied the heuristic
 * based on the starting sector and the goal sector. I had to do this for every possible start-to-goal
 * sector. There were also a couple other dynamic elements I planned on adding but were too difficult to
 * do in time. Overall, I'm happy with this final map and it's dynamic elements.
 */

char map[24][24] =
            {{'M','M','M','F','F','M','M','M','M','M','W','W','F','F','F','F','F','F','F','F','C','C','C','C'},
             {'M','M','F','F','G','F','M','M','M','F','W','W','B','B','F','G','G','G','G','G','C','C','C','C'},
             {'M','F','F','G','G','F','F','M','F','F','W','W','B','B','G','G','R','R','R','R','C','C','C','C'},
             {'F','F','F','F','F','F','F','F','F','F','W','W','F','R','R','R','R','G','G','G','C','C','C','C'},
             {'C','C','C','C','F','F','G','G','G','G','W','W','F','R','G','G','G','G','G','G','G','G','R','G'},
             {'C','C','C','C','G','G','G','R','R','R','R','R','R','R','G','F','F','F','F','G','G','G','R','G'},
             {'C','C','C','C','R','R','R','R','G','G','W','W','W','W','G','F','F','F','F','G','R','R','R','G'},
             {'C','C','C','C','G','F','F','F','F','G','W','W','W','W','W','W','W','W','W','G','R','G','G','F'},
             {'G','G','R','G','G','M','F','M','F','F','G','G','W','W','W','W','W','W','W','G','R','G','F','F'},
             {'F','R','R','F','F','M','M','M','M','F','C','C','C','C','C','F','F','W','W','G','R','G','G','F'},
             {'F','R','F','F','M','M','M','M','F','F','C','C','C','C','C','G','F','W','W','G','R','R','R','F'},
             {'F','R','R','F','F','M','M','M','M','F','C','C','C','C','C','G','G','W','W','G','G','G','R','F'},
             {'B','B','R','F','M','M','M','M','M','F','C','C','C','C','C','G','G','W','W','W','W','G','R','G'},
             {'B','B','R','F','M','M','M','M','M','F','C','C','C','C','C','G','G','W','W','W','W','G','R','G'},
             {'F','F','R','F','M','M','G','M','F','F','F','F','R','G','G','G','G','F','F','W','W','G','R','G'},
             {'F','R','R','F','G','G','G','F','F','R','R','R','R','R','R','R','R','G','F','W','W','B','R','R'},
             {'W','R','W','W','W','F','F','F','R','R','F','F','G','G','G','G','R','R','G','W','W','B','B','R'},
             {'W','R','W','W','W','W','F','R','R','B','B','F','F','F','G','G','G','R','R','W','W','W','W','R'},
             {'G','R','G','G','W','W','W','R','G','B','B','F','F','F','F','G','G','G','R','W','W','W','W','R'},
             {'C','C','C','C','G','W','W','R','G','F','F','F','M','F','F','F','G','G','R','G','G','W','W','R'},
             {'C','C','C','C','G','W','W','R','F','M','M','M','M','M','F','F','C','C','C','C','G','W','W','R'},
             {'C','C','C','C','R','R','R','R','M','M','M','M','M','M','M','F','C','C','C','C','R','R','R','R'},
             {'C','C','C','C','G','W','W','F','M','M','M','M','M','M','M','F','C','C','C','C','G','W','W','F'},
             {'F','F','G','G','G','W','W','M','M','M','M','M','M','M','M','F','C','C','C','C','F','W','W','F'},};

int eastBridges[4][2] =
    {{5,10},{5,11},{20,21},{20,22}};
int westBridges[4][2] =
    {{16,1},{17,1},{21,5},{21,6}};
int trees[4][2] =
    {{10,1},{14,2},{15,10},{15,10}};

/************ Modify these to be any properties that your heuristic will use */
static double storm;
static double hasPatrol;
static int isNight;
static int startCooridnates;
static int goalCooridnates;
static int city;
/*****************************************************************************/

// Used by A* to keep track of global properties
static int startX;
static int startY;
static int goalX;
static int goalY;
static int fringeCount; // Used for statistics
static int treeCount;

// Each tile is an object in this program. Each tile has a character
// representing its type on the global map, as well as a cost to enter
// the tile (a method you will define)
class Tile {
    public:
        Tile();
        char tileType;
        double getCost();
        static void setProperties();

        /************ Used for A*. Do not modify! */
        int x;
        int y;
        int treestate;
        int parentX;
        int parentY;
        double gDist;
        int inPath;
};




/********** Method definitions for Tile class ***************/
Tile::Tile() {}

/********** Modify to return cost of entering this tile based on
 * tile type, and character/world properties defined above */
double Tile::getCost() {

    // This is an fictional map of Italy with different terrains like roads, forests, grasslands,
    // water, mountains, bandit camps, and cities.
    switch(tileType) {
        // Road tiles always cost 5 to cross because they are easy to maneuver on.
        case 'R':
            return 5.0;
            break;

        // Forest tiles cost 25 to cross in day and 40 to cross at night
        // (due to low visibility and possible danger).
        case 'F':
            if (isNight) {
                return  40.0;
            }
            else {
                return 25.0;
            }

        // City tiles cost 20 to cross in day due to crowds and 10 to cross at night
        // (due to less people out on the streets).
        case 'C':
            if (isNight) {
                return  10.0;
            }
            else {
                return 20.0;
            }

        // Grassland tiles cost 10 to cross in day and 15 to cross at night
        // (due to low visibility).
        case 'G':
            if (isNight) {
                return  15.0;
            }
            else {
                return 10.0;
            }

        // If the guard has a patrol with  him he can pass through a bandit
        // camp at a cost of 50. Otherwise it is 10000 (which is effectively infinity).
        case 'B':
            if (hasPatrol) {
                return  50.0;
            }
            else {
                return 10000;
            }
            break;

        // Water tiles always cost 100 to cross because it is very slow to swim across a river.
        // They can also be impossible to cross if a storm hits.
        case 'W':
            if (storm){
                return 10000;
            }
            else{
                return 100.0;
            }
            break;

        // Mountain tiles always cost 10000 to cross because they impossible to climb over.
        case 'M':
            return 10000;
            break;

        // Fallen tree tiles always cost 10000 to cross because the tree completely blocks the path.
        case 'T':
            return 10000;
            break;
    }
}

/*********** Modify code here to ask questions to set properties for your character,
 * as well as to set the start and the goal locations */
void setProperties() {

    cout << "Do you want to start in a city or at a specific coordinate? (1 for city, 0 for coordinate): ";
    cin >> startCooridnates;
    if(startCooridnates){
        cout << "What city? (Rome = 1, Venice = 2, Milan = 3, Florence = 4, or Naples = 5): ";
        cin >> city;
        if(city == 1){
            startX = (rand()%5)+9;
            startY = (rand()%5)+10;
        }
        else if(city == 2){
            startX = (rand()%4);
            startY = (rand()%4)+20;
        }
        else if(city == 3){
            startX = (rand()%4)+4;
            startY = (rand()%4);
        }
        else if(city == 4){
            startX = (rand()%4)+19;
            startY = (rand()%4);
        }
        else if(city == 5){
            startX = (rand()%4)+20;
            startY = (rand()%4)+16;
        }
    }
    else{
        cout << "What is the starting x location (0-23, top to bottom): ";
        cin >> startX;
        cout << "What is the starting y location (0-23, left to right): ";
        cin >> startY;
    }

    cout << "Do you want to end in a city or at a specific coordinate? (1 for city, 0 for coordinate): ";
    cin >> goalCooridnates;
    if(goalCooridnates){
        cout << "What city? (Rome = 1, Venice = 2, Milan = 3, Florence = 4, or Naples = 5): ";
        cin >> city;
        if(city == 1){
            goalX = (rand()%5)+9;
            goalY = (rand()%5)+10;
        }
        else if(city == 2){
            goalX = (rand()%4);
            goalY = (rand()%4)+20;
        }
        else if(city == 3){
            goalX = (rand()%4)+4;
            goalY = (rand()%4);
        }
        else if(city == 4){
            goalX = (rand()%4)+19;
            goalY = (rand()%4);
        }
        else if(city == 5){
            goalX = (rand()%4)+20;
            goalY = (rand()%4)+16;
        }
    }else{
        cout << "What is the goal x location (0-23, top to bottom): ";
        cin >> goalX;
        cout << "What is the goal y location (0-23, left to right): ";
        cin >> goalY;
    }

    cout << "Does the guard have a patrol with him to goes through bandit camps? (1 for yes, 0 for no): ";
    cin >> hasPatrol;
    cout << "Is it day or night? (0 for day, 1 for night): ";
    cin >> isNight;

    /*
     * This code adds the dynamic elements to the map: the storm, flooded bridges, and fallen trees.
     */
    int stormChance = (rand()%5)-1;
    if(stormChance == 0){
        storm = 1;
        cout << "Finding the best path";
        Sleep(1500);
        cout << ". ";
        Sleep(1500);
        cout << ". ";
        Sleep(1500);
        cout << ". " << endl;
        cout << "A freak storm has hit the area!" << endl;
        int treeChance = (rand()%3);
        if(treeChance == 0){
            int numOfTrees = (rand()%4)+1;
            if(numOfTrees == 1){
                int randTree = rand()%4;
                int treeX = trees[randTree][0];
                int treeY = trees[randTree][1];
                map[treeX][treeY] = 'T';
                cout << "The storm has caused one of trees to fall on the road!" << endl;
            }
            else if(numOfTrees == 2){
                int randTree1 = rand()%4;
                int treeX1 = trees[randTree1][0];
                int treeY1 = trees[randTree1][1];
                map[treeX1][treeY1] = 'T';
                int randTree2 = rand()%4;
                int treeX2 = trees[randTree2][0];
                int treeY2 = trees[randTree2][1];
                map[treeX2][treeY2] = 'T';
                cout << "The storm has caused some trees to fall on the roads!" << endl;
            }
            else if(numOfTrees == 3){
                int randTree1 = rand()%4;
                int treeX1 = trees[randTree1][0];
                int treeY1 = trees[randTree1][1];
                map[treeX1][treeY1] = 'T';
                int randTree2 = rand()%4;
                int treeX2 = trees[randTree2][0];
                int treeY2 = trees[randTree2][1];
                map[treeX2][treeY2] = 'T';
                int randTree3 = rand()%4;
                int treeX3 = trees[randTree3][0];
                int treeY3 = trees[randTree3][1];
                map[treeX3][treeY3] = 'T';
                cout << "The storm has caused some trees to fall on the roads!" << endl;
            }
            else if(numOfTrees == 4){
                int randTree1 = rand()%4;
                int treeX1 = trees[randTree1][0];
                int treeY1 = trees[randTree1][1];
                map[treeX1][treeY1] = 'T';
                int randTree2 = rand()%4;
                int treeX2 = trees[randTree2][0];
                int treeY2 = trees[randTree2][1];
                map[treeX2][treeY2] = 'T';
                int randTree3 = rand()%4;
                int treeX3 = trees[randTree3][0];
                int treeY3 = trees[randTree3][1];
                map[treeX3][treeY3] = 'T';
                int randTree4 = rand()%4;
                int treeX4 = trees[randTree4][0];
                int treeY4 = trees[randTree4][1];
                map[treeX4][treeY4] = 'T';
                cout << "The storm has caused some trees to fall on the roads!" << endl;
            }
        }
        int floodChance = (rand()%2);
        if(floodChance == 0){
            int randBridge = rand()%4;
            int eBridgeX = eastBridges[randBridge][0];
            int eBridgeY = eastBridges[randBridge][1];
            map[eBridgeX][eBridgeY] = 'W';
            int wBridgeX = westBridges[randBridge][0];
            int wBridgeY = westBridges[randBridge][1];
            map[wBridgeX][wBridgeY] = 'W';
            cout << "The storm has caused flash flooding! The rivers are impossible to cross now and some bridges have been flooded." << endl;
            cout << "Updating the map";
            Sleep(1500);
            cout << ". ";
            Sleep(1500);
            cout << ". ";
            Sleep(1500);
            cout << ". " << endl;
        }else{
            cout << "The storm has caused flash flooding! The rivers are impossible to cross now." << endl;
            cout << "Updating the map";
            Sleep(1500);
            cout << ". ";
            Sleep(1500);
            cout << ". ";
            Sleep(1500);
            cout << ". " << endl;
        }
    }
}


/***** Heuristic for determining estimated distance from the tile at (x, y)
 ***** to the goal (note that you can access the goal location using the global
 ***** variables goalX, goalY). You can also access the 2D global map.
 ***** This is the main method you will be designing in this assignment! */
int sector(int x,int y){
    if (x <= 5 && y <= 5)
        return 1;
    else if (x <= 5 && (6 <= y <= 11))
        return 2;
    else if (x <= 5 && (12 <= y <= 17))
        return 3;
    else if (x <= 5 && (18 <= y <= 23))
        return 4;
    else if ((6 <= x <= 11) && y <= 5)
        return 5;
    else if ((6 <= x <= 11) && (6 <= y <= 11))
        return 6;
    else if ((6 <= x <= 11) && (12 <= y <= 17))
        return 7;
    else if ((6 <= x <= 11) && (18 <= y <= 23))
        return 8;
    else if ((12 <= x <= 17) && y <= 5)
        return 9;
    else if ((12 <= x <= 17) && (6 <= y <= 11))
        return 10;
    else if ((12 <= x <= 17) && (12 <= y <= 17))
        return 11;
    else if ((12 <= x <= 17) && (18 <= y <= 23))
        return 12;
    else if ((18 <= x <= 23) && y <= 5)
        return 13;
    else if ((18 <= x <= 23) && (6 <= y <= 11))
        return 14;
    else if ((18 <= x <= 23) && (12 <= y <= 17))
        return 15;
    else if ((18 <= x <= 23) && (18 <= y <= 23))
        return 16;
}

double heuristic(int x, int y) {
    /*
    * This heuristic divides the map into 16 sectors and multiples the
    * manhattan heuristic times the average costs of the all the tiles
    * in that sector.
    */

    int startQ = sector(startX,startY);
    int goalQ = sector(goalX,goalY);
    int manhattan = (abs(goalX - x) + abs(goalY - y));
    int shortPath = (abs(goalX - x) + abs(goalY - y));

    // Path from S1 to S1, S2 to S2, S3 to S3, S4 to S4, S5 to S5, and so on.
    if(startQ == 1 && goalQ == 1)
        shortPath = 24* manhattan;
    else if(startQ == 2 && goalQ == 2)
        shortPath = 45 * manhattan;
    else if(startQ == 3 && goalQ == 3)
        shortPath = 18 * manhattan;
    else if(startQ == 4 && goalQ == 4)
        shortPath = 16 * manhattan;
    else if(startQ == 5 && goalQ == 5)
        shortPath = 18 * manhattan;
    else if(startQ == 6 && goalQ == 6)
        shortPath = 30 * manhattan;
    else if(startQ == 7 && goalQ == 7)
        shortPath = 58 * manhattan;
    else if(startQ == 8 && goalQ == 8)
        shortPath = 27 * manhattan;
    else if(startQ == 9 && goalQ == 9)
        shortPath = 47 * manhattan;
    else if(startQ == 10 && goalQ == 10)
        shortPath = 21 * manhattan;
    else if(startQ == 11 && goalQ == 11)
        shortPath = 18 * manhattan;
    else if(startQ == 12 && goalQ == 12)
        shortPath = 54 * manhattan;
    else if(startQ == 13 && goalQ == 13)
        shortPath = 29 * manhattan;
    else if(startQ == 14 && goalQ == 14)
        shortPath = 42 * manhattan;
    else if(startQ == 15 && goalQ == 15)
        shortPath = 23 * manhattan;
    else if(startQ == 16 && goalQ == 16)
        shortPath = 42 * manhattan;

    //Path from S1 to S2 or vice versa
    else if((startQ == 1 || startQ == 2) && (goalQ == 2 || goalQ == 1))
        shortPath = 35 * manhattan;
    //1-3 or vice versa
    else if((startQ == 1 || startQ == 3) && (goalQ == 3 || goalQ == 1))
        shortPath = 21* manhattan;
    //1-4 or vice versa
    else if((startQ == 1 || startQ == 4) && (goalQ == 4 || goalQ == 1))
        shortPath = 20* manhattan;
    //1-5 or vice versa
    else if((startQ == 1 || startQ == 5) && (goalQ == 5 || goalQ == 1))
        shortPath = 21* manhattan;
    //1-6 or vice versa
    else if((startQ == 1 || startQ == 6) && (goalQ == 6 || goalQ == 1))
        shortPath = 27* manhattan;
    //1-7 or vice versa
    else if((startQ == 1 || startQ == 7) && (goalQ == 7 || goalQ == 1))
        shortPath = 41* manhattan;
    //1-8 or vice versa
    else if((startQ == 1 || startQ == 8) && (goalQ == 8 || goalQ == 1))
        shortPath = 26* manhattan;
    //1-9 or vice versa
    else if((startQ == 1 || startQ == 9) && (goalQ == 9 || goalQ == 1))
        shortPath = 36* manhattan;
    //1-10 or vice versa
    else if((startQ == 1 || startQ == 10) && (goalQ == 10 || goalQ == 1))
        shortPath = 23* manhattan;
    //1-11 or vice versa
    else if((startQ == 1 || startQ == 11) && (goalQ == 11 || goalQ == 1))
        shortPath = 21* manhattan;
    //1-12 or vice versa
    else if((startQ == 1 || startQ == 12) && (goalQ == 12 || goalQ == 1))
        shortPath = 39* manhattan;
    //1-13 or vice versa
    else if((startQ == 1 || startQ == 13) && (goalQ == 13 || goalQ == 1))
        shortPath = 27* manhattan;
    //1-14 or vice versa
    else if((startQ == 1 || startQ == 14) && (goalQ == 14 || goalQ == 1))
        shortPath = 33* manhattan;
    //1-15 or vice versa
    else if((startQ == 1 || startQ == 15) && (goalQ == 15 || goalQ == 1))
        shortPath = 24* manhattan;
    //1-16 or vice versa
    else if((startQ == 1 || startQ == 16) && (goalQ == 16 || goalQ == 1))
        shortPath = 33* manhattan;
    //2-3 or vice versa
    else if((startQ == 2 || startQ == 3) && (goalQ == 3 || goalQ == 2))
        shortPath = 32* manhattan;
    //2-4 or vice versa
    else if((startQ == 2 || startQ == 4) && (goalQ == 4 || goalQ == 2))
        shortPath = 31* manhattan;
    //2-5 or vice versa
    else if((startQ == 2 || startQ == 5) && (goalQ == 5 || goalQ == 2))
        shortPath = 32* manhattan;
    //2-6 or vice versa
    else if((startQ == 2 || startQ == 6) && (goalQ == 6 || goalQ == 2))
        shortPath = 38* manhattan;
    //2-7 or vice versa
    else if((startQ == 2 || startQ == 7) && (goalQ == 7 || goalQ == 2))
        shortPath = 52* manhattan;
    //2-8 or vice versa
    else if((startQ == 2 || startQ == 8) && (goalQ == 8 || goalQ == 2))
        shortPath = 36* manhattan;
    //2-9 or vice versa
    else if((startQ == 2 || startQ == 9) && (goalQ == 9 || goalQ == 2))
        shortPath = 46* manhattan;
    //2-10 or vice versa
    else if((startQ == 2 || startQ == 10) && (goalQ == 10 || goalQ == 2))
        shortPath = 33* manhattan;
    //2-11 or vice versa
    else if((startQ == 2 || startQ == 11) && (goalQ == 11 || goalQ == 2))
        shortPath = 32* manhattan;
    //2-12 or vice versa
    else if((startQ == 2 || startQ == 12) && (goalQ == 12 || goalQ == 2))
        shortPath = 50* manhattan;
    //2-13 or vice versa
    else if((startQ == 2 || startQ == 13) && (goalQ == 13 || goalQ == 2))
        shortPath = 37* manhattan;
    //2-14 or vice versa
    else if((startQ == 2 || startQ == 14) && (goalQ == 14 || goalQ == 2))
        shortPath = 44* manhattan;
    //2-15 or vice versa
    else if((startQ == 2 || startQ == 15) && (goalQ == 15 || goalQ == 2))
        shortPath = 34* manhattan;
    //2-16 or vice versa
    else if((startQ == 2 || startQ == 16) && (goalQ == 16 || goalQ == 2))
        shortPath = 44* manhattan;
    //3-4 or vice versa
    else if((startQ == 3 || startQ == 4) && (goalQ == 4 || goalQ == 3))
        shortPath = 17* manhattan;
    //3-5 or vice versa
    else if((startQ == 3 || startQ == 5) && (goalQ == 5 || goalQ == 3))
        shortPath = 18* manhattan;
    //3-6 or vice versa
    else if((startQ == 3 || startQ == 6) && (goalQ == 6 || goalQ == 3))
        shortPath = 24* manhattan;
    //3-7 or vice versa
    else if((startQ == 3 || startQ == 7) && (goalQ == 7 || goalQ == 3))
        shortPath = 38* manhattan;
    //3-8 or vice versa
    else if((startQ == 3 || startQ == 8) && (goalQ == 8 || goalQ == 3))
        shortPath = 23* manhattan;
    //3-9 or vice versa
    else if((startQ == 3 || startQ == 9) && (goalQ == 9 || goalQ == 3))
        shortPath = 33* manhattan;
    //3-10 or vice versa
    else if((startQ == 3 || startQ == 10) && (goalQ == 10 || goalQ == 3))
        shortPath = 20* manhattan;
    //3-11 or vice versa
    else if((startQ == 3 || startQ == 11) && (goalQ == 11 || goalQ == 3))
        shortPath = 18* manhattan;
    //3-12 or vice versa
    else if((startQ == 3 || startQ == 12) && (goalQ == 12 || goalQ == 3))
        shortPath = 36* manhattan;
    //3-13 or vice versa
    else if((startQ == 3 || startQ == 13) && (goalQ == 13 || goalQ == 3))
        shortPath = 24* manhattan;
    //3-14 or vice versa
    else if((startQ == 3 || startQ == 14) && (goalQ == 14 || goalQ == 3))
        shortPath = 30* manhattan;
    //3-15 or vice versa
    else if((startQ == 3 || startQ == 15) && (goalQ == 15 || goalQ == 3))
        shortPath = 21* manhattan;
    //3-16 or vice versa
    else if((startQ == 3 || startQ == 16) && (goalQ == 16 || goalQ == 3))
        shortPath = 30* manhattan;
    //4-5 or vice versa
    else if((startQ == 4 || startQ == 5) && (goalQ == 5 || goalQ == 4))
        shortPath = 17* manhattan;
    //4-6 or vice versa
    else if((startQ == 4 || startQ == 6) && (goalQ == 6 || goalQ == 4))
        shortPath = 23* manhattan;
    //4-7 or vice versa
    else if((startQ == 4 || startQ == 7) && (goalQ == 7 || goalQ == 4))
        shortPath = 37* manhattan;
    //4-8 or vice versa
    else if((startQ == 4 || startQ == 8) && (goalQ == 8 || goalQ == 4))
        shortPath = 22* manhattan;
    //4-9 or vice versa
    else if((startQ == 4 || startQ == 9) && (goalQ == 9 || goalQ == 4))
        shortPath = 32* manhattan;
    //4-10 or vice versa
    else if((startQ == 4 || startQ == 10) && (goalQ == 10 || goalQ == 4))
        shortPath = 19* manhattan;
    //4-11 or vice versa
    else if((startQ == 4 || startQ == 11) && (goalQ == 11 || goalQ == 4))
        shortPath = 17* manhattan;
    //4-12 or vice versa
    else if((startQ == 4 || startQ == 12) && (goalQ == 12 || goalQ == 4))
        shortPath = 35* manhattan;
    //4-13 or vice versa
    else if((startQ == 4 || startQ == 13) && (goalQ == 13 || goalQ == 4))
        shortPath = 23* manhattan;
    //4-14 or vice versa
    else if((startQ == 4 || startQ == 14) && (goalQ == 14 || goalQ == 4))
        shortPath = 29* manhattan;
    //4-15 or vice versa
    else if((startQ == 4 || startQ == 15) && (goalQ == 15 || goalQ == 4))
        shortPath = 20* manhattan;
    //4-16 or vice versa
    else if((startQ == 4 || startQ == 16) && (goalQ == 16 || goalQ == 4))
        shortPath = 29* manhattan;
    //5-6 or vice versa
    else if((startQ == 5 || startQ == 6) && (goalQ == 6 || goalQ == 5))
        shortPath = 24* manhattan;
    //5-7 or vice versa
    else if((startQ == 5 || startQ == 7) && (goalQ == 7 || goalQ == 5))
        shortPath = 38* manhattan;
    //5-8 or vice versa
    else if((startQ == 5 || startQ == 8) && (goalQ == 8 || goalQ == 5))
        shortPath = 23* manhattan;
    //5-9 or vice versa
    else if((startQ == 5 || startQ == 9) && (goalQ == 9 || goalQ == 5))
        shortPath = 33* manhattan;
    //5-10 or vice versa
    else if((startQ == 5 || startQ == 10) && (goalQ == 10 || goalQ == 5))
        shortPath = 20* manhattan;
    //5-11 or vice versa
    else if((startQ == 5 || startQ == 11) && (goalQ == 11 || goalQ == 5))
        shortPath = 18* manhattan;
    //5-12 or vice versa
    else if((startQ == 5 || startQ == 12) && (goalQ == 12 || goalQ == 5))
        shortPath = 36* manhattan;
    //5-13 or vice versa
    else if((startQ == 5 || startQ == 13) && (goalQ == 13 || goalQ == 5))
        shortPath = 24* manhattan;
    //5-14 or vice versa
    else if((startQ == 5 || startQ == 14) && (goalQ == 14 || goalQ == 5))
        shortPath = 30* manhattan;
    //5-15 or vice versa
    else if((startQ == 5 || startQ == 15) && (goalQ == 15 || goalQ == 5))
        shortPath = 21* manhattan;
    //5-16 or vice versa
    else if((startQ == 5 || startQ == 16) && (goalQ == 16 || goalQ == 5))
        shortPath = 30* manhattan;
    //6-7 or vice versa
    else if((startQ == 6 || startQ == 7) && (goalQ == 7 || goalQ == 6))
        shortPath = 44* manhattan;
    //6-8 or vice versa
    else if((startQ == 6 || startQ == 8) && (goalQ == 8 || goalQ == 6))
        shortPath = 29* manhattan;
    //6-9 or vice versa
    else if((startQ == 6 || startQ == 9) && (goalQ == 9 || goalQ == 6))
        shortPath = 39* manhattan;
    //6-10 or vice versa
    else if((startQ == 6 || startQ == 10) && (goalQ == 10 || goalQ == 6))
        shortPath = 26* manhattan;
    //6-11 or vice versa
    else if((startQ == 6 || startQ == 11) && (goalQ == 11 || goalQ == 6))
        shortPath = 24* manhattan;
    //6-12 or vice versa
    else if((startQ == 6 || startQ == 12) && (goalQ == 12 || goalQ == 6))
        shortPath = 42* manhattan;
    //6-13 or vice versa
    else if((startQ == 6 || startQ == 13) && (goalQ == 13 || goalQ == 6))
        shortPath = 30* manhattan;
    //6-14 or vice versa
    else if((startQ == 6 || startQ == 14) && (goalQ == 14 || goalQ == 6))
        shortPath = 36* manhattan;
    //6-15 or vice versa
    else if((startQ == 6 || startQ == 15) && (goalQ == 15 || goalQ == 6))
        shortPath = 27* manhattan;
    //6-16 or vice versa
    else if((startQ == 6 || startQ == 16) && (goalQ == 16 || goalQ == 6))
        shortPath = 36* manhattan;
    //7-8 or vice versa
    else if((startQ == 7 || startQ == 8) && (goalQ == 8 || goalQ == 7))
        shortPath = 43* manhattan;
    //7-9 or vice versa
    else if((startQ == 7 || startQ == 9) && (goalQ == 9 || goalQ == 7))
        shortPath = 53* manhattan;
    //7-10 or vice versa
    else if((startQ == 7 || startQ == 10) && (goalQ == 10 || goalQ == 7))
        shortPath = 40* manhattan;
    //7-11 or vice versa
    else if((startQ == 7 || startQ == 11) && (goalQ == 11 || goalQ == 7))
        shortPath = 38* manhattan;
    //7-12 or vice versa
    else if((startQ == 7 || startQ == 12) && (goalQ == 12 || goalQ == 7))
        shortPath = 56* manhattan;
    //7-13 or vice versa
    else if((startQ == 7 || startQ == 13) && (goalQ == 13 || goalQ == 7))
        shortPath = 44* manhattan;
    //7-14 or vice versa
    else if((startQ == 7 || startQ == 14) && (goalQ == 14 || goalQ == 7))
        shortPath = 50* manhattan;
    //7-15 or vice versa
    else if((startQ == 7 || startQ == 15) && (goalQ == 15 || goalQ == 7))
        shortPath = 41* manhattan;
    //7-16 or vice versa
    else if((startQ == 7 || startQ == 16) && (goalQ == 16 || goalQ == 7))
        shortPath = 50* manhattan;
    //8-9 or vice versa
    else if((startQ == 8 || startQ == 9) && (goalQ == 9 || goalQ == 8))
        shortPath = 37* manhattan;
    //8-10 or vice versa
    else if((startQ == 8 || startQ == 10) && (goalQ == 10 || goalQ == 8))
        shortPath = 24* manhattan;
    //8-11 or vice versa
    else if((startQ == 8 || startQ == 11) && (goalQ == 11 || goalQ == 8))
        shortPath = 23* manhattan;
    //8-12 or vice versa
    else if((startQ == 8 || startQ == 12) && (goalQ == 12 || goalQ == 8))
        shortPath = 41* manhattan;
    //8-13 or vice versa
    else if((startQ == 8 || startQ == 13) && (goalQ == 13 || goalQ == 8))
        shortPath = 28* manhattan;
    //8-14 or vice versa
    else if((startQ == 8 || startQ == 14) && (goalQ == 14 || goalQ == 8))
        shortPath = 35* manhattan;
    //8-15 or vice versa
    else if((startQ == 8 || startQ == 15) && (goalQ == 15 || goalQ == 8))
        shortPath = 25* manhattan;
    //8-16 or vice versa
    else if((startQ == 8 || startQ == 16) && (goalQ == 16 || goalQ == 8))
        shortPath = 35* manhattan;
    //9-10 or vice versa
    else if((startQ == 9 || startQ == 10) && (goalQ == 10 || goalQ == 9))
        shortPath = 34* manhattan;
    //9-11 or vice versa
    else if((startQ == 9 || startQ == 11) && (goalQ == 11 || goalQ == 9))
        shortPath = 33* manhattan;
    //9-12 or vice versa
    else if((startQ == 9 || startQ == 12) && (goalQ == 12 || goalQ == 9))
        shortPath = 51* manhattan;
    //9-13 or vice versa
    else if((startQ == 9 || startQ == 13) && (goalQ == 13 || goalQ == 9))
        shortPath = 38* manhattan;
    //9-14 or vice versa
    else if((startQ == 9 || startQ == 14) && (goalQ == 14 || goalQ == 9))
        shortPath = 45* manhattan;
    //9-15 or vice versa
    else if((startQ == 9 || startQ == 15) && (goalQ == 15 || goalQ == 9))
        shortPath = 35* manhattan;
    //9-16 or vice versa
    else if((startQ == 9 || startQ == 16) && (goalQ == 16 || goalQ == 9))
        shortPath = 45* manhattan;
    //10-11 or vice versa
    else if((startQ == 10 || startQ == 11) && (goalQ == 11 || goalQ == 10))
        shortPath = 20* manhattan;
    //10-12 or vice versa
    else if((startQ == 10 || startQ == 12) && (goalQ == 12 || goalQ == 10))
        shortPath = 38* manhattan;
    //10-13 or vice versa
    else if((startQ == 10 || startQ == 13) && (goalQ == 13 || goalQ == 10))
        shortPath = 25* manhattan;
    //10-14 or vice versa
    else if((startQ == 10 || startQ == 14) && (goalQ == 14 || goalQ == 10))
        shortPath = 32* manhattan;
    //10-15 or vice versa
    else if((startQ == 10 || startQ == 15) && (goalQ == 15 || goalQ == 10))
        shortPath = 22* manhattan;
    //10-16 or vice versa
    else if((startQ == 10 || startQ == 16) && (goalQ == 16 || goalQ == 10))
        shortPath = 32* manhattan;
    //11-12 or vice versa
    else if((startQ == 11 || startQ == 12) && (goalQ == 12 || goalQ == 11))
        shortPath = 36* manhattan;
    //11-13 or vice versa
    else if((startQ == 11 || startQ == 13) && (goalQ == 13 || goalQ == 11))
        shortPath = 24* manhattan;
    //11-14 or vice versa
    else if((startQ == 11 || startQ == 14) && (goalQ == 14 || goalQ == 11))
        shortPath = 30* manhattan;
    //11-15 or vice versa
    else if((startQ == 11 || startQ == 15) && (goalQ == 15 || goalQ == 11))
        shortPath = 21* manhattan;
    //11-16 or vice versa
    else if((startQ == 11 || startQ == 16) && (goalQ == 16 || goalQ == 11))
        shortPath = 30* manhattan;
    //12-13 or vice versa
    else if((startQ == 12 || startQ == 13) && (goalQ == 13 || goalQ == 12))
        shortPath = 42* manhattan;
    //12-14 or vice versa
    else if((startQ == 12 || startQ == 14) && (goalQ == 14 || goalQ == 12))
        shortPath = 48* manhattan;
    //12-15 or vice versa
    else if((startQ == 12 || startQ == 15) && (goalQ == 15 || goalQ == 12))
        shortPath = 39* manhattan;
    //12-16 or vice versa
    else if((startQ == 12 || startQ == 16) && (goalQ == 16 || goalQ == 12))
        shortPath = 48* manhattan;
    //13-14 or vice versa
    else if((startQ == 13 || startQ == 14) && (goalQ == 14 || goalQ == 13))
        shortPath = 36* manhattan;
    //13-15 or vice versa
    else if((startQ == 13 || startQ == 15) && (goalQ == 15 || goalQ == 13))
        shortPath = 26* manhattan;
    //13-16 or vice versa
    else if((startQ == 13 || startQ == 16) && (goalQ == 16 || goalQ == 13))
        shortPath = 36* manhattan;
    //14-15 or vice versa
    else if((startQ == 14 || startQ == 15) && (goalQ == 15 || goalQ == 14))
        shortPath = 33* manhattan;
    //14-16 or vice versa
    else if((startQ == 14 || startQ == 16) && (goalQ == 16 || goalQ == 14))
        shortPath = 42* manhattan;
    //15-16 or vice versa
    else if((startQ == 15 || startQ == 16) && (goalQ == 16 || goalQ == 15))
        shortPath = 33* manhattan;

    return shortPath;

}

/****************************************************************/
/******** Do not modify any code past this point! ***************/
/****************************************************************/

void makeGraph(Tile graph[24][24]) {
    // Construct and initialize the tiles.
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 24; j++) {
            Tile t;
            graph[i][j] = t;
            graph[i][j].tileType = map[i][j];   // Tile type based on above map
            graph[i][j].treestate = 0; // Initially unexplored
            graph[i][j].x = i;         // Each tile knows its location in array
            graph[i][j].y = j;
            graph[i][j].parentX = -1;  // Initially no parents on path fro start
            graph[i][j].parentY = -1;
            graph[i][j].inPath = 0;    // Initially not in path from start
        }
    }
}

// Returns the tile in the fringe with the lowest f = g + h measure
Tile getBest(Tile graph[24][24]) {
    double bestH = 100000;
    Tile bestTile;
    bestTile.x = -1; // Hack to let caller know that nothing was in fringe (failure)
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 24; j++) {
            if (graph[i][j].treestate == 1) {   // Fringe node
                double h = graph[i][j].gDist + heuristic(i, j);
                if (h < bestH) {
                    bestH = h;
                    bestTile = graph[i][j];
                }
            }
        }
    }
    return bestTile;
}

// When a node is expanded, we check each adjacent node. This method takes the graph,
// the coordinates of the parent tile (the one expanded) and its child (the adjacent tile).
void checkAdjacent(Tile graph[24][24], int parentX, int parentY, int childX, int childY) {

    // If already in tree, exit
    if (graph[childX][childY].treestate == 2) {
        return;
    }
    // If unexplored, add to fringe with path from parent and distance based on
    // distance from start to parent and cost of entering the node.
    if (graph[childX][childY].treestate == 0) {
        graph[childX][childY].treestate = 1;
        graph[childX][childY].gDist = graph[parentX][parentY].gDist + graph[childX][childY].getCost();
        graph[childX][childY].parentX = parentX;
        graph[childX][childY].parentY = parentY;

        // Add to stats of nodes added to fringe
        fringeCount++;
        return;
    }
    // If fringe, reevaluate based on new path
    if (graph[childX][childY].treestate == 1) {
        if (graph[parentX][parentY].gDist + graph[childX][childY].getCost() < graph[childX][childY].gDist) {
            // Shorter path through parent, so change path and cost.
            graph[childX][childY].gDist = graph[parentX][parentY].gDist + graph[childX][childY].getCost();
            graph[childX][childY].parentX = parentX;
            graph[childX][childY].parentY = parentY;
        }
        return;
    }
}

// Once the goal has been found, we need the path found to the goal. This method
// works backward from the goal through the parents of each tile. It also totals
// up the cost of the path and returns it.
double finalPath(Tile graph[24][24]) {
    double cost = 0;

    // Start at goal
    int x = goalX;
    int y = goalY;

    // Loop until start reached
    while (x != startX || y != startY) {

        // Add node to path and add to cost
        graph[x][y].inPath = 1;
        cost += graph[x][y].getCost();

        // Work backward to parent and continue
        int tempx = graph[x][y].parentX;
        int tempy = graph[x][y].parentY;
        x = tempx;
        y = tempy;
    }
    graph[startX][startY].inPath = 1;
    return cost;
}

// This method prints the map at the end. Each tile contains the tile type,
// its tree status (0=unexplored, 1=fringe, 2=tree), and a * if that tile
// was in the final path.
void printGraph(Tile graph[24][24]) {
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 24; j++) {
            cout << graph[i][j].tileType;
            cout << graph[i][j].treestate;
            if (graph[i][j].inPath) {
                cout << "*";
            }
            else {
                cout << " ";
            }
            cout << "  ";
        }
        cout << "\n";
    }

}

// Main A* search method. Takes graph as parameter, and returns whether search successful
int search(Tile graph[24][24]) {
    fringeCount = 1;
    int goalFound = 0;

    // Add start state to tree, path cost = 0
    graph[startX][startY].treestate = 1;
    graph[startX][startY].gDist = 0;

    // Loop until goal added to tree
    while (!goalFound) {

        // Get the best tile in the fringe
        Tile bestTile = getBest(graph);
        if (bestTile.x == -1) {
            // The default tile was returned, which means the fringe was empty.
            // This means the search has failed.
            Sleep(2000);
            cout << "Search failed!!!!!!\n";
            printGraph(graph);
            return 0;
        }

        // Otherwise, add that best tile to the tree (removing it from fringe)
        int x = bestTile.x;
        int y = bestTile.y;
        graph[x][y].treestate = 2;
        treeCount++;

        // If it is a goal, done!
        if (x == goalX && y == goalY) {
            goalFound = 1;
            cout << "Finding the best path";
            Sleep(1500);
            cout << ". ";
            Sleep(1500);
            cout << ". ";
            Sleep(1500);
            cout << ". ";
            Sleep(1000);
            cout << "Found the goal!!!!!\n";

            // Compute the path taken and its cost, printing the explored graph,
            // the path  cost, and the number of tiles explored (which should be
            // as small as possible!)
            double cost = finalPath(graph);
            printGraph(graph);
            cout << "\nPath cost: " << cost << "\n";
            cout << treeCount << " tiles added to tree\n";
            cout << fringeCount << " tiles added to fringe\n";

            return 1;
        }

        // Otherwise, we look at the 4 adjacent tiles to the one just added
        // to the tree (making sure each  is in the graph!) and either add it
        // to the tree or recheck its path.
        if (x > 0) { // Tile to left
            checkAdjacent(graph, x, y, x-1, y);
        }
        if (x < 23) { // Tile to right
            checkAdjacent(graph, x, y, x+1, y);
        }
        if (y > 0) { // Tile above
            checkAdjacent(graph, x, y, x, y-1);
        }
        if (y < 23) { // Tile below
            checkAdjacent(graph, x, y, x, y+1);
        }

    }
    return 1;
}


int main(int argc, char** argv) {
    srand(time(NULL));
    Tile graph[24][24];
    setProperties();
    makeGraph(graph);
    search(graph);

    return 0;

}
