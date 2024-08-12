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
StudentWorld* Actor::getWorld()
	{
		return m_world;
	}
bool Actor::isAlive()
 	{
		return m_alive;
	}
void Actor::die() 
	{
		m_alive = false;
	}

void Actor::moveTo(int x, int y)
{
    if (x > 60) 
    {
        x = 60;
    }
    if (x < 0) 
    {
        x = 0;
    }
    if (y > 60)  
    {
        y = 60;
    }
    if (y < 0) 
    {
        y = 0;
    }
    GraphObject::moveTo(x, y);
}


//Man
Man::Man(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp):Actor(world, imageID, startX, startY, dir, 1.0, 0), m_hp(hp) {}
int Man::gethp() const
	{
		return m_hp;
	}

void Man::dechp(int hp_points)
	{
		m_hp -= hp_points;
	}

//Tunnelman
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
    {
        return;
    }
    
    if (getWorld()->digEarth(getX(), getY())) 
    {
        getWorld()->playSound(SOUND_DIG);
    }

    int ch;

    if (getWorld()->getKey(ch) == true) 
    {
        
        if (ch == KEY_PRESS_ESCAPE)
        {
            die();
        }
        else if (ch == KEY_PRESS_LEFT)
        {
            moveInDirection(left);
        }
        else if (ch == KEY_PRESS_RIGHT)
        {
            moveInDirection(right);
        }
        else if (ch == KEY_PRESS_UP)
        {
            moveInDirection(up);
        }
        else if (ch == KEY_PRESS_DOWN)
        {
            moveInDirection(down);
        }
        else if (ch == KEY_PRESS_SPACE)
        {
            if (m_wtr > 0) 
            {
                m_wtr--;
                shoot();
            }
        }
        else if (ch == 'z' || ch == 'Z') 
        {
            if (m_sonar > 0)
            {
                m_sonar--;
                getWorld()->detectNearActors(getX(), getY(), 12);
                getWorld()->playSound(SOUND_SONAR);
            }
        }
        else if (ch == KEY_PRESS_TAB) 
        {
            if (m_gld > 0) 
            {
                getWorld()->addActor(new Gold(getWorld(), getX(), getY(), true, true));
                m_gld--;
            }
        }
    }
}



void Tunnelman::shoot() 
{
    if (getDirection() == left)
    {
        if (!getWorld()->isThereEarth(getX() - 4, getY()) && !getWorld()->isThereBoulder(getX() - 4, getY())) 
        {
            getWorld()->addActor(new Squirt(getWorld(), getX() - 4, getY(), getDirection()));
        }
    }
    else if (getDirection() == right)
    {
        if (!getWorld()->isThereEarth(getX() + 4, getY()) && !getWorld()->isThereBoulder(getX() + 4, getY())) 
        {
            getWorld()->addActor(new Squirt(getWorld(), getX() + 4, getY(), getDirection()));
        }
    }
    else if (getDirection() == down)
    {
        if (!getWorld()->isThereEarth(getX(), getY() - 4) && !getWorld()->isThereBoulder(getX(), getY() - 4)) 
        {
            getWorld()->addActor(new Squirt(getWorld(), getX(), getY() - 4, getDirection()));
        }
    }
    else if (getDirection() == up)
    {
        if (!getWorld()->isThereEarth(getX(), getY() + 4) && !getWorld()->isThereBoulder(getX(), getY() + 4)) 
        {
            getWorld()->addActor(new Squirt(getWorld(), getX(), getY() + 4, getDirection()));
        }
    }
    else
    {
        return;
    }

    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
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
    if (direction == left) 
    {
        if (getDirection() == left) 
        {
            if (!getWorld()->isThereBoulder(getX() - 1, getY())) 
            {
                moveTo(getX() - 1, getY());
            }
        } 
        else 
        {
            setDirection(left);
        }
    }
    else 
    {
        if (direction == right) 
        {
            if (getDirection() == right) 
            {
                if (!getWorld()->isThereBoulder(getX() + 1, getY())) 
                {
                    moveTo(getX() + 1, getY());
                }
            } 
            else 
            {
                setDirection(right);
            }
        }
        else 
        {
            if (direction == up) 
            {
                if (getDirection() == up) 
                {
                    if (!getWorld()->isThereBoulder(getX(), getY() + 1)) 
                    {
                        moveTo(getX(), getY() + 1);
                    }
                } 
                else 
                {
                    setDirection(up);
                }
            }
            else 
            {
                if (direction == down) 
                {
                    if (getDirection() == down) 
                    {
                        if (!getWorld()->isThereBoulder(getX(), getY() - 1)) 
                        {
                            moveTo(getX(), getY() - 1);
                        }
                    } 
                    else 
                    {
                        setDirection(down);
                    }
                }
                else
                {
                    return;
                }
            }
        }
    }
}

