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

StudentWorld::StudentWorld(std::string assetDir): GameWorld(assetDir), m_isFirstTick(true), m_tickSinceLast(0), m_protestersAlive(0), m_player(nullptr), m_barrelsLeft(0) {}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

Tunnelman* StudentWorld::getPlayer() const
{
    return m_player;
}

void StudentWorld::setDisplayText()
{
    int level = getLevel();
    int lives = getLives();
    int health = m_player->gethp();
    int squirts = m_player->getWtr();
    int gold = m_player->getGld();
    int barrelsLeft = m_barrelsLeft;
    int sonar = m_player->getSonar();
    int score = getScore();

    string s = displayText(level, lives, health, squirts, gold, barrelsLeft, sonar, score);
    setGameStatText(s);
}

string StudentWorld::displayText(int level, int lives, int health, int squirts, int gold, int barrels, int sonar, int score)
{
    stringstream s;
    s.fill('0');
    s << "Lvl: " << setw(2) << level;
    s.fill(' ');
    s << "  Lives: " << setw(1) << lives;
    s << "  Hlth: " << setw(3) << health * 10 << '%';
    s << "  Wtr: " << setw(2) << squirts;
    s << "  Gld: " << setw(2) << gold;
    s << "  Oil Left: " << setw(2) << barrels;
    s << "  Sonar: " << setw(2) << sonar;
    s.fill('0');
    s << "  Scr: " << setw(6) << score;
    return s.str();
}

void StudentWorld::removeEarthAt(int x, int y, bool* cleared)
{
    if (x >= 0 && x < 64 && y >= 0 && y < 60)
    {
        if (m_earth[x][y] != nullptr)
        {
            delete m_earth[x][y];
            m_earth[x][y] = nullptr;
            if (cleared != nullptr)
            {
                *cleared = true;
            }
        }
    }
}

bool StudentWorld::digEarth(int x, int y)
{
    bool cleared = false;
    for (int i = x; i < x + 4; i++)
    {
        for (int j = y; j < y + 4; j++)
        {
            removeEarthAt(i, j, &cleared);
        }
    }
    return cleared;
}

void StudentWorld::addActor(Actor* actor)
{
    m_actors.push_back(actor);
}

