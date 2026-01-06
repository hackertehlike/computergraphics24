/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Romain Prévost

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

#include <nori/shape.h>
#include <nori/bsdf.h>
#include <nori/emitter.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class Sphere : public Shape {
public:
    Sphere(const PropertyList & propList) {
        m_position = propList.getPoint3("center", Point3f());
        m_radius = propList.getFloat("radius", 1.f);

        m_bbox.expandBy(m_position - Vector3f(m_radius));
        m_bbox.expandBy(m_position + Vector3f(m_radius));
    }

    virtual BoundingBox3f getBoundingBox(uint32_t index) const override { return m_bbox; }

    virtual Point3f getCentroid(uint32_t index) const override { return m_position; }

    virtual bool rayIntersect(uint32_t index, const Ray3f &ray, float &u, float &v, float &t) const override {

        Vector3f oc = ray.o - m_position;
        
        double a = ray.d.dot(ray.d);
        double b = 2 * oc.dot(ray.d);
        double c = oc.dot(oc) - m_radius * m_radius;

        double discriminant = b*b - 4*a*c;

        if(discriminant < 0) {
            return false;
        }
        
        double sqrtDiscriminant = std::sqrt(discriminant);

        // Calculate the two potential intersection points
        double t0 = (-b - sqrtDiscriminant) / (2 * a);
        double t1 = (-b + sqrtDiscriminant) / (2 * a);

        // Check if the smaller root is a valid intersection
        if (t0 >= ray.mint && t0 <= ray.maxt) {
            t = t0;  // t0 is within the valid range
            return true;
        } else if (t1 >= ray.mint && t1 <= ray.maxt) {
            t = t1;  // t0 is not valid, but t1 is within the valid range
            return true;
        }

        return false;  // Neither t0 nor t1 is a valid intersection
    }
    
    virtual void setHitInformation(uint32_t index, const Ray3f &ray, Intersection &its) const override {
        // Compute the intersection point
        Point3f collision = ray(its.t);
        its.p = collision;

        // Compute the normal as the normalized vector from the sphere's center to the intersection point
        Vector3f n = (collision - m_position).normalized();
        Frame frame(n);

        // Set the shading frame and geometric frame
        its.shFrame = frame;
        its.geoFrame = frame;

        // Compute spherical coordinates for UV mapping
        float phi = std::atan2(n.y(), n.x());
        if (phi < 0)
            phi += 2 * M_PI; // Normalize phi to be within [0, 2π]

        float theta = std::acos(n.z());

        // Convert spherical coordinates to UV coordinates
        float u = phi / (2 * M_PI); // Normalize phi to [0, 1]
        float v = theta / M_PI;      // Normalize theta to [0, 1]

        its.uv = Point2f(u, v);
    }


    virtual void sampleSurface(ShapeQueryRecord & sRec, const Point2f & sample) const override {
        Vector3f q = Warp::squareToUniformSphere(sample);
        sRec.p = m_position + m_radius * q;
        sRec.n = q;
        sRec.pdf = std::pow(1.f/m_radius,2) * Warp::squareToUniformSpherePdf(Vector3f(0.0f,0.0f,1.0f));
    }
    virtual float pdfSurface(const ShapeQueryRecord & sRec) const override {
        return std::pow(1.f/m_radius,2) * Warp::squareToUniformSpherePdf(Vector3f(0.0f,0.0f,1.0f));
    }


    virtual std::string toString() const override {
        return tfm::format(
                "Sphere[\n"
                "  center = %s,\n"
                "  radius = %f,\n"
                "  bsdf = %s,\n"
                "  emitter = %s\n"
                "]",
                m_position.toString(),
                m_radius,
                m_bsdf ? indent(m_bsdf->toString()) : std::string("null"),
                m_emitter ? indent(m_emitter->toString()) : std::string("null"));
    }

protected:
    Point3f m_position;
    float m_radius;
};

NORI_REGISTER_CLASS(Sphere, "sphere");
NORI_NAMESPACE_END
