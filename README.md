<img width="849" height="477" alt="image" src="https://github.com/user-attachments/assets/f3b35a26-b4a4-4f2a-8bb7-241b94bdfbc5" />


---
## 📖 프로젝트 소개

### 프로젝트 이름
**경찰과 도둑 (Police & Thief)**

### 프로젝트 개요
본 프로젝트는 **경찰과 도둑**이라는 친숙한 컨셉을 기반으로 한 **1:1 실시간 멀티플레이 대전 게임**입니다.  
플레이어는 경찰 또는 도둑이 되어 **180초**의 제한 시간 내에 상대 팀을 제압하거나 특정 조건을 달성하여 승리하는 것을 목표로 합니다.
이 프로젝트를 통해 **Unreal Engine 5 C++ 기반 멀티플레이 게임 개발**, **AI 시스템 구현**, **네트워킹 동기화** 등의 실무 역량을 습득하고자 하였습니다.

### 주요 기능 요약
-  **실시간 라운드 기반 게임 시스템** (180초 제한)
-  **팀별 고유 승리 조건** (경찰: 도둑 사망 / 도둑: 시간 생존)
-  **전략적 기믹 시스템** (스피드업, 벨, 바인드)
-  **AI NPC 성격 시스템** (여유로움/보통/조급함)
-  **멀티플레이 네트워킹** (Listen Server, Replication)
-  **직관적인 UI/UX** (타이머, 점수, 킬로그, 스태미너 바)
-  **SeamlessTravel 기반 맵 전환**

### 사용 기술 및 라이브러리
- **Unreal Engine 5.5.4** (게임 엔진)
- **C++17** (게임 로직 구현)
- **Blueprint** (UI 및 일부 비주얼 로직)
- **UE5 Gameplay Framework** (GameMode, GameState, PlayerState, Character)
- **UE5 Replication System** (멀티플레이 동기화)
- **Behavior Tree & Blackboard** (AI 의사결정)
- **Enhanced Input System** (입력 처리)
- **UMG (Unreal Motion Graphics)** (UI 시스템)

- ---
## 📁 프로젝트 구조
```
PoliceAndThief/
├── Content/                          # 게임 콘텐츠 (에셋, 블루프린트)
│   ├── Blueprints/                   # 블루프린트 에셋
│   │   ├── Characters/               # 캐릭터 블루프린트
│   │   ├── UI/                       # UI 위젯 블루프린트
│   │   └── Gimmicks/                 # 기믹 블루프린트
│   ├── Maps/                         # 게임 맵
│   │   ├── TitleMap.umap             # 타이틀 화면
│   │   ├── LobbyMap.umap             # 로비 맵
│   │   └── InGameMap.umap            # 인게임 맵
│   ├── UI/                           # UI 위젯
│   │   ├── WBP_TitleLayout.uasset    # 타이틀 UI
│   │   ├── WBP_LobbyWidget.uasset    # 로비 UI
│   │   ├── WBP_InGameHUD.uasset      # 인게임 HUD
│   │   └── WBP_GameResult.uasset     # 결과 화면 UI
│   └── Materials/                    # 머티리얼 및 텍스처
│
├── Source/                           # C++ 소스 코드
│   ├── PoliceAndThief/               # 메인 모듈
│   │   ├── PoliceAndThief.Build.cs   # 빌드 설정
│   │   ├── PoliceAndThief.cpp        # 모듈 진입점
│   │   ├── PoliceAndThief.h          # 모듈 헤더
│   │   │
│   │   ├── Character/                # 캐릭터 시스템
│   │   │   ├── TCharacter.h          # 플레이어 캐릭터 헤더
│   │   │   ├── TCharacter.cpp        # 플레이어 캐릭터 구현
│   │   │   ├── TNpcCharacter.h       # NPC 캐릭터 헤더
│   │   │   └── TNpcCharacter.cpp     # NPC 캐릭터 구현
│   │   │
│   │   ├── Player/                   # 플레이어 관련
│   │   │   ├── TPlayerController.h   # 기본 플레이어 컨트롤러
│   │   │   ├── TPlayerController.cpp
│   │   │   ├── TInGamePlayerController.h   # 인게임 플레이어 컨트롤러
│   │   │   ├── TInGamePlayerController.cpp
│   │   │   ├── TPlayerState_InGame.h       # 인게임 플레이어 상태
│   │   │   └── TPlayerState_InGame.cpp
│   │   │
│   │   ├── GameMode/                 # 게임 모드 시스템
│   │   │   ├── TGameModeBase_InGame.h      # 인게임 게임 모드
│   │   │   ├── TGameModeBase_InGame.cpp
│   │   │   ├── TGameStateBase_InGame.h     # 인게임 게임 스테이트
│   │   │   └── TGameStateBase_InGame.cpp
│   │   │
│   │   ├── AI/                       # AI 시스템
│   │   │   ├── TNpcController.h      # AI 컨트롤러
│   │   │   ├── TNpcController.cpp
│   │   │   ├── BTTask_TGetEndPatrolPosition.h   # 순찰 위치 결정 태스크
│   │   │   ├── BTTask_TGetEndPatrolPosition.cpp
│   │   │   ├── BTTask_TPersonalityWait.h        # 성격별 대기 태스크
│   │   │   └── BTTask_TPersonalityWait.cpp
│   │   │
│   │   ├── Gimmick/                  # 기믹 시스템
│   │   │   ├── TSpeedup.h            # 스피드업 기믹
│   │   │   ├── TSpeedup.cpp
│   │   │   ├── TBell.h               # 벨 기믹 (경찰)
│   │   │   ├── TBell.cpp
│   │   │   ├── TBind.h               # 바인드 기믹 (도둑)
│   │   │   └── TBind.cpp
│   │   │
│   │   └── UI/                       # UI 시스템
│   │       ├── TInGameHUD.h          # 인게임 HUD
│   │       ├── TInGameHUD.cpp
│   │       ├── TTitleLayout.h        # 타이틀 UI
│   │       └── TTitleLayout.cpp
│   │
│   └── PoliceAndThief.Target.cs      # 타겟 빌드 설정
│
├── Config/                           # 설정 파일
│   ├── DefaultEngine.ini             # 엔진 설정
│   ├── DefaultGame.ini               # 게임 설정
│   └── DefaultInput.ini              # 입력 설정
│
├── Plugins/                          # 플러그인 (사용하지 않음)
│
├── Docs/                             # 문서 및 이미지
│   ├── images/                       # README용 이미지
│   └── presentation.pdf              # 발표 자료
│
├── PoliceAndThief.uproject           # 언리얼 프로젝트 파일
└── README.md                         # 프로젝트 설명서 (본 문서)
```

