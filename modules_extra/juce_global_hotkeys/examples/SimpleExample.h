/*
  ==============================================================================

   Simple Global HotKey Example
   
   이 예제는 JUCE 글로벌 단축키 모듈의 기본 사용법을 보여줍니다.
   
   기능:
   - Ctrl+Shift+G: 윈도우 표시/앞으로 가져오기
   - Ctrl+Shift+H: 윈도우 숨기기
   - Ctrl+Shift+Q: 애플리케이션 종료
   
   macOS에서는 Ctrl 대신 Cmd 키가 사용됩니다.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// juce_global_hotkeys 모듈이 추가되어 있다면 다음 라인의 주석을 해제하세요
// #include <juce_global_hotkeys/juce_global_hotkeys.h>

//==============================================================================
/**
    글로벌 단축키를 사용하는 간단한 JUCE 애플리케이션 예제입니다.
*/
class SimpleGlobalHotKeyApplication : public juce::JUCEApplicationBase
{
public:
    //==============================================================================
    SimpleGlobalHotKeyApplication() = default;

    const juce::String getApplicationName() override       { return "Global HotKey Example"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override             { return false; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // 플랫폼 지원 확인
        /* 
        if (!juce::GlobalHotKey::isSupported())
        {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "플랫폼 미지원",
                "이 플랫폼에서는 글로벌 단축키가 지원되지 않습니다.");
            quit();
            return;
        }
        */

        // macOS에서 권한 확인 및 요청
        /*
        if (!juce::GlobalHotKey::hasPermissions())
        {
            bool granted = juce::GlobalHotKey::requestPermissions();
            if (!granted)
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "권한 필요",
                    "글로벌 단축키 사용을 위해 시스템 환경설정에서 '입력 모니터링' 권한을 허용해주세요.");
            }
        }
        */

        // 메인 윈도우 생성
        mainWindow = std::make_unique<MainWindow>(getApplicationName());

        // 글로벌 단축키 등록
        setupGlobalHotKeys();
    }

    void shutdown() override
    {
        // 리소스 정리 (GlobalHotKey 소멸자가 자동으로 정리함)
        globalHotKeys.clear();
        mainWindow = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // 다른 인스턴스가 시작되면 현재 창을 앞으로 가져오기
        if (mainWindow != nullptr)
        {
            mainWindow->toFront(true);
        }
    }

    //==============================================================================
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name) 
            : DocumentWindow (name,
                            juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                        .findColour(juce::ResizableWindow::backgroundColourId),
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
            // 닫기 버튼을 눌러도 실제로는 숨기기만 함
            setVisible(false);
        }

    private:
        class MainComponent : public juce::Component
        {
        public:
            MainComponent()
            {
                setSize (600, 400);
                
                // 지시사항 텍스트 설정
                instructionsText = "글로벌 단축키 테스트\n\n"
                                 "다음 단축키를 사용해보세요:\n\n"
#if JUCE_MAC
                                 "⌘⇧G: 윈도우 표시/앞으로 가져오기\n"
                                 "⌘⇧H: 윈도우 숨기기\n" 
                                 "⌘⇧Q: 애플리케이션 종료\n\n"
#else
                                 "Ctrl+Shift+G: 윈도우 표시/앞으로 가져오기\n"
                                 "Ctrl+Shift+H: 윈도우 숨기기\n"
                                 "Ctrl+Shift+Q: 애플리케이션 종료\n\n"
#endif
                                 "이 단축키들은 다른 애플리케이션이 포커스를 가지고 있을 때도 작동합니다.";
            }

            void paint (juce::Graphics& g) override
            {
                g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

                g.setColour (juce::Colours::white);
                g.setFont (16.0f);
                g.drawText (instructionsText, getLocalBounds().reduced(20), 
                           juce::Justification::centredTop, true);
            }

        private:
            juce::String instructionsText;
        };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    //==============================================================================
    void setupGlobalHotKeys()
    {
        // 실제 구현에서는 다음 주석을 해제하고 사용하세요:
        
        /*
        // 1. 윈도우 표시/앞으로 가져오기 (Ctrl+Shift+G)
        auto showWindowHotKey = std::make_unique<juce::GlobalHotKey>(
            juce::KeyCode::G,
            juce::ModifierKeys::ctrlKey() | juce::ModifierKeys::shiftKey(),
            [this] { showMainWindow(); }
        );
        
        if (showWindowHotKey->isRegistered())
        {
            globalHotKeys.push_back(std::move(showWindowHotKey));
        }

        // 2. 윈도우 숨기기 (Ctrl+Shift+H)
        auto hideWindowHotKey = std::make_unique<juce::GlobalHotKey>(
            juce::KeyCode::H,
            juce::ModifierKeys::ctrlKey() | juce::ModifierKeys::shiftKey(),
            [this] { hideMainWindow(); }
        );
        
        if (hideWindowHotKey->isRegistered())
        {
            globalHotKeys.push_back(std::move(hideWindowHotKey));
        }

        // 3. 애플리케이션 종료 (Ctrl+Shift+Q)
        auto quitAppHotKey = std::make_unique<juce::GlobalHotKey>(
            juce::KeyCode::Q,
            juce::ModifierKeys::ctrlKey() | juce::ModifierKeys::shiftKey(),
            [this] { quit(); }
        );
        
        if (quitAppHotKey->isRegistered())
        {
            globalHotKeys.push_back(std::move(quitAppHotKey));
        }
        */
    }

    void showMainWindow()
    {
        if (mainWindow != nullptr)
        {
            mainWindow->setVisible(true);
            mainWindow->toFront(true);
            mainWindow->grabKeyboardFocus();
        }
    }

    void hideMainWindow()
    {
        if (mainWindow != nullptr)
        {
            mainWindow->setVisible(false);
        }
    }

    //==============================================================================
    std::unique_ptr<MainWindow> mainWindow;
    // std::vector<std::unique_ptr<juce::GlobalHotKey>> globalHotKeys;
};

//==============================================================================
// 이것은 JUCE 애플리케이션 객체를 생성하는 매크로입니다.
// GUI 앱에서는 Main.cpp 파일에 이 라인을 추가해야 합니다.
// START_JUCE_APPLICATION (SimpleGlobalHotKeyApplication) 