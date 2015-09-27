using System;

namespace CoffeeSharp
{
	public class CoffeeSharpTest
	{
		public CoffeeSharpTest ()
		{

		}
		public static void Main(String[] args)
		{
			CGLContextVersion ctxtVer = new CGLContextVersion (3,3);

			CDWindowProperties props = new CDWindowProperties ();
			props.flags = (ushort)(CDWindowProperties.WindowState.Decorated | CDWindowProperties.WindowState.Windowed | CDWindowProperties.WindowState.Resizable);
			props.monitor = 0;
			props.contextProperties.flags = (ushort)(CGLContextProperties.ContextProperties.GLAutoResize | CGLContextProperties.ContextProperties.GLDebug | CGLContextProperties.ContextProperties.GLCoreProfile);
			props.contextProperties.version = ctxtVer;
			props.title = "Hello you!";

			CDRenderer renderer = new CDRenderer ();
			renderer.run (props);

		}
	}
}
