#include "Actor.h"
#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>
using namespace std;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//Actor
Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth):GraphObject(imageID, startX, startY, dir, size, depth), m_world(world),m_alive(true)
	{
		setVisible(true);
	}
Actor::~Actor()
	{
		setVisible(false);
	}

void Actor::moveTo(int x, int y)
{
    if (x >= 0 && x <= 60 && y >= 0 && y <= 60)
    {   
        GraphObject::moveTo(x, y);
    }
}

Man::Man(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp):Actor(world, imageID, startX, startY, dir, 1.0, 0), m_hp(hp) {}

Tunnelman::Tunnelman(StudentWorld* world):Man(world, TID_PLAYER, 30, 60, right, 10), m_wtr(5), m_sonar(1), m_gld(0) {}

void Tunnelman::add(int id)
{
    if (id == TID_WATER_POOL) 
    {
        m_wtr += 5;
    }
    else if (id == TID_SONAR) 
    {
        m_sonar += 2;
    }
    else if (id == TID_GOLD) 
    {
        m_gld++;
    }
}

void Tunnelman::doSomething()
{
    if (!isAlive()) 
        return;

    if (getWorld()->digEarth(getX(), getY())) 
        getWorld()->playSound(SOUND_DIG);

    int ch;
    if (!getWorld()->getKey(ch)) 
        return;

    if (ch == KEY_PRESS_ESCAPE) {
        die();
    } else {
        switch (ch) {
            case KEY_PRESS_LEFT:
                setDirection(left);
                moveInDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                moveInDirection(right);
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                moveInDirection(up);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                moveInDirection(down);
                break;
            case KEY_PRESS_SPACE:
                if (m_wtr > 0) {
                    m_wtr--;
                    shoot();
                }
                break;
            case 'z':
            case 'Z':
                if (m_sonar > 0) {
                    m_sonar--;
                    getWorld()->ActorsWithInRadius(getX(), getY(), 12);
                    getWorld()->playSound(SOUND_SONAR);
                }
                break;
            case KEY_PRESS_TAB:
                if (m_gld > 0) {
                    getWorld()->addActor(new Gold(getWorld(), getX(), getY(), true, true));
                    m_gld--;
                }
                break;
        }
    }
}

void Tunnelman::shoot() 
{
    int targetX = getX();
    int targetY = getY();

    switch (getDirection())
    {
        case left:
            targetX -= 4;
            break;
        case right:
            targetX += 4;
            break;
        case up:
            targetY += 4;
            break;
        case down:
            targetY -= 4;
            break;
        default:
            return; 
    }

    if (!getWorld()->isThereEarth(targetX, targetY) && !getWorld()->isThereBoulder(targetX, targetY))
    {
        getWorld()->addActor(new Squirt(getWorld(), targetX, targetY, getDirection()));
        getWorld()->playSound(SOUND_PLAYER_SQUIRT);
    }
}

void Tunnelman::isAnnoyed(int hp)
{
	dechp(hp);
	if (gethp() <= 0)
	{
		die();
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	}
}

void Tunnelman::moveInDirection(Direction direction)
{
    if (getDirection() != direction)
    {
        setDirection(direction);
        return;  
    }

    int newX = getX();
    int newY = getY();

    switch (direction)
    {
        case left:  newX--; break;
        case right: newX++; break;
        case up:    newY++; break;
        case down:  newY--; break;
        default:    return;
    }

    if (getWorld()->digEarth(newX, newY))
    {
        getWorld()->playSound(SOUND_DIG);
    }

    if (getWorld()->canMoveTo(newX, newY))
    {
        moveTo(newX, newY);
    }
}

Earth::Earth(StudentWorld* world, int startX, int startY):Actor(world, TID_EARTH, startX, startY, right, 0.25, 3) {}

void Earth::doSomething() {} 

