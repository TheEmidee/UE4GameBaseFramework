#include "BlueprintLibraries/GBFInputBlueprintLibrary.h"

#if !PLATFORM_DESKTOP
#include "Kismet/GameplayStatics.h"
#endif

#include "IGameBaseFrameworkModule.h"
#include "Components/GBFPlatformInputSwitcherComponent.h"
#include "GameFramework/GBFPlayerController.h"

TArray< FKey > UGBFInputBlueprintLibrary::GetCancelKeys()
{
    static const TArray< FKey > CancelKeysForPlatform {
#if PLATFORM_DESKTOP
        EKeys::Escape,
        EKeys::Virtual_Back
#elif PLATFORM_XBOXONE || PLATFORM_PS4 || PLATFORM_SWITCH
        EKeys::VirtualBack
#endif
    };

    return CancelKeysForPlatform;
}

TArray< FKey > UGBFInputBlueprintLibrary::GetConfirmKeys()
{
    static const TArray< FKey > ConfirmKeysForPlatform = {
#if PLATFORM_DESKTOP
        EKeys::Enter,
        EKeys::Virtual_Accept,
#elif PLATFORM_XBOXONE || PLATFORM_PS4 || PLATFORM_SWITCH
        EKeys::VirtualAccept
#endif
    };

    return ConfirmKeysForPlatform;
}

const FKey & UGBFInputBlueprintLibrary::ResolvePlatformInputKey( const FGBFPlatformInputKey & platform_input_key, APlayerController * player_controller )
{
    auto platform_input_type = EGBFPlatformInputType::Gamepad;

#if PLATFORM_DESKTOP
    if ( auto * pc = Cast< AGBFPlayerController >( player_controller ) )
    {
        if ( auto * input_device_switcher = pc->GetPlatformInputSwitcherComponent() )
        {
            platform_input_type = input_device_switcher->GetPlatformInputType();
        }
    }

    if ( platform_input_type == EGBFPlatformInputType::Gamepad )
#endif
    {
        if ( ( platform_input_key.ProcessVirtualKeyFirstFlag & EGBFVirtualKeyProcessedFirst::GamepadOnly ) == EGBFVirtualKeyProcessedFirst::GamepadOnly )
        {
            switch ( platform_input_key.VirtualKey )
            {
                case EGBFVirtualKey::VirtualBack:
                {
                    return EKeys::Virtual_Back;
                }
                case EGBFVirtualKey::VirtualAccept:
                {
                    return EKeys::Virtual_Accept;
                }
                default:
                {
                    return platform_input_key.GamePadKey;
                }
            }
        }
        return platform_input_key.GamePadKey;
    }
#if PLATFORM_DESKTOP
    if ( ( platform_input_key.ProcessVirtualKeyFirstFlag & EGBFVirtualKeyProcessedFirst::KeyboardOnly ) == EGBFVirtualKeyProcessedFirst::KeyboardOnly )
    {
        switch ( platform_input_key.VirtualKey )
        {
            case EGBFVirtualKey::VirtualBack:
            {
                return EKeys::Escape;
            }
            case EGBFVirtualKey::VirtualAccept:
            {
                return EKeys::Enter;
            }
            default:
            {
                return platform_input_key.KeyboardKey;
            }
        }
    }
    return platform_input_key.KeyboardKey;
#endif
}

UTexture2D * UGBFInputBlueprintLibrary::GetPlatformInputKeyTexture( const FKey & key, APlayerController * player_controller )
{
#if PLATFORM_DESKTOP
    auto platform_name = FString( "Desktop" );

    if ( auto * pc = Cast< AGBFPlayerController >( player_controller ) )
    {
        if ( auto * input_device_switcher = pc->GetPlatformInputSwitcherComponent() )
        {
            if ( input_device_switcher->GetPlatformInputType() == EGBFPlatformInputType::Keyboard )
            {
                platform_name = "Keyboard";
            }
        }
    }
#else
    const auto platform_name = UGameplayStatics::GetPlatformName();
#endif

    return IGameBaseFrameworkModule::Get().GetPlatformInputTextureForKey( platform_name, key );
}
