#include "StudentWorld.h"
#include "Actor.h"
#include "GraphObject.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir): GameWorld(assetDir), m_isFirstTick(true), m_tickSinceLast(0), m_protestersAlive(0), m_player(nullptr), m_barrelsLeft(0), iscompleted(false) {}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

Tunnelman* StudentWorld::getPlayer() const
{
    return m_player;
}

void StudentWorld::updateDisplayText()
{
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int health = m_player->gethp();
    int squirts = m_player->getWtr();
    int gold = m_player->getGld();
    int sonar = m_player->getSonar();

    string s = displayText(score, level, lives, health, squirts, gold, sonar, m_barrelsLeft);
    setGameStatText(s);
}

string StudentWorld::displayText(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrels)
{
    stringstream s;
    s.fill('0');
    s << "Scr: " << setw(6) << score;
    s.fill(' ');
    s << " Lvl: " << setw(2) << level;
    s << " Lives: " << setw(1) << lives;
    s << "  Hlth: " << setw(3) << health * 10 << '%';
    s << "  Wtr: " << setw(2) << squirts;
    s << "  Gld: " << setw(2) << gold;
    s << "  Sonar: " << setw(2) << sonar;
    s << "  Oil Left: " << setw(2) << barrels;
    return s.str();
}

bool StudentWorld::digEarth(int x, int y)
{
    bool clear = false;
    for (int i = x; i < x + 4; i++)
    {
        for (int j = y; j < y + 4; j++)
        {
            if (m_earth[i][j] != nullptr)
            {
                delete m_earth[i][j];
                m_earth[i][j] = nullptr;
                clear = true;
            }
        }
    }
    return clear;
}

void StudentWorld::addActor(Actor* actor)
{
    m_actors.push_back(actor);
}

void StudentWorld::addGoodies()
{
    int x, y;
    int G = (int)getLevel() + 300;
    if (int(rand() % G) + 1 == 1)
    {
        if (int(rand() % 5) + 1 == 1)
        {
            addActor(new Sonar(this, 0, 60));
        }
        else
        {
            do
            {
                x = rand() % 60 + 1;
                y = rand() % 60 + 1;
            } while (isThereEarth(x, y));
            addActor(new WaterPool(this, x, y));
        }
    }
}

void StudentWorld::addProtesters()
{
    int T = max(25, 200 - (int)getLevel());
    int P = fmin(15, 2 + getLevel() * 1.5);
    int probabilityOfHardcore = min(90, (int)getLevel() * 10 + 30);
    if (m_isFirstTick || (m_tickSinceLast > T && m_protestersAlive < P))
    {
        if (rand() % 100 + 1 < probabilityOfHardcore)
        {
            addActor(new HardcoreProtester(this));
        }
        else
        {
            addActor(new RegularProtester(this));
        }
        m_tickSinceLast = 0;
        m_protestersAlive++;
        m_isFirstTick = false;
    }
    m_tickSinceLast++;
}

void StudentWorld::decBarrel()
{
    m_barrelsLeft--;
    if (m_barrelsLeft == 0)
    {
        iscompleted = true;
    }
}

void StudentWorld::decProtester()
{
    m_protestersAlive--;
}

bool StudentWorld::withInRadius(int x1, int y1, int x2, int y2, int radius)
{
    if ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= radius * radius)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool StudentWorld::ActorsWithInRadius(int x, int y, int radius)
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        if (withInRadius(x, y, (*it)->getX(), (*it)->getY(), radius))
        {
            return true;
        }
        it++;
    }
    return false;
}

void StudentWorld::addBoulderorGoldorBarrel(int num, char actor)
{
    int x, y;
    for (int i = 0; i < num; i++)
    {
        do
        {
            x = rand() % 60 + 1;
            if (actor == 'B')
            {
                y = rand() % 36 + 1 + 20;
            }
            else
            {
                y = rand() % 56 + 1;
            }
        } while (ActorsWithInRadius(x, y, 6) || (x > 26 && x < 34 && y > 0));

        if (actor == 'B')
        {
            addActor(new Boulder(this, x, y));
        }
        else if (actor == 'G')
        {
            addActor(new Gold(this, x, y, false, false));
        }
        else if (actor == 'L')
        {
            addActor(new Barrel(this, x, y));
            m_barrelsLeft++;
        }
    }
}

