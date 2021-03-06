/******************************************************************************
** Copyright (c) 2013-2020 Intel Corporation All Rights Reserved
**
** Licensed under the Apache License, Version 2.0 (the "License"); you may not
** use this file except in compliance with the License.
**
** You may obtain a copy of the License at
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
** WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**
** See the License for the specific language governing permissions and
** limitations under the License.
**
******************************************************************************/

#include "PolicyBase.h"
#include "DptfTime.h"
#include "ParticipantTracker.h"

using namespace std;

PolicyBase::PolicyBase(void)
	: m_enabled(false)
{
	m_time.reset(new DptfTime());
	m_trackedParticipants.reset(new ParticipantTracker());
	m_trackedParticipants->setTimeServiceObject(m_time);
}

PolicyBase::~PolicyBase(void)
{
}

// If a policy chooses not to load itself, it should throw out of its onCreate() function.
void PolicyBase::create(Bool enabled, const PolicyServicesInterfaceContainer& policyServices, UIntN policyIndex)
{
	m_enabled = enabled;
	m_policyServices = policyServices;
	m_trackedParticipants->setPolicyServices(policyServices);
	throwIfPolicyRequirementsNotMet();

	try
	{
		onCreate();
		sendOscRequest(m_enabled && autoNotifyPlatformOscOnCreateDestroy(), true);
	}
	catch (...)
	{
		sendOscRequest(m_enabled && autoNotifyPlatformOscOnCreateDestroy(), false);
		m_enabled = false;
		throw;
	}
}

void PolicyBase::destroy(void)
{
	try
	{
		onDestroy();
	}
	catch (...)
	{
		sendOscRequest(autoNotifyPlatformOscOnCreateDestroy(), false);
		m_enabled = false;
		throw;
	}

	sendOscRequest(autoNotifyPlatformOscOnCreateDestroy(), false);
	m_enabled = false;
}

void PolicyBase::enable(void)
{
	sendOscRequest(autoNotifyPlatformOscOnEnableDisable(), true);
	try
	{
		POLICY_LOG_MESSAGE_INFO({ return getName() + ": Policy enable event received."; });
		onEnable();
		m_enabled = true;
	}
	catch (...)
	{
		sendOscRequest(autoNotifyPlatformOscOnEnableDisable(), false);
		throw;
	}
}

void PolicyBase::disable(void)
{
	try
	{
		POLICY_LOG_MESSAGE_INFO({ return getName() + ": Policy disable event received."; });
		onDisable();
	}
	catch (...)
	{
		sendOscRequest(autoNotifyPlatformOscOnEnableDisable(), false);
		throw;
	}

	sendOscRequest(autoNotifyPlatformOscOnEnableDisable(), false);
	m_enabled = false;
}

void PolicyBase::bindParticipant(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Binding participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onBindParticipant(participantIndex);
}

void PolicyBase::unbindParticipant(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Unbinding participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});

	onUnbindParticipant(participantIndex);
}

void PolicyBase::bindDomain(UIntN participantIndex, UIntN domainIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Binding domain for participant. ParticipantIndex = " << participantIndex
				<< ". DomainIndex=" << domainIndex;
		return message.str();
	});
	onBindDomain(participantIndex, domainIndex);
}

void PolicyBase::unbindDomain(UIntN participantIndex, UIntN domainIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Unbinding domain for participant. ParticipantIndex = " << participantIndex
				<< ". DomainIndex =" << domainIndex;
		return message.str();
	});
	onUnbindDomain(participantIndex, domainIndex);
}

void PolicyBase::domainTemperatureThresholdCrossed(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName()
				<< ": Temperature threshold crossed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainTemperatureThresholdCrossed(participantIndex);
}

void PolicyBase::domainPowerControlCapabilityChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName()
				<< ": Power Control Capabilities Changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainPowerControlCapabilityChanged(participantIndex);
}

void PolicyBase::domainPerformanceControlCapabilityChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Performance Control Capabilities Changed for participant. ParticipantIndex = "
				<< participantIndex;
		return message.str();
	});
	onDomainPerformanceControlCapabilityChanged(participantIndex);
}

void PolicyBase::domainPerformanceControlsChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName()
				<< ": Performance control set changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainPerformanceControlsChanged(participantIndex);
}

void PolicyBase::domainCoreControlCapabilityChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName()
				<< ": Core control capabilities changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainCoreControlCapabilityChanged(participantIndex);
}

void PolicyBase::domainPriorityChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Domain priority changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainPriorityChanged(participantIndex);
}

void PolicyBase::domainDisplayControlCapabilityChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName()
				<< ": Display control capabilities changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainDisplayControlCapabilityChanged(participantIndex);
}

void PolicyBase::domainDisplayStatusChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Display status changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainDisplayStatusChanged(participantIndex);
}

void PolicyBase::domainRadioConnectionStatusChanged(
	UIntN participantIndex,
	RadioConnectionStatus::Type radioConnectionStatus)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() + ": Radio Connection Status Changed to "
				<< RadioConnectionStatus::ToString(radioConnectionStatus)
				<< ". ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainRadioConnectionStatusChanged(participantIndex, radioConnectionStatus);
}

void PolicyBase::domainRfProfileChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": RF Profile Changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainRfProfileChanged(participantIndex);
}

void PolicyBase::participantSpecificInfoChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Specific info changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onParticipantSpecificInfoChanged(participantIndex);
}

void PolicyBase::domainVirtualSensorCalibrationTableChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": VSCT changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainVirtualSensorCalibrationTableChanged(participantIndex);
}

void PolicyBase::domainVirtualSensorPollingTableChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": VSPT changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainVirtualSensorPollingTableChanged(participantIndex);
}

void PolicyBase::domainVirtualSensorRecalcChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName()
				<< ": Virtual Sensor recalculation requested for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainVirtualSensorRecalcChanged(participantIndex);
}

void PolicyBase::domainBatteryStatusChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Battery status changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainBatteryStatusChanged(participantIndex);
}

void PolicyBase::domainBatteryInformationChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName()
				<< ": Battery information changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainBatteryInformationChanged(participantIndex);
}

