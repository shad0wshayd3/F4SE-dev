#include "Forms.h"

// ------------------------------------------------------------------------------------------------
// Public
// ------------------------------------------------------------------------------------------------

bool Forms::Load() {
    FormLoader::SetPluginName(DEFAULT_PLUGIN_NAME);
    _LogMessage("Forms::Load - Game Plugin: %s.", FormLoader::GetPluginName().c_str());

    // ------------------------------------------------------------------------
    // Open Main Plugin
    // ------------------------------------------------------------------------
    if (!FormLoader::OpenPlugin()) {
        _LogError("Forms::Load - Game Plugin is not loaded.");
        return false;
    }

    _LogMessage("Forms::Load - Game Plugin loaded.");

    // ------------------------------------------------------------------------
    // Load Forms
    // ------------------------------------------------------------------------
    PipboyLightActive       = ToType(FormLoader::LookupFromPlugin(0x800, "FormIDs:PipboyLightActive"),      ActorValueInfo);
    RadiationSourceCount    = ToType(FormLoader::LookupFromPlugin(0x801, "FormIDs:RadiationSourceCount"),   ActorValueInfo);
    InventoryWeight         = ToType(FormLoader::LookupFromPlugin(0x802, "FormIDs:InventoryWeight"),        ActorValueInfo);

    // ------------------------------------------------------------------------
    // Check Forms
    // ------------------------------------------------------------------------
    if (ISettings::GetBool("General:CheckLoadedForms", false)) {
        if (!CheckLoadedForms()) {
            _LogError("Forms::Load - Error while checking Forms.");
            return false;
        }
    }

    // ------------------------------------------------------------------------
    // End
    // ------------------------------------------------------------------------
    _LogMessage("Forms::Load - Finished.");
    return true;
}

void Forms::Unload() {
    PipboyLightActive       = nullptr;
    RadiationSourceCount    = nullptr;
    InventoryWeight         = nullptr;

    _LogMessage("Forms::Unload - Finished.");
}

bool Forms::CheckLoadedForms() {
    _LogMessage("Forms::CheckLoadedForms() - Starting.");
    _LogMessage("Look for \"Finished.\" to know when the check is complete.");
    _DebugMessageNT("");

    ITimeKeeper CheckLoadedFormsTimer = ITimeKeeper();

    CheckForm(PipboyLightActive,        kFormType_AVIF, avName);
    CheckForm(RadiationSourceCount,     kFormType_AVIF, avName);
    CheckForm(InventoryWeight,          kFormType_AVIF, avName);

    _LogMessage("Forms::CheckLoadedForms() - Finished.");
    _LogMessage("Time Elapsed: %fms", CheckLoadedFormsTimer.Format(ITimeKeeper::kDuration_Milli));
    return true;
}

// ------------------------------------------------------------------------------------------------
// Initialize Statics
// ------------------------------------------------------------------------------------------------

ActorValueInfo* Forms::PipboyLightActive        = nullptr;
ActorValueInfo* Forms::RadiationSourceCount     = nullptr;
ActorValueInfo* Forms::InventoryWeight          = nullptr;
