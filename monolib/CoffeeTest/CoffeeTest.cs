using System;
using CoffeeSharp.Coffee;
using CoffeeSharp.Coffee.CDisplay;
using CoffeeSharp.Coffee.CPrimitiveDataTypes;

/*
 * 
 * NOTE:
 * Just because we are using C#, we are not free of library dependencies!
 * Remember to put glbinding and Assimp in the library path! 
 * (Otherwise, you *will* feel the pain of "DllNotFoundException".)
 * 
 */

namespace CoffeeTest
{
	class CoffeeTester
	{
		static void Main(string[] args)
		{
			//Testing constructors
			CObject obj = new CObject ();
			CObject obj2 = new CObject (obj);

			Console.WriteLine (obj2.parent());
		}
	}
}