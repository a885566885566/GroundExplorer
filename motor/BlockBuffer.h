#ifndef _BLOCKBUFFER
#define _BLOCKBUFFER

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Essential.cpp"
#include "Command.cpp"

#define DEBUGGER

#define nextInsertDist 5
#define maxBufferSize 60
//#define deltaDistance 40            //command will be seperated into blocks by this steps

#define minSpeed 10
#define normalSpeed 30
#define maxSpeed 80
#define speedDelta 4
#define accelerateStepRange 1000  //480 //calculate by min max speed and delta
#define VT 1                       //average timer count


class BLOCKBUFFER
{
	private:
  	volatile BLOCK blockArray[maxBufferSize];
    BLOCK newBlock;                            //used to add head, call by address
		int head;
		int tail;
		volatile ROUTE route;
		volatile CMD cmd;
		void addHead(BLOCK* input);
		int getCapcity();
    int nextIndex(int index);
		void setNextInsertBlock();                  //set head to tail, it will set every block to mode 0, too.
	
	public:
    volatile bool busy = false;
    volatile POINT machinePoint;             //global used for machine point
    volatile BLOCK* tempBlock;                  //global used address for block 
		BLOCKBUFFER();
		void init();
    void deleteTail();                          //after you pop a block, you have to delete the node manually
		void setPosition(POINT newPoint);
		BLOCK* pop();	
		bool push(BLOCK* newBlock);
		bool notFull();                             //it has a safty limit
		void refresh();
		void printInfo();
    void printBlockInfo(const BLOCK* const block);
    void printPointInfo(const POINT* const point);
    void clear();
};

#endif