void PolicyBase::domainBatteryHighFrequencyImpedanceChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Battery high frequency impedance changed for participant. ParticipantIndex = "
				<< participantIndex;
		return message.str();
	});
	onDomainBatteryHighFrequencyImpedanceChanged(participantIndex);
}

void PolicyBase::domainBatteryNoLoadVoltageChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName()
				<< ": Battery no-load voltage changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainBatteryNoLoadVoltageChanged(participantIndex);
}

void PolicyBase::domainMaxBatteryPeakCurrentChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName()
				<< ": Max battery peak current changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainMaxBatteryPeakCurrentChanged(participantIndex);
}

void PolicyBase::domainPlatformPowerSourceChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Platform power source (PSRC) changed."; });
	onDomainPlatformPowerSourceChanged(participantIndex);
}

void PolicyBase::domainAdapterPowerRatingChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Adapter power rating (ARTG) changed."; });
	onDomainAdapterPowerRatingChanged(participantIndex);
}

void PolicyBase::domainChargerTypeChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Charger type (CTYP) changed."; });
	onDomainChargerTypeChanged(participantIndex);
}

void PolicyBase::domainPlatformRestOfPowerChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Rest of Platform Power (PROP) changed."; });
	onDomainPlatformRestOfPowerChanged(participantIndex);
}

void PolicyBase::domainMaxBatteryPowerChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Battery Max Peak Power (PMAX) changed."; });
	onDomainMaxBatteryPowerChanged(participantIndex);
}

void PolicyBase::domainPlatformBatterySteadyStateChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Battery Sustained Peak Power (PBSS) changed."; });
	onDomainPlatformBatterySteadyStateChanged(participantIndex);
}

void PolicyBase::domainACNominalVoltageChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": AC Nominal Voltage (AVOL) changed."; });
	onDomainACNominalVoltageChanged(participantIndex);
}

void PolicyBase::domainACOperationalCurrentChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": AC Operational Current (ACUR) changed."; });
	onDomainACOperationalCurrentChanged(participantIndex);
}

void PolicyBase::domainAC1msPercentageOverloadChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": AC 1ms Percentage Overload (AP01) changed."; });
	onDomainAC1msPercentageOverloadChanged(participantIndex);
}

void PolicyBase::domainAC2msPercentageOverloadChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": AC 2ms Percentage Overload (AP02) changed."; });
	onDomainAC2msPercentageOverloadChanged(participantIndex);
}

void PolicyBase::domainAC10msPercentageOverloadChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": AC 10ms Percentage Overload (AP10) changed."; });
	onDomainAC10msPercentageOverloadChanged(participantIndex);
}

void PolicyBase::domainEnergyThresholdCrossed(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Energy threshold crossed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainEnergyThresholdCrossed(participantIndex);
}

void PolicyBase::domainFanCapabilityChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Fan Capabilities changed for participant. ParticipantIndex = " << participantIndex;
		return message.str();
	});
	onDomainFanCapabilityChanged(participantIndex);
}

void PolicyBase::domainSocWorkloadClassificationChanged(
	UIntN participantIndex,
	UIntN domainIndex,
	SocWorkloadClassification::Type socWorkloadClassification)
{
	throwIfPolicyIsDisabled();
	// TODO: want to pass in participant index instead
	POLICY_LOG_MESSAGE_INFO({
		std::stringstream message;
		message << getName() << ": Workload Classification changed for ParticipantIndex = " << participantIndex
				<< " and DomainIndex = " << domainIndex;
		return message.str();
	});
	onDomainSocWorkloadClassificationChanged(participantIndex, domainIndex, socWorkloadClassification);
}

void PolicyBase::activeRelationshipTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Active Relationship Table changed."; });
	onActiveRelationshipTableChanged();
}

void PolicyBase::thermalRelationshipTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Thermal Relationship Table changed"; });
	onThermalRelationshipTableChanged();
}

void PolicyBase::adaptiveUserPresenceTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Adaptive User Presence Table changed."; });
	onAdaptiveUserPresenceTableChanged();
}

void PolicyBase::adaptivePerformanceConditionsTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Adaptive Performance Conditions Table changed."; });
	onAdaptivePerformanceConditionsTableChanged();
}

void PolicyBase::adaptivePerformanceParticipantConditionTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Adaptive Performance Participant Condition Table changed."; });
	onAdaptivePerformanceParticipantConditionTableChanged();
}

void PolicyBase::adaptivePerformanceActionsTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Adaptive Performance Actions Table changed."; });
	Bool hasActiveControlCapabilityLastSet = hasActiveControlCapability();
	Bool hasPassiveControlCapabilityLastSet = hasPassiveControlCapability();
	onAdaptivePerformanceActionsTableChanged();
	updateOscRequestIfNeeded(hasActiveControlCapabilityLastSet, hasPassiveControlCapabilityLastSet);
}

void PolicyBase::pidAlgorithmTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": PID Algorithm Table changed."; });
	Bool hasActiveControlCapabilityLastSet = hasActiveControlCapability();
	Bool hasPassiveControlCapabilityLastSet = hasPassiveControlCapability();
	onPidAlgorithmTableChanged();
	updateOscRequestIfNeeded(hasActiveControlCapabilityLastSet, hasPassiveControlCapabilityLastSet);
}

void PolicyBase::activeControlPointRelationshipTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Active Control Point Relationship Table changed."; });
	onActiveControlPointRelationshipTableChanged();
}

void PolicyBase::powerShareAlgorithmTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Power Share Algorithm Table changed."; });
	onPowerShareAlgorithmTableChanged();
}

void PolicyBase::workloadHintConfigurationChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Workload Hint Configuration changed."; });
	onWorkloadHintConfigurationChanged();
}

void PolicyBase::powerShareAlgorithmTable2Changed(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Power Share Algorithm Table 2 changed."; });
	onPowerShareAlgorithmTable2Changed();
}

Bool PolicyBase::hasActiveControlCapability() const
{
	return false;
}

Bool PolicyBase::hasPassiveControlCapability() const
{
	return false;
}

Bool PolicyBase::hasCriticalShutdownCapability() const
{
	return false;
}

