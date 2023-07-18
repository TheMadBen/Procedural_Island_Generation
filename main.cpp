/*
Name: Benjamin Pham 
Date: 11/3/21
Description: Particle map generator that takes user inputs and then generates a 2d array of integers and applies particle roll algorithm.
             Particle map is then converted to polished map of characters as a final product.
*/

#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

//foward declare function prototypes
void interfaceMenu(int&, int&, int&, int&, int&, int&, int&, int&, int&);
void numCheck(int&);
void dropXCheck(int&, int);
void dropYCheck(int&, int);
void dropWidthCheck(int&, int);
void dropHeightCheck(int&, int);
void waterCheck(int&);
int** make2dArray(int, int);
int** makeParticleMap(int, int, int, int, int, int, int, int);
bool moveExists(int**, int, int, int, int);
int findMax(int**, int, int);
void normalizeMap(int**, int, int, int);
char** makePolishMap(int**, int, int, int);
void displayMap(int**, int, int);
void displayPolishMap(char**, int, int);
void deleteMap(int**, int);
void deletePolishMap(char**, int);

int main() {
    srand(time(NULL)); //seed the time

    int width, height, dropX, dropY, dropWidth, dropHeight, numParticles, maxLife, waterLine, maxVal; //initialize variables
    
    interfaceMenu(width, height, dropX, dropY, dropWidth, dropHeight, numParticles, maxLife, waterLine); //prompt user menu

    int** map = makeParticleMap(height, width, dropY, dropX, dropHeight, dropWidth, numParticles, maxLife); //map with values only
    
    maxVal = findMax(map, height, width); //find max value within map
    normalizeMap(map, height, width, maxVal); //normalize map so index range from 0 - 255
    
    
    char** polishMap = makePolishMap(map, height, width, waterLine); //char map with the symbols to signify land or water
    displayPolishMap(polishMap, height, width); //show polished map
    
    freopen("island.txt", "w" , stdout);
    displayPolishMap(polishMap, height, width); //polished map that will be in output file
    fclose(stdout); ////freopen will redirect to stdout which then goes to outputFile with write permission
    
    //free memory
    deleteMap(map, height);
    deletePolishMap(polishMap, height);
}

void interfaceMenu(int &width, int &height, int &dropX, int &dropY, int &dropWidth, int &dropHeight, int &numParticles, int &maxLife, int &waterLine) {
    bool contMenu = true; //boolean for while loop to continue displaying menu/interface options until all valid inputs are taken

    cout << "Welcome to <Benjamin Pham>’s island generator!" << endl << endl;

    while(contMenu) {
        cout << "Enter grid width: " << endl;
        cin >> width;
        numCheck(width);
        cout << "Enter grid height: " << endl;
        cin >> height;
        numCheck(height);
        cout << "Enter drop-window x-coordinate (0 - <width>): " << endl;
        cin >> dropX;
        dropXCheck(dropX, width);
        cout << "Enter drop-window y-coordinate (0 - <height>): " << endl;
        cin >> dropY;
        dropYCheck(dropY, height);
        cout << "Enter drop-window width (>= 1): " << endl;
        cin >> dropWidth;
        dropWidthCheck(dropWidth, width);
        cout << "Enter drop-window height (>= 1): " << endl;
        cin >> dropHeight;
        dropHeightCheck(dropHeight, height);
        cout << "Enter number of particles to drop: " << endl;
        cin >> numParticles;
        numCheck(numParticles);
        cout << "Enter max life of particles: " << endl;
        cin >> maxLife;
        numCheck(maxLife);
        cout << "Enter value for waterline (40-200): " << endl;
        cin >> waterLine;
        waterCheck(waterLine);
        contMenu = false;
    }
}

void numCheck(int &num) { //check if user input is a vaild positive number
    while(num < 1) {
        cin.clear(); //clear input and reprompt
        cout << "Enter a valid number: " << endl;
        cin >> num;
    }
}

