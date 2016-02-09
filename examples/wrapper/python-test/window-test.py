#! env python2

import CoffeePython as CL

CL.CoffeeInit()
win = CL.CreateWindow(800,600)

CL.ShowWindow(win)

while(not CL.ShouldClose(win)):
	CL.Process(win)

CL.DestroyWindow(win)
CL.CoffeeTerminate()
