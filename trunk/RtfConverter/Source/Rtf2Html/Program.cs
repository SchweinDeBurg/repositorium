// -- FILE ------------------------------------------------------------------
// name       : Program.cs
// project    : RTF Framelet
// created    : Jani Giannoudis - 2008.05.30
// language   : c#
// environment: .NET 2.0
// copyright  : (c) 2004-2010 by Itenso GmbH, Switzerland
// --------------------------------------------------------------------------
using System;
using System.Runtime.InteropServices;
using System.IO;
using System.Diagnostics;
using Itenso.Sys.Application;
using Itenso.Rtf;
using Itenso.Rtf.Support;
using Itenso.Rtf.Parser;
using Itenso.Rtf.Interpreter;
using Itenso.Rtf.Converter.Image;
using Itenso.Rtf.Converter.Html;

namespace Itenso.Solutions.Community.Rtf2Html
{

	// ------------------------------------------------------------------------
	enum ProgramExitCode
	{
		Successfully = 0,
		InvalidSettings = -1,
		ParseRtf = -2,
		DestinationDirectory = -3,
		InterpretRtf = -4,
		ConvertHtml = -5,
		SaveHtml = -6,
	} // enum ProgramExitCode

	// ------------------------------------------------------------------------
	public class Program
	{
		[DllImport("user32.dll", CharSet = CharSet.Auto)]
		static extern int MessageBox(IntPtr hWnd, String text, String caption, int options);

		// ----------------------------------------------------------------------
		public Program(string rtf, string html, string arg1, string arg2, string arg3,
			string arg4, string arg5, string arg6, string arg7, string arg8, string arg9)
		{
			ExitCode = ProgramExitCode.Successfully;
			string[] args = { rtf, html, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 };
			settings = new ProgramSettings(args);
		} // Program

		// ----------------------------------------------------------------------
		private static ProgramExitCode ExitCode
		{
			get { return (ProgramExitCode)Environment.ExitCode; }
			set { Environment.ExitCode = (int)value; }
		} // ExitCode

		// ----------------------------------------------------------------------
		public bool Execute()
		{
			// program settings
			if (ValidateProgramSettings() == false)
			{
				return false;
			}

			// parse rtf
			IRtfGroup rtfStructure = ParseRtf();
			if (ExitCode != ProgramExitCode.Successfully)
			{
				return false;
			}

			// destination directory
			EnsureDestinationDirectory();
			if (ExitCode != ProgramExitCode.Successfully)
			{
				return false;
			}

			// image handling
			RtfVisualImageAdapter imageAdapter = new RtfVisualImageAdapter(
				settings.ImageFileNamePattern,
				settings.ImageFormat);

			// interpret rtf
			IRtfDocument rtfDocument = InterpretRtf(rtfStructure, imageAdapter);
			if (ExitCode != ProgramExitCode.Successfully)
			{
				return false;
			}

			// convert to hmtl
			string html = ConvertHmtl(rtfDocument, imageAdapter);
			if (ExitCode != ProgramExitCode.Successfully)
			{
				return false;
			}

			// save html
			string fileName = SaveHmtl(html);
			if (ExitCode != ProgramExitCode.Successfully)
			{
				return false;
			}

			// open html file
			OpenHtmlFile(fileName);
			if (ExitCode != ProgramExitCode.Successfully)
			{
				return false;
			}

			// display html text
			DisplayHtmlText(html);
			if (ExitCode != ProgramExitCode.Successfully)
			{
				return false;
			}

			return true;
		} // Execute

		// ----------------------------------------------------------------------
		private bool ValidateProgramSettings()
		{
			if (!settings.IsValid)
			{
				ExitCode = ProgramExitCode.InvalidSettings;
				return false;
			}

			return true;
		} // ValidateProgramSettings