Boulder::Boulder(StudentWorld* world, int startX, int startY): Actor(world, TID_BOULDER, startX, startY, down, 1.0, 1),isStable(true),m_ticks(0)
	{
		world->digEarth(startX, startY);
	}
	
void Boulder::doSomething() 
{
    if (isAlive()) 
    {
        if (isStable) 
        {
            if (!getWorld()->isEarthPresent(getX(), getY() - 1, true)) 
            {
                isStable = false;
            }
        }

        if (!isStable) 
        {
            m_ticks++;

            if (m_ticks == 30) 
            {
                isFalling = true;
                getWorld()->playSound(SOUND_FALLING_ROCK);
            }

            if (isFalling) 
            {
                if (!getWorld()->isEarthPresent(getX(), getY() - 1, true) && 
                    !getWorld()->isThereBoulder(getX(), getY() - 4, 0) && 
                    getY() != 0) 
                {
                    moveTo(getX(), getY() - 1);
                    annoyMan();
                }
                else 
                {
                    die();
                }
            }
        }
    }
}

void Boulder::annoyMan()
{
    if (getWorld()->isPlayerInRadius(this, 3)) {
        getWorld()->getPlayer()->isAnnoyed(100);
    }
    
    if (Protester* p = getWorld()->protesterInRadius(this, 3)) {
        p->isAnnoyed(100);
    }
}

//Squirt
Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction dir):Actor(world, TID_WATER_SPURT, startX, startY, dir, 1.0, 1),m_travel(0) {}

void Squirt::doSomething() {
    if (!isAlive() || annoyProtesters() || m_travel == 4) {
        die();
        return;
    }

    if (moveInDirectionOrDie(getDirection())) {
        m_travel++;
    } else {
        die();
    }
}

bool Squirt::moveInDirectionOrDie(Direction dir) {
    int x = getX();
    int y = getY();
    
    switch (dir) 
    {
        case left:  x--; break;
        case right: x++; break;
        case up:    y++; break;
        case down:  y--; break;
        default:    return false;
    }

    if (getWorld()->isThereEarth(x, y) || getWorld()->isThereBoulder(x, y)) {
        return false;
    } else 
    {
        moveTo(x, y);
        return true;
    }
}

bool Squirt::annoyProtesters(){
    
    if (Protester* p = getWorld()->protesterInRadius(this, 3))
    {     
        p->isAnnoyed(2);
        return true;
    }
    return false;
}

//Pickupable
Pickupable::Pickupable(StudentWorld* world, int imageID, int startX, int startY):Actor(world, imageID, startX, startY, right, 1.0, 2) {}

void Pickupable::disappearIn(int ticks)
{
    if (m_tick == ticks) 
    {
        die();
        return;
    } 
    m_tick++;
}

//Barrel
Barrel::Barrel(StudentWorld* world, int startX, int startY):Pickupable(world, TID_BARREL, startX, startY)
	{
		setVisible(false);
	}

void Barrel::doSomething() {
    if (!isAlive()) return;

    if (!isVisible()) {
        if (getWorld()->isPlayerInRadius(this, 4)) {
            setVisible(true);
        }
        return;
    }

    if (getWorld()->isPlayerInRadius(this, 3)) {
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        getWorld()->decBarrel();
        die();
    }
}

//Gold
Gold::Gold(StudentWorld* world, int startX, int startY, bool isVisible, bool dropped):Pickupable(world, TID_GOLD, startX, startY), isDropped(dropped)
	{
		setVisible(isVisible);
	}

void Gold::doSomething() {
    if (!isAlive()) return;

    if (!isVisible() && getWorld()->isPlayerInRadius(this, 4)) {
        setVisible(true);
        return;
    }

    if (getWorld()->isPlayerInRadius(this, 3)) {
        if (!isDropped) {
            die();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            getWorld()->getPlayer()->add(getID());
            getWorld()->increaseScore(10);
        } else if (Protester* p = getWorld()->protesterInRadius(this, 3)) {
            die();
            p->isBribed();
        }
    }

    if (isDropped) {
        disappearIn(100);
    }
}

