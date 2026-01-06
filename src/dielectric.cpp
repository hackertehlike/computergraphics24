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

/// Ideal dielectric BSDF
class Dielectric : public BSDF {
public:
    Dielectric(const PropertyList &propList) {
        /* Interior IOR (default: BK7 borosilicate optical glass) */
        m_intIOR = propList.getFloat("intIOR", 1.5046f);

        /* Exterior IOR (default: air) */
        m_extIOR = propList.getFloat("extIOR", 1.000277f);
    }

    virtual Color3f eval(const BSDFQueryRecord &) const override {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return Color3f(0.0f);
    }

    virtual float pdf(const BSDFQueryRecord &) const override {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return 0.0f;
    }

    virtual Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const override {

        // Calculate the angle of incidence wrt normal
        float cosThetaI = Frame::cosTheta(bRec.wi);

        // Calculate the Fresnel coefficient
        float R = fresnel(cosThetaI, m_extIOR, m_intIOR);

        bRec.measure = EDiscrete;

        if (sample.x() < R) {
            // Reflect
            // Exactly like in mirror.cpp
            bRec.wo = Vector3f(-bRec.wi.x(), -bRec.wi.y(), bRec.wi.z());
            bRec.eta = 1.0f;
            return Color3f(1.f);
        } else {
            // Refract
            Normal3f normal = Normal3f(0, 0, 1);
            float eta = m_extIOR / m_intIOR;
            if(cosThetaI <= 0) {
                cosThetaI = -cosThetaI;
                normal = -normal;
                eta = 1.f / eta;
            }

            // Taken from the handout slides
            // Max, because PBRT said so
            float sinThetaI = sqrt(std::max(0.f, 1.f - cosThetaI * cosThetaI));
            float sinThetaT = eta * sinThetaI;
            float cosThetaT = sqrt(std::max(0.f, 1.f - sinThetaT * sinThetaT));

            bRec.eta = eta;
            bRec.wo = -cosThetaT * normal - eta * (bRec.wi - cosThetaI * normal);

            return Color3f(eta * eta);
        }
    }


    virtual std::string toString() const override {
        return tfm::format(
            "Dielectric[\n"
            "  intIOR = %f,\n"
            "  extIOR = %f\n"
            "]",
            m_intIOR, m_extIOR);
    }
private:
    float m_intIOR, m_extIOR;
};

NORI_REGISTER_CLASS(Dielectric, "dielectric");
NORI_NAMESPACE_END
