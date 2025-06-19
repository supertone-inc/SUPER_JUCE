# JUCE Global Hotkeys 모듈 구현 가이드

이 문서는 `juce_global_hotkeys` 모듈의 기술적 구현 세부사항과 각 플랫폼별 접근 방식을 설명합니다.

## 모듈 구조

```
juce_global_hotkeys/
├── juce_global_hotkeys.h          # 메인 모듈 헤더
├── juce_global_hotkeys.cpp        # 메인 모듈 구현
├── juce_global_hotkeys.mm         # Objective-C++ 구현 (macOS)
├── CMakeLists.txt                  # CMake 설정
├── README.md                       # 사용자 문서
├── IMPLEMENTATION_GUIDE.md         # 이 파일
├── hotkeys/                        # 핵심 클래스들
│   ├── juce_GlobalHotKey.h
│   ├── juce_GlobalHotKey.cpp
│   ├── juce_GlobalHotKeyManager.h
│   ├── juce_GlobalHotKeyManager.cpp
│   ├── juce_KeyCode.h
│   ├── juce_KeyCode.cpp
│   ├── juce_ModifierKeys.h
│   └── juce_ModifierKeys.cpp
├── native/                         # 플랫폼별 구현
│   ├── juce_GlobalHotKey_windows.h
│   ├── juce_GlobalHotKey_windows.cpp
│   ├── juce_GlobalHotKey_mac.h
│   ├── juce_GlobalHotKey_mac.cpp
│   ├── juce_GlobalHotKey_linux.h
│   └── juce_GlobalHotKey_linux.cpp
└── examples/                       # 사용 예제
    └── SimpleExample.h
```

## 아키텍처 개요

### 1. 핵심 클래스 계층구조

```
GlobalHotKey (메인 클래스)
├── KeyCode (키 코드 표현)
├── ModifierKeys (수정자 키 표현)
├── PlatformSpecificData (플랫폼별 구현)
│   ├── Windows: RegisterHotKey API
│   ├── macOS: Carbon + CGEventTap
│   └── Linux: X11 (향후)
└── HotKeyCallback (std::function<void()>)

GlobalHotKeyManager (유틸리티 클래스)
├── 여러 GlobalHotKey 인스턴스 관리
├── 문자열 식별자 기반 관리
└── HotKeyInfo 구조체
```

### 2. 플랫폼별 구현 전략

#### Windows (RegisterHotKey API)
- **API**: `RegisterHotKey()` / `UnregisterHotKey()`
- **메시지 처리**: `WM_HOTKEY` 메시지 수신
- **스레드 안전성**: JUCE MessageManager를 통한 콜백 실행
- **제한사항**: 
  - 다른 앱과 단축키 충돌 시 등록 실패
  - 관리자 권한 앱과의 충돌 가능

```cpp
// Windows 구현 핵심
BOOL RegisterHotKey(HWND hWnd, int id, UINT fsModifiers, UINT vk);
// WM_HOTKEY 메시지 처리 → MessageManager::callAsync()
```

#### macOS (이중 전략)

##### Carbon API (기본)
- **API**: `RegisterEventHotKey()` / `UnregisterEventHotKey()`
- **장점**: 간단, 권한 불필요
- **단점**: macOS Sequoia에서 일부 조합 제한

```cpp
// Carbon 구현 핵심
OSStatus RegisterEventHotKey(UInt32 inHotKeyCode, UInt32 inHotKeyModifiers, 
                            EventHotKeyID inHotKeyID, EventTargetRef inTarget, 
                            OptionBits inOptions, EventHotKeyRef* outRef);
```

##### CGEventTap API (대안)
- **API**: `CGEventTapCreate()` 
- **장점**: 모든 키 조합 지원, 더 강력
- **단점**: Input Monitoring 권한 필요

```cpp
// CGEventTap 구현 핵심  
CFMachPortRef CGEventTapCreate(CGEventTapLocation tap, CGEventTapPlacement place,
                              CGEventTapOptions options, CGEventMask eventsOfInterest,
                              CGEventTapCallBack callback, void* userInfo);
```

#### Linux (향후 구현)
- **API**: X11 `XGrabKey()` / `XUngrabKey()`
- **대안**: Wayland 프로토콜 고려

### 3. 키 코드 매핑

각 플랫폼은 서로 다른 키 코드 체계를 사용하므로, 통일된 `KeyCode` 클래스에서 플랫폼별 변환을 처리합니다.

#### 변환 테이블 예시

| JUCE KeyCode | Windows VK | macOS kVK | 설명 |
|--------------|------------|-----------|------|
| KeyCode::A | VK_A (0x41) | kVK_ANSI_A (0x00) | A 키 |
| KeyCode::Space | VK_SPACE (0x20) | kVK_Space (0x31) | 스페이스 |
| KeyCode::F1 | VK_F1 (0x70) | kVK_F1 (0x7A) | F1 키 |

### 4. 수정자 키 매핑

#### Windows
```cpp
MOD_CONTROL  // Ctrl
MOD_ALT      // Alt  
MOD_SHIFT    // Shift
MOD_WIN      // Windows 키
```

