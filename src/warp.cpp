/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <nori/warp.h>
#include <nori/vector.h>
#include <nori/frame.h>

NORI_NAMESPACE_BEGIN

Vector3f Warp::sampleUniformHemisphere(Sampler *sampler, const Normal3f &pole) {
    // Naive implementation using rejection sampling
    Vector3f v;
    do {
        v.x() = 1.f - 2.f * sampler->next1D();
        v.y() = 1.f - 2.f * sampler->next1D();
        v.z() = 1.f - 2.f * sampler->next1D();
    } while (v.squaredNorm() > 1.f);

    if (v.dot(pole) < 0.f)
        v = -v;
    v /= v.norm();

    return v;
}

Point2f Warp::squareToUniformSquare(const Point2f &sample) {
    return sample;
}

float Warp::squareToUniformSquarePdf(const Point2f &sample) {
    return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}

Point2f Warp::squareToUniformDisk(const Point2f &sample) {
    float r = sqrtf(sample.x());
    float theta = 2 * M_PI * sample.y();
    return Vector2f(r * cosf(theta), r * sinf(theta));
}

float Warp::squareToUniformDiskPdf(const Point2f &p) {
    return (p.squaredNorm() <= 1) ? (1.0f / M_PI) : 0.0f;
}

Vector3f Warp::squareToUniformSphereCap(const Point2f &sample, float cosThetaMax) {
    float z = (1 - sample.x()) * cosThetaMax + sample.x();
    float sinTheta = sqrtf(1.0f - z * z);
    float theta = 2 * M_PI * sample.y();
    float x = sinTheta * cosf(theta);
    float y = sinTheta * sinf(theta);
    return Vector3f(x, y, z);
}

float Warp::squareToUniformSphereCapPdf(const Vector3f &v, float cosThetaMax) {
    return (v.z() >= cosThetaMax) ? (1.0f / (2.0f * M_PI * (1.0f - cosThetaMax))) : 0.0f;
}

Vector3f Warp::squareToUniformSphere(const Point2f &sample) {
    float z = 1 - 2 * sample.x();
    float r = sqrtf(1 - z * z);
    float theta = 2 * M_PI * sample.y();
    return Vector3f(r * cosf(theta), r * sinf(theta), z);
}

float Warp::squareToUniformSpherePdf(const Vector3f &v) {
    return (v.squaredNorm() <= 1) ? (1.0f / (4.0f * M_PI)) : 0.0f;
}

Vector3f Warp::squareToUniformHemisphere(const Point2f &sample) {
    float z = sample.x();
    float r = std::max(0.0f, sqrtf(1 - z * z));
    float phi = 2 * M_PI * sample.y();
    return Vector3f(r * cosf(phi), r * sinf(phi), z);
}

float Warp::squareToUniformHemispherePdf(const Vector3f &v) {
    return (v.z() >= 0) ? (1.0f / (2.0f * M_PI)) : 0.0f;
}

Vector3f Warp::squareToCosineHemisphere(const Point2f &sample) {
    // Compute the angle theta and the radius r
    float theta = 2 * M_PI * sample.y();  // Azimuthal angle
    float r = sqrtf(1 - sample.x());       // Radius for disk sampling
    float z = sqrtf(sample.x());           // Cosine weighted z component (height)
    
    // Convert polar coordinates to Cartesian coordinates
    float x = r * cosf(theta);
    float y = r * sinf(theta);
    
    return Vector3f(x, y, z);
}

float Warp::squareToCosineHemispherePdf(const Vector3f &v) {
    return (v.z() >= 0) ? (v.z() / M_PI) : 0.0f;
}


Vector3f Warp::squareToBeckmann(const Point2f &sample, float alpha) {
    // Sample phi uniformly in [0, 2pi]
    float phi = 2 * M_PI * sample.y();

    // Sample theta using the inverse CDF of the Beckmann distribution
    float tanTheta2 = -alpha * alpha * std::log(1 - sample.x());
    float theta = std::atan(std::sqrt(tanTheta2));

    // Convert spherical coordinates to Cartesian
    float sinTheta = std::sin(theta);
    float cosTheta = std::cos(theta);

    return Vector3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}

float Warp::squareToBeckmannPdf(const Vector3f &m, float alpha) {
    float cosTheta = m.z();

    // Avoid division by zero
    if (cosTheta <= 0) return 0.0f;

    float tanTheta2 = (1 - cosTheta * cosTheta) / (cosTheta * cosTheta);
    float exponent = -tanTheta2 / (alpha * alpha);
    float pdf = std::exp(exponent) / (M_PI * alpha * alpha * std::pow(cosTheta, 3));

    return pdf;
}


Vector3f Warp::squareToUniformTriangle(const Point2f &sample) {
    float su1 = sqrtf(sample.x());
    float u = 1.f - su1, v = sample.y() * su1;
    return Vector3f(u,v,1.f-u-v);
}

NORI_NAMESPACE_END
