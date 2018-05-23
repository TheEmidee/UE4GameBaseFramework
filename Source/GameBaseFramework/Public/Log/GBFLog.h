#pragma once

#include "Logging/LogMacros.h"

#define DECLARE_SCOPED_LOG(CategoryName, InstanceName, InitialLogFormat, ...) \
struct FSQScopedLog \
{ \
    FSQScopedLog( const FString & log ) \
        : Log( log ) \
    { \
    } \
    ~FSQScopedLog() \
    { \
        UE_LOG( CategoryName, Verbose, TEXT( "%s" ), *Log ); \
    }\
    void Append( const FString & text ) \
    { \
        Log += "\r\n \t" + text; \
    } \
private: \
    FString Log; \
}; \
FSQScopedLog InstanceName( FString::Printf( InitialLogFormat, ##__VA_ARGS__ ) );

extern FColor GetColorForLogVerbosity( ELogVerbosity::Type verbosity );

#define UE_SLOG( CategoryName, Verbosity, Format, ... ) \
    UE_LOG( CategoryName, Verbosity, Format, ##__VA_ARGS__ ); \
    { \
    \
        static_assert( TIsArrayOrRefOfType<decltype( Format ), TCHAR>::Value, "Formatting string must be a TCHAR array." ); \
        static_assert( ( ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask ) < ELogVerbosity::NumVerbosity && ELogVerbosity::Verbosity > 0, "Verbosity must be constant and in range." ); \
        CA_CONSTANT_IF( ( ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask ) <= ELogVerbosity::COMPILED_IN_MINIMUM_VERBOSITY && ( ELogVerbosity::Warning & ELogVerbosity::VerbosityMask ) <= FLogCategory##CategoryName::CompileTimeVerbosity ) \
        { \
            UE_LOG_EXPAND_IS_FATAL(Verbosity, PREPROCESSOR_NOTHING, if (!CategoryName.IsSuppressed(ELogVerbosity::Verbosity))) \
            { \
                if ( GEngine != nullptr ) \
                { \
                    GEngine->AddOnScreenDebugMessage( -1, 5.0f, GetColorForLogVerbosity( ELogVerbosity::Verbosity ), FString::Printf( Format, ##__VA_ARGS__ ) ); \
                } \
            } \
        } \
    }