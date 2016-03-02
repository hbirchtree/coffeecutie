%module CoffeePython

%include "../coffee_common.i"

%{

#include "coffeeswig.h"

%}

%typemap(in) float[4](float temp[4]){
    int i;
    if(PyTuple_Check($input))
    {
        if(!PyArg_ParseTuple($input,"ffff",temp,temp+1,temp+2,temp+3))
        {
            PyErr_SetString(PyExc_TypeError,"tuple must have 4 elements");
            return NULL;
        }
        $1 = &temp[0];
    }else{
        PyErr_SetString(PyExc_TypeError,"expected a tuple");
        return NULL;
    }
}

%include "coffeeswig.h"