		// ----------------------------------------------------------------------
		private IRtfGroup ParseRtf()
		{
			IRtfGroup rtfStructure;
			RtfParserListenerFileLogger parserLogger = null;
			try
			{
				// logger
				if (settings.LogParser)
				{
					string logFileName = settings.BuildDestinationFileName(
						settings.LogDirectory,
						RtfParserListenerFileLogger.DefaultLogFileExtension);
					parserLogger = new RtfParserListenerFileLogger(logFileName);
				}

				// rtf parser
				// open readonly - in case of dominant locks...
				using (FileStream stream = File.Open(settings.SourceFile, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
				{
					// parse the rtf structure
					RtfParserListenerStructureBuilder structureBuilder = new RtfParserListenerStructureBuilder();
					RtfParser parser = new RtfParser(structureBuilder);
					parser.IgnoreContentAfterRootGroup = true; // support WordPad documents
					if (parserLogger != null)
					{
						parser.AddParserListener(parserLogger);
					}
					parser.Parse(new RtfSource(stream));
					rtfStructure = structureBuilder.StructureRoot;
				}
			}
			catch (Exception e)
			{
				if (parserLogger != null)
				{
					parserLogger.Dispose();
				}

				MessageBox(IntPtr.Zero, "Error while parsing RTF:\n" + e.Message, "Itenso.Rtf2Html", 0x010000);
				ExitCode = ProgramExitCode.ParseRtf;
				return null;
			}

			return rtfStructure;
		} // ParseRtf

		// ----------------------------------------------------------------------
		private void EnsureDestinationDirectory()
		{
			if (!settings.HasDestinationOutput)
			{
				return;
			}

			try
			{
				if (!Directory.Exists(settings.DestinationDirectory))
				{
					Directory.CreateDirectory(settings.DestinationDirectory);
				}
			}
			catch (Exception e)
			{
				MessageBox(IntPtr.Zero, "Error while creating destination directory:\n" + e.Message, "Itenso.Rtf2Html", 0x010000);
				ExitCode = ProgramExitCode.DestinationDirectory;
				return;
			}
		} // EnsureDestinationDirectory

		// ----------------------------------------------------------------------
		private IRtfDocument InterpretRtf(IRtfGroup rtfStructure, IRtfVisualImageAdapter imageAdapter)
		{
			IRtfDocument rtfDocument;
			RtfInterpreterListenerFileLogger interpreterLogger = null;
			try
			{
				// logger
				if (settings.LogInterpreter)
				{
					string logFileName = settings.BuildDestinationFileName(
						settings.LogDirectory,
						RtfInterpreterListenerFileLogger.DefaultLogFileExtension);
					interpreterLogger = new RtfInterpreterListenerFileLogger(logFileName);
				}

				// image converter
				RtfImageConverter imageConverter = null;
				if (settings.SaveImage)
				{
					RtfImageConvertSettings imageConvertSettings = new RtfImageConvertSettings(imageAdapter);
					imageConvertSettings.ImagesPath = settings.DestinationDirectory;
					imageConvertSettings.BackgroundColor = settings.ImageBackgroundColor;
					if (settings.ExtendedImageScale)
					{
						imageConvertSettings.ScaleExtension = 0.5f;
					}
					imageConverter = new RtfImageConverter(imageConvertSettings);
				}

				// rtf parser
				// interpret the rtf structure using the extractors
				rtfDocument = RtfInterpreterTool.BuildDoc(rtfStructure, interpreterLogger, imageConverter);

			}
			catch (Exception e)
			{
				if (interpreterLogger != null)
				{
					interpreterLogger.Dispose();
				}

				MessageBox(IntPtr.Zero, "Error while interpreting RTF:\n" + e.Message, "Itenso.Rtf2Html", 0x010000);
				ExitCode = ProgramExitCode.InterpretRtf;
				return null;
			}

			return rtfDocument;
		} // InterpretRtf

		// ----------------------------------------------------------------------
		private string ConvertHmtl(IRtfDocument rtfDocument, IRtfVisualImageAdapter imageAdapter)
		{
			string html;

			try
			{
				RtfHtmlConvertSettings htmlConvertSettings = new RtfHtmlConvertSettings(imageAdapter);
				if (settings.CharacterSet != null)
				{
					htmlConvertSettings.CharacterSet = settings.CharacterSet;
				}
				htmlConvertSettings.Title = settings.SourceFileNameWithoutExtension;
				htmlConvertSettings.ImagesPath = settings.ImagesPath;
				htmlConvertSettings.IsShowHiddenText = settings.ShowHiddenText;
				htmlConvertSettings.UseNonBreakingSpaces = settings.UseNonBreakingSpaces;
				if (settings.ConvertScope != RtfHtmlConvertScope.None)
				{
					htmlConvertSettings.ConvertScope = settings.ConvertScope;
				}
				if (!string.IsNullOrEmpty(settings.StyleSheets))
				{
					string[] styleSheets = settings.StyleSheets.Split(',');
					htmlConvertSettings.StyleSheetLinks.AddRange(styleSheets);
				}
				htmlConvertSettings.ConvertVisualHyperlinks = settings.ConvertVisualHyperlinks;
				if (!string.IsNullOrEmpty(settings.VisualHyperlinkPattern))
				{
					htmlConvertSettings.VisualHyperlinkPattern = settings.VisualHyperlinkPattern;
				}
				htmlConvertSettings.SpecialCharsRepresentation = settings.SpecialCharsRepresentation;

				RtfHtmlConverter htmlConverter = new RtfHtmlConverter(rtfDocument, htmlConvertSettings);
				html = htmlConverter.Convert();
			}
			catch (Exception e)
			{
				MessageBox(IntPtr.Zero, "Error while converting to HTML:\n" + e.Message, "Itenso.Rtf2Html", 0x010000);
				ExitCode = ProgramExitCode.ConvertHtml;
				return null;
			}

			return html;
		} // ConvertHmtl

		// ----------------------------------------------------------------------
		private string SaveHmtl(string text)
		{
			if (!settings.SaveHtml)
			{
				return null;
			}

			string fileName = settings.BuildDestinationFileName(null, RtfHtmlConverter.DefaultHtmlFileExtension);
			try
			{
				using (TextWriter writer = new StreamWriter(fileName, false, settings.Encoding))
				{
					writer.Write(text);
				}
			}
			catch (Exception e)
			{
				MessageBox(IntPtr.Zero, "Error while saving HTML:\n" + e.Message, "Itenso.Rtf2Html", 0x010000);
				ExitCode = ProgramExitCode.SaveHtml;
				return null;
			}

			return fileName;
		} // SaveHmtl

		// ----------------------------------------------------------------------
		private void OpenHtmlFile(string fileName)
		{
			if (!settings.SaveHtml || !settings.OpenHtml)
			{
				return;
			}
			Process.Start(fileName);
		} // OpenHtmlFile

		// ----------------------------------------------------------------------
		private void DisplayHtmlText(string htmlText)
		{
		} // DisplayHtmlText

		// ----------------------------------------------------------------------
		private static void ShowHelp()
		{
		} // ShowHelp

		// ----------------------------------------------------------------------
		// members
		private readonly ProgramSettings settings;

	} // class Program

} // namespace Itenso.Solutions.Community.Rtf2Html
// -- EOF -------------------------------------------------------------------
