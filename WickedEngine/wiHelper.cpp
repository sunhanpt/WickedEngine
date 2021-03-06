#include "wiHelper.h"
#include "wiRenderer.h"
#include "wiBackLog.h"
#include "wiWindowRegistration.h"

namespace wiHelper
{

	string toUpper(const std::string& s)
	{
		std::string result;
		std::locale loc;
		for (unsigned int i = 0; i < s.length(); ++i)
		{
			result += std::toupper(s.at(i), loc);
		}
		return result;
	}

	bool readByteData(const string& fileName, BYTE** data, size_t& dataSize){
		ifstream file(fileName, ios::binary | ios::ate);
		if (file.is_open()){

			dataSize = (size_t)file.tellg();
			file.seekg(0, file.beg);
			*data = new BYTE[dataSize];
			file.read((char*)*data, dataSize);
			file.close();

			return true;
		}
		stringstream ss("");
		ss << "File not found: " << fileName;
		messageBox(ss.str());
		return false;
	}

	void messageBox(const string& msg, const string& caption){
#ifndef WINSTORE_SUPPORT
		MessageBoxA(wiWindowRegistration::GetInstance()->GetRegisteredWindow(), msg.c_str(), caption.c_str(), 0);
#else
		wstring wmsg(msg.begin(), msg.end());
		wstring wcaption(caption.begin(), caption.end());
		Windows::UI::Popups::MessageDialog(ref new Platform::String(wmsg.c_str()), ref new Platform::String(wcaption.c_str())).ShowAsync();
#endif
	}

	void screenshot(const string& name)
	{
		CreateDirectoryA("screenshots", 0);
		stringstream ss("");
		if (name.length() <= 0)
			ss << "screenshots/sc_" << getCurrentDateTimeAsString() << ".png";
		else
			ss << name;
		if (SUCCEEDED(wiRenderer::GetDevice()->SaveTexturePNG(ss.str(), &wiRenderer::GetDevice()->GetBackBuffer())))
		{
			ss << " Saved successfully!";
			wiBackLog::post(ss.str().c_str());
		}
		else
		{
			wiBackLog::post("Screenshot failed");
		}
	}

	string getCurrentDateTimeAsString()
	{
		time_t t = std::time(nullptr);
		struct tm time_info;
		localtime_s(&time_info, &t);
		stringstream ss("");
		ss << std::put_time(&time_info, "%d-%m-%Y %H-%M-%S");
		return ss.str();
	}
}
