/* 
 * 
 * Last edited on Dec 9th, 2015
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>
#include <math.h>

#include "../include/Point.h"
#include "../include/errors.h"

using namespace std;

int getInterfernceParameter(char* radius);
void readFile(char* fileName, vector< vector<char> >& room);
void getEdgesAndVertices( vector< vector<char> >& room,
    vector< vector<int> >& roomKey, vector< Point* >& vertices,
    vector< vector< int > >& edges, int startEndPos[]);

bool traverseRoom( vector< Point* >& vertices,
    vector< vector< int > >& edges, vector< vector<int> >& key,
    int startEndPos[], const int r);

void cleanUp(vector< vector<char> >& v);
void cleanUp(vector< vector<int> >& v);
void cleanUp(vector< Point* > v);

enum SPECIAL_CHARS_IN_ROOM
{
    S,
    E,
    F,
    L,
};

/*
 * Correct command line arguments is given as ./executableName fileName r
 */
int main(int argc, char** argv) 
{
    if( argc != 3 )
    {
        cout << "Usage: ./executableName fileName r\n";
        exit(INCORRECT_COMMAND_LINE_ARGUMENTS);
    }
    
    int r = getInterfernceParameter(argv[2]);
    
    vector< vector<char> > room;
    vector<char> roomTemp;
    room.push_back(roomTemp);
    readFile(argv[1], room);    

    //Vector of pointers of points? 
    //Store points in an array so each point has a key.
    vector< Point* > vertices;
    vector< vector< int > > edges;
    
    vector< vector<int> > key;//Remembers the key of each point in the room in vertices.    
    int startEndPos[4];
    
    getEdgesAndVertices(room, key, vertices, edges, startEndPos);
    
    cleanUp(room);   
    
    bool successful = false; //Whether the traversal is successful or not.
    
    if( r != 0 )
    {
        if( abs(vertices.at(startEndPos[S])->getX()-vertices.at(startEndPos[F])->getX() ) 
            + abs(vertices.at(startEndPos[S])->getY()-vertices.at(startEndPos[F])->getY() ) <= r  )
        {
            cleanUp(key);
            cleanUp(vertices);
            cleanUp(edges);
            
            cout << endl << "There is no way for the robots to traverse the room as the starting points are too close.\n";
            exit(IMPOSSIBLE_TO_TRAVERSE);
               
        }
        
        if( abs(vertices.at(startEndPos[E])->getX()-vertices.at(startEndPos[L])->getX() ) 
            + abs(vertices.at(startEndPos[E])->getY()-vertices.at(startEndPos[L])->getY() ) <= r  )
        {
            cleanUp(key);
            cleanUp(vertices);
            cleanUp(edges);
            
            cout << endl << "There is no way for the robots to traverse the room as the ending points are too close.\n";
            exit(IMPOSSIBLE_TO_TRAVERSE);
               
        }                        
    }
    
    successful = traverseRoom(vertices, edges, key, startEndPos, r );
        
    cleanUp(key);
    cleanUp(vertices);
    cleanUp(edges);
    
    if(!successful)
    {
        cout << endl << "There is no way for the robots to traverse the room.\n";
        exit(IMPOSSIBLE_TO_TRAVERSE);
    }
    
    cout << "\n\nThe robots have reached their destinations.\n";
    
    return 0;
}

//Traverse the robots throughout the room when r = 0.
//The main algorithm used is a breadth-first search.

