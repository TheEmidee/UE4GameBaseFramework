#pragma once

#include <CoreMinimal.h>
#include <Engine/LocalPlayer.h>
#include <Interfaces/OnlineAchievementsInterface.h>
#include <OnlineStats.h>
#include <OnlineSubsystemTypes.h>

#include "GBFLocalPlayer.generated.h"

class UGBFSaveGame;

UCLASS( BlueprintType, Blueprintable )
class GAMEBASEFRAMEWORK_API UGBFLocalPlayer : public ULocalPlayer
{
    GENERATED_BODY()

public:
    UGBFLocalPlayer();

    void SetControllerId( int32 new_controller_id ) override;

    UFUNCTION( BlueprintPure )
    FString GetDisplayName() const;

    UGBFSaveGame * GetSaveGame() const;
    FPlatformUserId GetPlatformUserId() const;
    ELoginStatus::Type GetLoginStatus() const;

    void InitializeAfterLogin( const int controller_index );
    void SetPresenceStatus( const FText & status ) const;

    UFUNCTION( BlueprintCallable )
    void WriteAchievementCurrentCount( const FName & achievement_id, const int current_count, const int trigger_count );

protected:
    UPROPERTY( EditDefaultsOnly )
    TSubclassOf< UGBFSaveGame > SaveGameClass;

    UFUNCTION( BlueprintImplementableEvent )
    void ReceiveSaveGameLoaded( UGBFSaveGame * save_game );

private:
    FString GetSaveGameFilename() const;

    void QueryAchievements();
    void LoadSaveGame();
    UGBFSaveGame * LoadSaveGameOrCreateFromSlot( const FString & slot_name, const int user_index );
    void CheckChangedControllerId( const FString & save_name );
    void OnQueryAchievementsComplete( const FUniqueNetId & player_id, bool was_successful );

    UPROPERTY( BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
    UGBFSaveGame * SaveGame;

    FOnlineAchievementsWritePtr OnlineAchievementWriter;
    TArray< FOnlineAchievement > AchievementsArray;
    bool AreAchievementsCached;
};
