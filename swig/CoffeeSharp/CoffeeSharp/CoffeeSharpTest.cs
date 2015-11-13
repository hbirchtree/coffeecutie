using System;
using System.Threading;

namespace CoffeeSharp
{
	public class CoffeeSharpTest
	{
		public static CDRenderer renderer;
		public static CDWindowProperties props;

		public static void Main(String[] args)
		{
			Console.WriteLine ("Application directory: {0}\n" +
			                   "Current directory: {1}\n" +
			                   "Userdata directory: {2}",
			                   Coffee.coffee_get_application_dir(),
			                   Coffee.coffee_get_current_dir(),
			                   Coffee.coffee_get_userdata_dir("hbirchtree","Coffee of Time"));

			CResource res = new CResource ("ubw/models/ubw.fbx");
			Coffee.coffee_file_pull (res);
			CAssimpData meshes = CAssimpImporters.importResource (res,res.resource());

			CResource smp_file = new CResource (
				String.Format("{0}/Skrivebord/healing.ogg",
			              Coffee.coffee_get_env_variable("HOME")));
			Coffee.coffee_file_pull (smp_file);
			CAudioSample smp = new CAudioSample();
			Coffee.coffee_stb_audio_vorbis_load (smp,smp_file);

			CResource img_file = new CResource ("ubw/models/textures/gear.png");
			Coffee.coffee_file_pull (img_file);
			CStbImage img = new CStbImage ();
			Coffee.coffee_stb_image_load (img,img_file);

			Console.WriteLine ("Mesh data: {0}",meshes.numMeshes);

			CGLContextVersion ctxtVer = new CGLContextVersion (3,3);

			props = new CDWindowProperties ();
			props.flags = (ushort)(
				CDWindowProperties.WindowState.Windowed 
				| CDWindowProperties.WindowState.Resizable);
			props.monitor = 0;
			props.contextProperties.flags = (ushort)(
				CGLContextProperties.ContextProperties.GLAutoResize 
				| CGLContextProperties.ContextProperties.GLDebug 
				| CGLContextProperties.ContextProperties.GLCoreProfile);
			props.contextProperties.version = ctxtVer;
			props.title = "Hello you!";
			props.size = new CSize();

			renderer = new CDRenderer ();

			renderer.run (props);
		}
	}
}