## 🧩 핵심 로직/클래스 설명

### 1️⃣ 캐릭터 시스템 (`TCharacter`, `TNpcCharacter`)

#### **TCharacter** (플레이어 캐릭터)
```cpp
// 주요 기능
- PerformAttack(): 공격 실행 및 스태미너 소비
- TakeDamage(): 피해 처리 및 사망 로직
- UpdateStamina(): 스태미너 자동 회복
```

**핵심 구현 내용:**
- **스태미너 관리**: 공격 시 스태미너 소비, 시간에 따라 자동 회복
- **네트워크 동기화**: `Replicated` 속성으로 체력/스태미너 동기화
- **공격 판정**: 공격 성공 시 GameMode에 킬로그 전송

#### **TNpcCharacter** (AI 캐릭터)
```cpp
// 주요 기능
- BeginPlay(): 초기 성격(Personality) 설정
- Die(): 사망 처리 및 리스폰 로직 연동
- 성격 타입: Relaxed(여유로움), Normal(보통), Hasty(조급함)
```

**핵심 구현 내용:**
- **성격 시스템**: 랜덤 성격 부여 → Blackboard의 `PersonalityType`에 저장
- **동작 차별화**: 성격에 따라 대기 시간 변화 (5초 / 3초 / 1.5초)
- **AI Controller 연동**: Behavior Tree를 통한 순찰 및 대기 로직

---

### 2️⃣ 게임 모드 시스템 (`TGameModeBase_InGame`, `TGameStateBase_InGame`)

#### **TGameModeBase_InGame** (게임 규칙 관리)
```cpp
// 주요 함수
- TryStartRoundIfReady(): 라운드 시작 조건 확인 (모든 플레이어 준비 완료)
- StartRound(): 라운드 시작 및 180초 타이머 시작
- EndRoundByTeamWin(): 라운드 종료 및 점수 반영
- RespawnAllPlayers(): 사망한 플레이어만 리스폰
- EndMatchAndShowResult(): 매치 종료 및 결과 UI 표시
```

**핵심 구현 내용:**
- **라운드 관리**: 180초 타이머 관리 및 승리 조건 판정
- **점수 시스템**: 팀별 점수 누적 (●○○ 형태의 UI 반영)
- **리스폰 로직**: 사망자만 선택적으로 리스폰하여 효율성 향상