void PolicyBase::connectedStandbyEntry(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Connected standby entry event received."; });
	try
	{
		if (autoNotifyPlatformOscOnConnectedStandbyEntryExit())
		{
			sendOscRequest(autoNotifyPlatformOscOnConnectedStandbyEntryExit(), true);
		}
		onConnectedStandbyEntry();
	}
	catch (...)
	{
		sendOscRequest(autoNotifyPlatformOscOnConnectedStandbyEntryExit(), false);
		throw;
	}
}

void PolicyBase::connectedStandbyExit(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Connected standby exit event received."; });
	try
	{
		onConnectedStandbyExit();
		if (autoNotifyPlatformOscOnConnectedStandbyEntryExit())
		{
			sendOscRequest(autoNotifyPlatformOscOnConnectedStandbyEntryExit(), false);
		}
	}
	catch (...)
	{
		sendOscRequest(autoNotifyPlatformOscOnConnectedStandbyEntryExit(), false);
		throw;
	}
}

void PolicyBase::suspend(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Policy suspend event received."; });
	onSuspend();
}

void PolicyBase::resume(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Policy resume event received."; });
	onResume();
}

void PolicyBase::foregroundApplicationChanged(const std::string& foregroundApplicationName)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": Foreground application changed to " + foregroundApplicationName + "."; });
	onForegroundApplicationChanged(foregroundApplicationName);
}

void PolicyBase::policyInitiatedCallback(UInt64 policyDefinedEventCode, UInt64 param1, void* param2)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Policy Initiated Callback."; });
	onPolicyInitiatedCallback(policyDefinedEventCode, param1, param2);
}

void PolicyBase::operatingSystemPowerSourceChanged(OsPowerSource::Type powerSource)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS Power Source changed to " + OsPowerSource::toString(powerSource) + "."; });
	onOperatingSystemPowerSourceChanged(powerSource);
}

void PolicyBase::operatingSystemLidStateChanged(OsLidState::Type lidState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS Lid state changed to " + OsLidState::toString(lidState) + "."; });
	onOperatingSystemLidStateChanged(lidState);
}

void PolicyBase::operatingSystemBatteryPercentageChanged(UIntN batteryPercentage)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS battery percentage changed to " + std::to_string(batteryPercentage) + "."; });
	onOperatingSystemBatteryPercentageChanged(batteryPercentage);
}

void PolicyBase::operatingSystemPowerSchemePersonalityChanged(OsPowerSchemePersonality::Type powerSchemePersosnality)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": OS Power Scheme Personality changed to "
			   + OsPowerSchemePersonality::toString(powerSchemePersosnality) + ".";
	});
	onOperatingSystemPowerSchemePersonalityChanged(powerSchemePersosnality);
}

void PolicyBase::operatingSystemPlatformTypeChanged(OsPlatformType::Type platformType)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS Platform Type changed to " + OsPlatformType::toString(platformType) + "."; });
	onOperatingSystemPlatformTypeChanged(platformType);
}

void PolicyBase::operatingSystemDockModeChanged(OsDockMode::Type dockMode)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS Dock Mode changed to " + OsDockMode::toString(dockMode) + "."; });
	onOperatingSystemDockModeChanged(dockMode);
}

void PolicyBase::operatingSystemEmergencyCallModeStateChanged(OnOffToggle::Type emergencyCallModeState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": OS Emergency Call Mode State changed to " + OnOffToggle::toString(emergencyCallModeState)
			   + ".";
	});
	onOperatingSystemEmergencyCallModeChanged(emergencyCallModeState);
}

void PolicyBase::operatingSystemMobileNotification(OsMobileNotificationType::Type notificationType, UIntN value)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": OS Mobile Notification for " + OsMobileNotificationType::ToString(notificationType)
			   + " changed to " + std::to_string(value) + ".";
	});
	onOperatingSystemMobileNotification(notificationType, value);
}

void PolicyBase::operatingSystemMixedRealityModeChanged(OnOffToggle::Type mixedRealityMode)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS Mixed Reality mode changed to " + OnOffToggle::toString(mixedRealityMode) + "."; });
	onOperatingSystemMixedRealityModeChanged(mixedRealityMode);
}

void PolicyBase::operatingSystemUserPresenceChanged(OsUserPresence::Type userPresence)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS User Presence changed to " + OsUserPresence::toString(userPresence) + "."; });
	onOperatingSystemUserPresenceChanged(userPresence);
}

void PolicyBase::operatingSystemSessionStateChanged(OsSessionState::Type sessionState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS Session State changed to " + OsSessionState::toString(sessionState) + "."; });
	onOperatingSystemSessionStateChanged(sessionState);
}

void PolicyBase::operatingSystemScreenStateChanged(OnOffToggle::Type screenState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS Screen State changed to " + OnOffToggle::toString(screenState) + "."; });
	onOperatingSystemScreenStateChanged(screenState);
}

void PolicyBase::operatingSystemBatteryCountChanged(UIntN batteryCount)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS battery count changed to " + std::to_string(batteryCount) + "."; });
	onOperatingSystemBatteryCountChanged(batteryCount);
}

void PolicyBase::operatingSystemPowerSliderChanged(OsPowerSlider::Type powerSlider)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS power slider changed to " + OsPowerSlider::toString(powerSlider) + "."; });
	onOperatingSystemPowerSliderChanged(powerSlider);
}

void PolicyBase::coolingModePolicyChanged(CoolingMode::Type coolingMode)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": Cooling mode changed to " + CoolingMode::toString(coolingMode) + "."; });
	onCoolingModePolicyChanged(coolingMode);
}

void PolicyBase::passiveTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Passive Table changed."; });
	onPassiveTableChanged();
}

void PolicyBase::sensorOrientationChanged(SensorOrientation::Type sensorOrientation)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Sensor orientation changed to " + SensorOrientation::toString(sensorOrientation) + ".";
	});
	onSensorOrientationChanged(sensorOrientation);
}

void PolicyBase::sensorSpatialOrientationChanged(SensorSpatialOrientation::Type sensorSpatialOrientation)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Sensor spatial orientation changed to "
			   + SensorSpatialOrientation::toString(sensorSpatialOrientation) + ".";
	});
	onSensorSpatialOrientationChanged(sensorSpatialOrientation);
}

