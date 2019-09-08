#include "Definitions.h"
#include "Native.h"

ExtraCharge* ExtraCharge::Create(float value) {
    ExtraCharge* pCharge = (ExtraCharge*)BSExtraData::Create(sizeof(ExtraCharge), ExtraChargeVtbl.GetUIntPtr());
    pCharge->type = kExtraData_Charge;
    pCharge->charge = value;
    return pCharge;
}

ExtraObjectHealth* ExtraObjectHealth::Create(float value) {
    ExtraObjectHealth* pObjectHealth = (ExtraObjectHealth*)BSExtraData::Create(sizeof(ExtraObjectHealth), ExtraObjectHealthVtbl.GetUIntPtr());
    pObjectHealth->type = kExtraData_ObjectHealth;
    pObjectHealth->health = value;
    return pObjectHealth;
}