bool StudentWorld::isAboveEarth(int x, int y)
{
    for (int i = x; i < x + 4; i++)
    {
        if (m_earth[i][y] != nullptr)
        {
            return true;
        }
    }
    return false;
}

bool StudentWorld::isThereEarth(int x, int y)
{
    for (int i = x; i < x + 4; i++)
    {
        for (int j = y; j < y + 4; j++)
        {
            if (m_earth[i][j] != nullptr)
            {
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::isThereBoulder(int x, int y, int radius)
{
    vector<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->getID() == TID_BOULDER && withInRadius(x, y, (*it)->getX(), (*it)->getY(), radius))
        {
            return true;
        }
    }
    return false;
}

bool StudentWorld::canMoveInDirection(int x, int y, GraphObject::Direction direction)
{
    if (direction == GraphObject::left)
    {
        return (x != 0 && !isThereEarth(x - 1, y) && !isThereBoulder(x, y));
    }
    else if (direction == GraphObject::right)
    {
        return (x != 60 && !isThereEarth(x + 1, y) && !isThereBoulder(x + 1, y));
    }
    else if (direction == GraphObject::up)
    {
        return (y != 60 && !isThereEarth(x, y + 1) && !isThereBoulder(x, y + 1));
    }
    else if (direction == GraphObject::down)
    {
        return (y != 0 && !isThereEarth(x, y - 1) && !isThereBoulder(x, y - 1));
    }
    else if (direction == GraphObject::none)
    {
        return false;
    }
    else
    {
        return false;
    }
}

bool StudentWorld::isPlayerInRadius(Actor* actor, int radius)
{
    return withInRadius(actor->getX(), actor->getY(), m_player->getX(), m_player->getY(), radius);
}

Protester* StudentWorld::protesterInRadius(Actor* actor, int radius)
{
    vector<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->getID() == TID_PROTESTER || (*it)->getID() == TID_HARD_CORE_PROTESTER)
        {
            if (withInRadius(actor->getX(), actor->getY(), (*it)->getX(), (*it)->getY(), radius))
            {
                return dynamic_cast<Protester*> (*it);
            }
        }
    }
    return nullptr;
}

void StudentWorld::moveToExit(Protester* pr)
{
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            m_maze[i][j] = 0;
        }
    }
    int a = pr->getX();
    int b = pr->getY();
    queue<Grid> q;
    q.push(Grid(60, 60));
    m_maze[60][60] = 1;
    while (!q.empty())
    {
        Grid c = q.front();
        q.pop();
        int x = c.x;
        int y = c.y;

        if (canMoveInDirection(x, y, GraphObject::left) && m_maze[x - 1][y] == 0)
        {
            q.push(Grid(x - 1, y));
            m_maze[x - 1][y] = m_maze[x][y] + 1;
        }

        if (canMoveInDirection(x, y, GraphObject::right) && m_maze[x + 1][y] == 0)
        {
            q.push(Grid(x + 1, y));
            m_maze[x + 1][y] = m_maze[x][y] + 1;
        }

        if (canMoveInDirection(x, y, GraphObject::up) && m_maze[x][y + 1] == 0)
        {
            q.push(Grid(x, y + 1));
            m_maze[x][y + 1] = m_maze[x][y] + 1;
        }

        if (canMoveInDirection(x, y, GraphObject::down) && m_maze[x][y - 1] == 0)
        {
            q.push(Grid(x, y - 1));
            m_maze[x][y - 1] = m_maze[x][y] + 1;
        }
    }

    if (canMoveInDirection(a, b, GraphObject::left) && m_maze[a - 1][b] < m_maze[a][b])
    {
        pr->moveInDirection(GraphObject::left);
    }
    else if (canMoveInDirection(a, b, GraphObject::right) && m_maze[a + 1][b] < m_maze[a][b])
    {
        pr->moveInDirection(GraphObject::right);
    }
    else if (canMoveInDirection(a, b, GraphObject::up) && m_maze[a][b + 1] < m_maze[a][b])
    {
        pr->moveInDirection(GraphObject::up);
    }
    else if (canMoveInDirection(a, b, GraphObject::down) && m_maze[a][b - 1] < m_maze[a][b])
    {
        pr->moveInDirection(GraphObject::down);
    }
}

void StudentWorld::detectNearActors(int x, int y, int radius)
{
    int a, b;
    vector<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->getID() == TID_BARREL || (*it)->getID() == TID_GOLD)
        {
            a = (*it)->getX();
            b = (*it)->getY();
            if ((x - a) * (x - a) + (y - b) * (y - b) <= radius * radius)
            {
                (*it)->setVisible(true);
            }
        }
    }
}