#### macOS Carbon
```cpp
cmdKey       // ⌘ Command (JUCE의 ctrlKey에 매핑)
optionKey    // ⌥ Option (JUCE의 altKey에 매핑)
shiftKey     // ⇧ Shift
controlKey   // ⌃ Control (JUCE의 metaKey에 매핑)
```

#### macOS CGEventTap
```cpp
kCGEventFlagMaskCommand     // ⌘ Command
kCGEventFlagMaskAlternate   // ⌥ Option
kCGEventFlagMaskShift       // ⇧ Shift  
kCGEventFlagMaskControl     // ⌃ Control
```

## 스레드 안전성 및 메시지 처리

모든 플랫폼 구현에서 글로벌 단축키 콜백은 JUCE의 메인 메시지 스레드에서 실행되도록 보장합니다:

```cpp
// 플랫폼별 핸들러에서
MessageManager::callAsync([this]() {
    owner.triggerCallback();  // UI 스레드에서 안전하게 실행
});
```

## 권한 관리 (macOS)

### Input Monitoring 권한 확인
```cpp
bool hasPermissions() {
    return CGPreflightListenEventAccess();
}
```

### 권한 요청
```cpp  
bool requestPermissions() {
    return CGRequestListenEventAccess();
}
```

### Info.plist 설정
```xml
<key>NSAppleEventsUsageDescription</key>
<string>This app needs access to control other applications for global hotkeys.</string>
```

## 오류 처리 및 복구

### 1. 등록 실패 처리
```cpp
bool GlobalHotKey::registerHotKey() {
    // Carbon API 시도
    if (registerWithCarbon()) return true;
    
    // CGEventTap 대안 시도
    if (registerWithCGEventTap()) return true;
    
    // 모든 방법 실패
    return false;
}
```

### 2. 권한 오류 처리
```cpp
if (!GlobalHotKey::hasPermissions()) {
    if (!GlobalHotKey::requestPermissions()) {
        // 사용자에게 수동 설정 안내
        showPermissionInstructions();
    }
}
```

### 3. 충돌 감지
```cpp
if (!hotkey->isRegistered()) {
    // 다른 앱과 충돌 또는 시스템 제한
    showConflictWarning();
}
```

## 성능 최적화

### 1. 지연 초기화
```cpp
// 필요할 때만 플랫폼별 데이터 생성
platformData = std::make_unique<PlatformSpecificData>(*this);
```

### 2. 정적 데이터 공유
```cpp
// 모든 인스턴스가 공유하는 정적 맵
static std::unordered_map<int, PlatformSpecificData*> registeredHotkeys;
```

### 3. 메모리 관리
```cpp
// RAII 패턴으로 자동 리소스 정리
~GlobalHotKey() { unregisterHotKey(); }
```

## 테스트 및 디버깅

### 1. 단위 테스트 포인트
- 키 코드 변환 정확성
- 수정자 키 조합 검증
- 플랫폼별 API 호출 성공/실패
- 메모리 누수 방지

### 2. 통합 테스트
- 실제 키 입력과 콜백 실행
- 여러 단축키 동시 등록
- 앱 간 단축키 충돌 시나리오

### 3. 디버깅 도구
```cpp
#if JUCE_DEBUG
void GlobalHotKey::debugPrint() {
    DBG("HotKey: " + keyCode.toString() + "+" + modifiers.toString() 
        + " registered=" + String(isRegistered()));
}
#endif
```

## 알려진 제한사항

### Windows
- 다른 앱이 먼저 등록한 단축키는 사용 불가
- UAC 상승된 앱과의 충돌 가능
- 일부 시스템 단축키는 가로챌 수 없음

### macOS
- Sequoia에서 Option+Shift 조합 제한
- Input Monitoring 권한 필요 (CGEventTap 사용 시)
- Sandbox 앱에서의 제한

### 공통
- 접근성 앱과의 충돌 가능
- 가상 머신 환경에서의 제한된 동작

## 향후 개선 사항

### 1. Linux 지원 추가
- X11 기반 구현
- Wayland 지원 고려

### 2. 고급 기능
- 단축키 체인 (Emacs 스타일)
- 조건부 활성화 (특정 앱 포커스 시에만)
- 단축키 녹화 및 재생

### 3. 성능 개선
- 배치 등록/해제
- 백그라운드 스레드 최적화

### 4. 사용성 개선
- GUI 단축키 설정 컴포넌트
- 충돌 감지 및 대안 제시
- 자동 권한 가이드

## 기여 가이드

### 1. 코딩 스타일
- JUCE 코딩 표준 준수
- 플랫폼별 조건부 컴파일 사용
- RAII 패턴과 스마트 포인터 활용

### 2. 테스트 요구사항
- 모든 지원 플랫폼에서 테스트
- 메모리 누수 검사 (Valgrind, Address Sanitizer)
- 권한 시나리오 테스트

### 3. 문서화
- API 변경 시 README 업데이트
- 플랫폼별 제한사항 명시
- 예제 코드 제공

이 구현 가이드는 `juce_global_hotkeys` 모듈의 완전한 기술적 개요를 제공합니다. 추가 질문이나 개선 제안이 있으면 언제든지 연락해 주세요. 