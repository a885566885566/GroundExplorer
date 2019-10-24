#include "Command.h"

CmdQueue::CmdQueue(){
}

void CmdQueue::feedCmd(String s){
    Command cmd;
    if( cmdResolve(&cmd, s) == true ){
        cmd_queue.enqueue(cmd);
    }
    else{
        // Alart
    }
}
bool CmdQueue::cmdResolve(Command* cmd, String s){
    // Get the first char index after the first '?' 
    int start_index = s.indexOf('?') + 2;   
    int end_index = 0;
    // Ignore all msg before the first '?'
    String sub = s.substring(start_index);
    int counter = 0;    // Counter for column of data
    while(start_index != -1){
        end_index = sub.indexOf('?');
        String msg;
        // Get the true msg between '?=' and '?'
        // start -> inclusive
        // end   -> exclusive
        if(end_index == -1) msg = sub;  // Last one
        else                msg = sub.substring(start_index, end_index);
        
        if(counter == 0)      cmd->mode = msg.toInt();
        else if (counter==1)  cmd->time_stamp = msg.toInt();
        else if (counter < PARA_LEN + COL_BEFORE_PARA)
            cmd->para[counter - COL_BEFORE_PARA] = msg.toInt();
        // Para length doesn't match, warn it
        else 
            return false;
        // Ignore '?='
        start_index = end_index + 2;
        counter++;
    }
    return true;
}
Command CmdQueue::dequeue(){
    if(!cmd_queue.isEmpty()){
        return cmd_queue.dequeue();
    }
    Command cmd;
    cmd.mode = NULL;
    return cmd;
}
void CmdQueue::worker(){
    //if()
}
