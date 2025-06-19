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


/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.md file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 juce_global_hotkeys
  vendor:             juce
  version:            8.0.4
  name:               JUCE global hotkey handling classes
  description:        Classes for registering and handling global system-wide keyboard shortcuts that work even when the application is in the background.
  website:            http://www.juce.com/juce
  license:            AGPLv3/Commercial
  minimumCppStandard: 17

  dependencies:       juce_core juce_events
  OSXFrameworks:      Carbon IOKit ApplicationServices CoreServices
  linuxLibs:          X11 Xtst

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


#pragma once
#define JUCE_GLOBAL_HOTKEYS_H_INCLUDED

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/** Config: JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API
    On macOS, enables the use of Carbon RegisterEventHotKey API in addition to
    or instead of CGEventTap. This is simpler and requires fewer permissions,
    but has some limitations on modern macOS versions.
*/
#ifndef JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API
 #define JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API 1
#endif

/** Config: JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
    On macOS, enables the use of CGEventTap API for global hotkeys. This requires
    Input Monitoring permissions but provides more flexibility.
*/
#ifndef JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
 #define JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API 1
#endif

/** Config: JUCE_GLOBAL_HOTKEYS_SHOW_PERMISSION_DIALOG
    On macOS, automatically shows permission request dialogs when needed.
*/
#ifndef JUCE_GLOBAL_HOTKEYS_SHOW_PERMISSION_DIALOG
 #define JUCE_GLOBAL_HOTKEYS_SHOW_PERMISSION_DIALOG 1
#endif

/** Config: JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING
    Enables detailed debug logging for global hotkey registration and events.
    Set to 1 to enable debug output to help troubleshoot hotkey issues.
*/
#ifndef JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING
 #define JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING 1
#endif

//==============================================================================
// Platform-specific includes
#if JUCE_WINDOWS
 #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
 #endif
 #include <windows.h>
#endif

#if JUCE_MAC
 #if JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API
  #include <Carbon/Carbon.h>
 #endif
 #if JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
  #include <ApplicationServices/ApplicationServices.h>
  #include <CoreFoundation/CoreFoundation.h>
 #endif
#endif

#if JUCE_LINUX
 #include <X11/Xlib.h>
 #include <X11/extensions/XTest.h>
 #include <X11/keysym.h>
#endif

//==============================================================================
#include "hotkeys/juce_KeyCode.h"
#include "hotkeys/juce_GlobalHotKey.h"
#include "hotkeys/juce_GlobalHotKeyManager.h"

//==============================================================================
// Platform-specific implementations
#if JUCE_WINDOWS
 #include "native/juce_GlobalHotKey_windows.h"
#endif

#if JUCE_MAC
 #include "native/juce_GlobalHotKey_mac.h"
#endif

#if JUCE_LINUX
 #include "native/juce_GlobalHotKey_linux.h"
#endif 