**특별히 고민한 부분:**
```cpp
// 문제: 라운드 종료 시 모든 플레이어가 리스폰되는 문제
// 해결: 사망 상태 체크 후 선택적 리스폰
void ATGameModeBase_InGame::RespawnAllPlayers()
{
    for (APlayerState* PS : GameState->PlayerArray)
    {
        if (ATPlayerState_InGame* TPS = Cast(PS))
        {
            if (TPS->bIsDead)  // 사망한 플레이어만 리스폰
            {
                RespawnPlayer(TPS);
            }
        }
    }
}
```

#### **TGameStateBase_InGame** (게임 상태 저장)
```cpp
// 주요 변수
- RemainingTime: 라운드 남은 시간 (Replicated)
- PoliceScore, ThiefScore: 팀별 점수 (ReplicatedUsing)
```

**핵심 구현 내용:**
- **RepNotify**: 점수 변경 시 클라이언트 UI 자동 갱신
- **타이머 동기화**: 서버 타이머를 클라이언트에 실시간 반영

---

### 3️⃣ AI 시스템 (`TNpcController`, Custom BTTask`)

#### **TNpcController** (AI 컨트롤러)
```cpp
// 주요 기능
- OnPossess(): Behavior Tree 실행 및 Blackboard 초기화
- UpdatePerceptionInfo(): 지각 정보 갱신 (시야, 청각)
```

#### **BTTask_TPersonalityWait** (성격별 대기 시간 계산)
```cpp
// ExecuteTask 로직
EBTNodeResult::Type UBTTask_TPersonalityWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EPersonalityType Type = GetPersonalityType(OwnerComp);
    
    float WaitTime = 3.0f;  // 기본값
    switch(Type)
    {
        case EPersonalityType::Relaxed: WaitTime = 5.0f; break;
        case EPersonalityType::Normal:  WaitTime = 3.0f; break;
        case EPersonalityType::Hasty:   WaitTime = 1.5f; break;
    }
    
    OwnerComp.GetBlackboardComponent()->SetValueAsFloat("WaitTime", WaitTime);
    return EBTNodeResult::Succeeded;
}
```

**특별히 고민한 부분:**
> **문제**: 모든 NPC가 동일한 패턴으로 행동 → 도둑 플레이어가 NPC 흉내를 내기 어려움  
> **해결**: 3가지 성격 타입 도입 → 도둑 플레이어가 선택적으로 모방 가능 → 게임 밸런스 향상

---

### 4️⃣ 기믹 시스템 (`TSpeedup`, `TBell`, `TBind`)

#### **TSpeedup** (공용 가속 기믹)
```cpp
// 기능: 3초간 이동 속도 1.5배 증가
void ATSpeedup::ActivateGimmick(ACharacter* Character)
{
    UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
    Movement->MaxWalkSpeed *= 1.5f;
    
    // 3초 후 원래 속도로 복귀
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATSpeedup::DeactivateGimmick, 3.0f);
}
```

#### **TBell** (경찰 전용 탐지 기믹)
```cpp
// 기능: 일정 범위 내 도둑 위치 감지 (소리 효과)
void ATBell::ActivateGimmick(ACharacter* Character)
{
    TArray OverlappingActors;
    GetOverlappingActors(OverlappingActors, ATCharacter::StaticClass());
    
    for (AActor* Actor : OverlappingActors)
    {
        if (Actor->ActorHasTag("Thief"))
        {
            PlayDetectionSound();  // 소리 재생으로 위치 암시
        }
    }
}
```

#### **TBind** (도둑 전용 속박 기믹)
```cpp
// 기능: 2초간 경찰 이동 불가
void ATBind::ActivateGimmick(ACharacter* Character)
{
    if (Character->ActorHasTag("Police"))
    {
        Character->GetCharacterMovement()->DisableMovement();
        
        // 2초 후 이동 재개
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Character]()
        {
            Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        }, 2.0f, false);
    }
}
```

**특별히 고민한 부분:**
> **문제**: 기믹 액터 소환 시 캐릭터에 스킬이 적용되지 않음  
> **해결**: 태그 시스템을 BeginPlay가 아닌 Possess 시점에 적용하도록 변경

---

### 5️⃣ 네트워킹 시스템