void PolicyBase::sensorMotionChanged(OnOffToggle::Type sensorMotion)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": Sensor motion state changed to " + OnOffToggle::toString(sensorMotion) + "."; });
	onSensorMotionChanged(sensorMotion);
}

void PolicyBase::oemVariablesChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": OEM variable(s) changed."; });
	onOemVariablesChanged();
}

void PolicyBase::powerBossConditionsTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Power Boss Conditions Table changed."; });
	onPowerBossConditionsTableChanged();
}

void PolicyBase::powerBossActionsTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Power Boss Actions Table changed."; });
	Bool hasPassiveControlCapabilityLastSet = hasPassiveControlCapability();
	onPowerBossActionsTableChanged();
	updateOscRequestIfNeeded(false, hasPassiveControlCapabilityLastSet);
}

void PolicyBase::powerBossMathTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Power Boss Math Table changed."; });
	onPowerBossMathTableChanged();
}

void PolicyBase::voltageThresholdMathTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Voltage Threshold Math Table changed."; });
	onVoltageThresholdMathTableChanged();
}

void PolicyBase::emergencyCallModeTableChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Emergency Call Mode Table changed."; });
	onEmergencyCallModeTableChanged();
}

void PolicyBase::overrideTimeObject(std::shared_ptr<TimeInterface> timeObject)
{
	m_time = timeObject;
	m_trackedParticipants->setTimeServiceObject(m_time);
	onOverrideTimeObject(timeObject);
}

void PolicyBase::powerLimitChanged(void)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Power Limit Changed."; });
	onPowerLimitChanged();
}

void PolicyBase::performanceCapabilitiesChanged(UIntN participantIndex)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({ return getName() + ": Performance Capabilities Changed."; });
	onPerformanceCapabilitiesChanged(participantIndex);
}

void PolicyBase::sensorUserPresenceChanged(SensorUserPresence::Type userPresence)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Sensor User Presence changed to " + SensorUserPresence::toString(userPresence) + ".";
	});
	onSensorUserPresenceChanged(userPresence);
}

void PolicyBase::platformUserPresenceChanged(SensorUserPresence::Type userPresence)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Platform User Presence changed to " + SensorUserPresence::toString(userPresence) + ".";
	});
	onPlatformUserPresenceChanged(userPresence);
}

void PolicyBase::wakeOnApproachFeatureStateChanged(Bool wakeOnApproachFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Wake On Approach Feature State changed to "
			   + StatusFormat::friendlyValue(wakeOnApproachFeatureState) + ".";
	});
	onWakeOnApproachFeatureStateChanged(wakeOnApproachFeatureState);
}

void PolicyBase::wakeOnApproachWithExternalMonitorFeatureStateChanged(
	Bool wakeOnApproachWithExternalMonitorFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Wake On Approach With External Monitor Feature State changed to "
			   + StatusFormat::friendlyValue(wakeOnApproachWithExternalMonitorFeatureState) + ".";
	});
	onWakeOnApproachWithExternalMonitorFeatureStateChanged(wakeOnApproachWithExternalMonitorFeatureState);
}

void PolicyBase::wakeOnApproachOnLowBatteryFeatureStateChanged(
	Bool wakeOnApproachOnLowBatteryFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Wake On Approach On Low Battery Feature State changed to "
			   + StatusFormat::friendlyValue(wakeOnApproachOnLowBatteryFeatureState) + ".";
	});
	onWakeOnApproachOnLowBatteryFeatureStateChanged(wakeOnApproachOnLowBatteryFeatureState);
}

void PolicyBase::wakeOnApproachBatteryRemainingPercentageChanged(Percentage wakeOnApproachBatteryRemainingPercentage)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Wake On Approach Battery Remaining Percentage changed to "
			   + StatusFormat::friendlyValue(wakeOnApproachBatteryRemainingPercentage.toWholeNumber()) + ".";
	});
	onWakeOnApproachBatteryRemainingPercentageChanged(wakeOnApproachBatteryRemainingPercentage);
}

void PolicyBase::walkAwayLockFeatureStateChanged(Bool walkAwayLockFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Walk Away Lock Feature State changed to "
			   + StatusFormat::friendlyValue(walkAwayLockFeatureState) + ".";
	});
	onWalkAwayLockFeatureStateChanged(walkAwayLockFeatureState);
}

void PolicyBase::walkAwayLockWithExternalMonitorFeatureStateChanged(
	Bool walkAwayLockWithExternalMonitorFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Walk Away Lock With External Monitor Feature State changed to "
			   + StatusFormat::friendlyValue(walkAwayLockWithExternalMonitorFeatureState) + ".";
	});
	onWalkAwayLockWithExternalMonitorFeatureStateChanged(walkAwayLockWithExternalMonitorFeatureState);
}

void PolicyBase::walkAwayLockDimScreenFeatureStateChanged(Bool walkAwayLockDimScreenFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Walk Away Lock Dim Screen Feature State changed to "
			   + StatusFormat::friendlyValue(walkAwayLockDimScreenFeatureState) + ".";
	});
	onWalkAwayLockDimScreenFeatureStateChanged(walkAwayLockDimScreenFeatureState);
}

void PolicyBase::walkAwayLockDisplayOffAfterLockFeatureStateChanged(
	Bool walkAwayLockDisplayOffAfterLockFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Walk Away Lock Display Off After Lock Feature State changed to "
			   + StatusFormat::friendlyValue(walkAwayLockDisplayOffAfterLockFeatureState) + ".";
	});
	onWalkAwayLockDisplayOffAfterLockFeatureStateChanged(walkAwayLockDisplayOffAfterLockFeatureState);
}

void PolicyBase::walkAwayLockHonorPowerRequestsForDisplayFeatureStateChanged(
	Bool walkAwayLockHonorPowerRequestsForDisplayFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Walk Away Lock Honor Power Requests For Display Feature State changed to "
			   + StatusFormat::friendlyValue(walkAwayLockHonorPowerRequestsForDisplayFeatureState) + ".";
	});
	onWalkAwayLockHonorPowerRequestsForDisplayFeatureStateChanged(walkAwayLockHonorPowerRequestsForDisplayFeatureState);
}