//Separate functions were not used
//for BFS because there are different restrictions
//on the different vertices at different 
//stages of the program.
bool traverseRoom( vector< Point* >& vertices,
    vector< vector< int > >& edges,  vector< vector<int> >& key,
    int startEndPos[], const int r)
{
    //Keeps track of the key of the previous vertex through the BST.
    //prevVertex.at(10) would give the key of the parent of the vertex with key 10.
    //-1 signifies that there is no previous parent.
    vector<int> prevVertexA;
    vector<int> prevVertexB;   
    
    prevVertexA.resize( vertices.size(), -1 );   
    prevVertexB.resize( vertices.size(), -1 ); 
    
    int aMovesFirstCount = 0;
    
    //Clears colors.
    for( int i = 0; i < vertices.size(); i++ )
    {
        vertices.at(i)->setColorA(WHITE);
        vertices.at(i)->setColorB(WHITE);
    }
    
    int sPosition = startEndPos[E];
    int fPosition = startEndPos[L];
    
    //Does a BFS for the first robot.
    queue<int> q;
    q.push(startEndPos[S]);
    
    while( !q.empty())
    {
        int curr = q.front();
        q.pop();
        vertices.at(curr)->setColorA(BLACK);
        
        for( int i = 0; i < edges.at(curr).size(); i++ )
        {
            int v = edges.at(curr).at(i);
            //Vertex has not yet been explored.
            if( vertices.at(v)->getColorA() == WHITE )
            {                
                //Remember the parent.
                prevVertexA.at(key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY())) 
                    = key.at(vertices.at(curr)->getX()).at(vertices.at(curr)->getY());
                
                q.push(v);
                vertices.at(v)->setColorA(GREY);  
            }                      
        }
    }
    
    //Find the path of getting the first robot from start to finish.    
    stack<int> pathKeysA;
    pathKeysA.push(sPosition);
    
    //There is no path found.
    if( prevVertexA.at(sPosition) == -1)
        return false;
    while( prevVertexA.at(sPosition) != -1 )
    {
        vertices.at(sPosition)->setIsInPath(true);
        pathKeysA.push( prevVertexA.at(sPosition) );
        sPosition = prevVertexA.at(sPosition);
    }                                        
   

    //Does a BFS for the second robot.    
    q.push(startEndPos[F]);
    while( !q.empty() )
    {
        int curr = q.front();
        q.pop();
        vertices.at(curr)->setColorB(BLACK);
        
        for( int i = 0; i < edges.at(curr).size(); i++ )
        {
            int v = edges.at(curr).at(i);
            //Vertex has not yet been explored.
            if( vertices.at(v)->getColorB() == WHITE  )                
            {
                //Remember the parent.
                prevVertexB.at(key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY())) 
                    = key.at(vertices.at(curr)->getX()).at(vertices.at(curr)->getY());

                q.push(v);
                vertices.at(v)->setColorB(GREY);
            }                      
        }
    }
    
    //Find the path of getting the second robot from start to finish.    
    stack<int> pathKeysB;
    pathKeysB.push(fPosition);
    //There is no path found.
    if( prevVertexB.at(fPosition) == -1 )
        return false;
    while( prevVertexB.at(fPosition) != -1 )
    {
        vertices.at(fPosition)->setIsInPath(true);
        pathKeysB.push( prevVertexB.at(fPosition) );
        fPosition = prevVertexB.at(fPosition);
    }
    
    int count = 0;
    
    bool stuck = false; // a special case so we know that the two robots are stuck.
    
    while( sPosition != startEndPos[E] || fPosition != startEndPos[L] )
    {      
        if( !stuck )
        {
            //Move robot A.
            while ( !pathKeysA.empty() )
            {                      
                for( int i = 0; i < 100; i++ )
                    cout << endl;

                //Checks whether there is a conflict.
                if( pathKeysA.top() != fPosition )
                {
                    sPosition = pathKeysA.top();
                    pathKeysA.pop();

                    for( int i = 0; i < key.size(); i++ )
                    {
                        for( int j = 0; j < key.at(i).size(); j++ )
                        {
                            int pos = key.at(i).at(j);
                            if( pos == -1 )
                                cout << "#";
                            else if( pos == sPosition )
                                cout << "S";
                            else if( pos == fPosition )
                                cout << "F";
                            else if( pos == startEndPos[E] )
                                cout << "E";
                            else if( pos == startEndPos[L] )
                                cout << "L";
                            else
                                cout << " ";                
                        }
                        cout << endl;
                    }  
                    cin.ignore(); 
                }
                else
                    break;
            }                            

            //Move robot B.
            while ( !pathKeysB.empty() )
            {                      
                for( int i = 0; i < 100; i++ )
                    cout << endl;

                //Checks whether there is a conflict.
                if( pathKeysB.top() != sPosition )
                {
                    fPosition = pathKeysB.top();
                    pathKeysB.pop();

                    for( int i = 0; i < key.size(); i++ )
                    {
                        for( int j = 0; j < key.at(i).size(); j++ )
                        {
                            int pos = key.at(i).at(j);
                            if( pos == -1 )
                                cout << "#";
                            else if( pos == sPosition )
                                cout << "S";
                            else if( pos == fPosition )
                                cout << "F";
                            else if( pos == startEndPos[E] )
                                cout << "E";
                            else if( pos == startEndPos[L] )
                                cout << "L";
                            else
                                cout << " ";                
                        }
                        cout << endl;
                    }  
                    cin.ignore(); 
                }
                else
                    break;
            }

            //Clears colors.
            for( int i = 0; i < vertices.size(); i++ )
            {
                vertices.at(i)->setColorA(WHITE);
                vertices.at(i)->setColorB(WHITE);
            }  

            //The robots still haven't reached their destinations.
            if( !(pathKeysA.empty() && pathKeysB.empty()) )
            {
                //If robot B has already reached destination, then find another path for robot A.
                if( pathKeysB.empty() && aMovesFirstCount != 1)
                {
                    //Clear path of A.
                    while( !pathKeysA.empty() )
                        pathKeysA.pop();

                    prevVertexA.clear();
                    prevVertexA.resize( vertices.size(), -1 );  

                    //Does a BFS for the first robot, except don't consider the vertex where robot B is at right now.
                    q.push(sPosition);
                    while( !q.empty() )
                    {
                        int curr = q.front();
                        q.pop();
                        vertices.at(curr)->setColorA(BLACK);

                        for( int i = 0; i < edges.at(curr).size(); i++ )
                        {
                            int v = edges.at(curr).at(i);
                            //Vertex has not yet been explored.
                            if( vertices.at(v)->getColorA() == WHITE 
                                && key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY()) != fPosition )
                            {                
                                //Remember the parent.
                                prevVertexA.at(key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY())) 
                                    = key.at(vertices.at(curr)->getX()).at(vertices.at(curr)->getY());

                                q.push(v);
                                vertices.at(v)->setColorA(GREY);                                  
                            }                      
                        }
                    }

                    int origS = sPosition;
                    //Find the path of getting the first robot from start to finish.
                    sPosition = startEndPos[E];
                    pathKeysA.push(sPosition); 

                    //A flag that says cannot be found.
                    if( prevVertexA.at(sPosition) == -1 )
                    {
                        stuck = true;
                        sPosition = origS;
                    }
                    while( prevVertexA.at(sPosition) != -1 )
                    {
                        pathKeysA.push( prevVertexA.at(sPosition) );
                        sPosition = prevVertexA.at(sPosition);
                    }        

                }
                //If robot A has already reached destination, then find another path for robot B.
                //Or both haven't reached their destinations yet.
                else
                {
                    //clear path of B.
                    while( !pathKeysB.empty() )
                        pathKeysB.pop();

                    prevVertexB.clear();
                    prevVertexB.resize( vertices.size(), -1 );  

                    //Does a BFS for the second robot (robot B), except don't consider the vertex where robot A is at right now.
                    q.push(fPosition);
                    while( !q.empty() )
                    {
                        int curr = q.front();
                        q.pop();
                        vertices.at(curr)->setColorB(BLACK);

                        for( int i = 0; i < edges.at(curr).size(); i++ )
                        {
                            int v = edges.at(curr).at(i);
                            //Vertex has not yet been explored.
                            if( vertices.at(v)->getColorB() == WHITE 
                                && key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY()) != sPosition )
                            {                
                                //Remember the parent.
                                prevVertexB.at(key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY())) 
                                    = key.at(vertices.at(curr)->getX()).at(vertices.at(curr)->getY());

                                q.push(v);
                                vertices.at(v)->setColorB(GREY);                                  
                            }                      
                        }
                    }

                    //Find the path of getting the first robot from start to finish.
                    int origF = fPosition;
                    fPosition = startEndPos[L];
                    pathKeysB.push(fPosition);       

                    //A flag that says cannot be found.
                    if( prevVertexB.at(fPosition) == -1)
                    {
                        stuck = true;
                        fPosition = origF;
                    }
                    while( prevVertexB.at(fPosition) != -1 )
                    {                       
                        pathKeysB.push( prevVertexB.at(fPosition) );
                        fPosition = prevVertexB.at(fPosition);
                    } 

                }
            }

            //If the two are stuck, find a vertex with three or more adjacent neighbors 
            //so the conflict can be resolved.
            //If there is no such vertex, there is no way to traverse the room.
            if( stuck && aMovesFirstCount != 1)
            {
                aMovesFirstCount = 0;
                prevVertexA.clear();
                prevVertexA.resize( vertices.size(), -1 ); 
                prevVertexB.clear();
                prevVertexB.resize( vertices.size(), -1 ); 

                while( !pathKeysA.empty() )
                    pathKeysA.pop();
                while( !pathKeysB.empty() )
                    pathKeysB.pop();
                
                //Clears colors.
                for( int i = 0; i < vertices.size(); i++ )
                {
                    vertices.at(i)->setColorA(WHITE);
                    vertices.at(i)->setColorB(WHITE);
                }
                
                bool threeVs = false;
                int threeVKey;//Key of the vertex with three adjacent vertices.                
                
                //Does a BFS for the second robot to find a vertex with 
                //three more or more adjacent vertices.
                q.push(fPosition);
                while( !q.empty() )
                {
                    int curr = q.front();
                    q.pop();
                    vertices.at(curr)->setColorB(BLACK);
                    
                    if( edges.at(curr).size() >= 3 )
                    {
                        threeVs = true;
                        threeVKey = curr;
                    }
                    for( int i = 0; i < edges.at(curr).size(); i++ )
                    {
                        int v = edges.at(curr).at(i);
                        //Vertex has not yet been explored.
                        if( vertices.at(v)->getColorB() == WHITE 
                            && key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY()) != sPosition )
                        {                
                            //Remember the parent.
                            prevVertexB.at(key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY())) 
                                = key.at(vertices.at(curr)->getX()).at(vertices.at(curr)->getY());

                            q.push(v);
                            vertices.at(v)->setColorB(GREY);                                  
                        }                      
                    }
                }                

                //If not found for the second robot, do it for the first.
                if( !threeVs )
                {
                    prevVertexA.clear();
                    prevVertexA.resize( vertices.size(), -1 ); 
                    prevVertexB.clear();
                    prevVertexB.resize( vertices.size(), -1 ); 

                    while( !pathKeysA.empty() )
                        pathKeysA.pop();
                    while( !pathKeysB.empty() )
                        pathKeysB.pop();

                    //Clears colors.
                    for( int i = 0; i < vertices.size(); i++ )
                    {
                        vertices.at(i)->setColorA(WHITE);
                        vertices.at(i)->setColorB(WHITE);
                    }

                    threeVs = false;

                    //Does a BFS for the first robot to find a vertex with 
                    //three more or more adjacent vertices.
                    q.push(sPosition);
                    while( !q.empty() )
                    {
                        int curr = q.front();
                        q.pop();
                        vertices.at(curr)->setColorA(BLACK);

                        if( edges.at(curr).size() >= 3 )
                        {
                            threeVs = true;
                            threeVKey = curr;
                        }
                        for( int i = 0; i < edges.at(curr).size(); i++ )
                        {
                            int v = edges.at(curr).at(i);
                            //Vertex has not yet been explored.
                            if( vertices.at(v)->getColorA() == WHITE 
                                && key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY()) != fPosition )
                            {                
                                //Remember the parent.
                                prevVertexA.at(key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY())) 
                                    = key.at(vertices.at(curr)->getX()).at(vertices.at(curr)->getY());

                                q.push(v);
                                vertices.at(v)->setColorA(GREY);                                  
                            }                      
                        }
                    }
                    
                    //There is no way to traverse the room.
                    if( !threeVs )
                        return false;

                    //Find the path of getting the first robot from original position
                    //to a vertex that is not in the path and adjacent to a a vertex
                    //with at least three adjacent vertices.
                   
                    int remain = 0;//The adjacent vertex of the vertex that is not in the paths.
                    
                    //Find the path.
                    vector<int> pathKeysTemp;
                    int tempS = startEndPos[E];
                    pathKeysTemp.push_back(tempS);      

                    while( prevVertexA.at(tempS) != -1 )
                    {
                        pathKeysTemp.push_back( prevVertexA.at(tempS) );
                        tempS = prevVertexA.at(tempS);
                    } 
                    
                    //Find the neighbor of the vertex that is not in the path.
                    for( int i = 0; i < edges.at(threeVKey).size(); i++ )
                    {
                        if( !vertices.at( edges.at(threeVKey).at(i))->getIsInPath() )
                        {                            
                            remain = edges.at(threeVKey).at(i);   
                        }
                    }
                    pathKeysTemp.clear();
              
                    sPosition = remain;
                    
                    while( !pathKeysA.empty() )
                        pathKeysA.pop();
                    pathKeysA.push(sPosition);       

                    while( prevVertexA.at(sPosition) != -1 )
                    {
                        pathKeysA.push( prevVertexA.at(sPosition) );
                        sPosition = prevVertexA.at(sPosition);
                    } 
                    
                    //Clears colors.
                    for( int i = 0; i < vertices.size(); i++ )
                    {
                        vertices.at(i)->setColorA(WHITE);
                        vertices.at(i)->setColorB(WHITE);
                    }
                        
                    //Does a BFS for the second robot.
                    //Robots A and B could only be stuck if they belong in the same forest.
                    q.push(fPosition);
                    while( !q.empty() )
                    {
                        int curr = q.front();
                        q.pop();
                        vertices.at(curr)->setColorB(BLACK);

                        for( int i = 0; i < edges.at(curr).size(); i++ )
                        {
                            int v = edges.at(curr).at(i);
                            //Vertex has not yet been explored.
                            if( vertices.at(v)->getColorB() == WHITE )
                            {                
                                //Remember the parent.
                                prevVertexB.at(key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY())) 
                                    = key.at(vertices.at(curr)->getX()).at(vertices.at(curr)->getY());

                                q.push(v);
                                vertices.at(v)->setColorB(GREY);                                  
                            }                      
                        }
                    }                                                   
                    
                    //Find the path of getting the second robot from start to finish.
                    fPosition = startEndPos[L];
                    while( !pathKeysB.empty() )
                        pathKeysB.pop();
                    pathKeysB.push(fPosition); 

                    while( prevVertexB.at(fPosition) != -1 )
                    {
                        pathKeysB.push( prevVertexB.at(fPosition) );
                        fPosition = prevVertexB.at(fPosition);
                    }
                    
                    //Clears colors.
                    for( int i = 0; i < vertices.size(); i++ )
                    {
                        vertices.at(i)->setColorA(WHITE);
                        vertices.at(i)->setColorB(WHITE);
                    }
                    
                    prevVertexA.clear();
                    prevVertexA.resize( vertices.size(), -1 ); 
                    prevVertexB.clear();
                    prevVertexB.resize( vertices.size(), -1 ); 
                                                            
                }//End if threeVs
                
                //NEED TO FIX THIS..................
                else
                {              
                    aMovesFirstCount++; //Used for r>0, but this currently does not work.
                    
                    //Find the path of getting the first robot from original position
                    //to a vertex that is not in the path and adjacent to a a vertex
                    //with at least three adjacent vertices.                   
                    
                    int remain = 0;
                    
                    //Find the path.
                    vector<int> pathKeysTemp;
                    int tempF = startEndPos[L];
                    pathKeysTemp.push_back(tempF);      

                    while( prevVertexB.at(tempF) != -1 )
                    {
                        pathKeysTemp.push_back( prevVertexB.at(tempF) );
                        tempF = prevVertexB.at(tempF);
                    } 
                    
                    //Find the neighbor of the vertex that is not in the path.
                    for( int i = 0; i < edges.at(threeVKey).size(); i++ )
                    {
                        if( !vertices.at( edges.at(threeVKey).at(i))->getIsInPath() )
                        {                            
                            remain = edges.at(threeVKey).at(i);   
                        }
                    }
                    pathKeysTemp.clear();
              
                    fPosition = remain;
                    
                    while( !pathKeysB.empty() )
                        pathKeysB.pop();
                    pathKeysB.push(fPosition);       

                    while( prevVertexB.at(fPosition) != -1 )
                    {
                        pathKeysB.push( prevVertexB.at(fPosition) );
                        fPosition = prevVertexB.at(fPosition);
                    } 
                    
                    //Clears colors.
                    for( int i = 0; i < vertices.size(); i++ )
                    {
                        vertices.at(i)->setColorA(WHITE);
                        vertices.at(i)->setColorB(WHITE);
                    }
                        
                    //Does a BFS for the first robot.
                    //Robots A and B could only be stuck if they belong in the same forest.
                    q.push(sPosition);
                    while( !q.empty() )
                    {
                        int curr = q.front();
                        q.pop();
                        vertices.at(curr)->setColorA(BLACK);

                        for( int i = 0; i < edges.at(curr).size(); i++ )
                        {
                            int v = edges.at(curr).at(i);
                            //Vertex has not yet been explored.
                            if( vertices.at(v)->getColorA() == WHITE )
                            {                
                                //Remember the parent.
                                prevVertexA.at(key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY())) 
                                    = key.at(vertices.at(curr)->getX()).at(vertices.at(curr)->getY());

                                q.push(v);
                                vertices.at(v)->setColorA(GREY);                                  
                            }                      
                        }
                    }                                                   
                    
                    //Find the path of getting the first robot to the original position.
                    //Of the other robot.
                    sPosition = startEndPos[F];
                    while( !pathKeysA.empty() )
                        pathKeysA.pop();
                    pathKeysA.push(sPosition); 

                    while( prevVertexA.at(sPosition) != -1 )
                    {
                        pathKeysA.push( prevVertexA.at(sPosition) );
                        sPosition = prevVertexA.at(sPosition);
                    }
                    
                    //Clears colors.
                    for( int i = 0; i < vertices.size(); i++ )
                    {
                        vertices.at(i)->setColorA(WHITE);
                        vertices.at(i)->setColorB(WHITE);
                    }
                    
                    prevVertexA.clear();
                    prevVertexA.resize( vertices.size(), -1 ); 
                    prevVertexB.clear();
                    prevVertexB.resize( vertices.size(), -1 ); 
                }
            }            
        }
        else
        {
            stuck = false;
        }
        
        
        //If after 10 passes, it still does not work, do a BFS one last time.
        if( count > 10 )
        {            
            prevVertexA.clear();
            prevVertexB.clear();
            prevVertexA.resize( vertices.size(), -1 );   
            prevVertexB.resize( vertices.size(), -1 ); 

            //Clears colors.
            for( int i = 0; i < vertices.size(); i++ )
            {
                vertices.at(i)->setColorA(WHITE);
                vertices.at(i)->setColorB(WHITE);
            }

            //Does a BFS for the first robot.
            q.push(sPosition);

            while( !q.empty() )
            {
                int curr = q.front();
                q.pop();
                vertices.at(curr)->setColorA(BLACK);

                for( int i = 0; i < edges.at(curr).size(); i++ )
                {
                    int v = edges.at(curr).at(i);
                    //Vertex has not yet been explored.
                    if( vertices.at(v)->getColorA() == WHITE )
                    {                
                        //Remember the parent.
                        prevVertexA.at(key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY())) 
                            = key.at(vertices.at(curr)->getX()).at(vertices.at(curr)->getY());

                        q.push(v);
                        vertices.at(v)->setColorA(GREY);                                  
                    }                      
                }
            }

            //Find the path of getting the first robot from start to finish.
            while( !pathKeysA.empty() )                
                pathKeysA.pop();
            sPosition = startEndPos[E];
            pathKeysA.push(sPosition);
            while( prevVertexA.at(sPosition) != -1 )
            {
                pathKeysA.push( prevVertexA.at(sPosition) );
                sPosition = prevVertexA.at(sPosition);
            }                                        

            //Does a BFS for the second robot.    
            q.push(fPosition);
            while( !q.empty() )
            {
                int curr = q.front();
                q.pop();
                vertices.at(curr)->setColorB(BLACK);

                for( int i = 0; i < edges.at(curr).size(); i++ )
                {
                    int v = edges.at(curr).at(i);
                    //Vertex has not yet been explored.
                    if( vertices.at(v)->getColorB() == WHITE  )                
                        {
                            //Remember the parent.
                            prevVertexB.at(key.at(vertices.at(v)->getX()).at(vertices.at(v)->getY())) 
                                = key.at(vertices.at(curr)->getX()).at(vertices.at(curr)->getY());

                            q.push(v);
                            vertices.at(v)->setColorB(GREY);                                  
                        }                      
                }
            }

            //Find the path of getting the second robot from start to finish. 
            while( !pathKeysB.empty() )                
                pathKeysB.pop();
            fPosition = startEndPos[L];
            pathKeysB.push(fPosition);
            while( prevVertexB.at(fPosition) != -1 )
            {
                pathKeysB.push( prevVertexB.at(fPosition) );
                fPosition = prevVertexB.at(fPosition);
            }
        }

        count++;
    }

    prevVertexA.clear();
    prevVertexB.clear();
    
    return true;
}

