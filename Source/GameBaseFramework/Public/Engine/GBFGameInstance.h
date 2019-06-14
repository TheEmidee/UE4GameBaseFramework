#pragma once

#include <CoreMinimal.h>
#include <Engine/GameInstance.h>
#include <Interfaces/OnlineExternalUIInterface.h>
#include <OnlineSubsystemTypes.h>
#include <TextProperty.h>
#include <Ticker.h>

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

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnStateChangedEvent, const UGBFGameState *, new_state );

    FORCEINLINE FOnStateChangedEvent & OnStateChanged()
    {
        return OnStateChangedEvent;
    }

    void Init() override;
    void Shutdown() override;
    class AGameModeBase * CreateGameModeForURL( FURL InURL ) override;

    bool IsOnWelcomeScreenState() const;

    bool Tick( float delta_seconds );

    UFUNCTION( BlueprintCallable )
    void GoToWelcomeScreenState();

    UFUNCTION( BlueprintCallable )
    void GoToState( UGBFGameState * game_state );

    UFUNCTION( BlueprintCallable )
    void PushSoundMixModifier() const;

    UFUNCTION( BlueprintCallable )
    void PopSoundMixModifier() const;

    UFUNCTION( BlueprintCallable )
    bool ProfileUISwap( const int controller_index );

    bool ShowLoginUI( const int controller_index, const FOnLoginUIClosedDelegate & delegate = FOnLoginUIClosedDelegate() );

    UFUNCTION( BlueprintCallable )
    void SetPresenceForLocalPlayer( const FText & status ) const;

    ULocalPlayer * GetFirstLocalPlayer() const;

private:
    const UGBFGameState * GetGameStateFromGameMode( const TSubclassOf< AGameModeBase > & game_mode_class ) const;
    const UGBFGameState * GetGameStateFromName( FName state_name ) const;
    bool IsStateWelcomeScreenState( const UGBFGameState * state ) const;

    void LoadGameStates() const;
    void HandleAppWillDeactivate();
    void HandleAppHasReactivated();
    void HandleAppWillEnterBackground();
    void HandleAppHasEnteredForeground();
    void HandleAppDeactivateOrBackground() const;
    void HandleAppReactivateOrForeground();
    void HandleSafeFrameChanged();
    void HandleAppLicenseUpdate();
    void HandleUserLoginChanged( int32 game_user_index, ELoginStatus::Type previous_login_status, ELoginStatus::Type login_status, const FUniqueNetId & user_id );
    void HandleControllerPairingChanged( int game_user_index, const FUniqueNetId & previous_user, const FUniqueNetId & new_user );
    void HandleNetworkConnectionStatusChanged( const FString & service_name, EOnlineServerConnectionStatus::Type last_connection_status, EOnlineServerConnectionStatus::Type connection_status );
    void HandleControllerConnectionChange( bool b_is_connection, int32 unused, int32 game_user_index );
    void HandleSignInChangeMessaging();
    void ShowMessageThenGotoState( const FText & title, const FText & content, UGBFGameState * next_state );
    void OnLoginUIClosed( TSharedPtr< const FUniqueNetId > UniqueId, int controller_index, const FOnlineError & error );

    UPROPERTY( BlueprintAssignable )
    FOnStateChangedEvent OnStateChangedEvent;

    UPROPERTY()
    const UGameBaseFrameworkSettings * Settings;

    UPROPERTY( EditDefaultsOnly )
    TSoftObjectPtr< USoundMix > SoundMix;

    EOnlineServerConnectionStatus::Type CurrentConnectionStatus;
    bool bIsLicensed;
    int IgnorePairingChangeForControllerId;
    FTickerDelegate TickDelegate;
    FDelegateHandle TickDelegateHandle;
    TSharedPtr< const FUniqueNetId > CurrentUniqueNetId;
    FOnLoginUIClosedDelegate LoginUIClosedDelegate;
    TWeakObjectPtr< const UGBFGameState > CurrentGameState;
    TArray< ELoginStatus::Type > LocalPlayerOnlineStatus;
};
