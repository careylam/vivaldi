// Copyright (c) 2017 Vivaldi Technologies AS. All rights reserved

#include "extensions/api/prefs/prefs_api.h"

#include "apps/switches.h"
#include "base/memory/ptr_util.h"
#include "chrome/browser/profiles/profile.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "components/prefs/pref_service.h"
#include "extensions/browser/event_router.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/browser/extensions_browser_client.h"
#include "prefs/native_settings_observer.h"
#include "prefs/vivaldi_gen_prefs.h"
#include "prefs/vivaldi_pref_names.h"

namespace extensions {

namespace {
std::unique_ptr<base::Value> GetPrefValue(PrefService* prefs,
                                          const std::string& path) {
  return std::make_unique<base::Value>(
      prefs->FindPreference(path)->GetValue()->Clone());
}

std::unique_ptr<base::Value> GetPrefDefaultValue(PrefService* prefs,
                                                 const std::string& path) {
  return std::make_unique<base::Value>(
      prefs->GetDefaultPrefValue(path)->Clone());
}

::vivaldi::PrefProperties* GetPrefProperties(Profile* profile,
                                             const std::string& path,
                                             std::string* error) {
  ::vivaldi::PrefProperties* properties =
      VivaldiPrefsApiNotificationFactory::GetPrefProperties(path);
  // There might be security implications to letting the UI query any and all
  // prefs so, we want to only allow accessing prefs that we know the UI neeeds.
  if (!properties) {
    error->assign(std::string("The pref api is not allowed to access ") + path);
  }
  return properties;
}

std::unique_ptr<base::Value> TranslateEnumValue(
    std::unique_ptr<base::Value> original,
    ::vivaldi::PrefProperties* pref_properties) {
  if (!pref_properties->enum_properties)
    return original;
  if (!original->is_int())
    return base::MakeUnique<base::Value>();
  const auto& result = pref_properties->enum_properties->value_to_string.find(
      original->GetInt());
  if (result == pref_properties->enum_properties->value_to_string.end())
    return base::MakeUnique<base::Value>();
  return base::MakeUnique<base::Value>(result->second);
}
}  // anonymous namespace

/*static*/
void VivaldiPrefsApiNotification::BroadcastEvent(
    const std::string& eventname,
    std::unique_ptr<base::ListValue> args,
    content::BrowserContext* context) {
  std::unique_ptr<extensions::Event> event(
      new extensions::Event(extensions::events::VIVALDI_EXTENSION_EVENT,
                            eventname, std::move(args), context));
  EventRouter* event_router = EventRouter::Get(context);
  if (event_router) {
    event_router->BroadcastEvent(std::move(event));
  }
}

VivaldiPrefsApiNotification::VivaldiPrefsApiNotification(Profile* profile)
    : profile_(profile), weak_ptr_factory_(this) {
  prefs_registrar_.Init(profile->GetPrefs());

  // NOTE(andre@vivaldi.com) : Make sure the ExtensionPrefs has been created in
  // the ExtensionPrefsFactory-map in case another extension changes a setting
  // that we are observing. This could cause a race-condition and hitting a
  // DCHECK. See VB-27642.
  ExtensionPrefs::Get(profile);

  PrefService* prefs = profile_->GetPrefs();

  const base::Value* old_mouse_gesture_pref =
      prefs->GetUserPrefValue(vivaldiprefs::kOldMousegesturesEnabled);
  if (old_mouse_gesture_pref)
    prefs->Set(vivaldiprefs::kMouseGesturesEnabled, *old_mouse_gesture_pref);
  prefs->ClearPref(vivaldiprefs::kOldMousegesturesEnabled);

  native_settings_observer_.reset(
      ::vivaldi::NativeSettingsObserver::Create(profile));
}

// static
::vivaldi::PrefProperties*
VivaldiPrefsApiNotificationFactory::GetPrefProperties(const std::string& path) {
  const auto& item = GetInstance()->prefs_properties_.find(path);
  if (item == GetInstance()->prefs_properties_.end())
    return nullptr;
  return &(item->second);
}

void VivaldiPrefsApiNotification::RegisterPref(const std::string& path) {
  if (prefs_registrar_.IsObserved(path))
    return;

  prefs_registrar_.Add(path, base::Bind(&VivaldiPrefsApiNotification::OnChanged,
                                        weak_ptr_factory_.GetWeakPtr()));
}

void VivaldiPrefsApiNotification::OnChanged(const std::string& path) {
  vivaldi::prefs::PreferenceValue pref_value;
  pref_value.path = path;

  ::vivaldi::PrefProperties* properties =
      VivaldiPrefsApiNotificationFactory::GetPrefProperties(path);
  DCHECK(properties);
  pref_value.value =
      TranslateEnumValue(GetPrefValue(profile_->GetPrefs(), path), properties);

  std::unique_ptr<base::ListValue> args =
      vivaldi::prefs::OnChanged::Create(pref_value);
  BroadcastEvent(vivaldi::prefs::OnChanged::kEventName, std::move(args),
                 profile_);
}

VivaldiPrefsApiNotification::~VivaldiPrefsApiNotification() {}

VivaldiPrefsApiNotification* VivaldiPrefsApiNotificationFactory::GetForProfile(
    Profile* profile) {
  return static_cast<VivaldiPrefsApiNotification*>(
      GetInstance()->GetServiceForBrowserContext(profile, true));
}

VivaldiPrefsApiNotificationFactory*
VivaldiPrefsApiNotificationFactory::GetInstance() {
  return base::Singleton<VivaldiPrefsApiNotificationFactory>::get();
}

VivaldiPrefsApiNotificationFactory::VivaldiPrefsApiNotificationFactory()
    : BrowserContextKeyedServiceFactory(
          "VivaldiPrefsApiNotification",
          BrowserContextDependencyManager::GetInstance()) {}

VivaldiPrefsApiNotificationFactory::~VivaldiPrefsApiNotificationFactory() {}

KeyedService* VivaldiPrefsApiNotificationFactory::BuildServiceInstanceFor(
    content::BrowserContext* profile) const {
  return new VivaldiPrefsApiNotification(static_cast<Profile*>(profile));
}

bool VivaldiPrefsApiNotificationFactory::ServiceIsNULLWhileTesting() const {
  return false;
}

bool VivaldiPrefsApiNotificationFactory::ServiceIsCreatedWithBrowserContext()
    const {
  return true;
}

content::BrowserContext*
VivaldiPrefsApiNotificationFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  // Redirected in incognito.
  return extensions::ExtensionsBrowserClient::Get()->GetOriginalContext(
      context);
}