#### **Replication 전략**
```cpp
// TPlayerState_InGame.h
UPROPERTY(ReplicatedUsing = OnRep_TeamChanged)
ETeamType Team;

UPROPERTY(ReplicatedUsing = OnRep_ReadyStateChanged)
bool bIsReady;

// 변경 사항 클라이언트 자동 반영
void OnRep_TeamChanged();
void OnRep_ReadyStateChanged();
```

#### **SeamlessTravel 구현**
```cpp
// 로비 → 인게임 맵 전환 시 플레이어 정보 유지
void APlayerState::CopyProperties(APlayerState* NewPlayerState)
{
    Super::CopyProperties(NewPlayerState);
    
    ATPlayerState_InGame* OldPS = Cast(this);
    ATPlayerState_InGame* NewPS = Cast(NewPlayerState);
    
    if (OldPS && NewPS)
    {
        NewPS->Team = OldPS->Team;       // 팀 정보 복사
        NewPS->bIsReady = OldPS->bIsReady;  // 준비 상태 복사
    }
}
```

## 🎯 과제 추가 구현 사항

### 1️⃣ AI 성격 시스템 (독창적 구현)

#### 구현 동기
- 기본 요구사항은 단순 AI 순찰이었으나, 게임플레이 깊이 향상을 위해 성격 시스템 도입
- 도둑 플레이어가 NPC 흉내를 내기 어렵다는 피드백 반영

#### 구현 방법
1. **3가지 성격 타입 정의**
```cpp
   enum class EPersonalityType : uint8
   {
       Relaxed,   // 여유로움: 5초 대기
       Normal,    // 보통: 3초 대기
       Hasty      // 조급함: 1.5초 대기
   };
```

2. **Blackboard 연동**
   - `PersonalityType` 키에 랜덤 성격 저장
   - `WaitTime` 키에 성격별 대기 시간 저장

3. **Behavior Tree 활용**
   - Custom BTTask로 성격별 대기 시간 계산
   - Unreal 기본 Wait 노드로 실제 대기 수행

#### 결과
- 도둑 플레이어가 3가지 행동 패턴 중 하나를 선택적으로 모방 가능
- 경찰 플레이어의 NPC/플레이어 구분 난이도 증가 → 게임 밸런스 향상

---

### 2️⃣ 선택적 리스폰 시스템

#### 구현 동기
- 기존: 라운드 종료 시 모든 플레이어 리스폰 → 불필요한 처리 발생
- 개선: 사망한 플레이어만 리스폰하여 효율성 향상

#### 구현 방법
```cpp
void ATGameModeBase_InGame::RespawnAllPlayers()
{
    for (APlayerState* PS : GameState->PlayerArray)
    {
        ATPlayerState_InGame* TPS = Cast(PS);
        if (TPS && TPS->bIsDead)  // 사망자만 체크
        {
            // 리스폰 위치 결정
            AActor* SpawnPoint = FindPlayerStart(TPS->GetPlayerController());
            
            // 새 캐릭터 스폰
            ACharacter* NewCharacter = GetWorld()->SpawnActor(
                DefaultPawnClass, 
                SpawnPoint->GetActorLocation(), 
                SpawnPoint->GetActorRotation()
            );
            
            // 컨트롤러 빙의
            TPS->GetPlayerController()->Possess(NewCharacter);
            TPS->bIsDead = false;
        }
    }
}
```

#### 결과
- 서버 부하 감소 및 코드 최적화
- 라운드 전환 속도 개선

---

### 3️⃣ UI 반응형 시스템

#### 구현 동기
- 다양한 해상도에서 UI가 짤리거나 깨지는 문제 발생
- 모든 클라이언트 환경에서 일관된 UI 제공 필요

#### 구현 방법
1. **Safe Zone 적용**: 모바일/콘솔 호환성 확보
2. **Scale Box**: 해상도에 따라 UI 자동 크기 조정
3. **Size Box**: 최소/최대 크기 제한 설정
```
WBP_InGameHUD
└─ Safe Zone Slot
   └─ Scale Box (Scale to Fit)
      └─ Size Box (Min: 1280x720, Max: 1920x1080)
         └─ Canvas Panel
            ├─ Timer Widget
            ├─ Score Widget
            └─ Stamina Bar
```

#### 결과
- 모든 해상도에서 UI 정상 표시
- 16:9, 21:9 비율 모두 지원

---

## 🎮 게임 플레이 가이드

### 조작법
| 키 | 기능 |
|------|------|
| **W/A/S/D** | 이동 |
| **마우스** | 시점 회전 |
| **좌클릭** | 공격 |
| **E** | 기믹 사용 |
| **Shift** | 달리기 |
| **ESC** | 메뉴 |

