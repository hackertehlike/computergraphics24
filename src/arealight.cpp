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

#include <nori/emitter.h>
#include <nori/warp.h>
#include <nori/shape.h>

NORI_NAMESPACE_BEGIN

class AreaEmitter : public Emitter {
public:
    AreaEmitter(const PropertyList &props) {
        m_radiance = props.getColor("radiance");
    }

    virtual std::string toString() const override {
        return tfm::format(
                "AreaLight[\n"
                "  radiance = %s,\n"
                "]",
                m_radiance.toString());
    }

    virtual Color3f eval(const EmitterQueryRecord & lRec) const override {
        if(!m_shape)
            throw NoriException("There is no shape attached to this Area light!");

        // Get the direction from the intersection point to the light source
        Vector3f wi = lRec.wi;

        // Check if the light source is visible from the intersection point
        float cosTheta = lRec.n.dot(-wi);
        if (cosTheta <= 0) {
            // If the emitter is back-facing, return zero radiance
            return Color3f(0.0f);
        }
        // If no occlusion, return the radiance with cosine weighting
        return m_radiance;
    }

    virtual Color3f sample(EmitterQueryRecord & lRec, const Point2f & sample) const override {
        if(!m_shape)
            throw NoriException("There is no shape attached to this Area light!");

        
        // Sample a point on the shape surface
        ShapeQueryRecord sRec(lRec.ref);
        m_shape->sampleSurface(sRec, sample);

        // Set up the emitter query record
        lRec.p = sRec.p;
        lRec.n = sRec.n;
        lRec.wi = (lRec.p - lRec.ref).normalized();
        lRec.pdf = sRec.pdf;
        lRec.shadowRay = Ray3f(lRec.ref, lRec.wi, Epsilon, (lRec.p - lRec.ref).norm() - Epsilon);

        // Check for zero PDF and return radiance if valid
        float pdfVal = pdf(lRec);
        if (pdfVal == 0.0f)
            return Color3f(0.0f);
        
        return eval(lRec) / pdfVal;
    }

    virtual float pdf(const EmitterQueryRecord &lRec) const override {

       // As seen on https://pbr-book.org/3ed-2018/Light_Transport_I_Surface_Reflection/Sampling_Light_Sources#SamplingShapes
       if (!m_shape)
            throw NoriException("There is no shape attached to this Area light!");

        float cosTheta = lRec.n.dot(-lRec.wi);
        if (cosTheta <= 0)
            return 0.0f;

        // Set up the shape query record and get the PDF
        ShapeQueryRecord sRec(lRec.ref, lRec.p);

        return  m_shape->pdfSurface(sRec) * (lRec.p - lRec.ref).squaredNorm() / cosTheta;
    }


    virtual Color3f samplePhoton(Ray3f &ray, const Point2f &sample1, const Point2f &sample2) const override {
        if(!m_shape)
            throw NoriException("There is no shape attached to this Area light!");

        // Sample a point on the shape's surface
        ShapeQueryRecord sRec;
        m_shape->sampleSurface(sRec, sample1);

        // Sample a cosine-weighted direction over the hemisphere around the normal
        Vector3f sampledDirection = Warp::squareToCosineHemisphere(sample2);
        Vector3f worldDirection = Frame(sRec.n).toWorld(sampledDirection);

        // Setup the ray from the sampled point in the sampled direction
        ray = Ray3f(sRec.p, worldDirection, Epsilon, std::numeric_limits<float>::infinity());
        
        if (sRec.pdf <= 0) return 0;

        Color3f power = M_PI * m_radiance / sRec.pdf;

        return power;
    }



protected:
    Color3f m_radiance;
};

NORI_REGISTER_CLASS(AreaEmitter, "area")
NORI_NAMESPACE_END