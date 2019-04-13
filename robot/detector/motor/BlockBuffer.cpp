#include "BlockBuffer.h"
#define DEBUGGER

int BLOCKBUFFER::nextIndex(int index)
{
  //Serial.println("asd");
  if(index <= 0)
      return maxBufferSize-1;  //make the index in the range of buffer
  else
      return index-1;
}

//delete a tail-----------------------------------------
void BLOCKBUFFER::deleteTail()
{
    blockArray[tail].mode = 0;
    tail = nextIndex(tail);
}
		
//add a tail-----------------------------------------
void BLOCKBUFFER::addHead(BLOCK* input)
{
    head = nextIndex(head);
    blockArray[head].dx = (*input).dx;
    blockArray[head].dy = (*input).dy;
    blockArray[head].tx = (*input).tx;
    blockArray[head].ty = (*input).ty;
    blockArray[head].mode = (*input).mode;/*
    Serial.print(head);
    Serial.print(",");
    Serial.print(tail);
    Serial.print(",");
    Serial.println(getCapcity());*/
    //Serial.print("add head recheck= ");
    //printBlockInfo(&(blockArray[head]));
}

//return capcity-----------------------------------------
int BLOCKBUFFER::getCapcity()
{
    if(tail - head >= 0)
       return tail - head;
    else 
       return tail + maxBufferSize - head;
}

//set head to tail, and set the mode to 0
void BLOCKBUFFER::setNextInsertBlock()
{
  int j = tail;   
  for(int u = 0;u< nextInsertDist;u++)
    j = nextIndex(j);
  for(int i = 0;i <maxBufferSize- nextInsertDist-1;i++){
    j = nextIndex(j);
    blockArray[j].mode = 0;
  }
	head = tail;
}

BLOCKBUFFER::BLOCKBUFFER()
{
}

void BLOCKBUFFER::init()
{
  cmd.CmdInit();
  route.workFlag = false;
  machinePoint.x = 0;
  machinePoint.y = 0; 
  head = tail = 0;
  busy = false;
}

void BLOCKBUFFER::setPosition(POINT newPoint)
{
  if(newPoint.x != -1)
    machinePoint.x = newPoint.x;
  if(newPoint.y != -1)
    machinePoint.y = newPoint.y; 
}

//get tail-----------------------------------------
BLOCK* BLOCKBUFFER::pop()
{
    if(getCapcity()<=0){                  //no block available
        tempBlock->mode = 99;              //send a block which is mode 0
        return tempBlock;
    }
    tempBlock = &blockArray[tail];
    return tempBlock;                     //don't delete this block, so addhead won't use this block, you have to delete the block after you use it manually(ISR)
}

//push head-----------------------------------------
bool BLOCKBUFFER::push(BLOCK* pushBlock)
{
    if(!notFull())
	      return false;
    addHead(pushBlock);
    return true;
}

bool BLOCKBUFFER::notFull()
{
    if(getCapcity()<40)//maxBufferSize-1
    	return true;
    else
    	return false;
}