void StudentWorld::addGoodies()
{
    int G = getLevel() * 25 + 300;
    if (rand() % G == 0)  
    {
        if (rand() % 5 == 0)  
        {
            addActor(new Sonar(this, 0, 60));
        }
        else  
        {
            static std::vector<int> validX;
            static std::vector<int> validY;

            
            validX.clear();
            validY.clear();

            for (int x = 0; x <= 60; ++x)
            {
                for (int y = 0; y <= 60; ++y)
                {
                    if (!isThereEarth(x, y))
                    {
                        validX.push_back(x);
                        validY.push_back(y);
                    }
                }
            }
            if (!validX.empty())
            {
                int index = rand() % validX.size();
                addActor(new WaterPool(this, validX[index], validY[index]));
            }
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
        Actor* protester;
        if (rand() % 100 < probabilityOfHardcore)
        {
            protester = new HardcoreProtester(this);
        }
        else
        {
            protester = new RegularProtester(this);
        }
        
        addActor(protester);
        m_tickSinceLast = 0;
        m_protestersAlive++;
        m_isFirstTick = false;
    }
    m_tickSinceLast++;
}

bool StudentWorld::isCompleted() const
{
    return m_barrelsLeft == 0;
}

void StudentWorld::decBarrel()
{
    m_barrelsLeft--;
}

void StudentWorld::decProtester()
{
    m_protestersAlive--;
}

bool StudentWorld::withInRadius(int x1, int y1, int x2, int y2, int radius)
{
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= radius * radius;
}

bool StudentWorld::ActorsWithInRadius(int x, int y, int radius)
{
    for (Actor* actor : m_actors)
    {
        int actorX = actor->getX();
        int actorY = actor->getY();
        if (withInRadius(x, y, actorX, actorY, radius))
        {
            return true;
        }
    }
    return false;
}

void StudentWorld::addBoulderorGoldorBarrel(int num, char actor)
{
    int x, y;
    int yMin = 1;
    int yMax = (actor == 'B') ? 36 + 20 : 56;

    for (int i = 0; i < num; i++)
    {
        do
        {
            x = rand() % 60 + 1;
            y = rand() % (yMax - yMin + 1) + yMin;
        } while (ActorsWithInRadius(x, y, 6) || (x > 26 && x < 34 && y > 0));

        switch (actor)
        {
            case 'B':
                addActor(new Boulder(this, x, y));
                break;
            case 'G':
                addActor(new Gold(this, x, y, false, false));
                break;
            case 'L':
                addActor(new Barrel(this, x, y));
                m_barrelsLeft++;
                break;
            default:
                break;
        }
    }
}

bool StudentWorld::canMoveTo(int x, int y){
    if (x < 0 || x > VIEW_WIDTH - 1 || y < 0 || y > VIEW_HEIGHT - 1)
        return false;

    if (isThereBoulder(x, y))
        return false;

    if (isThereEarth(x, y))
        return false;

    return true;
}

bool StudentWorld::isEarthPresent(int x, int y, bool checkOnlyAbove = false)
{
    for (int i = x; i < x + 4; i++)
    {
        if (checkOnlyAbove)
        {
            if (m_earth[i][y] != nullptr)
            {
                return true;
            }
        }
        else
        {
            for (int j = y; j < y + 4; j++)
            {
                if (m_earth[i][j] != nullptr)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool StudentWorld::isAboveEarth(int x, int y)
{
    return isEarthPresent(x, y, true);
}

bool StudentWorld::isThereEarth(int x, int y)
{
    return isEarthPresent(x, y, false);
}

bool StudentWorld::isThereBoulder(int x, int y, int radius)
{
    for (std::vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it)
    {
        Actor* actor = *it;
        if (actor->getID() == TID_BOULDER && withInRadius(x, y, actor->getX(), actor->getY(), radius))
        {
            return true;
        }
    }
    return false;
}

bool StudentWorld::canMoveInDirection(int x, int y, GraphObject::Direction direction)
{
    switch (direction)
    {
        case GraphObject::left:
            return (x > 0 && !isThereEarth(x - 1, y) && !isThereBoulder(x - 1, y));
        case GraphObject::right:
            return (x < 60 && !isThereEarth(x + 1, y) && !isThereBoulder(x + 1, y));
        case GraphObject::up:
            return (y < 60 && !isThereEarth(x, y + 1) && !isThereBoulder(x, y + 1));
        case GraphObject::down:
            return (y > 0 && !isThereEarth(x, y - 1) && !isThereBoulder(x, y - 1));
        case GraphObject::none:
        default:
            return false;
    }
}

void StudentWorld::initializeMaze()
{
    std::fill(&m_maze[0][0], &m_maze[0][0] + sizeof(m_maze) / sizeof(m_maze[0][0]), 0);
}

void StudentWorld::performBFS(int startX, int startY)
{
    std::queue<grid> q;
    q.push(grid(startX, startY));
    m_maze[startX][startY] = 1;

    const int dx[] = {-1, 1, 0, 0};
    const int dy[] = {0, 0, 1, -1};

    while (!q.empty())
    {
        grid current = q.front();
        q.pop();

        int currentX = current.m_x;
        int currentY = current.m_y;

        for (int i = 0; i < 4; ++i)
        {
            int nextX = currentX + dx[i];
            int nextY = currentY + dy[i];

            if (canMoveInDirection(currentX, currentY, static_cast<GraphObject::Direction>(i)) && m_maze[nextX][nextY] == 0)
            {
                q.push(grid(nextX, nextY));
                m_maze[nextX][nextY] = m_maze[currentX][currentY] + 1;
            }
        }
    }
}

bool StudentWorld::isPlayerInRadius(Actor* actor, int radius)
{
    return withInRadius(actor->getX(), actor->getY(), m_player->getX(), m_player->getY(), radius);
}

Protester* StudentWorld::protesterInRadius(Actor* actor, int radius)
{
    int actorX = actor->getX();
    int actorY = actor->getY();

    for (Actor* act : m_actors)
    {
        if (act->getID() == TID_PROTESTER || act->getID() == TID_HARD_CORE_PROTESTER)
        {
            if (withInRadius(actorX, actorY, act->getX(), act->getY(), radius))
            {
                return dynamic_cast<Protester*>(act);
            }
        }
    }
    return nullptr;
}

void StudentWorld::moveToExit(Protester* pr)
{
    initializeMaze();

    int startX = 60; 
    int startY = 60;
    int targetX = pr->getX();
    int targetY = pr->getY();

    performBFS(startX, startY);

    const int dx[] = {-1, 1, 0, 0};
    const int dy[] = {0, 0, 1, -1};
    const GraphObject::Direction directions[] = {
        GraphObject::left, GraphObject::right,
        GraphObject::up, GraphObject::down};

    for (int i = 0; i < 4; ++i)
    {
        int nextX = targetX + dx[i];
        int nextY = targetY + dy[i];

        if (canMoveInDirection(targetX, targetY, directions[i]) &&
            m_maze[nextX][nextY] < m_maze[targetX][targetY])
        {
            pr->moveInDirection(directions[i]);
            break;
        }
    }
}

void StudentWorld::addEarth()
{
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
}

void StudentWorld::addGameObjects(char objectType, int numObjects)
{
    addBoulderorGoldorBarrel(numObjects, objectType);
}

int StudentWorld::init()
{
    m_barrelsLeft = 0;
    m_isFirstTick = true;
    m_tickSinceLast = 0;
    m_protestersAlive = 0;

    addEarth();

    m_player = new Tunnelman(this);

    const int level = getLevel();
    addGameObjects('B', std::min(level / 2 + 2, 9));
    addGameObjects('L', std::min(2 + level, 21));
    addGameObjects('G', std::max(level / 2, 2));

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    
    setDisplayText();

    
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

            if (isCompleted())
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
    for (int x = 0; x < 64; ++x)
    {
        for (int y = 0; y < 60; ++y)
        {
            removeEarthAt(x, y, nullptr);
        }
    }

    for (auto* actor : m_actors)
    {
        delete actor; 
    }
    m_actors.clear();  

    if (m_player != nullptr)
    {
        delete m_player;
        m_player = nullptr;
    }
}