void VivaldiPrefsApiNotificationFactory::RegisterProfilePrefs(
    user_prefs::PrefRegistrySyncable* registry) {
  // We need to register the old version of prefs for which the name has changed
  // in order rto be able to migrate them
  registry->RegisterBooleanPref(
      vivaldiprefs::kOldMousegesturesEnabled, true,
      user_prefs::PrefRegistrySyncable::SYNCABLE_PREF);

  prefs_properties_ = ::vivaldi::RegisterBrowserPrefs(registry);
}

bool PrefsToggleFunction::RunAsync() {
  std::unique_ptr<vivaldi::prefs::Toggle::Params> params(
      vivaldi::prefs::Toggle::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());
  const std::string& path = params->path;
  if (!GetPrefProperties(GetProfile(), path, &error_))
    return false;

  PrefService* prefs = GetProfile()->GetPrefs();
  if (prefs->FindPreference(path)->GetType() != base::Value::Type::BOOLEAN) {
    error_ = std::string("Cannot toggle a non-boolean pref: ") + path;
    return false;
  }
  bool currentValue = prefs->GetBoolean(path);

  prefs->SetBoolean(path, !currentValue);
  results_ = vivaldi::prefs::Toggle::Results::Create(!currentValue);
  SendResponse(true);
  return true;
}