void PolicyBase::walkAwayLockHonorUserInCallFeatureStateChanged(
	Bool walkAwayLockHonorUserInCallFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Walk Away Lock Honor User In Call Feature State changed to "
			   + StatusFormat::friendlyValue(walkAwayLockHonorUserInCallFeatureState) + ".";
	});
	onWalkAwayLockHonorUserInCallFeatureStateChanged(walkAwayLockHonorUserInCallFeatureState);
}

void PolicyBase::userInCallStateChanged(Bool userInCallState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": User In Call State changed to "
			   + StatusFormat::friendlyValue(userInCallState) + ".";
	});
	onUserInCallStateChanged(userInCallState);
}

void PolicyBase::walkAwayLockScreenLockWaitTimeChanged(TimeSpan walkAwayLockScreenLockWaitTime)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Walk Away Lock Screen Lock Wait Time changed to "
			   + StatusFormat::friendlyValue(walkAwayLockScreenLockWaitTime.asSeconds()) + ".";
	});
	onWalkAwayLockScreenLockWaitTimeChanged(walkAwayLockScreenLockWaitTime);
}

void PolicyBase::walkAwayLockPreDimWaitTimeChanged(TimeSpan walkAwayLockPreDimWaitTime)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Walk Away Lock Pre Dim Wait Time changed to "
			   + StatusFormat::friendlyValue(walkAwayLockPreDimWaitTime.asSeconds()) + ".";
	});
	onWalkAwayLockPreDimWaitTimeChanged(walkAwayLockPreDimWaitTime);
}

void PolicyBase::walkAwayLockUserPresentWaitTimeChanged(TimeSpan walkAwayLockUserPresentWaitTime)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Walk Away Lock HID Interaction Timeout changed to "
			   + StatusFormat::friendlyValue(walkAwayLockUserPresentWaitTime.asSeconds()) + ".";
	});
	onWalkAwayLockUserPresentWaitTimeChanged(walkAwayLockUserPresentWaitTime);
}

void PolicyBase::walkAwayLockDimIntervalChanged(TimeSpan walkAwayLockDimInterval)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Walk Away Lock Dim Interval changed to "
			   + StatusFormat::friendlyValue(walkAwayLockDimInterval.asSeconds())
			   + ".";
	});
	onWalkAwayLockDimIntervalChanged(walkAwayLockDimInterval);
}

void PolicyBase::adaptiveDimmingFeatureStateChanged(Bool adaptiveDimmingFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Adaptive Dimming Feature State changed to "
			   + StatusFormat::friendlyValue(adaptiveDimmingFeatureState) + ".";
	});
	onAdaptiveDimmingFeatureStateChanged(adaptiveDimmingFeatureState);
}

void PolicyBase::adaptiveDimmingWithExternalMonitorFeatureStateChanged(
	Bool adaptiveDimmingWithExternalMonitorFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Adaptive Dimming With External Monitor Feature State changed to "
			   + StatusFormat::friendlyValue(adaptiveDimmingWithExternalMonitorFeatureState) + ".";
	});
	onAdaptiveDimmingWithExternalMonitorFeatureStateChanged(adaptiveDimmingWithExternalMonitorFeatureState);
}

void PolicyBase::adaptiveDimmingWithPresentationModeFeatureStateChanged(
	Bool adaptiveDimmingWithPresentationModeFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Adaptive Dimming With Presentation Mode Feature State changed to "
			   + StatusFormat::friendlyValue(adaptiveDimmingWithPresentationModeFeatureState) + ".";
	});
	onAdaptiveDimmingWithPresentationModeFeatureStateChanged(adaptiveDimmingWithPresentationModeFeatureState);
}

void PolicyBase::adaptiveDimmingPreDimWaitTimeChanged(
	TimeSpan adaptiveDimmingPreDimWaitTime)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Adaptive Dimming Pre Dim Wait Time changed to "
			   + StatusFormat::friendlyValue(adaptiveDimmingPreDimWaitTime.asSeconds()) + ".";
	});
	onAdaptiveDimmingPreDimWaitTimeChanged(adaptiveDimmingPreDimWaitTime);
}

void PolicyBase::mispredictionFaceDetectionFeatureStateChanged(Bool mispredictionFaceDetectionFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Misprediction Face Detection Feature State changed to "
			   + StatusFormat::friendlyValue(mispredictionFaceDetectionFeatureState) + ".";
	});
	onMispredictionFaceDetectionFeatureStateChanged(mispredictionFaceDetectionFeatureState);
}

void PolicyBase::mispredictionTimeWindowChanged(TimeSpan mispredictionTimeWindow)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ":Misprediction Time Window changed to "
			   + StatusFormat::friendlyValue(mispredictionTimeWindow.asSeconds()) + ".";
	});
	onMispredictionTimeWindowChanged(mispredictionTimeWindow);
}

void PolicyBase::misprediction1DimWaitTimeChanged(TimeSpan misprediction1DimWaitTime)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Misprediction 1 Dim Wait Time changed to "
			   + StatusFormat::friendlyValue(misprediction1DimWaitTime.asSeconds()) + ".";
	});
	onMisprediction1DimWaitTimeChanged(misprediction1DimWaitTime);
}

void PolicyBase::misprediction2DimWaitTimeChanged(TimeSpan misprediction2DimWaitTime)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Misprediction 2 Dim Wait Time changed to "
			   + StatusFormat::friendlyValue(misprediction2DimWaitTime.asSeconds()) + ".";
	});
	onMisprediction2DimWaitTimeChanged(misprediction2DimWaitTime);
}

void PolicyBase::misprediction3DimWaitTimeChanged(TimeSpan misprediction3DimWaitTime)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Misprediction 3 Dim Wait Time changed to "
			   + StatusFormat::friendlyValue(misprediction3DimWaitTime.asSeconds()) + ".";
	});
	onMisprediction3DimWaitTimeChanged(misprediction3DimWaitTime);
}

void PolicyBase::misprediction4DimWaitTimeChanged(TimeSpan misprediction4DimWaitTime)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": Misprediction 4 Dim Wait Time changed to "
			   + StatusFormat::friendlyValue(misprediction4DimWaitTime.asSeconds()) + ".";
	});
	onMisprediction4DimWaitTimeChanged(misprediction4DimWaitTime);
}