//Converts the r from command line into an integer.
int getInterfernceParameter(char* radius)
{
    int counter = 0;
    while(radius[counter]!='\0')
    {
        if( !isdigit(radius[counter]) ) //There is a non-digit character.
        {
            cout << "Interference parameter given is not a non-negative integer.";
            exit(INVALID_COMMAND_INPUT);
        }
        counter++;
    }
    
    return (int) strtol(radius, NULL, 10 );
}

//Read contents of file into vector. Also, return the maximum of
//the number of rows or number of columns.
void readFile(char* fileName, vector< vector<char> >& room)
{
    fstream file;
    file.open( fileName, fstream::in );
    if( !file.is_open() )
    {
        cleanUp(room);
        cout << "Cannot open input file.\n";
        exit(INPUT_FILE_FAILED_TO_OPEN); 
    }
    
    //Whether the starting and ending positions are found or not.
    bool foundS = false, foundE = false, foundF = false, foundL = false;
    
    //Current position in room.
    int x = 0, y = 0;
    
    char c = '#'; // A dummy value.
    while( c != EOF )
    {
        c = file.get();
        
        switch(c)
        {
            case EOF :
                break;
                
            case 'S' :
                if( foundS )
                {
                    cleanUp(room);
                    cout << "Invalid room: duplicate S.\n";
                    exit(INVALID_ROOM_DUPLICATE_STARTING_ENDING_POINTS);
                }
                foundS = true;
                break;
                
            case 'E' :
                if( foundE )
                {
                    cleanUp(room);
                    cout << "Invalid room: duplicate E.\n";
                    exit(INVALID_ROOM_DUPLICATE_STARTING_ENDING_POINTS);
                }
                foundE = true;
                break;
                
            case 'F' :
                if( foundF )
                {
                    cleanUp(room);
                    cout << "Invalid room: duplicate F.\n";
                    exit(INVALID_ROOM_DUPLICATE_STARTING_ENDING_POINTS);
                }
                foundF = true;
                break;
            
            case 'L' :
                if( foundL )
                {
                    cleanUp(room);
                    cout << "Invalid room: duplicate L.\n";
                    exit(INVALID_ROOM_DUPLICATE_STARTING_ENDING_POINTS);
                }
                foundL = true;
                break;

            case '\n':
            case '#':
            case ' ':
                //A valid character.
                break;
                
            default:
                cleanUp(room);
                cout << "Invalid character encountered while parsing.\n";
                exit(PARSING_ERROR_INVALID_CHARACTER_ENCOUNTERED);
                break;
        }
        
        //Insert character into vector.
        if( c != EOF )
        room.at(y).push_back(c);
        
        //Increment current position.
        if( c == '\n' )
        {
            x=0;
            y++;
            vector<char> roomTemp;
            room.push_back(roomTemp);
        }
        else
        {
            x++;
        }
    }
    
    //Not all starting and ending positions are found, so invalid file.
    if( !( foundS && foundE && foundF && foundL ) )
    {
        cleanUp(room);
        cout << "Invalid Room: One or multiple starting/ending point(s) not found.\n";
        exit(INVALID_ROOM_MISSING_STARTING_ENDING_POINTS);
    }
    
    
    file.clear(fstream::goodbit);
    file.close();
    
    
    //Checks whether file has successfully closed. 
    if ( file.fail() )
    {
        cout << "Cannot close input file.\n";
        exit(INPUT_FILE_FAILED_TO_CLOSE);
    }
    
}

