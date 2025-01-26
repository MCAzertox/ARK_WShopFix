#include "ProcessEventDispatcher.hpp"

void Hook_UObject_ProcessEvent_Hooker(UObject* _this, UFunction* Function, void* Parms) {
    if (!_this || !Function) {
        Log::GetLog()->warn("UObject_ProcessEvent_Hooker: Invalid parameters");
        // Call original if parameters are not valid
        return UObject_ProcessEvent_Hooker_original(_this, Function, Parms);
    }

    int internalIndex = Function->InternalIndexField();
    int handled = PEFunctionMapper::Instance().ExecuteFunction(internalIndex, _this, Function, Parms);

    if (handled == -1) {
        // No handlers for this function => call original
        return UObject_ProcessEvent_Hooker_original(_this, Function, Parms);
    }

    if (handled == 0) {
        // At least one handler returned false => do not call the original
        return;
    }

    // Handlers found and all returned true => call original
    UObject_ProcessEvent_Hooker_original(_this, Function, Parms);
}
