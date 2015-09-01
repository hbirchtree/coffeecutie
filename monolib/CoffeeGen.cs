using System;
using CppSharp;
using CppSharp.Generators;

namespace CoffeeGeneration
{
	class Program
	{
		static void Main (string[] args)
		{
			if (args.Length != 3) {
				Console.WriteLine ("usage: cmd <header-path> <lib-path> <out-path>\n");
				return;
			}

			var p1 = System.IO.Path.GetFullPath (args [0]);
			var p2 = System.IO.Path.GetFullPath (args [1]);
			var p3 = System.IO.Path.GetFullPath (args [2]);

			Console.WriteLine ("HeaderPath: {0}", p1);
			Console.WriteLine ("LibPath: {0}", p2);
			Console.WriteLine ("OutPath: {0}", p3);

			ConsoleDriver.Run (new CoffeeLib (p1, p2, p3));
			Console.WriteLine ("CoffeeSharp generated!");
		}
	}
	class CoffeeLib : ILibrary
	{
		string m_headers,m_libs, m_outdir;

		public CoffeeLib(string headerdir, string libdir, string outdir)
		{
			m_headers = headerdir;
			m_libs = libdir;
			m_outdir = outdir;
		}
		public void Setup(Driver driver)
		{
			var options = driver.Options;

			options.GeneratorKind = GeneratorKind.CSharp;
			options.LibraryName = "CoffeeSharp";
			options.OutputDir = m_outdir;
			options.addLibraryDirs (m_libs);
			options.Libraries.Add ("libCoffeeSharp.a"); //TODO: Support Windows and OS X here

			options.addIncludeDirs (m_headers);
			options.addIncludeDirs ("../../../../cppsharp/deps/llvm/build/lib/clang/3.8.0/include"); //TODO: Find a solution to this madness
			options.Headers.Add ("coffeesharp.h");

			options.NoBuiltinIncludes = false;
			options.Verbose = true;
			options.CheckSymbols = true;
			options.Quiet = false;
		}

		public void SetupPasses(Driver driver)
		{

		}

		public void Preprocess(Driver driver,CppSharp.AST.ASTContext ctxt)
		{

		}

		public void Postprocess(Driver driver,CppSharp.AST.ASTContext ctxt)
		{

		}
	}
}

