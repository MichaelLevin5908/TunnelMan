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
    bool canMoveTo(int x, int y);
    void removeEarthAt(int x, int y, bool* cleared);
    bool digEarth(int x, int y); 
    void addActor(Actor* actor);
    void addGoodies();
    void addProtesters();
    void decBarrel();
    void decProtester();
    bool inRadius(int x1, int y1, int x2, int y2, int radius); 
    bool actorsInRadius(int x, int y, int radius);  
    void addGoldorBarrelorBoulder(int num, char actor);
    bool isThereEarth(int x, int y);
    bool isEarthPresent(int x, int y, bool checkOnlyAbove);
    bool isCompleted() const;
    bool isThereBoulder(int x, int y, int radius = 3);
    bool canMoveInDirection(int x, int y, GraphObject::Direction direction);
    bool playerInRadius(Actor* actor, int radius);
    Protester* protesterInRadius(Actor* actor, int radius);
    void moveToExit(Protester* pr);
    
    void initializeMaze();  
    void addEarth();  
    void addGameObjects(char objectType, int numObjects);  
    void performBFS(int startX, int startY);  

private:
    bool m_isFirstFrame;
    int m_ticksSinceLast;
    int m_numActiveProtesters;
    int m_numBarrelsLeft;
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