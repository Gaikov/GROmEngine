cmake_minimum_required(VERSION 3.10)
project(nsLib)

include_directories(
    .
)

set(sources
        EditString.cpp
        FilePath.cpp
        Line2.cpp
        Lock.cpp
        MathTools.cpp
        StrTools.cpp
        Triangulate2.cpp
        Vec2.cpp
        color.cpp
        headers.cpp
        matrix4.cpp
        log.cpp
        events/EventDispatcher.cpp
        events/EventDispatcher.h
        events/BaseEvent.cpp
        events/BaseEvent.h
        math/ortho/OrthogonalFitView.cpp
        math/Rect.cpp
        math/Rect.h
        models/Property.cpp
        models/Property.h
        math/Transform2.cpp
        math/Transform2.h
        math/Matrix2.cpp
        math/Matrix2.h
        structs/FastList.cpp
        structs/FastList.h
        math/ortho/OrthogonalView.cpp
        math/ortho/OrthogonalView.h
        math/ortho/OrthogonalNativeView.cpp
        math/ortho/OrthogonalNativeView.h
        math/LineMath.cpp
        math/LineMath.h
        math/TriMath.cpp
        math/TriMath.h
        factory/AbstractFactory.cpp
        factory/AbstractFactory.h
        locator/ServiceLocator.cpp
        locator/ServiceLocator.h
        locator/ClassMapping.cpp
        locator/ClassMapping.h
        locator/Locatable.h
        math/collision/CollisionMath.cpp
        math/collision/CollisionMath.h
        utils/AutoDestroy.cpp
        utils/AutoDestroy.h
)

add_library(nsLib STATIC ${sources})