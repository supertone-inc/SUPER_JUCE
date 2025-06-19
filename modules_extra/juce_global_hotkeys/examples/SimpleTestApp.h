/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

  ==============================================================================
*/

#pragma once

#include "../juce_global_hotkeys.h"
#include "HotKeyDebuggingGuide.h"

using namespace juce;

//==============================================================================
/**
    Simple test application for global hotkeys.
    
    This demonstrates how to use global hotkeys in a real application
    and provides debugging information.
    
    @tags{GlobalHotkeys}
*/
class GlobalHotKeyTestApp : public JUCEApplication
{
public:
    //==============================================================================
    GlobalHotKeyTestApp() = default;

    const String getApplicationName() override       { return "GlobalHotKey Test"; }
    const String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        // Run diagnostics first
        std::cout << "Starting GlobalHotKey Test Application" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        // Check if debug mode is requested
        if (commandLine.contains("--debug") || commandLine.contains("-d"))
        {
            runGlobalHotKeyDiagnostics();
        }
        
        // Request permissions
        std::cout << "Requesting permissions..." << std::endl;
        if (!GlobalHotKey::requestPermissions())
        {
            std::cout << "⚠️  Failed to get required permissions!" << std::endl;
            std::cout << "Please grant Input Monitoring permission and restart." << std::endl;
        }
        
        // Create main window
        mainWindow = std::make_unique<MainWindow> (getApplicationName());
        
        // Register test hotkeys
        registerTestHotkeys();
    }

    void shutdown() override
    {
        // Unregister hotkeys
        testHotkeys.clear();
        mainWindow = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        // When another instance is started with --debug, run diagnostics
        if (commandLine.contains("--debug") || commandLine.contains("-d"))
        {
            MessageManager::callAsync([]() {
                runGlobalHotKeyDiagnostics();
            });
        }
    }

private:
    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow (String name)
            : DocumentWindow (name,
                            Desktop::getInstance().getDefaultLookAndFeel()
                                                  .findColour (ResizableWindow::backgroundColourId),
                            DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        class MainComponent : public Component
        {
        public:
            MainComponent()
            {
                setSize (600, 400);
                addAndMakeVisible (instructionsLabel);
                addAndMakeVisible (statusLabel);
                addAndMakeVisible (runDiagnosticsButton);
                
                instructionsLabel.setText (
                    "Global HotKey Test Application\n\n"
                    "Registered hotkeys:\n"
                    "• Cmd+Shift+F1 - Show test message\n"
                    "• Cmd+Shift+F2 - Show window\n"
                    "• Cmd+Shift+F3 - Toggle status\n\n"
                    "These hotkeys work even when the app is in the background.\n"
                    "Check the console output for debug information.",
                    dontSendNotification);
                
                instructionsLabel.setJustificationType (Justification::topLeft);
                instructionsLabel.setFont (Font (FontOptions (14.0f)));
                
                statusLabel.setText ("Status: Ready", dontSendNotification);
                statusLabel.setFont (Font (FontOptions (16.0f)));
                statusLabel.setColour (Label::textColourId, Colours::green);
                
                runDiagnosticsButton.setButtonText ("Run Diagnostics");
                runDiagnosticsButton.onClick = [this] { runDiagnostics(); };
            }

            void paint (Graphics& g) override
            {
                g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
            }

            void resized() override
            {
                auto area = getLocalBounds().reduced (20);
                
                statusLabel.setBounds (area.removeFromTop (30));
                area.removeFromTop (10);
                
                runDiagnosticsButton.setBounds (area.removeFromBottom (30));
                area.removeFromBottom (10);
                
                instructionsLabel.setBounds (area);
            }
            
            void updateStatus (const String& message, Colour colour = Colours::blue)
            {
                statusLabel.setText ("Status: " + message, dontSendNotification);
                statusLabel.setColour (Label::textColourId, colour);
                repaint();
            }
            
            void runDiagnostics()
            {
                // Run diagnostics in a separate thread to avoid blocking UI
                Thread::launch ([this]() {
                    std::cout << "\n=== Running Diagnostics from UI ===" << std::endl;
                    runGlobalHotKeyDiagnostics();
                    
                    MessageManager::callAsync ([this]() {
                        updateStatus ("Diagnostics completed - check console", Colours::blue);
                    });
                });
            }

        private:
            Label instructionsLabel;
            Label statusLabel;
            TextButton runDiagnosticsButton;
            
            friend class MainWindow;
        };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };
    
    void registerTestHotkeys()
    {
        std::cout << "Registering test hotkeys..." << std::endl;
        
        // Hotkey 1: Cmd+Shift+F1 - Show test message
        testHotkeys.push_back (std::make_unique<GlobalHotKey> (
            KeyCode::F1,
            ModifierKeys (ModifierKeys::commandModifier | ModifierKeys::shiftModifier),
            [this]() {
                std::cout << "🎉 Hotkey F1 triggered!" << std::endl;
                updateMainWindowStatus ("F1 hotkey triggered!", Colours::green);
            }
        ));
        
        // Hotkey 2: Cmd+Shift+F2 - Show/focus window
        testHotkeys.push_back (std::make_unique<GlobalHotKey> (
            KeyCode::F2,
            ModifierKeys (ModifierKeys::commandModifier | ModifierKeys::shiftModifier),
            [this]() {
                std::cout << "🎉 Hotkey F2 triggered - showing window!" << std::endl;
                MessageManager::callAsync ([this]() {
                    if (mainWindow != nullptr)
                    {
                        mainWindow->toFront (true);
                        mainWindow->setVisible (true);
                        updateMainWindowStatus ("F2 hotkey - window shown!", Colours::orange);
                    }
                });
            }
        ));
        
        // Hotkey 3: Cmd+Shift+F3 - Toggle status
        testHotkeys.push_back (std::make_unique<GlobalHotKey> (
            KeyCode::F3,
            ModifierKeys (ModifierKeys::commandModifier | ModifierKeys::shiftModifier),
            [this]() {
                std::cout << "🎉 Hotkey F3 triggered - toggling status!" << std::endl;
                toggleStatus = !toggleStatus;
                updateMainWindowStatus (toggleStatus ? "Status: ON" : "Status: OFF", 
                                      toggleStatus ? Colours::green : Colours::red);
            }
        ));
        
        // Report registration status
        int registeredCount = 0;
        for (const auto& hotkey : testHotkeys)
        {
            if (hotkey->isRegistered())
                registeredCount++;
        }
        
        std::cout << "Successfully registered " << registeredCount << " out of " 
                  << testHotkeys.size() << " hotkeys." << std::endl;
        
        if (registeredCount == 0)
        {
            std::cout << "⚠️  No hotkeys were registered! Check permissions and conflicts." << std::endl;
            std::cout << "Run with --debug flag for detailed diagnostics." << std::endl;
        }
    }
    
    void updateMainWindowStatus (const String& message, Colour colour)
    {
        MessageManager::callAsync ([this, message, colour]() {
            if (mainWindow != nullptr)
            {
                auto* mainComponent = dynamic_cast<MainWindow::MainComponent*>(
                    mainWindow->getContentComponent());
                if (mainComponent != nullptr)
                {
                    mainComponent->updateStatus (message, colour);
                }
            }
        });
    }

    std::unique_ptr<MainWindow> mainWindow;
    std::vector<std::unique_ptr<GlobalHotKey>> testHotkeys;
    bool toggleStatus = false;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (GlobalHotKeyTestApp) 