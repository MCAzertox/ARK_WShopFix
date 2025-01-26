#include "Hooks.h"

#include "ProcessEventDispatcher.hpp"

namespace WShopBDFix::Hooks
{

	inline bool hooked = false;

	struct ParamsStruct
	{
		FString command;
	};

	bool Hook_VulnerableFunction(UObject* _this, UFunction* Function, void* Parms)
	{
		ParamsStruct* params = static_cast<ParamsStruct*>(Parms);
		if (!params->command.ToLower().StartsWith("wshop") || params->command.Contains("||"))
		{
			Log::GetLog()->warn("Cheater exploited WShop vulnerability, command='{}'", params->command.ToString());
			return false;
		}
		return true;
	}

	inline int iiFunc;

	DECLARE_HOOK(AGameMode_HasMatchStarted, bool, AGameMode*);
	bool Hook_AGameMode_HasMatchStarted(AGameMode* _this)
	{
		auto result = AGameMode_HasMatchStarted_original(_this);
		iiFunc = Utils::GetInternalIndex(
			FString("Blueprint'/Game/Mods/WShop/Buff/Buff_Base_WShop.Buff_Base_WShop'"),
			FString("ExectuteConsoleCmdOnServer"));

		auto& peFuncMapper = PEFunctionMapper::Instance();
		peFuncMapper.RegisterFunction(iiFunc, &Hook_VulnerableFunction);

		return result;
	}

	
	void Init()
	{
		if (ArkApi::GetApiUtils().GetStatus() != ArkApi::ServerStatus::Ready)
		{
			ArkApi::GetHooks().SetHook("AGameMode.HasMatchStarted", &Hook_AGameMode_HasMatchStarted, &AGameMode_HasMatchStarted_original);
			hooked = true;
			return;
		}
		else
		{
			iiFunc = Utils::GetInternalIndex(
				FString("Blueprint'/Game/Mods/WShop/Buff/Buff_Base_WShop.Buff_Base_WShop'"),
				FString("ExectuteConsoleCmdOnServer"));

			auto& peFuncMapper = PEFunctionMapper::Instance();
			peFuncMapper.RegisterFunction(iiFunc, &Hook_VulnerableFunction);
		}
	}

	void Unload()
	{
		auto& peFuncMapper = PEFunctionMapper::Instance();
		peFuncMapper.UnregisterFunction(iiFunc);

		if (hooked)
		{
			ArkApi::GetHooks().DisableHook("AGameMode.HasMatchStarted", &Hook_AGameMode_HasMatchStarted);
		}

	}
}
