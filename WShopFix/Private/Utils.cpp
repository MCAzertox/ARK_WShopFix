#include <regex>
#include "Utils.h"
#include <iomanip>
#include <sstream>
#include <Windows.h>

namespace WShopBDFix::Utils
{
    FString GetBlueprint(UObjectBase* object)
    {
        if (object != nullptr && object->ClassField() != nullptr)
        {
            FString path_name;
            object->ClassField()->GetDefaultObject(true)->GetFullName(&path_name, nullptr);

            if (int find_index = 0; path_name.FindChar(' ', find_index))
            {
                path_name = "Blueprint'" + path_name.Mid(find_index + 1,
                    path_name.Len() - (find_index + (path_name.EndsWith(
                        "_C", ESearchCase::
                        CaseSensitive)
                        ? 3
                        : 1))) + "'";
                return path_name.Replace(L"Default__", L"", ESearchCase::CaseSensitive);
            }
        }

        return FString("");
    }

    FString FloatAsStringWithPrecision(double number, int precision)
    {
        return FString(fmt::format("{:.{}f}", number, precision));
    }

    // Function to convert seconds to a formatted string
    std::string formatTime(int totalSeconds) {
        int seconds = totalSeconds % 60;
        int totalMinutes = totalSeconds / 60;
        int minutes = totalMinutes % 60;
        int totalHours = totalMinutes / 60;
        int hours = totalHours % 24;
        int days = totalHours / 24;

        std::string result;
        if (days > 0) result += std::to_string(days) + "d";
        if (hours > 0 || !result.empty()) result += std::to_string(hours) + "h";
        if (minutes > 0 || !result.empty()) result += std::to_string(minutes) + "m";
        result += std::to_string(seconds) + "s";

        return result;
    }

    AShooterPlayerController* getPlayerForSteamId(uint64 steamId)
    {
        const auto& player_controllers = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();
        for (TWeakObjectPtr<APlayerController> player_controller : player_controllers)
        {
            auto* shooter_pc = static_cast<AShooterPlayerController*>(player_controller.Get());
            if (shooter_pc != nullptr && shooter_pc->GetUniqueNetIdAsUINT64() == steamId)
            {
                return shooter_pc;
            }
        }
        return nullptr;
    }

    void SplitString(const std::string& s, char delim, std::vector<std::string>& out)
	{
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim))
		{
			out.push_back(item);
		}
	}

    int GetInternalIndex(const FString& classBlueprint, const FString& functionName)
    {
        UClass* cls = UVictoryCore::BPLoadClass(classBlueprint);
        if (cls) {
            UFunction* func = cls->FindFunctionByName(FName(functionName.ToString().c_str(), EFindName::FNAME_Add), EIncludeSuperFlag::Type::IncludeSuper);
            if (func)
            {
                return func->InternalIndexField();
            }
            else
            {
                Log::GetLog()->error("Function not found: {} in class {}", functionName.ToString(), classBlueprint.ToString());
            }
        }
        else
        {
            Log::GetLog()->error("Class not found: {}", classBlueprint.ToString());
        }
        return -1;
    }
}

