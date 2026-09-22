// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#include <gtest/gtest.h>

#include "Core/buffer/StringWriter.h"
#include "Core/serialization/SerializableGroup.h"
#include "Core/serialization/SerializeUtils.h"
#include "Core/serialization/var/FloatVar.h"
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
