#pragma once

#ifndef collision_h
#define collision_h

#include "util.h"
#include <iostream>

#undef min
#undef max

// requires util.h to run (unless you want to define these structs yourself, that's cool too)

inline bool checkAABBCollision(AABB& a, AABB& b) {
    if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
    if (a.max.y < b.min.y || a.min.y > b.max.y) return false;

    return true;
}

inline bool checkPtCollision(vec2D& point, AABB rect) {
    if (point.x < rect.min.x || point.x > rect.max.x) return false;
    if (point.y < rect.min.y || point.y > rect.max.y) return false;

    return true;
}

inline bool checkTwoCirclesCollision(vec2D& pos1, float& radius1, vec2D& pos2, float& radius2) {
    return getDist(pos1, pos2) < radius1 + radius2;
}

// courtesy of eJames
// https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection
inline bool checkCircleRectCollision(vec2D &circlePos, float &radius, AABB &rect) {
    vec2D rectCenter = { (rect.min.x + rect.max.x) / 2.0f, (rect.min.y + rect.max.y) / 2.0f };
    float rectWidth = rect.max.x - rect.min.x;
    float rectHeight = rect.max.y - rect.min.y;

    float circleDistanceX = std::abs(circlePos.x - rectCenter.x);
    float circleDistanceY = std::abs(circlePos.y - rectCenter.y);

    // not even close
    if (circleDistanceX > (rectWidth / 2 + radius)) { return false; }
    if (circleDistanceY > (rectHeight / 2 + radius)) { return false; }

    // definitely colliding on x and y
    if (circleDistanceX <= (rectWidth / 2)) { return true; }
    if (circleDistanceY <= (rectHeight / 2)) { return true; }

    // check corners
    float cornerDistX = circleDistanceX - rectWidth / 2;
    float cornerDistY = circleDistanceY - rectHeight / 2;
    float cornerDistance_sq = (cornerDistX * cornerDistX) + (cornerDistY * cornerDistY);

    return (cornerDistance_sq <= (radius * radius));
}

// 1. position of origin of ray
// 2. direction of ray as vector (not normalised, i think)
// 3. AABB coordinates of target rect
// i NEED to learn how to properly use vectors / get familiar with them
inline bool checkRayCollision(vec2D& rayOrigin, vec2D& rayDir, AABB& targetRect, collisionDirectionState& colDirState,
    vec2D& contactPoint, vec2D& contactNormal, float& tHitNear) {
    // get near and far collision vectors
    // d = d_0 + t(v) (0 <= t <= 1)
    vec2D dNear = vec2DSub(targetRect.min, rayOrigin);
    vec2D dFar = vec2DSub(targetRect.max, rayOrigin);
    // its possible here that rayDir is 0, so results in errors
    vec2D tNear = vec2DElementwiseDiv(dNear, rayDir);
    vec2D tFar = vec2DElementwiseDiv(dFar, rayDir);

    // fixed here
    if (std::isnan(tNear.x) || std::isnan(tNear.y)) return false;
    if (std::isnan(tFar.x) || std::isnan(tFar.y)) return false;

    // near has to be near, and far has to be far...
    // since min and max are used to calculate, origin and direction are used to determine actual near and far
    if (tNear.x > tFar.x) std::swap(tNear.x, tFar.x);
    if (tNear.y > tFar.y) std::swap(tNear.y, tFar.y);

    // for a collision, near x must be less than far y, or nyear y must be less than farx (for rects)
    // since nears and fars are proportional to time, use that to compare
    if (tNear.x > tFar.y || tFar.x < tNear.y) return false;

    // check which axis it was hit on for both far and near t
    tHitNear = std::max(tNear.x, tNear.y);
    float tHitFar = std::min(tFar.x, tFar.y);

    if (tHitFar < 0) return false;

    //P_near = P_init + rayDir * tHitNear
    contactPoint = { rayOrigin.x + (tHitNear * rayDir.x), rayOrigin.y + (tHitNear * rayDir.y) };

    // collision on right or left
    if (tNear.x > tNear.y) {
        // initial direction of ray is used to determine which side it collided in
        if (rayDir.x < 0) {
            contactNormal = { 1, 0 };
            colDirState.left = true;
        }
        else {
            contactNormal = { -1, 0 };
            colDirState.right = true;
        }
    } // collision on top or bottom
    else {
        if (rayDir.y < 0) {
            contactNormal = { 0 , 1 };
            colDirState.up = true;
        }
        else {
            contactNormal = { 0, -1 };
            colDirState.down = true;
        }
    }

    return true;
}

#endif