void dropXCheck(int &dropX, int width) { //check if user input is a valid positve number and greater than width
    while(dropX < 0 || dropX > width) {
        cin.clear();
        cout << "Enter a valid number: " << endl;
        cin >> dropX;
    }
}

void dropYCheck(int &dropY, int height) { //check if user input is a valid positive number and greater than height
    while(dropY < 0 || dropY > height) {
        cin.clear();
        cout << "Enter a valid number: " << endl;
        cin >> dropY;
    }
}

void dropWidthCheck(int &dropWidth, int width) { //check if user input is valid positive number and dropWidth is greater than width
    while(dropWidth < 1 || dropWidth > width) {
        cin.clear();
        cout << "Enter a valid number: " << endl;
        cin >> dropWidth;
    }
}

void dropHeightCheck(int &dropHeight, int height) { //check if user input is valid positive number and dropHeight is greater than height
    while(dropHeight < 1 || dropHeight > height) {
        cin.clear();
        cout << "Enter a valid number: " << endl;
        cin >> dropHeight;
    }
}

void waterCheck(int& waterLine) { //check if waterLine input is between 40 and 200
    while(waterLine < 40 || waterLine > 200) {
        cin.clear();
        cout << "Enter a valid number: " << endl;
        cin >> waterLine;
    }
}