GraphObject::Direction StudentWorld::senseSignalFromPlayer(Protester* pr, int M)
{
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            m_maze[i][j] = 0;
        }
    }
    int a = pr->getX();
    int b = pr->getY();
    queue<Grid> q;
    q.push(Grid(getPlayer()->getX(), getPlayer()->getY()));
    m_maze[getPlayer()->getX()][getPlayer()->getY()] = 1;
    while (!q.empty())
    {
        Grid c = q.front();
        q.pop();
        int x = c.x;
        int y = c.y;

        if (canMoveInDirection(x, y, GraphObject::left) && m_maze[x - 1][y] == 0)
        {
            q.push(Grid(x - 1, y));
            m_maze[x - 1][y] = m_maze[x][y] + 1;
        }

        if (canMoveInDirection(x, y, GraphObject::right) && m_maze[x + 1][y] == 0)
        {
            q.push(Grid(x + 1, y));
            m_maze[x + 1][y] = m_maze[x][y] + 1;
        }

        if (canMoveInDirection(x, y, GraphObject::up) && m_maze[x][y + 1] == 0)
        {
            q.push(Grid(x, y + 1));
            m_maze[x][y + 1] = m_maze[x][y] + 1;
        }

        if (canMoveInDirection(x, y, GraphObject::down) && m_maze[x][y - 1] == 0)
        {
            q.push(Grid(x, y - 1));
            m_maze[x][y - 1] = m_maze[x][y] + 1;
        }
    }

    if (m_maze[a][b] <= M + 1)
    {
        if (canMoveInDirection(a, b, GraphObject::left) && m_maze[a - 1][b] < m_maze[a][b])
        {
            return GraphObject::left;
        }
        else if (canMoveInDirection(a, b, GraphObject::right) && m_maze[a + 1][b] < m_maze[a][b])
        {
            return GraphObject::right;
        }
        else if (canMoveInDirection(a, b, GraphObject::up) && m_maze[a][b + 1] < m_maze[a][b])
        {
            return GraphObject::up;
        }
        else if (canMoveInDirection(a, b, GraphObject::down) && m_maze[a][b - 1] < m_maze[a][b])
        {
            return GraphObject::down;
        }
    }
    return GraphObject::none;
}

int StudentWorld::init()
{
    m_barrelsLeft = 0;
    m_isFirstTick = true;
    m_tickSinceLast = 0;
    m_protestersAlive = 0;
    iscompleted = false;

    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 60; y++)
        {
            if (x < 30 || x > 33 || y < 4)
            {
                m_earth[x][y] = new Earth(this, x, y);
            }
            else
            {
                m_earth[x][y] = nullptr;
            }
        }
    }

    // Create the player (Tunnelman)
    m_player = new Tunnelman(this);

    // Calculate the number of Boulders, Gold Nuggets, and Barrels of Oil
    const int level = getLevel();
    const int numBoulders = std::min(level / 2 + 2, 9);
    const int numGoldNuggets = std::max(level / 2, 2);
    const int numBarrels = std::min(2 + level, 21);

    // Add actors to the game world based on the calculated numbers
    addBoulderorGoldorBarrel(numBoulders, 'B');
    addBoulderorGoldorBarrel(numBarrels, 'L');
    addBoulderorGoldorBarrel(numGoldNuggets, 'G');

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    
    updateDisplayText();

    
    for (size_t i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isAlive())
        {
            m_actors[i]->doSomething();

            if (!m_player->isAlive())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }

            if (iscompleted)
            {
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }

    m_player->doSomething();

    
    addGoodies();
    addProtesters();

    for (size_t i = 0; i < m_actors.size(); )
    {
        if (!m_actors[i]->isAlive())
        {
            delete m_actors[i];
            m_actors.erase(m_actors.begin() + i);
        }
        else
        {
            i++;
        }
    }

    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp()
{
    // Cleanup Earth objects
    int x = 0;
    while (x < 64)
    {
        int y = 0;
        while (y < 60)
        {
            delete m_earth[x][y]; 
            m_earth[x][y] = nullptr; 
            y++;
        }
        x++;
    }

    
    while (!m_actors.empty())
    {
        delete m_actors.back();
        m_actors.pop_back(); 
    }

    
    delete m_player;
    m_player = nullptr; 
}