int Tunnelman::getWtr() const
	{
		return m_wtr;
	}

int Tunnelman::getSonar() const
	{
		return m_sonar;
	}

int Tunnelman::getGld() const
	{
		return m_gld;
	}

//Earth
Earth::Earth(StudentWorld* world, int startX, int startY):Actor(world, TID_EARTH, startX, startY, right, 0.25, 3) {}

void Earth::doSomething() {} 


//Boulder
Boulder::Boulder(StudentWorld* world, int startX, int startY): Actor(world, TID_BOULDER, startX, startY, down, 1.0, 1),isStable(true),m_ticks(0)
	{
		world->digEarth(startX, startY);
	}
	
void Boulder::doSomething() 
{
    if (!isAlive()) 
    {
        return;
    }

    if (isStable) 
    {
        if (getWorld()->isAboveEarth(getX(), getY() - 1)) 
        {
            return;
        } 
        else 
        {
            isStable = false;
        }
    }

    if (m_ticks == 30) 
    {
        isFalling = true;
        getWorld()->playSound(SOUND_FALLING_ROCK);
    }

    m_ticks++;

    if (isFalling) 
    {
        if (getWorld()->isAboveEarth(getX(), getY() - 1) || 
            getWorld()->isThereBoulder(getX(), getY() - 4, 0) || 
            getY() == 0) 
        {
            die();
        } 
        else 
        {
            moveTo(getX(), getY() - 1);
        }

        annoyMan();
    }
}


void Boulder::annoyMan()
{
	if (getWorld()->isPlayerInRadius(this, 3)) 
	{
		getWorld()->getPlayer()->isAnnoyed(100);
	}
	Protester * p = getWorld()->protesterInRadius(this, 3);
	if (p != nullptr)
	{
		p->isAnnoyed(100);
	}
}

//Squirt
Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction dir):Actor(world, TID_WATER_SPURT, startX, startY, dir, 1.0, 1),m_travel(0) {}

void Squirt::doSomething() {
	if (!isAlive())
	{ 
		return;
	}
	
	if (annoyProtesters() || m_travel == 4)
	{
		die();
		return;
	}

	Direction dir = getDirection();
	
	if (dir == left)
	{
		if (getWorld()->isThereEarth(getX() - 1, getY()) || getWorld()->isThereBoulder(getX() - 1, getY()))
		{
			die();
			return;
		}
		else
		{
			moveTo(getX() - 1, getY());
		}
	}
	else if (dir == right)
	{
		if (getWorld()->isThereEarth(getX() + 1, getY()) || getWorld()->isThereBoulder(getX() + 1, getY()))
		{
			die();
			return;
		}
		else
		{
			moveTo(getX() + 1, getY());
		}
	}
	else if (dir == up)
	{
		if (getWorld()->isThereEarth(getX(), getY() + 1) || getWorld()->isThereBoulder(getX(), getY() + 1))
		{
			die();
			return;
		}
		else
		{
			moveTo(getX(), getY() + 1);
		}
	}
	else if (dir == down)
	{
		if (getWorld()->isThereEarth(getX(), getY() - 1) || getWorld()->isThereBoulder(getX(), getY() - 1))
		{
			die();
			return;
		}
		else
		{
			moveTo(getX(), getY() - 1);
		}
	}
	else
	{
		return;
	}

	m_travel++;
}


bool Squirt::annoyProtesters()
	{
		Protester * p= getWorld()->protesterInRadius(this,3);
		if (p != nullptr) 
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
    } 
    else 
    {
        m_tick++;
    }
}


//Barrel
Barrel::Barrel(StudentWorld* world, int startX, int startY):Pickupable(world, TID_BARREL, startX, startY)
	{
		setVisible(false);
	}
void Barrel::doSomething() {
	if (!isAlive()) 
		{
			return;
		}
	if (!isVisible() && getWorld()->isPlayerInRadius(this, 4))
		{
			setVisible(true);
			return;
		}
	if (getWorld()->isPlayerInRadius(this, 3))
	 	{
			die();
			getWorld()->playSound(SOUND_FOUND_OIL);
			getWorld()->increaseScore(1000);
			getWorld()->decBarrel();
			return;
		}
}

//Gold
Gold::Gold(StudentWorld* world, int startX, int startY, bool isVisible, bool dropped):Pickupable(world, TID_GOLD, startX, startY), isDropped(dropped)
	{
		setVisible(isVisible);
	}