bool PrefsGetFunction::RunAsync() {
  std::unique_ptr<vivaldi::prefs::Get::Params> params(
      vivaldi::prefs::Get::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());
  const std::string& path = params->path;
  ::vivaldi::PrefProperties* properties =
      GetPrefProperties(GetProfile(), path, &error_);
  if (!properties)
    return false;

  std::unique_ptr<base::Value> value(
      GetPrefValue(GetProfile()->GetPrefs(), path));
  results_ = vivaldi::prefs::Get::Results::Create(
      *TranslateEnumValue(std::move(value), properties));
  SendResponse(true);
  return true;
}

bool PrefsSetFunction::RunAsync() {
  std::unique_ptr<vivaldi::prefs::Set::Params> params(
      vivaldi::prefs::Set::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());

  const std::string& path = params->new_value.path;
  const auto& value = params->new_value.value;
  ::vivaldi::PrefProperties* properties =
      GetPrefProperties(GetProfile(), path, &error_);
  if (!properties)
    return false;

  if (!value.get()) {
    error_ = "Expected new value";
    return false;
  }

  PrefService* prefs = GetProfile()->GetPrefs();

  if (!properties->enum_properties) {
    const PrefService::Preference* pref = prefs->FindPreference(path);
    if (pref->GetType() != value->GetType()) {
      error_ = std::string("Cannot assign a ") +
               base::Value::GetTypeName(value->GetType()) + " value to a " +
               base::Value::GetTypeName(pref->GetType()) +
               " preference: " + path;
      return false;
    }

    prefs->Set(path, *value);
  } else {
    if (!value->is_string()) {
      error_ = std::string("Cannot assign a ") +
               base::Value::GetTypeName(value->GetType()) +
               " value to an enumerated preference: " + path;
      return false;
    }
    const auto& result =
        properties->enum_properties->string_to_value.find(value->GetString());
    if (result == properties->enum_properties->string_to_value.end()) {
      error_ =
          std::string("The value") + value->GetString() +
          "is not part of the accepted values for the enumerated preference:" +
          path;
      return false;
    }
    prefs->Set(path, base::Value(result->second));
  }

  SendResponse(true);
  return true;
}

bool PrefsResetFunction::RunAsync() {
  std::unique_ptr<vivaldi::prefs::Get::Params> params(
      vivaldi::prefs::Get::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());
  const std::string& path = params->path;
  if (!GetPrefProperties(GetProfile(), path, &error_))
    return false;

  PrefService* prefs = GetProfile()->GetPrefs();
  prefs->ClearPref(path);

  std::unique_ptr<base::Value> value(GetPrefValue(prefs, path));
  results_ = vivaldi::prefs::Get::Results::Create(*value.get());
  SendResponse(true);
  return true;
}

bool PrefsGetForCacheFunction::RunAsync() {
  std::unique_ptr<vivaldi::prefs::GetForCache::Params> params(
      vivaldi::prefs::GetForCache::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());

  PrefService* prefs = GetProfile()->GetPrefs();

  std::vector<vivaldi::prefs::PreferenceValueAndDefault> results;

  for (const auto& path : params->paths) {
    vivaldi::prefs::PreferenceValueAndDefault result;
    ::vivaldi::PrefProperties* properties =
        VivaldiPrefsApiNotificationFactory::GetPrefProperties(path);
    if (!properties)
      continue;
    result.path = path;
    result.default_value =
        TranslateEnumValue(GetPrefDefaultValue(prefs, path), properties);
    result.value = TranslateEnumValue(GetPrefValue(prefs, path), properties);
    VivaldiPrefsApiNotificationFactory::GetForProfile(GetProfile())
        ->RegisterPref(path);
    results.push_back(std::move(result));
  }

  results_ = vivaldi::prefs::GetForCache::Results::Create(results);

  SendResponse(true);
  return true;
}

}  // namespace extensions
