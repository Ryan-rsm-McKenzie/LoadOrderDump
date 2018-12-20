#include "skse64/GameData.h"  // ModInfo
#include "skse64/PluginAPI.h"  // PluginHandle, SKSEInterface, PluginInfo
#include "skse64_common/skse_version.h"  // RUNTIME_VERSION

#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "version.h"  // LOADORDERDUMP_VERSION_VERSTRING, LOADORDERDUMP_VERSION_MAJOR

#include "RE/TESDataHandler.h"  // TESDataHandler

#include <fstream>  // ofstream
#include <sstream>  // ostringstream
#include <string>  // string


static PluginHandle				g_pluginHandle = kPluginHandle_Invalid;
static SKSEMessagingInterface*	g_messaging = 0;


std::string Format(const char* a_fmt, ...)
{
	static char formatBuf[8192] = { 0 };
	va_list args;
	va_start(args, a_fmt);
	vsprintf_s(formatBuf, sizeof(formatBuf), a_fmt, args);
	std::ostringstream oss;
	oss << formatBuf;
	return oss.str().c_str();
}


std::string OpenRelative(int a_folderID, std::string& a_elPath)
{
	char path[MAX_PATH];

	HRESULT err = SHGetFolderPath(NULL, a_folderID | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, path);
	if (!SUCCEEDED(err)) {
		_FATALERROR("SHGetFolderPath %08X failed (result = %08X lasterr = %08X)", a_folderID, err, GetLastError());
	}
	ASSERT_CODE(SUCCEEDED(err), err);

	strcat_s(path, sizeof(path), a_elPath.c_str());

	return path;
}


void MessageHandler(SKSEMessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSEMessagingInterface::kMessage_SaveGame:
	{
		const char* saveName = static_cast<const char*>(a_msg->data);
		RE::TESDataHandler* dataHandler = RE::TESDataHandler::GetSingleton();
		std::string str;
		str = "\\My Games\\Skyrim Special Edition\\Saves\\";
		str += saveName;
		str += ".loadorder";
		std::ofstream ostream(OpenRelative(CSIDL_MYDOCUMENTS, str).c_str());
		for (auto& mod : dataHandler->modList.modInfoList) {
			if (mod->modIndex == 0xFF) {
				continue;
			} else if (mod->modIndex == 0xFE) {
				str = Format("[%02X:%03X] %s\n", mod->modIndex, mod->lightIndex, mod->name);
				ostream << str;
			} else {
				str = Format("[%02X] %s\n", mod->modIndex, mod->name);
				ostream << str;
			}
		}
		ostream.close();
	}
	break;
	}
}


extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface* a_skse, PluginInfo* a_info)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\LoadOrderDump.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("LoadOrderDump v%s", LOADORDERDUMP_VERSION_VERSTRING);

		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = "LoadOrderDump";
		a_info->version = LOADORDERDUMP_VERSION_MAJOR;

		g_pluginHandle = a_skse->GetPluginHandle();

		if (a_skse->isEditor) {
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!\n");
			return false;
		}

		if (a_skse->runtimeVersion != RUNTIME_VERSION_1_5_62) {
			_FATALERROR("[FATAL ERROR] Unsupported runtime version %08X!\n", a_skse->runtimeVersion);
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSEInterface* a_skse)
	{
		g_messaging = (SKSEMessagingInterface*)a_skse->QueryInterface(kInterface_Messaging);
		if (g_messaging->RegisterListener(g_pluginHandle, "SKSE", MessageHandler)) {
			_MESSAGE("[MESSAGE] Messaging interface registration successful");
		} else {
			_FATALERROR("[FATAL ERROR] Messaging interface registration failed!\n");
			return false;
		}

		return true;
	}
};
