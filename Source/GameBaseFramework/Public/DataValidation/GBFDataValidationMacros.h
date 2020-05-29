#pragma once

#include <CoreMinimal.h>
#include <UObject/UObjectGlobals.h>

#if WITH_EDITOR

#define DATA_VALIDATION_INTERNAL_CONDITION_SUFFIX( ErrorCondition, ErrorMessageText, SuffixText )           \
    if ( ErrorCondition )                                                                                   \
    {                                                                                                       \
        FText text = SuffixText.IsEmpty()                                                                   \
                         ? ErrorMessageText                                                                 \
                         : FText::Join( FText::FromString( TEXT( " - " ) ), ErrorMessageText, SuffixText ); \
        validation_errors.Emplace( MoveTemp( text ) );                                                      \
    }

#define DATA_VALIDATION_INTERNAL_CONDITION( ErrorCondition, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION_SUFFIX( ErrorCondition, ErrorMessageText, FText::FromString( TEXT( "" ) ) )

#define DATA_VALIDATION_CALL_IS_DATA_VALID( VariableName ) \
    VariableName->IsDataValid( validation_errors );

#define DATA_VALIDATION_IS_VALID( VariableName, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( !VariableName.IsValid(), ErrorMessageText )

#define DATA_VALIDATION_IS_VALID_AUTO_MESSAGE( VariableName ) \
    DATA_VALIDATION_IS_VALID( VariableName, FText::FromString( FString::Printf( TEXT( "%s must be valid" ), TEXT( #VariableName ) ) ) )

#define DATA_VALIDATION_IS_TRUE( VariableName, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( !VariableName, ErrorMessageText )

#define DATA_VALIDATION_IS_TRUE_AUTO_MESSAGE( VariableName ) \
    DATA_VALIDATION_IS_TRUE( VariableName, FText::FromString( FString::Printf( TEXT( "%s must be true" ), TEXT( #VariableName ) ) ) )

#define DATA_VALIDATION_IS_FALSE( VariableName, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( VariableName, ErrorMessageText )

#define DATA_VALIDATION_IS_FALSE_AUTO_MESSAGE( VariableName ) \
    DATA_VALIDATION_IS_FALSE( VariableName, FText::FromString( FString::Printf( TEXT( "%s must be false" ), TEXT( #VariableName ) ) ) )

#define DATA_VALIDATION_IS_GREATER_THAN( VariableName, MinValue, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( VariableName > 0, ErrorMessageText )

#define DATA_VALIDATION_IS_GREATER_THAN_AUTO_MESSAGE( VariableName, MinValue ) \
    DATA_VALIDATION_IS_GREATER_THAN( VariableName, MinValue, FText::FromString( FString::Printf( TEXT( "%s must be greater than %i" ), TEXT( #VariableName ), MinValue ) ) )

#define DATA_VALIDATION_NOT_NULL( VariableName, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( VariableName == nullptr, ErrorMessageText )

#define DATA_VALIDATION_NOT_NULL_AUTO_MESSAGE( VariableName ) \
    DATA_VALIDATION_NOT_NULL( VariableName, FText::FromString( FString::Printf( TEXT( "%s can not be null" ), TEXT( #VariableName ) ) ) )

#define DATA_VALIDATION_NOT_ISNULL( VariableName, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( VariableName.IsNull(), ErrorMessageText )

#define DATA_VALIDATION_NOT_ISNULL_AUTO_MESSAGE( VariableName ) \
    DATA_VALIDATION_NOT_ISNULL( VariableName, FText::FromString( FString::Printf( TEXT( "%s can not be null" ), TEXT( #VariableName ) ) ) )

#define DATA_VALIDATION_NAME_NOT_NONE( VariableName, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( VariableName.IsNone(), ErrorMessageText )

#define DATA_VALIDATION_NAME_NOT_NONE_AUTO_MESSAGE( VariableName ) \
    DATA_VALIDATION_NAME_NOT_NONE( VariableName, FText::FromString( FString::Printf( TEXT( "%s can not be None" ), TEXT( #VariableName ) ) ) )

#define DATA_VALIDATION_CONTAINER_NOT_EMPTY( ContainerName, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( ContainerName.Num() == 0, ErrorMessageText )

#define DATA_VALIDATION_CONTAINER_NOT_EMPTY_AUTO_MESSAGE( ContainerName ) \
    DATA_VALIDATION_CONTAINER_NOT_EMPTY( ContainerName, FText::FromString( FString::Printf( TEXT( "%s can not be empty" ), TEXT( #ContainerName ) ) ) )

#define DATA_VALIDATION_CONTAINER_HAS_SIZE( ContainerName, ExpectedSize, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( ContainerName.Num() != ExpectedSize, ErrorMessageText )

#define DATA_VALIDATION_CONTAINER_HAS_SIZE_AUTO_MESSAGE( ContainerName, ExpectedSize ) \
    DATA_VALIDATION_CONTAINER_HAS_SIZE( ContainerName, ExpectedSize, FText::FromString( FString::Printf( TEXT( "%s must have %i elements" ), TEXT( #ContainerName ), ExpectedSize ) ) )

#define DATA_VALIDATION_CONTAINER_NO_NULL_ITEM( ContainerName )                                                                                  \
    for ( const auto & item : ContainerName )                                                                                                    \
    {                                                                                                                                            \
        if ( item == nullptr )                                                                                                                   \
        {                                                                                                                                        \
            validation_errors.Emplace( FText::FromString( FString::Printf( TEXT( "%s cannot contain null items" ), TEXT( #ContainerName ) ) ) ); \
        }                                                                                                                                        \
    }

#define DATA_VALIDATION_ARE_EQUAL( FirstItemName, SecondItemName, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( FirstItemName != SecondItemName, ErrorMessageText )

#define DATA_VALIDATION_ARE_EQUAL_AUTO_MESSAGE( FirstItemName, SecondItemName ) \
    DATA_VALIDATION_ARE_EQUAL( FirstItemName, SecondItemName, FText::FromString( FString::Printf( TEXT( "%s must be equal to %s" ), TEXT( #FirstItemName ), TEXT( #SecondItemName ) ) ) );

#define DATA_VALIDATION_ARE_NOT_EQUAL( FirstItemName, SecondItemName, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( FirstItemName == SecondItemName, ErrorMessageText )

#define DATA_VALIDATION_ARE_NOT_EQUAL_AUTO_MESSAGE( FirstItemName, SecondItemName ) \
    DATA_VALIDATION_ARE_NOT_EQUAL( FirstItemName, SecondItemName, FText::FromString( FString::Printf( TEXT( "%s must not be equal to %s" ), TEXT( #FirstItemName ), TEXT( #SecondItemName ) ) ) );

#define DATA_VALIDATION_TAG_CONTAINER_HAS_NOT_TAG( ContainerTagName, TagName, ErrorMessageText ) \
    DATA_VALIDATION_INTERNAL_CONDITION( ContainerTagName.HasTag( TagName ), ErrorMessageText )

#define DATA_VALIDATION_TAG_CONTAINER_HAS_NOT_TAG_AUTO_MESSAGE_SUFFIX( ContainerTagName, TagName, SuffixText ) \
    DATA_VALIDATION_INTERNAL_CONDITION_SUFFIX( ContainerTagName.HasTag( TagName ), FText::FromString( FString::Printf( TEXT( "%s must not contain the tag %s" ), TEXT( #ContainerTagName ), *TagName.ToString() ) ), SuffixText );

#define DATA_VALIDATION_TAG_CONTAINER_HAS_NOT_TAG_AUTO_MESSAGE( ContainerTagName, TagName ) \
    DATA_VALIDATION_TAG_CONTAINER_HAS_NOT_TAG_AUTO_MESSAGE_SUFFIX( ContainerTagName, TagName, "" );

#define DATA_VALIDATION_RETURN() \
    return validation_errors.Num() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;

class GAMEBASEFRAMEWORK_API FGBFDataValidator
{
public:
    FGBFDataValidator( UObject & object, TArray< FText > & validation_errors );

    FGBFDataValidator & NotNull( FName property_name );
    FGBFDataValidator & Custom( const TFunctionRef< FText() > & predicate );

    template < class _TYPE_ >
    FGBFDataValidator & GreaterThan( const FName property_name, const _TYPE_ value )
    {
        return CompareProperty< TProperty_Numeric< _TYPE_ >, _TYPE_, std::greater< _TYPE_ > >(
            property_name,
            value,
            FText::FromString( FString::Printf( TEXT( "%s must be greater than %f" ), *property_name.ToString(), value ) ) );
    }

    template < class _TYPE_ >
    FGBFDataValidator & GreaterOrEqualThan( const FName property_name, const _TYPE_ value )
    {
        return CompareProperty< TProperty_Numeric< _TYPE_ >, _TYPE_, std::greater_equal< _TYPE_ > >(
            property_name,
            value,
            FText::FromString( FString::Printf( TEXT( "%s must be greater or equal than %f" ), *property_name.ToString(), value ) ) );
    }

    template < class _TYPE_ >
    FGBFDataValidator & LessThan( const FName property_name, const _TYPE_ value )
    {
        return CompareProperty< TProperty_Numeric< _TYPE_ >, _TYPE_, std::less< _TYPE_ > >(
            property_name,
            value,
            FText::FromString( FString::Printf( TEXT( "%s must be less than %f" ), *property_name.ToString(), value ) ) );
    }

    template < class _TYPE_ >
    FGBFDataValidator & LessOrEqualThan( const FName property_name, const _TYPE_ value )
    {
        return CompareProperty< TProperty_Numeric< _TYPE_ >, _TYPE_, std::less_equal< _TYPE_ > >(
            property_name,
            value,
            FText::FromString( FString::Printf( TEXT( "%s must be less or equal than %f" ), *property_name.ToString(), value ) ) );
    }

    FGBFDataValidator & NotEmpty( FName property_name );
    FGBFDataValidator & Empty( FName property_name );
    FGBFDataValidator & NoNullItem( FName property_name );

    EDataValidationResult Result() const;

private:
    template < class _PREDICATE_TYPE_ >
    FGBFDataValidator & CheckContainer( const FName property_name, const FText & error_message, _PREDICATE_TYPE_ predicate )
    {
        auto found_type = false;

        const auto check = [this, &error_message, &found_type, &predicate]( const auto * container_property ) {
            if ( container_property == nullptr )
            {
                return;
            }

            found_type = true;

            if ( !predicate( container_property ) )
            {
                AddError( error_message );
            }
        };

        const auto container_properties_tuple = GetContainerPropertyTuple( property_name );
        VisitTupleElements( check, container_properties_tuple );

        if ( !found_type )
        {
            AddError( FText::FromString( FString::Printf( TEXT( "%s could not be used as a container property" ), *property_name.ToString() ) ) );
        }

        return *this;
    }

    template < class _COMPARATOR_TYPE_, int _CONTAINER_SIZE_ = 0 >
    FGBFDataValidator & CheckContainerSize( const FName property_name, const FText & error_message, _COMPARATOR_TYPE_ comparator = _COMPARATOR_TYPE_() )
    {
        return CheckContainer( property_name, error_message, [comparator, this]( const auto * container_property ) {
            return comparator( container_property->GetPropertyValue_InContainer( &Object ).Num(), _CONTAINER_SIZE_ );
        } );
    }

    TTuple< UArrayProperty * > GetContainerPropertyTuple( const FName property_name )
    {
        return MakeTuple( GetTypedProperty< UArrayProperty >( property_name, false )/*, GetTypedProperty< USetProperty >( property_name, false )*/ );
    }

    template < class _PROPERTY_TYPE_, class _VALUE_TYPE_, class _COMPARATOR_TYPE_ >
    FGBFDataValidator & CompareProperty( const FName property_name, const _VALUE_TYPE_ value, const FText & error_message, _COMPARATOR_TYPE_ comparator = _COMPARATOR_TYPE_() )
    {
        if ( auto * property = GetTypedProperty< _PROPERTY_TYPE_ >( property_name ) )
        {
            const auto property_value = property->GetPropertyValue_InContainer( &Object );

            if ( !comparator( property_value, value ) )
            {
                AddError( error_message );
            }
        }

        return *this;
    }

    template < class _PROPERTY_TYPE_ >
    _PROPERTY_TYPE_ * GetTypedProperty( const FName property_name, const bool add_error_if_type_not_found = true )
    {
        if ( auto * property = ObjectClass->FindPropertyByName( property_name ) )
        {
            if ( auto * typed_property = Cast< _PROPERTY_TYPE_ >( property ) )
            {
                return typed_property;
            }
        }

        if ( add_error_if_type_not_found )
        {
            AddError( FText::FromString( FString::Printf( TEXT( "Could not find or cast to the correct type a property named %s" ), *property_name.ToString() ) ) );
        }
        return nullptr;
    }

    void AddError( FText text );

    UObject & Object;
    EDataValidationResult ValidationResult;
    UClass * ObjectClass;
    TArray< FText > & ValidationErrors;
};

#endif