void Gold::doSomething() {
	if (!isAlive()) 
	{
		return;
	}
	if (!isVisible() && getWorld()->isPlayerInRadius(this, 4))
		{
			setVisible(true);
			return;
		}
	if (!isDropped && getWorld()->isPlayerInRadius(this, 3))
 	{
		die();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->add(getID());
		getWorld()->increaseScore(10);
		return;
	}
	if (isDropped)
	{
		Protester* p = (getWorld()->protesterInRadius(this, 3));
		if (p != nullptr)
		{
			die();
			p->getBribed();
		};
		disappearIn(100);
	}
}

//Goodie
Goodie::Goodie(StudentWorld* world, int imageID, int startX, int startY):Pickupable(world, imageID, startX, startY) {}
void Goodie::doSomething(){
	if (!isAlive())
	{ 
		return;
	}
	if (getWorld()->isPlayerInRadius(this, 3))
	{
		die();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->add(getID());
		getWorld()->increaseScore(75);
		return;
	}
	disappearIn(std::max(100, 300 - 10 * (int)getWorld()->getLevel()));
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
	else 
	{
		m_tickRest = max(0, 3 - (int)getWorld()->getLevel() / 4);
		m_tickSinceLastTurn++;
		m_tickNoYell++;
	}
	
	if (m_leave)
	{
		if (getX() == 60 && getX() == 60)
		{
			die();
            getWorld()->decProtester();
			return;
		}
        getWorld()->moveToExit(this);
		return;	
	}

	

	if (getWorld()->isPlayerInRadius(this, 4) && isFacingPlayer())
	{
		if (m_tickNoYell > 15)
		{
			getWorld()->getPlayer()->isAnnoyed(2);
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			m_tickNoYell = 0;
			return;
		}
		return;
	}
    
    if(getID()==TID_HARD_CORE_PROTESTER)
	{
        int M =16 + int(getWorld()->getLevel());
        Direction s= getWorld()->senseSignalFromPlayer(this, M);
        if(s != none)
		{
            moveInDirection(s);
            return;
        }
    }
	
	Direction d = directionToPlayer();
	if (d != none && straightPathToPlayer(d) && (!getWorld()->isPlayerInRadius(this, 4)))
	{
		setDirection(d);
		moveInDirection(d);
		m_numToMove = 0;
		return;
	}
	
	m_numToMove--;
	if (m_numToMove <= 0)
	{
		Direction k = none;
		k = randomDirection();
		while (true)
		{
			k = randomDirection();
			if (getWorld()->canMoveInDirection(getX(),getY(), k))
			{
				break;
			}
		}
		setDirection(k);
		randomNumToMove();
	}
	
	else if (isAtIntersection() && m_tickSinceLastTurn > 200)
	{
		pickViableDirectionToTurn();
		m_tickSinceLastTurn = 0;
		randomNumToMove();
	}
	
	moveInDirection(getDirection());
	
	if (!getWorld()->canMoveInDirection(getX(),getY(),getDirection()))
		{
			m_numToMove = 0;
		}
}

void Protester::moveInDirection(Direction direction)
{
    if (direction == left) 
    {
        if (getDirection() == left) 
        {
            if (getX() == 0) 
            {
                setDirection(right);
            }
            else
            {
                moveTo(getX() - 1, getY());
            }
        } 
        else 
        {
            setDirection(left);
        }
    }
    else if (direction == right) 
    {
        if (getDirection() == right) 
        {
            if (getX() == 60) 
            {
                setDirection(left);
            }
            else
            {
                moveTo(getX() + 1, getY());
            }
        } 
        else 
        {
            setDirection(right);
        }
    }
    else if (direction == up) 
    {
        if (getDirection() == up) 
        {
            if (getY() == 60) 
            {
                setDirection(down);
            }
            else
            {
                moveTo(getX(), getY() + 1);
            }
        } 
        else 
        {
            setDirection(up);
        }
    }
    else if (direction == down) 
    {
        if (getDirection() == down) 
        {
            if (getY() == 0) 
            {
                setDirection(up);
            }
            else
            {
                moveTo(getX(), getY() - 1);
            }
        } 
        else 
        {
            setDirection(down);
        }
    }
    else
    {
        return;
    }
}




GraphObject:: Direction Protester::directionToPlayer()
{
	int playerX = getWorld()->getPlayer()->getX();
	int playerY = getWorld()->getPlayer()->getY();
	if (getY() == playerY && getX() == playerX)
		{
			return getDirection();
		}
	if (getX() == playerX)
	{
		if (getY() < playerY)
			{
				return up;
			}
		if (getY() > playerY)
			{
				return down;
			}
	}
	if (getY() == playerY)
	{
		if (getX() > playerX)
			{
				return left;
			}
		if (getY() < playerY)
			{
				return right;
			}
	}
	
	return none;   // didnt see player in straight line
}

