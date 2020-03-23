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

}

bool Forms::CheckLoadedForms() {
    _LogMessage("Forms::CheckLoadedForms() - Starting.");
    _LogMessage("Look for \"Finished.\" to know when the check is complete.");
    _DebugMessageNT("");

    ITimeKeeper CheckLoadedFormsTimer = ITimeKeeper();

    _LogMessage("Forms::CheckLoadedForms() - Finished.");
    _LogMessage("Time Elapsed: %fms", CheckLoadedFormsTimer.Format(ITimeKeeper::kDuration_Milli));
    return true;
}

// ------------------------------------------------------------------------------------------------
// Initialize Statics
// ------------------------------------------------------------------------------------------------