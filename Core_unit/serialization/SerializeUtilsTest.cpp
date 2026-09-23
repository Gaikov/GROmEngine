// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#include <gtest/gtest.h>

#include "Core/buffer/StringWriter.h"
#include "Core/serialization/SerializableGroup.h"
#include "Core/serialization/SerializeUtils.h"
#include "Core/serialization/var/FloatVar.h"
#include "Core/serialization/var/ArrayVar.h"
#include "Core/serialization/var/IntVar.h"
#include "Core/serialization/var/StringVar.h"

namespace {
    class FailingSerializable final : public nsSerializable {
    public:
        void ResetDefault() override {}
        bool Serialize( nsScriptSaver & ) override { return false; }
        bool Deserialize( script_state_t * ) override { return false; }
    };
}

TEST( SerializeUtils, SerializeToStringUsesScriptSaverMarkup ) {
    nsSerializableGroup model;
    nsFloatVar number = 3.5f;
    nsStringVar text = "value";
    model.AddItem( "number", &number );
    model.AddItem( "text", &text );

    auto writer = std::make_shared<nsStringWriter>();
    nsScriptSaver saver( writer );
    ASSERT_TRUE( model.Serialize( saver ) );

    std::string result;
    ASSERT_TRUE( nsSerializeUtils::SerializeToString( model, result ) );
    EXPECT_EQ( writer->GetBuffer(), result );
}

TEST( SerializeUtils, SerializeToStringClearsResultOnFailure ) {
    FailingSerializable model;
    std::string result = "stale";
    EXPECT_FALSE( nsSerializeUtils::SerializeToString( model, result ) );
    EXPECT_TRUE( result.empty() );
}

TEST( SerializeUtils, IntVarRoundTripThroughSerializableGroup ) {
    nsSerializableGroup source;
    nsIntVar sourceValue = -13;
    source.AddItem( "value", &sourceValue );
    std::string markup;
    ASSERT_TRUE( nsSerializeUtils::SerializeToString( source, markup ) );

    nsSerializableGroup destination;
    nsIntVar destinationValue = 0;
    destination.AddItem( "value", &destinationValue );
    auto state = ps_begin( markup.data() );
    ASSERT_NE( state, nullptr );
    EXPECT_TRUE( destination.Deserialize( state ) );
    ps_end( state );
    EXPECT_EQ( destinationValue.GetValue(), -13 );
}

TEST( SerializeUtils, MissingScalarArrayDoesNotCreateDefaultItem ) {
    nsSerializableGroup model;
    nsArrayVar<nsStringVar> items;
    model.AddItem( "item", &items );

    std::string markup = "$other \"value\"\n";
    auto state = ps_begin( markup.data() );
    ASSERT_NE( state, nullptr );
    EXPECT_TRUE( model.Deserialize( state ) );
    ps_end( state );

    EXPECT_EQ( items.Size(), 0 );
    std::string canonical;
    ASSERT_TRUE( nsSerializeUtils::SerializeToString( model, canonical ) );
    EXPECT_TRUE( canonical.empty() );
}

TEST( SerializeUtils, CloneCreatesIndependentCanonicalCopy ) {
    nsSerializableGroup source;
    nsIntVar sourceValue = 7;
    nsArrayVar<nsStringVar> sourceItems;
    source.AddItem( "value", &sourceValue );
    source.AddItem( "item", &sourceItems );
    sourceItems.Add( std::make_shared<nsStringVar>( "first" ) );

    nsSerializableGroup destination;
    nsIntVar destinationValue = -1;
    nsArrayVar<nsStringVar> destinationItems;
    destination.AddItem( "value", &destinationValue );
    destination.AddItem( "item", &destinationItems );
    ASSERT_TRUE( nsSerializeUtils::Clone( source, destination ) );

    std::string sourceMarkup;
    std::string destinationMarkup;
    ASSERT_TRUE( nsSerializeUtils::SerializeToString( source, sourceMarkup ) );
    ASSERT_TRUE( nsSerializeUtils::SerializeToString( destination, destinationMarkup ) );
    EXPECT_EQ( sourceMarkup, destinationMarkup );

    destinationValue = 9;
    ASSERT_NE( destinationItems.GetItem( 0 ), nullptr );
    destinationItems.GetItem( 0 )->SetValue( "changed" );
    EXPECT_EQ( sourceValue.GetValue(), 7 );
    EXPECT_EQ( sourceItems.GetItem( 0 )->GetValue(), "first" );
}

TEST( SerializeUtils, CloneFailureLeavesDestinationDefault ) {
    FailingSerializable source;
    nsSerializableGroup destination;
    nsIntVar value = 3;
    destination.AddItem( "value", &value );
    value = 9;

    EXPECT_FALSE( nsSerializeUtils::Clone( source, destination ) );
    EXPECT_EQ( value.GetValue(), 3 );
}
