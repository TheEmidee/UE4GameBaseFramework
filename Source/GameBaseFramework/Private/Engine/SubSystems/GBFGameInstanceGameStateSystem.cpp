#include "Engine/SubSystems/GBFGameInstanceGameStateSystem.h"

#include "Engine/GBFGameState.h"
#include "GBFLog.h"

#include <Engine/AssetManager.h>
#include <Engine/GameInstance.h>
#include <Engine/World.h>
#include <GameBaseFrameworkSettings.h>

void UGBFGameInstanceGameStateSystem::Initialize( FSubsystemCollectionBase & collection )
{
    Super::Initialize( collection );

    Settings = GetDefault< UGameBaseFrameworkSettings >();

    LoadGameStates();

    CurrentGameState = NAME_None;
}

bool UGBFGameInstanceGameStateSystem::IsOnWelcomeScreenState() const
{
    return CurrentGameState == UGBFGameState::WelcomeScreenStateName;
}

bool UGBFGameInstanceGameStateSystem::IsOnMainMenuState() const
{
    return CurrentGameState == UGBFGameState::InGameStateName;
}

void UGBFGameInstanceGameStateSystem::GoToWelcomeScreenState()
{
    GoToState( UGBFGameState::WelcomeScreenStateName );
}

void UGBFGameInstanceGameStateSystem::GoToMainMenuState()
{
    GoToState( UGBFGameState::MainMenuStateName );
}

void UGBFGameInstanceGameStateSystem::GoToInGameState()
{
    GoToState( UGBFGameState::InGameStateName );
}

void UGBFGameInstanceGameStateSystem::GoToState( FName new_state )
{
    if ( !ensureAlwaysMsgf( new_state != NAME_None, TEXT( "new_state must not be None" ) ) )
    {
        return;
    }

    if ( const auto * state_ptr = GameStates.Find( new_state ) )
    {
        if ( const auto * state = *state_ptr )
        {
            if ( CurrentGameState != new_state )
            {
                CurrentGameState = new_state;

                OnStateChangedDelegate.Broadcast( new_state, state );
            }
        }
    }
}

FName UGBFGameInstanceGameStateSystem::GetCurrentState() const
{
    return CurrentGameState;
}

void UGBFGameInstanceGameStateSystem::UpdateCurrentGameStateFromCurrentWorld()
{
    auto * play_world_context = GetOuterUGameInstance()->GetWorldContext();
    check( play_world_context );

    auto * play_world = play_world_context->World();
    check( play_world );

    auto current_map_name = play_world->GetOutermost()->GetName();
    if ( !play_world_context->PIEPrefix.IsEmpty() )
    {
        current_map_name.ReplaceInline( *play_world_context->PIEPrefix, TEXT( "" ) );
    }

    for ( const auto & key_pair : GameStates )
    {
        if ( key_pair.Value->Map.IsValid() && key_pair.Value->Map->GetName() == current_map_name )
        {
            GoToState( key_pair.Key );
            return;
        }
    }

    GoToInGameState();

    //// Workaround for when running in PIE, to set the correct state based on the game mode created by the URL
    //if ( GetWorld()->WorldType != EWorldType::Game )
    //{
    //    if ( !CurrentGameState.IsValid() )
    //    {
    //        if ( auto * game_mode = GetWorld()->GetAuthGameMode() )
    //        {
    //            if ( auto * current_state = GetGameStateFromGameMode( game_mode->GetClass() ) )
    //            {
    //                CurrentGameState = current_state;
    //            }
    //        }
    //    }
    //}
}

void UGBFGameInstanceGameStateSystem::LoadGameStates()
{
    if ( ensureAlwaysMsgf( Settings != nullptr, TEXT( "Null settings" ) ) )
    {
        UE_LOG( LogGBF, Log, TEXT( "Loading game states" ) )

        for ( auto & game_state : Settings->GameStates )
        {
            if ( ensureAlwaysMsgf( !game_state.Value.IsNull(), TEXT( "Invalid asset for game state %s" ), *game_state.Key.ToString() ) )
            {
                auto * state = UAssetManager::Get().GetStreamableManager().LoadSynchronous< UGBFGameState >( game_state.Value.ToSoftObjectPath() );
                GameStates.Add( game_state.Key, state );

                UE_LOG( LogGBF, Log, TEXT( "Loaded game state %s" ), *game_state.Key.ToString() )
            }
        }
    }
}