#pragma once

#include <Containers/Ticker.h>
#include <CoreMinimal.h>
#include <Engine/GameInstance.h>
#include <Interfaces/OnlineExternalUIInterface.h>
#include <OnlineSubsystemTypes.h>
#include <UObject/TextProperty.h>

#include "GBFGameInstance.generated.h"

class UGBFGameState;
class UGameBaseFrameworkSettings;
class USoundMix;

UCLASS()
class GAMEBASEFRAMEWORK_API UGBFGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UGBFGameInstance();

    void Init() override;
    void Shutdown() override;
    class AGameModeBase * CreateGameModeForURL( FURL url ) override;

    bool Tick( float delta_seconds );

    UFUNCTION( BlueprintCallable )
    void PushSoundMixModifier() const;

    UFUNCTION( BlueprintCallable )
    void PopSoundMixModifier() const;

    UFUNCTION( BlueprintCallable )
    bool ProfileUISwap( const int controller_index );

    bool ShowLoginUI( const int controller_index, const FOnLoginUIClosedDelegate & delegate = FOnLoginUIClosedDelegate() );

    ULocalPlayer * GetFirstLocalPlayer() const;

private:
    void HandleAppWillDeactivate();
    void HandleAppHasReactivated();
    void HandleAppWillEnterBackground();
    void HandleAppHasEnteredForeground();
    void HandleAppDeactivateOrBackground() const;
    void HandleAppReactivateOrForeground();
    void HandleSafeFrameChanged();
    void HandleAppLicenseUpdate();
    void HandleUserLoginChanged( const int32 game_user_index, const ELoginStatus::Type previous_login_status, const ELoginStatus::Type login_status, const FUniqueNetId & user_id );
    void HandleControllerPairingChanged( const int game_user_index, const FUniqueNetId & previous_user, const FUniqueNetId & new_user );
    void HandleNetworkConnectionStatusChanged( const FString & service_name, const EOnlineServerConnectionStatus::Type last_connection_status, const EOnlineServerConnectionStatus::Type connection_status );
    void HandleControllerConnectionChange( const bool is_connection, const int32 unused, const int32 game_user_index );
    void HandleSignInChangeMessaging();
    void ShowMessageThenGotoState( const FText & title, const FText & content, UGBFGameState * next_state );
    void OnLoginUIClosed( const TSharedPtr< const FUniqueNetId > unique_id, int controller_index, const FOnlineError & error );

    UFUNCTION()
    void OnGameStateChanged( const UGBFGameState * new_state );

    UPROPERTY()
    const UGameBaseFrameworkSettings * Settings;

    UPROPERTY( EditDefaultsOnly )
    TSoftObjectPtr< USoundMix > SoundMix;

    EOnlineServerConnectionStatus::Type CurrentConnectionStatus;
    bool IsLicensed;
    int IgnorePairingChangeForControllerId;
    FTickerDelegate TickDelegate;
    FDelegateHandle TickDelegateHandle;
    TSharedPtr< const FUniqueNetId > CurrentUniqueNetId;
    FOnLoginUIClosedDelegate LoginUIClosedDelegate;
    TArray< ELoginStatus::Type > LocalPlayerOnlineStatus;
};