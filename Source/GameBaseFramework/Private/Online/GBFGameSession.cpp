#include "Online/GBFGameSession.h"

#include "GameFramework/GBFPlayerController.h"
#include "Online/GBFOnlineSessionSettings.h"

#include <OnlineSubsystemSessionSettings.h>
#include <OnlineSubsystemUtils.h>

namespace
{
    const FString CustomMatchKeyword( "Custom" );
}

AGBFGameSession::AGBFGameSession( const FObjectInitializer & object_initializer ) :
    Super( object_initializer )
{
    if ( !HasAnyFlags( RF_ClassDefaultObject ) )
    {
        OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject( this, &AGBFGameSession::OnCreateSessionComplete );
        OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject( this, &AGBFGameSession::OnDestroySessionComplete );

        OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject( this, &AGBFGameSession::OnFindSessionsComplete );
        OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject( this, &AGBFGameSession::OnJoinSessionComplete );

        OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject( this, &AGBFGameSession::OnStartOnlineGameComplete );
    }
}

void AGBFGameSession::HandleMatchHasStarted()
{
    // start online game locally and wait for completion
    if ( auto * oss = Online::GetSubsystem( GetWorld() ) )
    {
        auto session_interface_ptr = oss->GetSessionInterface();

        if ( session_interface_ptr.IsValid() && session_interface_ptr->GetNamedSession( NAME_GameSession ) != nullptr )
        {
            UE_LOG( LogOnlineGame, Log, TEXT( "Starting session %s on server" ), *FName( NAME_GameSession ).ToString() );
            OnStartSessionCompleteDelegateHandle = session_interface_ptr->AddOnStartSessionCompleteDelegate_Handle( OnStartSessionCompleteDelegate );
            session_interface_ptr->StartSession( NAME_GameSession );
        }
    }
}

void AGBFGameSession::HandleMatchHasEnded()
{
    // end online game locally
    if ( auto * oss = Online::GetSubsystem( GetWorld() ) )
    {
        auto session_interface_ptr = oss->GetSessionInterface();
        if ( session_interface_ptr.IsValid() && session_interface_ptr->GetNamedSession( NAME_GameSession ) != nullptr )
        {
            // tell the clients to end
            for ( auto player_controller_iterator = GetWorld()->GetPlayerControllerIterator(); player_controller_iterator; ++player_controller_iterator )
            {
                if ( auto * pc = Cast< AGBFPlayerController >( *player_controller_iterator ) )
                {
                    if ( pc && !pc->IsLocalPlayerController() )
                    {
                        // :TODO:
                        //pc->ClientEndOnlineGame();
                    }
                }
            }

            // server is handled here
            UE_LOG( LogOnlineGame, Log, TEXT( "Ending session %s on server" ), *FName( NAME_GameSession ).ToString() );
            session_interface_ptr->EndSession( NAME_GameSession );
        }
    }
}

EOnlineAsyncTaskState::Type AGBFGameSession::GetSearchResultStatus( int32 & search_result_idx, int32 & num_search_results ) const
{
    search_result_idx = 0;
    num_search_results = 0;

    if ( SearchSettings.IsValid() )
    {
        if ( SearchSettings->SearchState == EOnlineAsyncTaskState::Done )
        {
            search_result_idx = CurrentSessionParams.BestSessionIdx;
            num_search_results = SearchSettings->SearchResults.Num();
        }
        return SearchSettings->SearchState;
    }

    return EOnlineAsyncTaskState::NotStarted;
}

const TArray< FOnlineSessionSearchResult > & AGBFGameSession::GetSearchResults() const
{
    return SearchSettings->SearchResults;
}

bool AGBFGameSession::JoinSession( const TSharedPtr< const FUniqueNetId > & user_id, const FName session_name, const int32 session_index_in_search_results )
{
    if ( session_index_in_search_results >= 0 && session_index_in_search_results < SearchSettings->SearchResults.Num() )
    {
        return JoinSession( user_id, session_name, SearchSettings->SearchResults[ session_index_in_search_results ] );
    }

    return false;
}