void BLOCKBUFFER::refresh()
{
	if(!busy){								//make sure the process won't be interrupt by itself
		if(cmd.available()){    //deal with new command
      busy = true;
		  COMMAND newCmd = cmd.read();	//get new cmd
     /*
      Serial.print("new cmd= ");
      Serial.print(newCmd.mode);
      Serial.print(",");
      Serial.print(newCmd.d1);
      Serial.print(",");
      Serial.println(newCmd.d2);*/
      
			if(newCmd.mode == CmdMode_MOVE){                  //only add new cmd when mode == move
		     route.workFlag = true;								          //mark that it's a route working
         route.goalP.x = newCmd.d1;
         route.goalP.y = newCmd.d2;
         route.vec.x = newCmd.d1 - machinePoint.x;
         route.vec.y = newCmd.d2 - machinePoint.y;
         if(abs(route.vec.x)-abs(route.vec.y)>0)
            route.longerSide = true;
         else
          	route.longerSide = false;

         //auto accelerate
         route.mode = ((route.longerSide?abs(route.vec.x):abs(route.vec.y)) > 2*accelerateStepRange) ? true : false;
         if(route.mode){                //longer
           route.stage = 1;
           route.nowSpeed = minSpeed;
           route.nowSpeedDelta = speedDelta;
         }
         else{                          //shorter
           route.nowSpeed = normalSpeed;
           route.nowSpeedDelta = 0;
         }
         
         setNextInsertBlock();                          //set next index to tail
			}
			else{							//deal with other command
				switch(newCmd.mode){
					case CmdMode_HOME:
						newBlock.mode = CmdMode_HOME;
						push(&newBlock);
						break;
					case CmdMode_DELAY:
						newBlock.mode = CmdMode_DELAY;
						newBlock.dx = newCmd.d1;
						push(&newBlock);
						break;
          default:
            Serial.println("warning");
            break;
				}
			}
		}

    int deltaDistance = route.nowSpeed;
		if(notFull()&&route.workFlag){   //add blocks to buffer
      int ODx,ODy,OTx,OTy;    //output dx
			if(route.longerSide){		//longer side is x axis 
      	if(abs(route.vec.x)>deltaDistance){
            if(route.vec.x>0)
           		ODx = deltaDistance;
            else
           		ODx = -1*deltaDistance;
            ODy = route.vec.y * ODx / route.vec.x;
        }
        else{            //end not precisely
            ODx = route.vec.x;
            ODy = route.vec.y;
            route.workFlag = false;
        }
      }
      else{ 
        if(abs(route.vec.y)>deltaDistance){
          if(route.vec.y>0)
            ODy = deltaDistance;
          else
            ODy = -1*deltaDistance;
          ODx = route.vec.x * ODy / route.vec.y;
        }
        else{
          ODx = route.vec.x;
          ODy = route.vec.y;
          route.workFlag = false;
        }
      }
      route.vec.x -= ODx;       //updata vec
      route.vec.y -= ODy;
      if(ODx==0 && ODy==0)
        return;
      else if(ODx == 0){
      	OTx = 0;
      	OTy = VT;
      }else if(ODy == 0){
      	OTy = 0;
      	OTx = VT;
    	}else{
        route.unit = VT / (route.longerSide ? ODy : ODx);
        OTx = abs(route.unit * ODy);
        OTy = abs(route.unit * ODx);
    	}
     	newBlock.dx = ODx;
     	newBlock.dy = ODy;
     	newBlock.tx = OTx;
    	newBlock.ty = OTy;
    	newBlock.mode = CmdMode_MOVE;
    	//Serial.print("data before push= ");
      //printBlockInfo(&newBlock);
    	push(&newBlock);					//push into block buffer

      //auto accelerate
      route.nowSpeed += route.nowSpeedDelta;
      if(route.mode){
        switch(route.stage){
          case 1:
            if(route.nowSpeed > maxSpeed){
              route.nowSpeed = maxSpeed;
              route.nowSpeedDelta = 0;
              route.stage = 2;
            }
            break;
          case 2:
            if((route.longerSide?abs(route.vec.x):abs(route.vec.y)) < accelerateStepRange){
              route.nowSpeedDelta = -speedDelta;
              route.stage = 3;
            }
            break;
          case 3:
            if(route.nowSpeed < minSpeed){
              route.nowSpeed = minSpeed;
              route.nowSpeedDelta = 0;
              route.stage = 4;
            }
            break;
        }   
      }     //auto accelerate
		}       //fill block
		busy = false;						//set the state back to free
	}
}
void BLOCKBUFFER::clear(){
  for(int i = 0;i<maxBufferSize;i++){
    if(i==tail)
    continue;
    blockArray[i].mode = 0;
  }
  head = tail = 0;
}
void BLOCKBUFFER::printPointInfo(const POINT* const point){
  Serial.print("Point =");
  Serial.print(point->x);
  Serial.print(", ");
  Serial.print(point->y);
}
    
void BLOCKBUFFER::printBlockInfo(const BLOCK* const block){
  Serial.print("mode=");
  Serial.print(block->mode);
  Serial.print(", dx=");
  Serial.print(block->dx);
  Serial.print(", dy=");
  Serial.print(block->dy);
  Serial.print(", tx=");
  Serial.print(block->tx);
  Serial.print(", ty=");
  Serial.println(block->ty);
}
void BLOCKBUFFER::printInfo()
{
    Serial.println("BLOCK=====================");
    Serial.print("head =");
    Serial.print(head);
    Serial.print(",tail =");
    Serial.print(tail);
    Serial.print(",capcity = ");
    Serial.println(getCapcity());
    for(int i = 0;i<maxBufferSize;i++)
      printBlockInfo(&blockArray[i]);
}
