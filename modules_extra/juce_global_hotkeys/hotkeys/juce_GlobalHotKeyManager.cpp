/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

//==============================================================================
GlobalHotKeyManager::~GlobalHotKeyManager()
{
    unregisterAllHotKeys();
}

//==============================================================================
bool GlobalHotKeyManager::registerHotKey (const String& identifier,
                                          const KeyCode& keyCode,
                                          const ModifierKeys& modifiers,
                                          std::function<void()> callback)
{
    // 이미 등록된 식별자가 있으면 먼저 해제
    if (isHotKeyRegistered (identifier))
        unregisterHotKey (identifier);
    
    // 새로운 글로벌 단축키 생성
    auto hotkey = std::make_unique<GlobalHotKey> (keyCode, modifiers, callback);
    
    if (hotkey->isRegistered())
    {
        // HotKeyInfo 생성
        HotKeyInfo info;
        info.identifier = identifier;
        info.keyCode = keyCode;
        info.modifiers = modifiers;
        info.callback = callback;
        info.isRegistered = true;
        
        // 저장
        registeredHotKeys[identifier] = std::move (hotkey);
        hotKeyInfos[identifier] = info;
        
        return true;
    }
    
    return false;
}

bool GlobalHotKeyManager::unregisterHotKey (const String& identifier)
{
    auto it = registeredHotKeys.find (identifier);
    if (it != registeredHotKeys.end())
    {
        registeredHotKeys.erase (it);
        hotKeyInfos.erase (identifier);
        return true;
    }
    
    return false;
}

void GlobalHotKeyManager::unregisterAllHotKeys()
{
    registeredHotKeys.clear();
    hotKeyInfos.clear();
}

//==============================================================================
bool GlobalHotKeyManager::isHotKeyRegistered (const String& identifier) const
{
    return registeredHotKeys.find (identifier) != registeredHotKeys.end();
}

StringArray GlobalHotKeyManager::getRegisteredHotKeyIdentifiers() const
{
    StringArray identifiers;
    
    for (const auto& pair : registeredHotKeys)
        identifiers.add (pair.first);
    
    return identifiers;
}

int GlobalHotKeyManager::getNumRegisteredHotKeys() const
{
    return static_cast<int> (registeredHotKeys.size());
}

//==============================================================================
bool GlobalHotKeyManager::triggerHotKey (const String& identifier)
{
    auto it = registeredHotKeys.find (identifier);
    if (it != registeredHotKeys.end())
    {
        it->second->triggerCallback();
        return true;
    }
    
    return false;
}

//==============================================================================
bool GlobalHotKeyManager::isSupported()
{
    return GlobalHotKey::isSupported();
}

bool GlobalHotKeyManager::requestPermissions()
{
    return GlobalHotKey::requestPermissions();
}

bool GlobalHotKeyManager::hasPermissions()
{
    return GlobalHotKey::hasPermissions();
}

//==============================================================================
const GlobalHotKeyManager::HotKeyInfo* GlobalHotKeyManager::getHotKeyInfo (const String& identifier) const
{
    auto it = hotKeyInfos.find (identifier);
    return it != hotKeyInfos.end() ? &it->second : nullptr;
}

Array<GlobalHotKeyManager::HotKeyInfo> GlobalHotKeyManager::getAllHotKeyInfo() const
{
    Array<HotKeyInfo> result;
    
    for (const auto& pair : hotKeyInfos)
        result.add (pair.second);
    
    return result;
}

//==============================================================================
String GlobalHotKeyManager::HotKeyInfo::toString() const
{
    String result = identifier + ": ";
    
    if (modifiers.isAnyModifierKeyDown())
    {
        result += "Cmd";
        result += "+";
    }
    
    result += keyCode.toString();
    
    if (!isRegistered)
        result += " (NOT REGISTERED)";
    
    return result;
}

} // namespace juce 
