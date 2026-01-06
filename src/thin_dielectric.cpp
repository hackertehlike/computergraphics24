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
#include <nori/common.h>

NORI_NAMESPACE_BEGIN

/// Thin dielectric BSDF for single-sheet surfaces like windows
class ThinDielectric : public BSDF {
public:
    ThinDielectric(const PropertyList &propList) {
        /* Interior IOR (default: BK7 borosilicate optical glass) */
        m_intIOR = propList.getFloat("intIOR", 1.5046f);
        /* Exterior IOR (default: air) */
        m_extIOR = propList.getFloat("extIOR", 1.000277f);
        
        /* Calculate eta for internal/external IOR ratio */
        m_eta = m_intIOR / m_extIOR;
    }

    virtual Color3f eval(const BSDFQueryRecord &) const override {
        // Discrete BRDFs evaluate to zero
        return Color3f(0.0f);
    }

    virtual float pdf(const BSDFQueryRecord &) const override {
        // Discrete BRDFs evaluate to zero
        return 0.0f;
    }

    virtual Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const override {
        float cosThetaI = Frame::cosTheta(bRec.wi);
        float R = fresnelThinDielectric(cosThetaI, m_extIOR, m_intIOR);
        float T = 1 - R;

        bRec.measure = EDiscrete;
        if (sample.x() < R) {
            // Reflect
            bRec.wo = Vector3f(-bRec.wi.x(), -bRec.wi.y(), bRec.wi.z());
            bRec.eta = 1.0f;
            return 1.0;
        } else {
            // Transmit without changing direction
            bRec.wo = -bRec.wi;
            bRec.eta = 1.0f;
            return 1.0;
        }
    }

    virtual std::string toString() const override {
        return tfm::format(
            "ThinDielectric[\n"
            "  intIOR = %f,\n"
            "  extIOR = %f,\n"
            "  eta = %f,\n"
            "  specularReflectance = %s,\n"
            "  specularTransmittance = %s\n"
            "]",
            m_intIOR, m_extIOR, m_eta
        );
    }

private:
    float m_intIOR, m_extIOR, m_eta;

float fresnelThinDielectric(float cosThetaI, float etaI, float etaT) const {
    float R = fresnel(cosThetaI, etaI, etaT);
    float T = 1.f - R;

    if(R > 0.9999f) {
        // Total internal reflection
        return 1.0f;
    }

    // As stated in PBRT
    R = R + T * T * R / (1.f - R * R);
    return R;
} 


};



NORI_REGISTER_CLASS(ThinDielectric, "thin_dielectric");
NORI_NAMESPACE_END