bool AGBFGameSession::JoinSession( const TSharedPtr< const FUniqueNetId > & user_id, const FName session_name, const FOnlineSessionSearchResult & search_result )
{
    auto result = false;

    if ( auto * oss = Online::GetSubsystem( GetWorld() ) )
    {
        auto session_interface_ptr = oss->GetSessionInterface();
        if ( session_interface_ptr.IsValid() && user_id.IsValid() )
        {
            OnJoinSessionCompleteDelegateHandle = session_interface_ptr->AddOnJoinSessionCompleteDelegate_Handle( OnJoinSessionCompleteDelegate );
            result = session_interface_ptr->JoinSession( *user_id, session_name, search_result );
        }
    }

    return result;
}

void AGBFGameSession::FindSessions( const TSharedPtr< const FUniqueNetId > & user_id, const FName session_name, const bool is_lan, const bool is_presence )
{
    if ( auto * oss = Online::GetSubsystem( GetWorld() ) )
    {
        CurrentSessionParams.SessionName = session_name;
        CurrentSessionParams.IsLAN = is_lan;
        CurrentSessionParams.IsPresence = is_presence;
        CurrentSessionParams.UserId = user_id;

        auto session_interface_ptr = oss->GetSessionInterface();
        if ( session_interface_ptr.IsValid() && CurrentSessionParams.UserId.IsValid() )
        {
            SearchSettings = MakeShareable( new FGBFOnlineSearchSettings( is_lan, is_presence ) );
            SearchSettings->QuerySettings.Set( SEARCH_KEYWORDS, CustomMatchKeyword, EOnlineComparisonOp::Equals );

            const TSharedRef< FOnlineSessionSearch > search_settings_ref = SearchSettings.ToSharedRef();

            OnFindSessionsCompleteDelegateHandle = session_interface_ptr->AddOnFindSessionsCompleteDelegate_Handle( OnFindSessionsCompleteDelegate );
            session_interface_ptr->FindSessions( *CurrentSessionParams.UserId, search_settings_ref );
        }
    }
    else
    {
        OnFindSessionsComplete( false );
    }
}

bool AGBFGameSession::HostSession( const TSharedPtr< const FUniqueNetId > & user_id, const FName session_name, const FString & /*game_type*/, const FString & /*map_name*/, const bool is_lan, const bool is_presence, const int32 max_num_players )
{
    if ( auto * oss = Online::GetSubsystem( GetWorld() ) )
    {
        CurrentSessionParams.SessionName = session_name;
        CurrentSessionParams.IsLAN = is_lan;
        CurrentSessionParams.IsPresence = is_presence;
        CurrentSessionParams.UserId = user_id;
        MaxPlayers = max_num_players;

        auto session_interface_ptr = oss->GetSessionInterface();
        if ( session_interface_ptr.IsValid() && CurrentSessionParams.UserId.IsValid() )
        {
            HostSettings = MakeShareable( new FGBFOnlineSessionSettings( is_lan, is_presence, MaxPlayers ) );
            FillHostSettings( *HostSettings );

            OnCreateSessionCompleteDelegateHandle = session_interface_ptr->AddOnCreateSessionCompleteDelegate_Handle( OnCreateSessionCompleteDelegate );
            return session_interface_ptr->CreateSession( *CurrentSessionParams.UserId, CurrentSessionParams.SessionName, *HostSettings );
        }
        
        OnCreateSessionComplete( session_name, false );
    }
#if !UE_BUILD_SHIPPING
    else
    {
        // Hack workflow in development
        OnCreatePresenceSessionComplete().Broadcast( NAME_GameSession, true );
        return true;
    }
#endif

    return false;
}

bool AGBFGameSession::HostSession( const TSharedPtr< const FUniqueNetId > & user_id, const FName session_name, const FOnlineSessionSettings & session_settings )
{
    auto result = false;

    if ( auto * oss = Online::GetSubsystem( GetWorld() ) )
    {
        CurrentSessionParams.SessionName = session_name;
        CurrentSessionParams.IsLAN = session_settings.bIsLANMatch;
        CurrentSessionParams.IsPresence = session_settings.bUsesPresence;
        CurrentSessionParams.UserId = user_id;
        MaxPlayers = session_settings.NumPrivateConnections + session_settings.NumPublicConnections;

        auto session_interface_ptr = oss->GetSessionInterface();
        if ( session_interface_ptr.IsValid() && CurrentSessionParams.UserId.IsValid() )
        {
            OnCreateSessionCompleteDelegateHandle = session_interface_ptr->AddOnCreateSessionCompleteDelegate_Handle( OnCreateSessionCompleteDelegate );
            result = session_interface_ptr->CreateSession( *user_id, session_name, session_settings );
        }
        else
        {
            OnCreateSessionComplete( session_name, false );
        }
    }

    return result;
}

