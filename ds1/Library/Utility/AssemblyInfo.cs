//
// The MIT License(MIT)
//
// Copyright(c) 2014 Demonsaw LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

namespace DemonSaw.Utility
{
	using System.Diagnostics;
	using System.Reflection;

	public sealed class AssemblyInfo
	{
		// Properties
		public Assembly Assembly { get; set; }

		public string Title { get { return GetTitle(); } }
		public string Description { get { return GetDescription(); } }
		public string Company { get { return GetProduct(); } }
		public string Copyright { get { return GetCopyright(); } }
		public string Trademark { get { return GetTrademark(); } }
		public string Version { get { return GetVersion(); } }		
		public string FileVersion { get { return GetFileVersion(); } }
		public string Guid { get { return GetGuid(); } }
		public string FileName { get { return GetFileName(); } }
		public string FilePath { get { return GetFilePath(); } }

		// Constructors
		public AssemblyInfo() { Assembly = Assembly.GetEntryAssembly(); }
		public AssemblyInfo(Assembly assembly) { Assembly = assembly; }
		
		// Utility
		public string GetTitle()
		{
			string result = string.Empty;

			if (Assembly != null)
			{
				object[] customAttributes = Assembly.GetCustomAttributes(typeof(AssemblyTitleAttribute), false);
				if ((customAttributes != null) && (customAttributes.Length > 0))
					result = ((AssemblyTitleAttribute) customAttributes[0]).Title;
			}

			return result;
		}

		public string GetDescription()
		{
			string result = string.Empty;

			if (Assembly != null)
			{
				object[] customAttributes = Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute), false);
				if ((customAttributes != null) && (customAttributes.Length > 0))
					result = ((AssemblyDescriptionAttribute) customAttributes[0]).Description;
			}

			return result;
		}

		public string GetCompany()
		{
			string result = string.Empty;

			if (Assembly != null)
			{
				object[] customAttributes = Assembly.GetCustomAttributes(typeof(AssemblyCompanyAttribute), false);
				if ((customAttributes != null) && (customAttributes.Length > 0))
					result = ((AssemblyCompanyAttribute) customAttributes[0]).Company;
			}

			return result;
		}

		public string GetProduct()
		{
			string result = string.Empty;

			if (Assembly != null)
			{
				object[] customAttributes = Assembly.GetCustomAttributes(typeof(AssemblyProductAttribute), false);
				if ((customAttributes != null) && (customAttributes.Length > 0))
					result = ((AssemblyProductAttribute) customAttributes[0]).Product;
			}
			return result;
		}

		public string GetCopyright()
		{
			string result = string.Empty;

			if (Assembly != null)
			{
				object[] customAttributes = Assembly.GetCustomAttributes(typeof(AssemblyCopyrightAttribute), false);
				if ((customAttributes != null) && (customAttributes.Length > 0))
					result = ((AssemblyCopyrightAttribute) customAttributes[0]).Copyright;
			}
			return result;
		}

		public string GetTrademark()
		{
			string result = string.Empty;

			if (Assembly != null)
			{
				object[] customAttributes = Assembly.GetCustomAttributes(typeof(AssemblyTrademarkAttribute), false);
				if ((customAttributes != null) && (customAttributes.Length > 0))
					result = ((AssemblyTrademarkAttribute) customAttributes[0]).Trademark;
			}
			return result;
		}

		public string GetVersion()
		{
			return Assembly.GetName().Version.ToString();
		}

		public string GetFileVersion()
		{
			FileVersionInfo fvi = FileVersionInfo.GetVersionInfo(Assembly.Location);
			return fvi.FileVersion;
		}

		public string GetGuid()
		{
			string result = string.Empty;

			if (Assembly != null)
			{
				object[] customAttributes = Assembly.GetCustomAttributes(typeof(System.Runtime.InteropServices.GuidAttribute), false);
				if ((customAttributes != null) && (customAttributes.Length > 0))
					result = ((System.Runtime.InteropServices.GuidAttribute) customAttributes[0]).Value;
			}
			return result;
		}

		public string GetFileName()
		{
			FileVersionInfo fvi = FileVersionInfo.GetVersionInfo(Assembly.Location);
			return fvi.OriginalFilename;
		}

		public string GetFilePath()
		{
			FileVersionInfo fvi = FileVersionInfo.GetVersionInfo(Assembly.Location);
			return fvi.FileName;
		}
	}
}