int** make2dArray(int height, int width) { //function to create 2d array
    int** myArray = new int*[height];

    for(int i = 0; i < height; ++i) {
        myArray[i] = new int[width];
        for(int j = 0; j < width; ++j) {
            myArray[i][j] = 0; //set all indeces to zero
        }
    }
    
    return myArray;
}
//function to implement particle roll algo and 2d array of zeroes
int** makeParticleMap(int height, int width, int dropY, int dropX, int winHeight, int winWidth, int numParticles, int maxLife) { 
    int** particleMap = make2dArray(height, width);

    int randX, randY, randIndex, nextX, nextY; //initialize variables
    int windowXMax = (dropX + winWidth); //x range
    int windowYMax = (dropY + winHeight); //y range

    int loop = 0;
    
    while(numParticles != 0) {
        //randon x and y within bounds;
        randX = rand() % (windowXMax - dropX + 1) + dropX;
        randY = rand() % (windowYMax - dropY + 1) + dropY;

        particleMap[randY][randX] += 1; //increment each spot by one

        while(maxLife != 0) {
                if(moveExists(particleMap, height, width, randY, randX) == true) { //check to see if there's a valid move

                    while(loop == 0) {
                        //moore neighbor index
                        int moore[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0 ,1}, {-1, 1}, {-1,0}}; 
                        randIndex = rand() % 8; //generate random number 0-7
                        
                        nextX = randX + moore[randIndex][0]; //random next x value
                        nextY = randY + moore[randIndex][1]; //random next y value
                        
                        if(nextY >= 0 && nextX >= 0 && nextY < (height) && nextX < (width)) { //check to see if random spot is valid
                            if(particleMap[nextY][nextX] <= particleMap[randY][randX]) {
                                loop++;
                            }
                            else{
                            break;
                            }
                        }
                        
                        if(loop == 1) { //if random spot was valid then move to it
                            randY = nextY;
                            randX = nextX;
                            
                            particleMap[nextY][nextX] = particleMap[randY][randX] + 1; //increment each spot by one
                            loop = 0;
                        }
                    }
                }
            --maxLife; //decrement maxLife
        }
        --numParticles; //decrement numParticles
    }
    return particleMap;
}
//function to check if 8 spots surrounding current spot is valid, return true as soon as one is found
bool moveExists(int** map, int height, int width, int y, int x) {
    
    bool exists = false;
    if(y - 1 >= 0 && x - 1 >= 0 && y - 1 < height && x - 1 < width) { //top left
        if(map[y-1][x-1] <= map[y][x]) {
            exists = true;
        }
    }
    if(y - 1 >= 0 && x >= 0 && y - 1 < height && x < width) { //top
        if(map[y-1][x] <= map[y][x]) {
            exists = true;
        }
    }
    if(y - 1 >= 0 && x + 1 >= 0 && y - 1 < height && x + 1 < width) { //top right
        if(map[y-1][x+1] <= map[y][x]) {
            exists = true;
        }
    }
    if(y >= 0 && x + 1 >= 0 && y < height && x + 1 < width) { //right
        if(map[y][x+1] <= map[y][x]) {
            exists = true;
        }
    }
    if(y + 1 >= 0 && x + 1 >= 0 && y + 1 < height && x + 1 < width) { //bottom right
        if(map[y+1][x+1] <= map[y][x]) {
            exists = true;
        }
    }
    if(y + 1 >= 0 && x >= 0 && y + 1 < height && x < width) { //bottom
        if(map[y+1][x] <= map[y][x]) {
            exists = true;
        }
    }
    if(y + 1 >= 0 && x - 1 >= 0 && y + 1 < height && x - 1 < width) { //bottom left
        if(map[y+1][x-1] <= map[y][x]) {
            exists = true;
        }
    }
    if(y >= 0 && x - 1 >= 0 && y < height && x - 1 < width) { //left
        if(map[y][x-1] <= map[y][x]) {
            exists = true;
        }
    }

    return exists;
}
//function to find max within map
int findMax(int** map, int height, int width) {
    int max = map[0][0];
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            if(map[i][j] > max) {
                max = map[i][j];
            }
        }
    }
    return max;
}
//normalize map to make every index a value between 0 and 255
void normalizeMap(int** map, int height, int width, int maxVal) {
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            map[i][j] = (( ((float)map[i][j]) / (float)maxVal) ) * 255;
        }
    }
}
//function to convert the values within the first map and match them with a character
char** makePolishMap(int** map, int height, int width, int waterLine) {
    int landZone = 255 - waterLine;
    char** polishMap = new char*[height]; //initialize 2d array of type char  

    for(int i = 0; i < height; ++i) { //for loops to populate char array with respective characters relative to waterLine from particle map
        polishMap[i] = new char[width];
        for(int j = 0; j < width; ++j) {
            if(map[i][j] < (0.5 * waterLine)) {
                polishMap[i][j] = '#'; //deep water
            }
            else if(map[i][j] > (0.5 * waterLine) && map[i][j] <= waterLine) {
                polishMap[i][j] = '~'; //shallow water
            }
            else if(map[i][j] > waterLine) {
                if(map[i][j] < (waterLine + (0.15 * landZone))) {
                    polishMap[i][j] = '.'; //coast or beach
                }
                else if(map[i][j] >= (waterLine + (0.15 * landZone)) && map[i][j] < (waterLine + (0.4 * landZone))) {
                    polishMap[i][j] = '-'; //plains or grass
                }
                else if(map[i][j] >= (waterLine + (0.4 * landZone)) && map[i][j] < (waterLine + (0.8 * landZone))) {
                    polishMap[i][j] = '*'; //forests
                }
                else {
                    polishMap[i][j] = '^'; //mountains
                }
            }
        }
    }
    return polishMap;
}
//function to display map to terminal
void displayMap(int** map, int height, int width) {
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            cout << map[i][j] << " ";
        }
        cout << endl;
    }
}
//function to display polished map to terminal
void displayPolishMap(char** polishMap, int height, int width) {
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            cout << polishMap[i][j] << " ";
        }
        cout << endl;
    }
}
//function to free memory of original int map
void deleteMap(int** firstArr, int height) {
    for(int i = 0; i < height; ++i) {
        delete [] firstArr[i];
    }
    delete [] firstArr;
}
//function to free memory of polished map
void deletePolishMap(char** polishMap, int height) {
    for(int i = 0; i < height; ++i) {
        delete [] polishMap[i];
    }
    delete [] polishMap;
}