//Goodie
Goodie::Goodie(StudentWorld* world, int imageID, int startX, int startY):Pickupable(world, imageID, startX, startY) {}

void Goodie::doSomething() {
    if (!isAlive()) return;

    if (getWorld()->isPlayerInRadius(this, 3)) {
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getPlayer()->add(getID());
        getWorld()->increaseScore(75);
        die();
    } else {
        disappearIn(std::max(100, 300 - 10 * (int)getWorld()->getLevel()));
    }
}

//Sonar
Sonar::Sonar(StudentWorld* world, int startX, int startY):Goodie(world, TID_SONAR, startX, startY) {}

//WaterPool
WaterPool::WaterPool(StudentWorld* world, int startX, int startY):Goodie(world, TID_WATER_POOL, startX, startY) {}

//Protester
Protester::Protester(StudentWorld* world, int imageID, int hp): Man(world, imageID, 60, 60, left, hp), m_leave(false), m_tickSinceLastTurn(200), m_tickNoYell(15)
	{
		randomNumToMove();
		m_tickRest = max(0, 3 - (int)getWorld()->getLevel() / 4);
	}

void Protester::randomNumToMove()
	{
		m_numToMove = rand() % 53 + 8;
	}

void Protester::doSomething()
{
    if (!isAlive())
    {
        return;
    }

    if (m_tickRest > 0)
    {
        m_tickRest--;
        return;
    }

    m_tickRest = std::max(0, 3 - static_cast<int>(getWorld()->getLevel()) / 4);
    m_tickSinceLastTurn++;
    m_tickNoYell++;

    if (m_leave)
    {
        if (getX() == 60 && getY() == 60)
        {
            die();
            getWorld()->decProtester();
        }
        else
        {
            getWorld()->moveToExit(this);
        }
        return;
    }

    if (getWorld()->isPlayerInRadius(this, 4) && isFacingPlayer() && m_tickNoYell > 15)
    {
        getWorld()->getPlayer()->isAnnoyed(2);
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        m_tickNoYell = 0;
        return;
    }

    if (getID() == TID_HARD_CORE_PROTESTER)
        {
            GraphObject::Direction s = directionToPlayer();
            if (s != GraphObject::none)
            {
                setDirection(s);
                moveInDirection(s);
                return;
            }
        }

    Direction d = directionToPlayer();
    if (d != none && straightPathToPlayer(d) && !getWorld()->isPlayerInRadius(this, 4))
    {
        setDirection(d);
        moveInDirection(d);
        m_numToMove = 0;
        return;
    }

    m_numToMove--;
    if (m_numToMove <= 0)
    {
        Direction newDir;
        do
        {
            newDir = randomDirection();
        } while (!getWorld()->canMoveInDirection(getX(), getY(), newDir));

        setDirection(newDir);
        randomNumToMove();
    }

    else if (isAtIntersection() && m_tickSinceLastTurn > 200)
    {
        pickViableDirectionToTurn();
        m_tickSinceLastTurn = 0;
        randomNumToMove();
    }

    moveInDirection(getDirection());

    if (!getWorld()->canMoveInDirection(getX(), getY(), getDirection()))
    {
        m_numToMove = 0;
    }
}

void Protester::moveInDirection(Direction direction) {
    int x = getX();
    int y = getY();

    switch (direction) {
        case left:  x--; break;
        case right: x++; break;
        case up:    y++; break;
        case down:  y--; break;
        default:    return;
    }

    if (!getWorld()->canMoveTo(x, y)) {
        setOppositeDirection();
    } else {
        moveTo(x, y);
    }
}

void Protester::setOppositeDirection() {
    switch (getDirection()) {
        case left:  setDirection(right); break;
        case right: setDirection(left);  break;
        case up:    setDirection(down);  break;
        case down:  setDirection(up);    break;
        default:    break;
    }
}

