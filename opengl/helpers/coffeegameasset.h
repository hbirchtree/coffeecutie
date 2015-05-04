#ifndef COFFEEGAMEASSET
#define COFFEEGAMEASSET

#include "general/common.h"
class CoffeeGameAsset{
    //We use this to keep track of an object's usage. When it is no longer used, we may delete it or do other things.
public:
    int getActiveUses(){
        return activeUsers;
    }
    void addActiveUser(){
        activeUsers++;
    }
    void removeActiveUser(){
        activeUsers--;
        if(activeUsers<0)
            qDebug() << "Negative amount of active users!";
    }

    void addUser(){
        users++;
    }
    void removeUser(){
        users--;
        if(users<0)
            qDebug() << "Negative amount of users!";
    }
    int getUsers(){
        return users;
    }
private:
    int users = 0;
    int activeUsers = 0;
};

#endif // COFFEEGAMEASSET

