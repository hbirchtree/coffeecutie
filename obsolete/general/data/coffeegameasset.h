#ifndef COFFEEGAMEASSET
#define COFFEEGAMEASSET

#include "general/common.h"
class CoffeeGameAsset{
    //We use this to keep track of an object's usage. When it is no longer used, we may delete it or do other things.
public:
    void addConsumer(){
        consumers++;
    }
    void removeConsumer(){
        consumers--;
    }
    bool isConsumed(){
        if(consumers>0)
            return true;
        return false;
    }

    void addAllocation(){
        allocations++;
    }
    void removeAllocation(){
        allocations--;
    }
    bool isAllocated(){
        if(allocations>0)
            return true;
        return false;
    }

    void addPreAllocation(){
        preallocs++;
    }
    void removePreAllocation(){
        preallocs--;
    }
    bool isPreAllocated(){
        if(preallocs>0)
            return true;
        return false;
    }

    virtual void load() = 0;
    virtual void unload() = 0;

private:
    int consumers = 0;
    int allocations = 0;
    int preallocs = 0;
};

#endif // COFFEEGAMEASSET

