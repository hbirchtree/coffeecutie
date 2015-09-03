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

			CSize t = new CSize (800,600);
			Console.WriteLine ("CSize: {0}x{1}",t.w, t.h);

			CDRenderer renderer = new CDRenderer(obj2);

			renderer.run(CDRendererBase.WindowState.Windowed,t,0);

			Console.WriteLine ("Renderer exited");
		}
	}
}