using System;
using System.Diagnostics;
using System.IO;

class FXCCompiler
{
	static void Main(string[] prms)
	{
		var fxFiles = new (string input, string output)[]
		{
			("car.fx", "IDI_CAR_FX"),
			("GlossyWindow.fx", "IDI_GLOSSYWINDOW_FX"),
			("skybox.fx", "IDI_SKYBOX_FX"),
			("tree.fx", "IDI_TREE_FX"),
			("world.fx", "IDI_WORLD_FX"),
			("worldnormalmap.fx", "IDI_WORLDNORMALMAP_FX"),
			("worldreflect.fx", "IDI_WORLDREFLECT_FX"),
			("filter.fx", "IDI_FILTER_FX"),
			("screenfilter.fx", "IDI_SCREENFILTER_FX"),
			("worldprelit.fx", "IDI_WORLDPRELIT_FX"),
			("particles.fx", "IDI_PARTICLES_FX"),
		};

		string fxcPath = "C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.26100.0\\x86\\fxc.exe";

		string opt = prms.Length > 0 && prms[0] == "fast" ? "/Od" : "/O3";

		bool errors = false;
		foreach (var (inputFile, outputFile) in fxFiles)
		{
			string inputLocation = "D:\\Programming\\NFSMW\\NFSMW_SunRise\\fx\\";
			string outputLocation = "D:\\Games\\Need for Speed Most Wanted SR\\shaders\\";
			string inputPath = inputLocation + inputFile;
			string outputPath = outputLocation + outputFile;

			if (!File.Exists(inputPath))
			{
				Console.WriteLine($"Input file not found: {inputFile}");
				continue;
			}

			string args = $"/T fx_2_0 /Fo \"{outputPath}\" {opt} /nologo \"{inputPath}\"";

			Console.WriteLine($"Compiling {inputFile} -> {outputFile}");

			var process = new Process();
			process.StartInfo.FileName = fxcPath;
			process.StartInfo.Arguments = args;
			process.StartInfo.RedirectStandardOutput = true;
			process.StartInfo.RedirectStandardError = true;
			process.StartInfo.UseShellExecute = false;
			process.StartInfo.CreateNoWindow = true;

			process.Start();

			string stdout = process.StandardOutput.ReadToEnd();
			string stderr = process.StandardError.ReadToEnd();

			process.WaitForExit();

			if (process.ExitCode == 0)
			{
				Console.WriteLine($"Success: {outputFile}");
			}
			else
			{
				Console.WriteLine($"Error compiling {inputFile}:\n{stderr}");
				errors = true;
			}
		}

		Console.WriteLine("Compilation finished.");

		if (errors)
		{
			Console.ReadLine();
		}
	}
}
