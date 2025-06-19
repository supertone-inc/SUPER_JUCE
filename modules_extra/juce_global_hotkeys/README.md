# JUCE Global Hotkeys Module

JUCE 프레임워크를 위한 크로스 플랫폼 글로벌 단축키 모듈입니다. 애플리케이션이 백그라운드에 있을 때도 시스템 전역에서 작동하는 단축키를 등록할 수 있습니다.

## 지원 플랫폼

- **Windows**: RegisterHotKey API 사용
- **macOS**: Carbon RegisterEventHotKey 및 CGEventTap API 지원
- **Linux**: X11 (향후 지원 예정)

## 주요 기능

### 🔥 백그라운드 단축키 지원
애플리케이션이 최소화되어 있거나 다른 앱이 포커스를 가지고 있어도 단축키가 작동합니다.

### 🎯 크로스 플랫폼 호환성
Windows와 macOS에서 동일한 코드로 작동하며, 각 플랫폼의 네이티브 API를 사용합니다.

### 🛡️ 권한 관리
macOS에서 필요한 접근 권한을 자동으로 요청하고 관리합니다.

### 🧩 JUCE 통합
JUCE의 메시지 시스템과 완벽하게 통합되어 UI 스레드에서 안전하게 콜백이 실행됩니다.

## 설치 방법

1. `juce_global_hotkeys` 폴더를 JUCE 프로젝트의 `modules` 디렉토리에 복사합니다.
2. Projucer에서 프로젝트를 열고 모듈을 추가합니다:
   - **Modules** 탭에서 **Add a module** 클릭
   - `juce_global_hotkeys` 선택
3. macOS에서는 다음 권한을 `Info.plist`에 추가해야 할 수 있습니다:
   ```xml
   <key>NSAppleEventsUsageDescription</key>
   <string>This app needs access to control other applications for global hotkeys.</string>
   ```

## 기본 사용법

### 단일 단축키 등록

```cpp
#include <juce_global_hotkeys/juce_global_hotkeys.h>

class MyApplication : public juce::JUCEApplicationBase
{
public:
    void initialise(const juce::String& commandLine) override
    {
        // 권한 요청 (macOS에서만 필요)
        if (!juce::GlobalHotKey::hasPermissions())
        {
            juce::GlobalHotKey::requestPermissions();
        }
        
        // Ctrl+Shift+G 단축키 등록
        globalHotKey = std::make_unique<juce::GlobalHotKey>(
            juce::KeyCode::G,
            juce::ModifierKeys::ctrlKey() | juce::ModifierKeys::shiftKey(),
            [this] { showApplicationWindow(); }
        );
    }
    
private:
    std::unique_ptr<juce::GlobalHotKey> globalHotKey;
    
    void showApplicationWindow()
    {
        // 애플리케이션 창을 앞으로 가져오기
        if (mainWindow != nullptr)
        {
            mainWindow->toFront(true);
            mainWindow->setVisible(true);
        }
    }
};
```

### 다중 단축키 관리

```cpp
class MyApplicationWithManager : public juce::JUCEApplicationBase
{
public:
    void initialise(const juce::String& commandLine) override
    {
        // 권한 요청
        juce::GlobalHotKeyManager::requestPermissions();
        
        // 여러 단축키 등록
        hotKeyManager.registerHotKey("showWindow", 
                                    juce::KeyCode::G,
                                    juce::ModifierKeys::ctrlKey() | juce::ModifierKeys::shiftKey(),
                                    [this] { showWindow(); });
        
        hotKeyManager.registerHotKey("hideWindow",
                                    juce::KeyCode::H, 
                                    juce::ModifierKeys::ctrlKey() | juce::ModifierKeys::shiftKey(),
                                    [this] { hideWindow(); });
        
        hotKeyManager.registerHotKey("toggleWindow",
                                    juce::KeyCode::T,
                                    juce::ModifierKeys::ctrlKey() | juce::ModifierKeys::altKey(),
                                    [this] { toggleWindow(); });
    }
    
private:
    juce::GlobalHotKeyManager hotKeyManager;
    
    void showWindow() { /* 구현 */ }
    void hideWindow() { /* 구현 */ }
    void toggleWindow() { /* 구현 */ }
};
```

## 주요 클래스

### GlobalHotKey
개별 글로벌 단축키를 나타내는 클래스입니다.

```cpp
// 생성자
GlobalHotKey(const KeyCode& keyCode, 
             const ModifierKeys& modifiers,
             std::function<void()> callback);

// 정적 메서드
static bool isSupported();        // 플랫폼 지원 여부
static bool requestPermissions(); // 권한 요청 (macOS)
static bool hasPermissions();     // 권한 확인

// 인스턴스 메서드
bool isRegistered() const;        // 등록 상태 확인
void triggerCallback();           // 수동 콜백 실행 (테스트용)
```

### GlobalHotKeyManager
여러 글로벌 단축키를 관리하는 유틸리티 클래스입니다.

```cpp
// 단축키 등록/해제
bool registerHotKey(const String& identifier, const KeyCode& keyCode, 
                   const ModifierKeys& modifiers, std::function<void()> callback);
bool unregisterHotKey(const String& identifier);
void unregisterAllHotKeys();

// 정보 조회
bool isHotKeyRegistered(const String& identifier) const;
StringArray getRegisteredHotKeyIdentifiers() const;
int getNumRegisteredHotKeys() const;

// 테스트
bool triggerHotKey(const String& identifier);
```

