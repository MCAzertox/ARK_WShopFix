#pragma once

#include "API/ARK/Ark.h"
#include <unordered_map>
#include <vector>
#include <functional>
#include <stdexcept>

DECLARE_HOOK(UObject_ProcessEvent_Hooker, void, UObject*, UFunction*, void*);

class PEFunctionMapper {
public:
    using FunctionType = std::function<bool(UObject*, UFunction*, void*)>;

    static PEFunctionMapper& Instance() {
        static PEFunctionMapper instance;
        return instance;
    }

    void RegisterFunction(int id, FunctionType func) {
        if (id <= 0) {
            throw std::runtime_error("Tried to register a function with internal index <= 0.");
        }

        bool was_empty = functionMap.empty();

        functionMap[id].push_back(func);

        // If we previously had no functions, set the hook now
        if (was_empty) {
            InstallHook();
        }
    }

    void UnregisterFunction(int id) {
        auto it = functionMap.find(id);
        if (it != functionMap.end()) {

            // Remove all functions for this ID
            functionMap.erase(it);

            // If we now have no functions left, disable the hook
            if (functionMap.empty()) {
                RemoveHook();
            }
        }
    }

    void UnregisterAllFunctions() {
        if (!functionMap.empty()) {
            functionMap.clear();
            RemoveHook();
        }
    }

    // ExecuteFunction:
    // -1 = No handlers found => call original
    //  0 = At least one handler returned false => skip original
    //  1 = Handlers found, all returned true => call original
    int ExecuteFunction(int id, UObject* _this, UFunction* Function, void* Parms) {
        auto it = functionMap.find(id);

        if (it == functionMap.end() || it->second.empty()) {
            return -1; // no handlers
        }



        bool anyFalse = false;
        // Execute all handlers
        for (auto& handler : it->second) {
            bool result = handler(_this, Function, Parms);
            if (!result) {
                anyFalse = true;
                // We still continue calling other handlers; 
                // if any is false, final result is 0 (skip original).
            }
        }

        return anyFalse ? 0 : 1;
    }

private:
    std::unordered_map<int, std::vector<FunctionType>> functionMap;

    void InstallHook() {
        ArkApi::GetHooks().SetHook("UObject.ProcessEvent", &Hook_UObject_ProcessEvent_Hooker, &UObject_ProcessEvent_Hooker_original);
    }

    void RemoveHook() {
        ArkApi::GetHooks().DisableHook("UObject.ProcessEvent", &Hook_UObject_ProcessEvent_Hooker);
    }

    PEFunctionMapper() = default;
    PEFunctionMapper(const PEFunctionMapper&) = delete;
    PEFunctionMapper& operator=(const PEFunctionMapper&) = delete;
};

void Hook_UObject_ProcessEvent_Hooker(UObject* _this, UFunction* Function, void* Parms);

namespace Hooks::Plugin::PEDispatcher
{
    inline void Load()
    {
        // Instantiate the PEFunctionMapper singleton
        PEFunctionMapper::Instance();
    }

    inline void Unload()
    {
        PEFunctionMapper::Instance().UnregisterAllFunctions();
    }
}
