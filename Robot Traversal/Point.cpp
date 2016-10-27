/* 
 *
 * Last edited on Dec 9th, 2015
 */

#include "../include/Point.h"

Point::Point()
{
    x = -1;
    y = -1;
    colorA = WHITE;
    colorB = WHITE;
    isInPath = false;
}

Point::Point( int xCoor, int yCoor ) 
{
    x = xCoor;
    y = yCoor;
    colorA = WHITE;
    colorB = WHITE;
    isInPath = false;
}

Point::Point(const Point& orig) 
{
    this->x = orig.x;
    this->y = orig.y;
    this->colorA = orig.colorA;
    this->colorB = orig.colorB;
    this->isInPath = orig.isInPath;
}

Point::~Point() 
{
    
}

void Point::set(int xCoor, int yCoor)
{
    x = xCoor;
    y = yCoor;
}

void Point::setColorA(COLOR c)
{
    colorA = c;
}

void Point::setColorB(COLOR c)
{
    colorB = c;
}

int Point::getX()
{
    return x;
}

int Point::getY()
{
    return y;
}

COLOR Point::getColorA()
{
    return colorA;
}

COLOR Point::getColorB()
{
    return colorB;
}

bool Point::getIsInPath()
{
    return isInPath;
}

void Point::setIsInPath(bool inPath)
{
    isInPath = inPath;
}
