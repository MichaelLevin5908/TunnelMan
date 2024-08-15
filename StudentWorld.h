#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GraphObject.h"
#include <string>
#include <queue>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class GraphObject;
class Earth;
class Tunnelman;
class Actor;
class Protester;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();

    Tunnelman* getPlayer() const;
    void setDisplayText();
    std::string displayText(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrels);
    void detectNearActors(int x, int y, int radius);
    bool canMoveTo(int x, int y);
    bool digEarth(int x, int y); 
    void removeEarthAt(int i, int j, bool* cleared);
    void addActor(Actor* actor);
    void addGoodies();
    void addProtesters();
    void decBarrel();
    void decProtester();
    bool withInRadius(int x1, int y1, int x2, int y2, int radius); 
    bool ActorsWithInRadius(int x, int y, int radius);  
    void addBoulderorGoldorBarrel(int num, char actor);
    bool isAboveEarth(int x, int y);
    bool isThereEarth(int x, int y);
    bool isEarthPresent(int x, int y, bool checkOnlyAbove);
    bool isCompleted();
    bool isThereBoulder(int x, int y, int radius = 3);
    bool canMoveInDirection(int x, int y, GraphObject::Direction direction);
    bool isPlayerInRadius(Actor* actor, int radius);
    Protester* protesterInRadius(Actor* actor, int radius);
    void moveToExit(Protester* pr);
    GraphObject::Direction senseSignalFromPlayer(Protester* pr, int M);
    
    void initializeMaze();  
    void addEarth();  
    void addGameObjects(char objectType, int numObjects);  
    void performBFS(int startX, int startY);  

private:
    bool m_isCompleted;
    bool m_isFirstTick;
    int m_tickSinceLast;
    int m_protestersAlive;
    int m_barrelsLeft;
    Earth* m_earth[64][64];
    Tunnelman* m_player;
    std::vector<Actor*> m_actors;
    int m_maze[64][64];

    struct grid {
        int m_x;
        int m_y;
        grid(int x, int y) : m_x(x), m_y(y) {}
    };
};

#endif // STUDENTWORLD_H_


