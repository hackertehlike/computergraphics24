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

#include <nori/bsdf.h>
#include <nori/frame.h>
#include <nori/warp.h>
#include <nori/common.h>

NORI_NAMESPACE_BEGIN

class Microfacet : public BSDF {
public:
    Microfacet(const PropertyList &propList) {
        /* RMS surface roughness */
        m_alpha = propList.getFloat("alpha", 0.1f);

        /* Interior IOR (default: BK7 borosilicate optical glass) */
        m_intIOR = propList.getFloat("intIOR", 1.5046f);

        /* Exterior IOR (default: air) */
        m_extIOR = propList.getFloat("extIOR", 1.000277f);

        /* Albedo of the diffuse base material (a.k.a "kd") */
        m_kd = propList.getColor("kd", Color3f(0.5f));

        /* To ensure energy conservation, we must scale the 
           specular component by 1-kd. 

           While that is not a particularly realistic model of what 
           happens in reality, this will greatly simplify the 
           implementation. Please see the course staff if you're 
           interested in implementing a more realistic version 
           of this BRDF. */
        m_ks = 1 - m_kd.maxCoeff();
    }

    /// Evaluate the microfacet normal distribution D
    float evalBeckmann(const Normal3f &m) const {
        float temp = Frame::tanTheta(m) / m_alpha,
              ct = Frame::cosTheta(m), ct2 = ct*ct;

        return std::exp(-temp*temp) 
            / (M_PI * m_alpha * m_alpha * ct2 * ct2);
    }

    /// Evaluate Smith's shadowing-masking function G1 
    float smithBeckmannG1(const Vector3f &v, const Normal3f &m) const {
        float tanTheta = Frame::tanTheta(v);

        /* Perpendicular incidence -- no shadowing/masking */
        if (tanTheta == 0.0f)
            return 1.0f;

        /* Can't see the back side from the front and vice versa */
        if (m.dot(v) * Frame::cosTheta(v) <= 0)
            return 0.0f;

        float a = 1.0f / (m_alpha * tanTheta);
        if (a >= 1.6f)
            return 1.0f;
        float a2 = a * a;

        /* Use a fast and accurate (<0.35% rel. error) rational
           approximation to the shadowing-masking function */
        return (3.535f * a + 2.181f * a2) 
             / (1.0f + 2.276f * a + 2.577f * a2);
    }

    /// Evaluate the BRDF for the given pair of directions
    virtual Color3f eval(const BSDFQueryRecord &bRec) const override {
    	// Ensure that we are in the correct hemisphere
        if (Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
            return Color3f(0.0f);

        // Calculate the half-vector
        Vector3f wh = (bRec.wi + bRec.wo).normalized();

        // Diffuse component
        Color3f diffuse = m_kd / M_PI;

        // Specular component
        float D = evalBeckmann(wh);
        float F = fresnel(bRec.wi.dot(wh), m_extIOR, m_intIOR);
        float G = smithBeckmannG1(bRec.wi, wh) * smithBeckmannG1(bRec.wo, wh);

        float cosThetaI = Frame::cosTheta(bRec.wi);
        float cosThetaO = Frame::cosTheta(bRec.wo);

        Color3f specular = m_ks * D * F * G / (4.0f * cosThetaI * cosThetaO);

        // Return the combined result
        return diffuse + specular;
    }

    /// Evaluate the sampling density of \ref sample() wrt. solid angles
    virtual float pdf(const BSDFQueryRecord &bRec) const override {

        if (Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0) return 0.0f;

        // Calculate the half-vector
        Vector3f wh = (bRec.wi + bRec.wo).normalized();

        // Specular component PDF
        float D = evalBeckmann(wh);  // Beckmann distribution function
        float cosThetaH = Frame::cosTheta(wh);
        float jacobian = 1.0f / (4.0f * bRec.wo.dot(wh));
        float pdf_specular = m_ks * D * cosThetaH * jacobian;

        // Diffuse component PDF
        float pdf_diffuse = (1.0f - m_ks) * Frame::cosTheta(bRec.wo) / M_PI;

        // Total PDF as the weighted sum of specular and diffuse components
        return pdf_specular + pdf_diffuse;
    }

    /// Sample the BRDF
    virtual Color3f sample(BSDFQueryRecord &bRec, const Point2f &_sample) const override {
        bRec.measure = ESolidAngle;

        // Determine if we're sampling specular or diffuse component based on m_ks
        if (_sample.x() < m_ks) {
            // Specular sampling: Sample the half-vector (wh) using Beckmann distribution
            Vector3f wh = Warp::squareToBeckmann(Point2f(_sample.x() / m_ks, _sample.y()), m_alpha);
            bRec.wo = 2.f * bRec.wi.dot(wh) * wh - bRec.wi;
        } else {
            // Diffuse sampling: Generate a cosine-weighted direction
            bRec.wo = Warp::squareToCosineHemisphere(Point2f((_sample.x() - m_ks) / (1 - m_ks), _sample.y()));
        }

        // Return black if the outgoing direction is below the surface
        if (Frame::cosTheta(bRec.wo) <= 0) return Color3f(0.0f);

        // Calculate the cosine term and return the evaluated BRDF scaled by the PDF
        return eval(bRec) * Frame::cosTheta(bRec.wo) / pdf(bRec);
    }

    virtual std::string toString() const override {
        return tfm::format(
            "Microfacet[\n"
            "  alpha = %f,\n"
            "  intIOR = %f,\n"
            "  extIOR = %f,\n"
            "  kd = %s,\n"
            "  ks = %f\n"
            "]",
            m_alpha,
            m_intIOR,
            m_extIOR,
            m_kd.toString(),
            m_ks
        );
    }
private:
    float m_alpha;
    float m_intIOR, m_extIOR;
    float m_ks;
    Color3f m_kd;
};

NORI_REGISTER_CLASS(Microfacet, "microfacet");
NORI_NAMESPACE_END