void PolicyBase::noLockOnPresenceFeatureStateChanged(Bool noLockOnPresenceFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": No Lock On Presence Feature State changed to "
			   + StatusFormat::friendlyValue(noLockOnPresenceFeatureState) + ".";
	});
	onNoLockOnPresenceFeatureStateChanged(noLockOnPresenceFeatureState);
}

void PolicyBase::noLockOnPresenceExternalMonitorFeatureStateChanged(Bool noLockOnPresenceExternalMonitorFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": No Lock On Presence External Monitor Feature State changed to "
			   + StatusFormat::friendlyValue(noLockOnPresenceExternalMonitorFeatureState) + ".";
	});
	onNoLockOnPresenceExternalMonitorFeatureStateChanged(noLockOnPresenceExternalMonitorFeatureState);
}

void PolicyBase::noLockOnPresenceOnBatteryFeatureStateChanged(
	Bool noLockOnPresenceOnBatteryFeatureState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": No Lock On Presence On Battery Feature State changed to "
			   + StatusFormat::friendlyValue(noLockOnPresenceOnBatteryFeatureState) + ".";
	});
	onNoLockOnPresenceOnBatteryFeatureStateChanged(noLockOnPresenceOnBatteryFeatureState);
}

void PolicyBase::noLockOnPresenceBatteryRemainingPercentageChanged(
	Percentage noLockOnPresenceBatteryRemainingPercentage)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": No Lock On Presence Battery Remaining Percentage changed to "
			   + StatusFormat::friendlyValue(noLockOnPresenceBatteryRemainingPercentage.toWholeNumber()) + ".";
	});
	onNoLockOnPresenceBatteryRemainingPercentageChanged(noLockOnPresenceBatteryRemainingPercentage);
}

void PolicyBase::noLockOnPresenceResetWaitTimeChanged(TimeSpan noLockOnPresenceResetWaitTime)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": No Lock On Presence Battery Reset Wait Time changed to "
			   + StatusFormat::friendlyValue(noLockOnPresenceResetWaitTime.asSeconds()) + ".";
	});
	onNoLockOnPresenceResetWaitTimeChanged(noLockOnPresenceResetWaitTime);
}

void PolicyBase::failsafeTimeoutChanged(TimeSpan failsafeTimeout)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ":Failsafe Timeout changed to "
			   + StatusFormat::friendlyValue(failsafeTimeout.asSeconds()) + ".";
	});
	onFailsafeTimeoutChanged(failsafeTimeout);
}

void PolicyBase::userPresenceAppStateChanged(Bool userPresenceAppState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": User Presence App State changed to " + StatusFormat::friendlyValue(userPresenceAppState)
			   + ".";
	});
	onUserPresenceAppStateChanged(userPresenceAppState);
}

void PolicyBase::externalMonitorStateChanged(Bool externalMonitorState)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": External Monitor State changed to " + StatusFormat::friendlyValue(externalMonitorState)
			   + ".";
	});
	onExternalMonitorStateChanged(externalMonitorState);
}

void PolicyBase::userNotPresentDimTargetChanged(Percentage userNotPresentDimTarget)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": User Not Present Dim Target changed to "
			   + StatusFormat::friendlyValue(userNotPresentDimTarget) + ".";
	});
	onUserNotPresentDimTargetChanged(userNotPresentDimTarget);
}

void PolicyBase::userDisengagedDimmingIntervalChanged(TimeSpan userDisengagedDimmingInterval)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": User Disengaged Dimming Interval changed to "
			   + StatusFormat::friendlyValue(userDisengagedDimmingInterval.asSeconds()) + ".";
	});
	onUserDisengagedDimmingIntervalChanged(userDisengagedDimmingInterval);
}

void PolicyBase::userDisengagedDimTargetChanged(Percentage userDisengagedDimTarget)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": User Disengaged Dim Target changed to " + StatusFormat::friendlyValue(userDisengagedDimTarget)
			   + ".";
	});
	onUserDisengagedDimTargetChanged(userDisengagedDimTarget);
}

void PolicyBase::userDisengagedDimWaitTimeChanged(TimeSpan userDisengagedDimWaitTime)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO({
		return getName() + ": User Disengaged Dim Wait Time changed to "
			   + StatusFormat::friendlyValue(userDisengagedDimWaitTime.asSeconds()) + ".";
	});
	onUserDisengagedDimWaitTimeChanged(userDisengagedDimWaitTime);
}

