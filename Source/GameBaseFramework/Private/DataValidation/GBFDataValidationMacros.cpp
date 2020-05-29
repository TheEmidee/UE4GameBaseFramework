#include "DataValidation/GBFDataValidationMacros.h"

#include "UObject/UnrealType.h"

#if WITH_EDITOR

FGBFDataValidator::FGBFDataValidator( UObject & object, TArray< FText > & validation_errors ) :
    Object( object ),
    ValidationResult( validation_errors.Num() == 0 ? EDataValidationResult::Valid : EDataValidationResult::Invalid ),
    ObjectClass( Object.GetClass() ),
    ValidationErrors( validation_errors )
{}

FGBFDataValidator & FGBFDataValidator::NotNull( const FName property_name )
{
    if ( auto * property = GetTypedProperty< UObjectPropertyBase >( property_name ) )
    {
        auto * value = property->GetObjectPropertyValue_InContainer( &Object );
        if ( value == nullptr )
        {
            AddError( FText::FromString( FString::Printf( TEXT( "%s is null" ), *property_name.ToString() ) ) );
        }
    }
    return *this;
}

FGBFDataValidator & FGBFDataValidator::Custom( const TFunctionRef< FText( void ) > & predicate )
{
    const auto error = predicate();

    if ( !error.IsEmpty() )
    {
        AddError( error );
    }
    return *this;
}

FGBFDataValidator & FGBFDataValidator::NotEmpty( const FName property_name )
{
    return CheckContainerSize< std::not_equal_to< int >, 0 >( property_name, FText::FromString( FString::Printf( TEXT( "%s can not be empty" ), *property_name.ToString() ) ) );
}

FGBFDataValidator & FGBFDataValidator::Empty( const FName property_name )
{
    return CheckContainerSize< std::equal_to< int >, 0 >( property_name, FText::FromString( FString::Printf( TEXT( "%s must be empty" ), *property_name.ToString() ) ) );
}

FGBFDataValidator & FGBFDataValidator::NoNullItem( FName property_name )
{
    const auto error_message = FText::FromString( FString::Printf( TEXT( "%s can not contain null items" ), *property_name.ToString() ) );

    return CheckContainer( property_name, error_message, [this]( const auto * container_property ) {
        auto * inner_prop = Cast< UObjectProperty >( container_property->Inner );
        const auto & container = container_property->GetPropertyValue_InContainer( &Object );

        FScriptArrayHelper ArrayHelper( container_property, container_property->ContainerPtrToValuePtr<void>( &Object ) );

        for ( auto index = 0; index < container.Num(); index++ )
        {
            UObject* object = inner_prop->GetObjectPropertyValue( ArrayHelper.GetRawPtr( index ) );

            if ( object == nullptr )
            {
                return false;
            }
        }

        return true;
    } );
}

EDataValidationResult FGBFDataValidator::Result() const
{
    return ValidationResult;
}

void FGBFDataValidator::AddError( FText text )
{
    ValidationErrors.Emplace( text );
    ValidationResult = EDataValidationResult::Invalid;
}

#endif