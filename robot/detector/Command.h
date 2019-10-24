#ifndef COMMAND_H
#define COMMAND_H

#include <Arduino.h>
#include "QueueArray/QueueArray.h"

// Columns before para array. EX. mode, time_stamp
#define COL_BEFORE_PARA 2
// Size of para array
#define PARA_LEN 3

typedef struct{
    int mode;
    unsigned long time_stamp;
    int para[PARA_LEN];
}Command;

class CmdQueue{
    public:
        QueueArray<Command> cmd_queue;

        /* Constructor */
        CmdQueue();

        /* Feed raw server msg into cmd_queue. Note that the number
           of column of s must equal to the commad struct. */
        void feedCmd(String s);

        Command dequeue();

        /* Command excuter, this function should be called in loop() */
        void worker();
    private:
        /* Parse the string into command struct */
        bool cmdResolve(Command* cmd, String s);
};

#endif