void PolicyBase::onDomainTemperatureThresholdCrossed(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainPowerControlCapabilityChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainPerformanceControlCapabilityChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainPerformanceControlsChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainCoreControlCapabilityChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainPriorityChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainDisplayControlCapabilityChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainDisplayStatusChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainRadioConnectionStatusChanged(
	UIntN participantIndex,
	RadioConnectionStatus::Type radioConnectionStatus)
{
	throw not_implemented();
}

void PolicyBase::onDomainRfProfileChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onParticipantSpecificInfoChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainVirtualSensorCalibrationTableChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainVirtualSensorPollingTableChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainVirtualSensorRecalcChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainBatteryStatusChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainBatteryInformationChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainBatteryHighFrequencyImpedanceChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainBatteryNoLoadVoltageChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainMaxBatteryPeakCurrentChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainPlatformPowerSourceChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainAdapterPowerRatingChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainChargerTypeChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainPlatformRestOfPowerChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainMaxBatteryPowerChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainPlatformBatterySteadyStateChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainACNominalVoltageChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainACOperationalCurrentChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainAC1msPercentageOverloadChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainAC2msPercentageOverloadChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainAC10msPercentageOverloadChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainEnergyThresholdCrossed(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainFanCapabilityChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onDomainSocWorkloadClassificationChanged(
	UIntN participantIndex,
	UIntN domainIndex,
	SocWorkloadClassification::Type socWorkloadClassification)
{
	throw not_implemented();
}

void PolicyBase::onActiveRelationshipTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onThermalRelationshipTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onAdaptivePerformanceConditionsTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onAdaptiveUserPresenceTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onAdaptivePerformanceParticipantConditionTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onAdaptivePerformanceActionsTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onConnectedStandbyEntry(void)
{
	throw not_implemented();
}

void PolicyBase::onConnectedStandbyExit(void)
{
	throw not_implemented();
}

void PolicyBase::onSuspend(void)
{
	throw not_implemented();
}

void PolicyBase::onResume(void)
{
	throw not_implemented();
}

void PolicyBase::onForegroundApplicationChanged(const std::string& foregroundApplicationName)
{
	throw not_implemented();
}

void PolicyBase::onPolicyInitiatedCallback(UInt64 policyDefinedEventCode, UInt64 param1, void* param2)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemPowerSourceChanged(OsPowerSource::Type powerSource)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemLidStateChanged(OsLidState::Type lidState)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemBatteryPercentageChanged(UIntN batteryPercentage)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemPowerSchemePersonalityChanged(OsPowerSchemePersonality::Type powerSchemePersonality)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemPlatformTypeChanged(OsPlatformType::Type platformType)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemDockModeChanged(OsDockMode::Type dockMode)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemUserPresenceChanged(OsUserPresence::Type userPresence)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemSessionStateChanged(OsSessionState::Type sessionState)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemScreenStateChanged(OnOffToggle::Type screenState)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemEmergencyCallModeChanged(OnOffToggle::Type emergencyCallMode)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemMobileNotification(UIntN mobileNotificationType, UIntN value)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemMixedRealityModeChanged(OnOffToggle::Type mixedRealityMode)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemBatteryCountChanged(UIntN batteryCount)
{
	throw not_implemented();
}

void PolicyBase::onOperatingSystemPowerSliderChanged(OsPowerSlider::Type powerSlider)
{
	throw not_implemented();
}

void PolicyBase::onCoolingModePolicyChanged(CoolingMode::Type coolingMode)
{
	throw not_implemented();
}

void PolicyBase::onPassiveTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onSensorOrientationChanged(SensorOrientation::Type sensorOrientation)
{
	throw not_implemented();
}

void PolicyBase::onSensorSpatialOrientationChanged(SensorSpatialOrientation::Type sensorSpatialOrientation)
{
	throw not_implemented();
}

void PolicyBase::onSensorMotionChanged(OnOffToggle::Type sensorMotion)
{
	throw not_implemented();
}

void PolicyBase::onOemVariablesChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onPowerBossConditionsTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onPowerBossActionsTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onPowerBossMathTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onVoltageThresholdMathTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onEmergencyCallModeTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onPidAlgorithmTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onActiveControlPointRelationshipTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onPowerShareAlgorithmTableChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onWorkloadHintConfigurationChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onPowerShareAlgorithmTable2Changed(void)
{
	throw not_implemented();
}

void PolicyBase::onPowerLimitChanged(void)
{
	throw not_implemented();
}

void PolicyBase::onPerformanceCapabilitiesChanged(UIntN participantIndex)
{
	throw not_implemented();
}

void PolicyBase::onSensorUserPresenceChanged(SensorUserPresence::Type userPresence)
{
	throw not_implemented();
}

void PolicyBase::onPlatformUserPresenceChanged(SensorUserPresence::Type userPresence)
{
	throw not_implemented();
}

void PolicyBase::onWakeOnApproachFeatureStateChanged(Bool wakeOnApproachFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onWakeOnApproachWithExternalMonitorFeatureStateChanged(
	Bool wakeOnApproachWithExternalMonitorFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onWakeOnApproachOnLowBatteryFeatureStateChanged(
	Bool wakeOnApproachOnLowBatteryFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onWakeOnApproachBatteryRemainingPercentageChanged(Percentage wakeOnApproachBatteryRemainingPercentage)
{
	throw not_implemented();
}

void PolicyBase::onWalkAwayLockFeatureStateChanged(Bool walkAwayLockFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onWalkAwayLockWithExternalMonitorFeatureStateChanged(
	Bool walkAwayLockWithExternalMonitorFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onWalkAwayLockDimScreenFeatureStateChanged(
	Bool walkAwayLockDimScreenFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onWalkAwayLockDisplayOffAfterLockFeatureStateChanged(
	Bool walkAwayLockDisplayOffAfterLockFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onWalkAwayLockHonorPowerRequestsForDisplayFeatureStateChanged(
	Bool walkAwayLockHonorPowerRequestsForDisplayFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onWalkAwayLockHonorUserInCallFeatureStateChanged(
	Bool walkAwayLockHonorUserInCallFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onUserInCallStateChanged(Bool userInCallState)
{
	throw not_implemented();
}

void PolicyBase::onWalkAwayLockScreenLockWaitTimeChanged(TimeSpan walkAwayLockScreenLockWaitTime)
{
	throw not_implemented();
}

void PolicyBase::onWalkAwayLockPreDimWaitTimeChanged(TimeSpan walkAwayLockPreDimWaitTime)
{
	throw not_implemented();
}

void PolicyBase::onWalkAwayLockUserPresentWaitTimeChanged(TimeSpan walkAwayLockUserPresentWaitTime)
{
	throw not_implemented();
}

void PolicyBase::onWalkAwayLockDimIntervalChanged(TimeSpan walkAwayLockDimInterval)
{
	throw not_implemented();
}

void PolicyBase::onAdaptiveDimmingFeatureStateChanged(Bool adaptiveDimmingFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onAdaptiveDimmingWithExternalMonitorFeatureStateChanged(
	Bool adaptiveDimmingWithExternalMonitorFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onAdaptiveDimmingWithPresentationModeFeatureStateChanged(
	Bool adaptiveDimmingWithPresentationModeFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onAdaptiveDimmingPreDimWaitTimeChanged(TimeSpan adaptiveDimmingPreDimWaitTime)
{
	throw not_implemented();
}

void PolicyBase::onMispredictionFaceDetectionFeatureStateChanged(Bool mispredictionFaceDetectionFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onMispredictionTimeWindowChanged(TimeSpan mispredictionTimeWindow)
{
	throw not_implemented();
}

void PolicyBase::onMisprediction1DimWaitTimeChanged(TimeSpan misprediction1DimWaitTime)
{
	throw not_implemented();
}

void PolicyBase::onMisprediction2DimWaitTimeChanged(TimeSpan misprediction2DimWaitTime)
{
	throw not_implemented();
}

void PolicyBase::onMisprediction3DimWaitTimeChanged(TimeSpan misprediction3DimWaitTime)
{
	throw not_implemented();
}

void PolicyBase::onMisprediction4DimWaitTimeChanged(TimeSpan misprediction4DimWaitTime)
{
	throw not_implemented();
}

void PolicyBase::onNoLockOnPresenceFeatureStateChanged(Bool noLockOnPresenceFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onNoLockOnPresenceExternalMonitorFeatureStateChanged(Bool noLockOnPresenceExternalMonitorFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onNoLockOnPresenceOnBatteryFeatureStateChanged(
	Bool noLockOnPresenceOnBatteryFeatureState)
{
	throw not_implemented();
}

void PolicyBase::onNoLockOnPresenceBatteryRemainingPercentageChanged(
	Percentage noLockOnPresenceBatteryRemainingPercentage)
{
	throw not_implemented();
}

void PolicyBase::onNoLockOnPresenceResetWaitTimeChanged(TimeSpan noLockOnPresenceResetWaitTime)
{
	throw not_implemented();
}

void PolicyBase::onFailsafeTimeoutChanged(TimeSpan failsafeTimeout)
{
	throw not_implemented();
}

void PolicyBase::onUserPresenceAppStateChanged(Bool userPresenceAppState)
{
	throw not_implemented();
}

void PolicyBase::onExternalMonitorStateChanged(Bool externalMonitorState)
{
	throw not_implemented();
}

void PolicyBase::onUserNotPresentDimTargetChanged(Percentage userNotPresentDimTarget)
{
	throw not_implemented();
}

void PolicyBase::onUserDisengagedDimmingIntervalChanged(TimeSpan userDisengagedDimmingInterval)
{
	throw not_implemented();
}

void PolicyBase::onUserDisengagedDimTargetChanged(Percentage userDisengagedDimTarget)
{
	throw not_implemented();
}

void PolicyBase::onUserDisengagedDimWaitTimeChanged(TimeSpan userDisengagedDimWaitTime)
{
	throw not_implemented();
}

void PolicyBase::onOverrideTimeObject(std::shared_ptr<TimeInterface> timeObject)
{
	// optional to implement
}

PolicyServicesInterfaceContainer& PolicyBase::getPolicyServices() const
{
	return m_policyServices;
}

std::shared_ptr<ParticipantTrackerInterface> PolicyBase::getParticipantTracker() const
{
	return m_trackedParticipants;
}

std::shared_ptr<TimeInterface>& PolicyBase::getTime() const
{
	return m_time;
}

void PolicyBase::throwIfPolicyRequirementsNotMet()
{
	if (m_policyServices.platformConfigurationData == nullptr)
	{
		throw dptf_exception(
			"Policy Services does not have an implementation \
							  for platformConfigurationData interface.");
	}
}

void PolicyBase::throwIfPolicyIsDisabled()
{
	if (m_enabled == false)
	{
		throw dptf_exception("The policy has been disabled.");
	}
}

void PolicyBase::sendOscRequest(Bool shouldSendOscRequest, Bool isPolicyEnabled)
{
	if (shouldSendOscRequest)
	{
		try
		{
			UInt32 oscInputCapabilitiesDWord = POLICY_DISABLED;

			if (isPolicyEnabled)
			{
				oscInputCapabilitiesDWord = POLICY_ENABLED;
				if (hasActiveControlCapability())
				{
					oscInputCapabilitiesDWord = oscInputCapabilitiesDWord | ACTIVE_CONTROL_SUPPORTED;
				}
				if (hasPassiveControlCapability())
				{
					oscInputCapabilitiesDWord = oscInputCapabilitiesDWord | PASSIVE_CONTROL_SUPPORTED;
				}
				if (hasCriticalShutdownCapability())
				{
					oscInputCapabilitiesDWord = oscInputCapabilitiesDWord | CRITICAL_SHUTDOWN_SUPPORTED;
				}
			}

			DptfRequest request(DptfRequestType::PlatformNotificationSetOsc);
			request.setDataFromUInt32(oscInputCapabilitiesDWord);
			auto result = m_policyServices.serviceRequest->submitRequest(request);
			result.throwIfFailure();
			POLICY_LOG_MESSAGE_INFO({ return getName() + ": " + result.getMessage(); });
		}
		catch (std::exception& ex)
		{
			POLICY_LOG_MESSAGE_WARNING_EX({ return getName() + ": Failed to set _OSC: " + string(ex.what()); });
		}
		catch (...)
		{
			POLICY_LOG_MESSAGE_WARNING({ return getName() + ": Failed to set _OSC."; });
		}
	}
}

void PolicyBase::updateOscRequestIfNeeded(
	Bool hasActiveControlCapabilityLastSet,
	Bool hasPassiveControlCapabilityLastSet,
	Bool hasCriticalShutdownCapabilityLastSet)
{

	if (hasActiveControlCapabilityLastSet != hasActiveControlCapability()
		|| hasPassiveControlCapabilityLastSet != hasPassiveControlCapability()
		|| hasCriticalShutdownCapabilityLastSet != hasCriticalShutdownCapability())
	{
		sendOscRequest(m_enabled && autoNotifyPlatformOscOnCreateDestroy(), true);
	}
}

std::shared_ptr<XmlNode> PolicyBase::getXmlForTripPointStatistics(std::set<UIntN> targetIndexes) const
{
	auto status = XmlNode::createWrapperElement("trip_point_statistics");

	for (auto targetIndex = targetIndexes.begin(); targetIndex != targetIndexes.end(); ++targetIndex)
	{
		if (*targetIndex != Constants::Invalid)
		{
			auto participant = getParticipantTracker()->getParticipant(*targetIndex);
			status->addChild(participant->getXmlForTripPointStatistics());
		}
	}

	return status;
}

void PolicyBase::operatingSystemGameModeChanged(OnOffToggle::Type gameMode)
{
	throwIfPolicyIsDisabled();
	POLICY_LOG_MESSAGE_INFO(
		{ return getName() + ": OS game mode changed to " + OnOffToggle::toString(gameMode) + "."; });
	onOperatingSystemGameModeChanged(gameMode);
}

void PolicyBase::onOperatingSystemGameModeChanged(OnOffToggle::Type gameMode)
{
	throw not_implemented();
}
