#include "framework.h"

#include <cstring>

using namespace System;
using namespace Runtime::InteropServices;

void convertString(String^ string, std::string& stdstr)
{
	//msclr::interop::marshal_context context;

	////Actual format is UTF16, so represent as wstring
	//std::wstring utf16NativeString = context.marshal_as<std::wstring>(string);

	////C++11 format converter
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;

	////convert to UTF8 and std::string
	//std::string utf8NativeString = convert.to_bytes(utf16NativeString);

	//return utf8NativeString;

	
	const char* chars =
		(const char*)(Marshal::StringToHGlobalAnsi(string)).ToPointer();
	//stdstr = chars;

	for (int i = 0; i < string->Length; i++)
	{
		stdstr += chars[i];
	}
	
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}
