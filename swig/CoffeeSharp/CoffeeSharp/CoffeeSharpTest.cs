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
			Console.WriteLine ("Application directory: {0}\n" +
			                   "Current directory: {1}\n" +
			                   "Userdata directory: {2}",
			                   Coffee.coffee_get_application_dir(),
			                   Coffee.coffee_get_current_dir(),
			                   Coffee.coffee_get_userdata_dir("hbirchtree","Coffee of Time"));

			CResource res = new CResource ("ubw/models/ubw.fbx");
			res.read_data ();
			CAssimpData meshes = CAssimpImporters.importResource (res,res.resource());

			Console.WriteLine ("Mesh data: {0}",meshes.numMeshes);

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
