#include "InGameUI/TTitleLayout.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OutGameUI/TUPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

#define LOGPLAY(fmt, ...) UE_LOG(LogTemp, Log, TEXT("[TITLE-PLAY] " fmt), ##__VA_ARGS__)

static bool IsMapPathLike(const FString& S)
{
	// 맨 앞이 '/'거나, "/Game/" 같은 패턴이면 맵 경로로 간주
	return S.StartsWith(TEXT("/")) || S.Contains(TEXT("/Game/"));
}

static FString NormalizeAddr(const FString& Raw, bool& bOutValid)
{
	bOutValid = false;

	FString S = Raw;
	S = S.TrimStartAndEnd();

	// 맵 경로/옵션이 들어온 경우 → 접속주소가 아님
	if (IsMapPathLike(S))
	{
		return TEXT(""); // invalid
	}

	// 옵션 제거
	int32 QIdx; if (S.FindChar('?', QIdx)) S = S.Left(QIdx);

	// 만약 슬래시가 있으면(호스트 뒤에 뭔가 붙은 경우) 거기까지만 사용
	int32 SlashIdx; if (S.FindChar('/', SlashIdx)) S = S.Left(SlashIdx);

	// 공란이면 기본 호스트
	if (S.IsEmpty())
	{
		S = TEXT("127.0.0.1:7777");
		bOutValid = true;
		return S;
	}

	// 호스트:포트 분리
	FString Host, PortStr;
	if (S.Split(TEXT(":"), &Host, &PortStr, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	{
		if (Host.IsEmpty())
		{
			Host = TEXT("127.0.0.1");
		}
		if (PortStr.IsEmpty())
		{
			PortStr = TEXT("7777");
		}
		
		S = Host + TEXT(":") + PortStr;
	}

	else
	{
		// 포트가 없으면 기본 포트
		S += TEXT(":7777");
	}

	// 최소 유효성: 호스트에 알파벳/숫자/점이 하나 이상은 있어야 함
	bool bHasHostChar = false;
	for (int32 i = 0; i < S.Len(); ++i)
	{
		const TCHAR C = S[i];
		if (FChar::IsAlpha(C) || FChar::IsDigit(C) || C == TEXT('.'))
		{
			bHasHostChar = true;
			break;
		}
	}
	if (!bHasHostChar)
	{
		S = TEXT("127.0.0.1:7777");
	}

	bOutValid = true;
	return S;
}

UTTitleLayout::UTTitleLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTTitleLayout::NativeConstruct()
{
	Super::NativeConstruct();
	if (PlayButton) PlayButton->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	if (ExitButton) ExitButton->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UTTitleLayout::OnPlayButtonClicked()
{
	ATUPlayerController* PC = GetOwningPlayer<ATUPlayerController>();
	if (!PC) return;

	UWorld* W = GetWorld();
	if (!W) return;

	const ENetMode NM = W->GetNetMode();

	if (W->WorldType == EWorldType::PIE)
	{
		// 클라이언트 PIE 창이면: 서버에 "로비로 이동"만 요청
		if (NM == NM_Client)
		{
			UE_LOG(LogTemp, Log, TEXT("[TITLE-PLAY][PIE] Client -> Server_RequestEnterLobby()"));
			PC->Server_RequestEnterLobby();
			return;
		}
		// 리슨 서버 PIE 창이면: 로비를 리슨으로 오픈
		if (NM == NM_ListenServer || PC->HasAuthority())
		{
			UE_LOG(LogTemp, Log, TEXT("[TITLE-PLAY][PIE] ListenServer -> ServerTravel Lobby?listen"));
			W->ServerTravel(TEXT("/Game/Team02/OutGameUI/Map/LobbyMap?listen"));
			return;
		}
		// Standalone PIE 같은 예외 상황이면 그냥 종료
		UE_LOG(LogTemp, Log, TEXT("[TITLE-PLAY][PIE] Unknown net mode. Do nothing."));
		return;
	}

	const FString Raw = ServerIPEditableText ? ServerIPEditableText->GetText().ToString() : FString();

	if (Raw.IsEmpty())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
				TEXT("서버 주소를 입력하세요 (예: 127.0.0.1:7777)"));
		}
		LOGPLAY("No address -> not joining");
		return;
	}

	bool bAddrValid = false;
	const FString Addr = NormalizeAddr(Raw, bAddrValid);

	if (!bAddrValid || Addr.IsEmpty())
	{
		LOGPLAY("Invalid address input: '%s' -> abort join", *Raw);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
				TEXT("잘못된 주소입니다. 예) 127.0.0.1:7777"));
		}
		return;
	}

	LOGPLAY("Client Join -> %s", *Addr);
	PC->JoinServer(Addr); // ClientTravel 사용
}


void UTTitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