### KeyCode
키보드 키를 나타내는 클래스입니다.

```cpp
// 상수들
KeyCode::A, KeyCode::B, ..., KeyCode::Z     // 알파벳
KeyCode::Key0, KeyCode::Key1, ..., KeyCode::Key9  // 숫자
KeyCode::F1, KeyCode::F2, ..., KeyCode::F12       // 함수키
KeyCode::Space, KeyCode::Return, KeyCode::Escape  // 특수키
```

### ModifierKeys
수정자 키(Ctrl, Alt, Shift 등)를 나타내는 클래스입니다.

```cpp
// 정적 생성 메서드
ModifierKeys::ctrlKey()   // Ctrl (Windows) / Cmd (Mac)
ModifierKeys::altKey()    // Alt / Option
ModifierKeys::shiftKey()  // Shift
ModifierKeys::metaKey()   // Windows / Control (Mac)

// 조합
auto modifiers = ModifierKeys::ctrlKey() | ModifierKeys::shiftKey();
```

## 플랫폼별 고려사항

### Windows
- `RegisterHotKey` API 사용
- 특별한 권한 요구사항 없음
- Windows 키 조합도 지원 (ModifierKeys::metaKey())

### macOS
- **Carbon API**: 간단하지만 macOS Sequoia에서 일부 제한
- **CGEventTap API**: 더 강력하지만 "입력 모니터링" 권한 필요
- 권한 요청이 필수:
  ```cpp
  GlobalHotKey::requestPermissions();  // 권한 대화상자 표시
  ```
- 시스템 환경설정 → 개인정보 보호 및 보안 → 입력 모니터링에서 앱 허용 필요

### Linux (향후 지원)
- X11 API 사용 예정
- Wayland 지원도 고려 중

## 모범 사례

### 1. 애플리케이션 시작 시 권한 요청
```cpp
void initialise(const String& commandLine) override
{
    // 플랫폼 지원 확인
    if (!GlobalHotKey::isSupported())
    {
        // 대체 방안 구현
        return;
    }
    
    // 권한 요청 (macOS에서만 대화상자 표시)
    if (!GlobalHotKey::hasPermissions())
    {
        bool granted = GlobalHotKey::requestPermissions();
        if (!granted)
        {
            // 사용자에게 수동으로 권한 설정 안내
            showPermissionInstructions();
        }
    }
}
```

### 2. 충돌 방지를 위한 고유한 단축키 조합 사용
```cpp
// 좋은 예: 여러 수정자 키 사용
hotKeyManager.registerHotKey("myApp_show", KeyCode::G,
    ModifierKeys::ctrlKey() | ModifierKeys::shiftKey() | ModifierKeys::altKey(),
    callback);

// 피해야 할 예: 일반적인 조합 (다른 앱과 충돌 가능)
hotKeyManager.registerHotKey("show", KeyCode::C,
    ModifierKeys::ctrlKey(),  // Ctrl+C는 복사 단축키
    callback);
```

### 3. 오류 처리
```cpp
auto hotkey = std::make_unique<GlobalHotKey>(keyCode, modifiers, callback);
if (!hotkey->isRegistered())
{
    // 등록 실패 처리
    juce::AlertWindow::showMessageBox(
        juce::AlertWindow::WarningIcon,
        "단축키 등록 실패",
        "글로벌 단축키를 등록할 수 없습니다. 다른 애플리케이션에서 이미 사용 중일 수 있습니다.");
}
```

### 4. 리소스 정리
```cpp
class MyApp : public JUCEApplicationBase
{
public:
    void shutdown() override
    {
        // GlobalHotKey 소멸자가 자동으로 정리하지만, 명시적으로 할 수도 있음
        hotKeyManager.unregisterAllHotKeys();
        globalHotKeys.clear();
    }
    
private:
    GlobalHotKeyManager hotKeyManager;
    std::vector<std::unique_ptr<GlobalHotKey>> globalHotKeys;
};
```

## 문제 해결

### macOS에서 단축키가 작동하지 않는 경우
1. 시스템 환경설정 → 개인정보 보호 및 보안 → 입력 모니터링 확인
2. 애플리케이션이 목록에 있고 체크되어 있는지 확인
3. 애플리케이션 재시작 후 다시 시도

### Windows에서 특정 단축키가 작동하지 않는 경우
1. 다른 애플리케이션에서 같은 단축키를 사용하고 있는지 확인
2. 관리자 권한으로 실행 중인 다른 앱과의 충돌 확인

### 일반적인 문제
- **단축키 충돌**: 시스템이나 다른 앱에서 이미 사용 중인 조합
- **권한 부족**: macOS에서 입력 모니터링 권한 미허용
- **플랫폼 제한**: macOS Sequoia에서 일부 단축키 조합 제한

## 예제 프로젝트

완전한 예제는 `examples/` 폴더에서 확인할 수 있습니다:
- `SimpleGlobalHotkey/`: 기본 사용법 데모
- `MultipleHotkeys/`: 여러 단축키 관리 예제
- `TrayApplication/`: 시스템 트레이 앱과 함께 사용하는 예제

## 기여

버그 리포트, 기능 요청, 풀 리퀘스트를 환영합니다!

## 라이선스

이 모듈은 JUCE 프레임워크와 동일한 라이선스(AGPLv3/Commercial)를 따릅니다. 