bool Protester::straightPathToPlayer(Direction direction)
{
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getY(); // Corrected to getY(), not getX()

    if (direction == left)
    {
        for (int i = getX(); i >= playerX; i--) 
        {
            if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereBoulder(i, getY()))
            {
                return false;
            }
        }
        return true;
    }
    else if (direction == right)
    {
        for (int i = getX(); i <= playerX; i++) 
        {
            if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereBoulder(i, getY()))
            {
                return false;
            }
        }
        return true;
    }
    else if (direction == up)
    {
        for (int j = getY(); j <= playerY; j++) 
        {
            if (getWorld()->isThereEarth(getX(), j) || getWorld()->isThereBoulder(getX(), j))
            {
                return false;
            }
        }
        return true;
    }
    else if (direction == down)
    {
        for (int j = getY(); j >= playerY; j--) 
        {
            if (getWorld()->isThereEarth(getX(), j) || getWorld()->isThereBoulder(getX(), j))
            {
                return false;
            }
        }
        return true;
    }
    else if (direction == none)
    {
        return false;
    }
    else
    {
        return false;
    }
}


GraphObject::Direction Protester::randomDirection()
{
	int r;
	r = rand() % 4;
	switch (r) {
		case 0: return left; 
		case 1: return  right;
		case 2: return up;
		case 3: return down;
	}
    return none;
}
bool Protester::isAtIntersection()
{
	if (getDirection() == up || getDirection() == down) {
		return (getWorld()->canMoveInDirection(getX(),getY(), left) || getWorld()->canMoveInDirection(getX(),getY(), right));
	}
	else
		return (getWorld()->canMoveInDirection(getX(),getY(), up) || getWorld()->canMoveInDirection(getX(),getY(), down));
}
void Protester::pickViableDirectionToTurn()
{
    if (getDirection() == up || getDirection() == down) 
    {
        if (!getWorld()->canMoveInDirection(getX(), getY(), left)) 
        {
            setDirection(right);
        }
        else if (!getWorld()->canMoveInDirection(getX(), getY(), right)) 
        {
            setDirection(left);
        }
        else 
        {
            switch (rand() % 2) 
            {
                case 0: 
                    setDirection(left);
                    break;
                case 1: 
                    setDirection(right);
                    break;
            }
        }
    }
    else 
    {
        if (!getWorld()->canMoveInDirection(getX(), getY(), up)) 
        {
            setDirection(down);
        }
        else if (!getWorld()->canMoveInDirection(getX(), getY(), down)) 
        {
            setDirection(up);
        }
        else 
        {
            switch (rand() % 2) 
            {
                case 0: 
                    setDirection(up);
                    break;
                case 1: 
                    setDirection(down);
                    break;
            }
        }
    }
}

void Protester::isAnnoyed(int hp)
{
    if (m_leave) 
    {
        return;
    }
    
    dechp(hp);
    getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    getStunned();
    
    if (gethp() <= 0) 
    {
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        m_leave = true;
        m_tickRest = 0;

        if (hp == 100) 
        {
            getWorld()->increaseScore(500); 
        } 
        else if (getID() == TID_PROTESTER) 
        {
            getWorld()->increaseScore(100);
        } 
        else 
        {
            getWorld()->increaseScore(250);
        }
    }
}

void Protester::getStunned()
{
	m_tickRest = max(50, 100 - (int)getWorld()->getLevel()*10);
}
void Protester::getBribed()
{
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    if(getID()==TID_PROTESTER)
		{
			getWorld()->increaseScore(25);
			m_leave = true;
		}
    else 
		{
			getWorld()->increaseScore(50);
			m_tickRest = max(50,100- int(getWorld()->getLevel())*10);
		}
}

bool Protester::isFacingPlayer()
{
    if (getDirection() == left) 
    {
        return getWorld()->getPlayer()->getX() <= getX();
    }
    else if (getDirection() == right) 
    {
        return getWorld()->getPlayer()->getX() >= getX();
    }
    else if (getDirection() == up) 
    {
        return getWorld()->getPlayer()->getY() >= getY();
    }
    else if (getDirection() == down) 
    {
        return getWorld()->getPlayer()->getY() <= getY();
    }
    else if (getDirection() == none) 
    {
        return false;
    }
    else 
    {
        return false;
    }
}


//RegularProtester
RegularProtester::RegularProtester(StudentWorld* world): Protester(world, TID_PROTESTER, 5) {}

//HardcoreProtester
HardcoreProtester::HardcoreProtester(StudentWorld* world): Protester(world, TID_HARD_CORE_PROTESTER, 20) {}