GraphObject::Direction Protester::directionToPlayer()
{
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getY();

    int protesterX = getX();
    int protesterY = getY();

    if (protesterX == playerX)
    {
        return (protesterY < playerY) ? up : down;
    }
    
    if (protesterY == playerY)
    {
        return (protesterX > playerX) ? left : right;
    }

    return none;
}

bool Protester::straightPathToPlayer(Direction direction)
{
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getY();
    int x = getX();
    int y = getY();

    int dx = 0, dy = 0;

    switch (direction) {
        case left:  dx = -1; break;
        case right: dx = 1;  break;
        case up:    dy = 1;  break;
        case down:  dy = -1; break;
        default:    return false;
    }

    while (x != playerX || y != playerY) {
        if (getWorld()->isThereEarth(x, y) || getWorld()->isThereBoulder(x, y))
            return false;
        x += dx;
        y += dy;
    }

    return true;
}

GraphObject::Direction Protester::randomDirection()
{
    switch (rand() % 4) {
        case 0: return left; 
        case 1: return right;
        case 2: return up;
        case 3: return down;
        default: return none;
    }
}

bool Protester::isAtIntersection()
{
    return (getDirection() == up || getDirection() == down) ?(getWorld()->canMoveInDirection(getX(), getY(), left) ||  getWorld()->canMoveInDirection(getX(), getY(), right)) :(getWorld()->canMoveInDirection(getX(), getY(), up) || getWorld()->canMoveInDirection(getX(), getY(), down));
}

void Protester::pickViableDirectionToTurn()
{
    Direction dir1, dir2;

    if (getDirection() == up || getDirection() == down) {
        dir1 = left;
        dir2 = right;
    } else {
        dir1 = up;
        dir2 = down;
    }

    bool canMoveDir1 = getWorld()->canMoveInDirection(getX(), getY(), dir1);
    bool canMoveDir2 = getWorld()->canMoveInDirection(getX(), getY(), dir2);

    if (!canMoveDir1 && canMoveDir2) {
        setDirection(dir2);
    } else if (canMoveDir1 && !canMoveDir2) {
        setDirection(dir1);
    } else if (canMoveDir1 && canMoveDir2) {
        setDirection(rand() % 2 == 0 ? dir1 : dir2);
    }
}

void Protester::isAnnoyed(int hp)
{
    if (m_leave) 
        return;

    dechp(hp);
    getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    isStunned();

    if (gethp() > 0)
        return;

    getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
    m_leave = true;
    m_tickRest = 0;

    int score = (hp == 100) ? 500 : (getID() == TID_PROTESTER) ? 100 : 250;
    getWorld()->increaseScore(score);
}

void Protester::isStunned()
{
	m_tickRest = max(50, 100 - (int)getWorld()->getLevel()*10);
}

void Protester::isBribed()
{
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    int scoreIncrease = (getID() == TID_PROTESTER) ? 25 : 50;
    getWorld()->increaseScore(scoreIncrease);

    if (getID() == TID_PROTESTER)
    {
        m_leave = true;
    }
    else
    {
        m_tickRest = max(50, 100 - int(getWorld()->getLevel()) * 10);
    }
}

bool Protester::isFacingPlayer() {
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getY();

    switch (getDirection()) {
        case left:
            return playerX <= getX() && playerY == getY();
        case right:
            return playerX >= getX() && playerY == getY();
        case up:
            return playerY >= getY() && playerX == getX();
        case down:
            return playerY <= getY() && playerX == getX();
        default:
            return false;
    }
}

RegularProtester::RegularProtester(StudentWorld* world): Protester(world, TID_PROTESTER, 5) {}

HardcoreProtester::HardcoreProtester(StudentWorld* world): Protester(world, TID_HARD_CORE_PROTESTER, 20) {}