### 팀별 전략 팁

#### 경찰 팀 🚔
- **벨(Bell) 기믹 활용**: 도둑 위치 파악 후 추격
- **좁은 길목 활용**: 도둑의 도주로 차단
- **NPC 관찰**: 부자연스러운 움직임 포착

#### 도둑 팀 🏃
- **NPC 흉내**: AI 성격 타입 중 하나를 모방하여 위장
- **바인드(Bind) 기믹 활용**: 위기 상황에서 경찰 속박 후 도주
- **시간 관리**: 180초 생존이 목표이므로 무리한 전투 자제

---

## 👥 팀 소개

### Team2 CH4 Project 멤버

<table>
  <tr>
    <th>이름</th>
    <th>역할</th>
    <th>담당 파트</th>
    <th>주요 구현 내용</th>
  </tr>
  <tr>
    <td><b>신동하</b></td>
    <td>팀장</td>
    <td>OutGameUI<br>패키징<br>코드 리뷰</td>
    <td>
      • 타이틀/로비 UI 구현<br>
      • 최종 빌드 및 패키징<br>
      • 전체 코드 품질 관리
    </td>
  </tr>
  <tr>
    <td><b>김예준</b></td>
    <td>부팀장</td>
    <td>NPC (AI 시스템)</td>
    <td>
      • AI 성격 시스템 설계<br>
      • Behavior Tree 구현<br>
      • Custom BTTask 개발
    </td>
  </tr>
  <tr>
    <td><b>안성호</b></td>
    <td>팀원</td>
    <td>Level Design</td>
    <td>
      • 인게임 맵 디자인<br>
      • 밸런스 조정 (구역 차단)<br>
      • 스폰 포인트 배치
    </td>
  </tr>
  <tr>
    <td><b>나영진</b></td>
    <td>팀원</td>
    <td>Gimmick</td>
    <td>
      • 기믹 시스템 설계<br>
      • 스피드업/벨/바인드 구현<br>
      • 태그 시스템 문제 해결
    </td>
  </tr>
  <tr>
    <td><b>김성빈</b></td>
    <td>팀원</td>
    <td>Character</td>
    <td>
      • TCharacter 클래스 구현<br>
      • 공격/스태미너 시스템<br>
      • 네트워크 동기화
    </td>
  </tr>
  <tr>
    <td><b>조한형</b></td>
    <td>팀원</td>
    <td>InGameUI<br>PPT 준비</td>
    <td>
      • 인게임 HUD 구현<br>
      • RepNotify UI 갱신 로직<br>
      • 발표 자료 제작
    </td>
  </tr>
</table>

---

## 📊 기술 트러블슈팅 요약

### 1. NPC 패턴 획일화 문제
- **원인**: 모든 NPC가 동일한 대기 시간으로 순찰
- **해결**: 3가지 성격 타입 도입 (5초/3초/1.5초)
- **결과**: 게임플레이 다양성 증가, 도둑 플레이어의 위장 전략 가능

### 2. 스킬 발동 오류
- **원인**: 블루프린트에 스킬 카테고리 설정이 필요한데 if문으로 분기 처리
- **해결**: 불필요한 if문 제거, 블루프린트 카테고리 설정으로 해결
- **결과**: 코드 간결화 및 스킬 정상 작동

### 3. 플레이어 리스폰 로직 문제
- **원인**: 라운드 종료 시 모든 플레이어 리스폰
- **해결**: `bIsDead` 플래그로 사망자만 선택적 리스폰
- **결과**: 서버 부하 감소 및 효율성 향상

### 4. 패키징 오류 (서버 미접속)
- **원인**: 타이틀 맵에서 로비로 이동 시 `?Listen` 파라미터 추가
- **해결**: Listen 파라미터 제거, ClientTravel 방식 변경
- **결과**: 패키징 후 정상 멀티플레이 가능

### 5. UI 짤림 현상
- **원인**: 고정 크기 UI가 해상도 변경 시 화면 밖으로 나감
- **해결**: Safe Zone + Scale Box + Size Box 조합
- **결과**: 모든 해상도에서 UI 정상 표시

### 6. 기믹 액터 적용 오류
- **원인**: 캐릭터 태그가 BeginPlay 시점에 부여되지 않음
- **해결**: Possess 시점으로 태그 부여 시점 변경
- **결과**: 기믹 정상 작동

---