void AGBFGameSession::OnCreateSessionComplete( const FName session_name, const bool was_successful )
{
    UE_LOG( LogOnlineGame, Verbose, TEXT( "OnCreateSessionComplete %s bSuccess: %d" ), *session_name.ToString(), was_successful );

    if ( auto * oss = Online::GetSubsystem( GetWorld() ) )
    {
        auto session_interface_ptr = oss->GetSessionInterface();
        session_interface_ptr->ClearOnCreateSessionCompleteDelegate_Handle( OnCreateSessionCompleteDelegateHandle );
    }

    OnCreatePresenceSessionCompleteEvent.Broadcast( session_name, was_successful );
}

void AGBFGameSession::OnStartOnlineGameComplete( FName /* session_name */, const bool was_successful )
{
    if ( auto * oss = Online::GetSubsystem( GetWorld() ) )
    {
        auto session_interface_ptr = oss->GetSessionInterface();

        if ( session_interface_ptr.IsValid() )
        {
            session_interface_ptr->ClearOnStartSessionCompleteDelegate_Handle( OnStartSessionCompleteDelegateHandle );
        }
    }

    if ( was_successful )
    {
        // tell non-local players to start online game
        for ( auto player_controller_iterator = GetWorld()->GetPlayerControllerIterator(); player_controller_iterator; ++player_controller_iterator )
        {
            if ( auto * pc = Cast< AGBFPlayerController >( *player_controller_iterator ) )
            {
                if ( pc && !pc->IsLocalPlayerController() )
                {
                    // :TODO:
                    //pc->ClientStartOnlineGame();
                }
            }
        }
    }
}

void AGBFGameSession::OnFindSessionsComplete( const bool was_successful )
{
    UE_LOG( LogOnlineGame, Verbose, TEXT( "OnFindSessionsComplete bSuccess: %d" ), was_successful );

    if ( auto * oss = Online::GetSubsystem( GetWorld() ) )
    {
        auto session_interface_ptr = oss->GetSessionInterface();

        if ( session_interface_ptr.IsValid() )
        {
            session_interface_ptr->ClearOnFindSessionsCompleteDelegate_Handle( OnFindSessionsCompleteDelegateHandle );

            UE_LOG( LogOnlineGame, Verbose, TEXT( "Num Search Results: %d" ), SearchSettings->SearchResults.Num() );
            for ( auto search_idx = 0; search_idx < SearchSettings->SearchResults.Num(); search_idx++ )
            {
                const auto & search_result = SearchSettings->SearchResults[ search_idx ];
                DumpSession( &search_result.Session );
            }

            OnFindSessionsCompleteEvent.Broadcast( was_successful );
        }
    }
}

void AGBFGameSession::OnJoinSessionComplete( const FName session_name, const EOnJoinSessionCompleteResult::Type result )
{
    UE_LOG( LogOnlineGame, Verbose, TEXT( "OnJoinSessionComplete %s bSuccess: %d" ), *session_name.ToString(), static_cast< int32 >( result ) );

    if ( auto * oss = Online::GetSubsystem( GetWorld() ) )
    {
        auto session_interface_ptr = oss->GetSessionInterface();

        if ( session_interface_ptr.IsValid() )
        {
            session_interface_ptr->ClearOnJoinSessionCompleteDelegate_Handle( OnJoinSessionCompleteDelegateHandle );
        }
    }

    OnJoinSessionCompleteEvent.Broadcast( result );
}

void AGBFGameSession::OnDestroySessionComplete( const FName session_name, const bool was_successful )
{
    UE_LOG( LogOnlineGame, Verbose, TEXT( "OnDestroySessionComplete %s bSuccess: %d" ), *session_name.ToString(), was_successful );

    if ( auto * oss = Online::GetSubsystem( GetWorld() ) )
    {
        auto session_interface_ptr = oss->GetSessionInterface();
        session_interface_ptr->ClearOnDestroySessionCompleteDelegate_Handle( OnDestroySessionCompleteDelegateHandle );
        HostSettings = nullptr;
    }
}

void AGBFGameSession::RegisterServer()
{
    checkf( false, TEXT( "To be implemented by the game project" ) );
}

void AGBFGameSession::FillHostSettings( FGBFOnlineSessionSettings & host_settings )
{
    checkf( false, TEXT( "To be implemented by the game project" ) );
}