//Gets the edges and vertices.
void getEdgesAndVertices( vector< vector<char> >& room,
    vector< vector<int> >& roomKey, vector< Point* >& vertices,
    vector< vector< int > >& edges, int startEndPos[] )
{
    int counter = 0;
    for( int i = 0; i < room.size(); i++  )
    {   
        vector<int> roomKeyTemp;
        roomKey.push_back(roomKeyTemp);
        
        for( int j = 0; j < room.at(i).size(); j++  )
        {
            switch( room.at(i).at(j) )
            {
                case ' ' :
                case 'S' :
                case 'E' :
                case 'F' :
                case 'L' :
                {
                    Point* pPtr = new Point(i,j);
                    vertices.push_back(pPtr);
                    roomKey.at(i).push_back(counter);                    
                    
                    vector<int> edgesTemp;
                    edges.push_back(edgesTemp);
                    
                    //Checks whether the current cell is at a topmost position.
                    if( i != 0 )
                    {                                                
                        //Checks the cell above the current cell.
                        if( room.at(i-1).size() > j )
                        {
                            switch( room.at(i-1).at(j) )
                            {
                                case ' ' :
                                case 'S' :
                                case 'E' :
                                case 'F' :
                                case 'L' :
                                    edges.at(counter).push_back(roomKey.at(i-1).at(j));
                                    edges.at(roomKey.at(i-1).at(j)).push_back(counter);
                            }
                        }
                    }
                    
                    //Checks whether the current cell is at a leftmost position.
                    if( j != 0 )
                    {
                        //Checks the cell to the left of the current cell.
                        switch( room.at(i).at(j-1) )
                        {
                            case ' ' :
                            case 'S' :
                            case 'E' :
                            case 'F' :
                            case 'L' :
                                edges.at(counter).push_back(roomKey.at(i).at(j-1));
                                edges.at(roomKey.at(i).at(j-1)).push_back(counter);
                        }
                    }

                    counter++;
                    break;
                }
                
                case '\n': // Do nothing.
                    break;
                    
                default:
                    roomKey.at(i).push_back(-1);
                    break;
            }
            
            //Record special characters.
            switch( room.at(i).at(j) )
            {
                case 'S' :
                    startEndPos[S] = roomKey.at(i).at(j);
                    break;
                case 'E' :
                    startEndPos[E] = roomKey.at(i).at(j);
                    break;
                case 'F' :
                    startEndPos[F] = roomKey.at(i).at(j);
                    break;
                case 'L' :
                    startEndPos[L] = roomKey.at(i).at(j);
                    break;
                default:
                    break;
            }
        }      
    }
}

//Clean up memory
void cleanUp(vector< vector<char> >& v)
{
    for( int i = 0; i < v.size(); i++ )
    {
        v.at(i).clear();
    }
    v.clear();
}

//Clean up memory
void cleanUp(vector< vector<int> >& v)
{
    for( int i = 0; i < v.size(); i++ )
    {
        v.at(i).clear();
    }
    v.clear();
}

//Clean up memory 
void cleanUp(vector< Point* > v)
{
    for( int i = 0; i < v.size(); i++ )
    {
        delete v.at(i);
    }
    v.clear();
}