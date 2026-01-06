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

/// Ideal smooth conductor BSDF
class SmoothConductor : public BSDF {
public:
    SmoothConductor(const PropertyList &props) {
        /* Complex IOR components for conductor (real part eta, imaginary part k) */
        m_eta = props.getColor("eta", Color3f(0.0f)); // Real part of IOR
        m_k   = props.getColor("k", Color3f(0.0f));   // Imaginary part of IOR
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
        if (Frame::cosTheta(bRec.wi) <= 0)
            return Color3f(0.0f);

        // Perfect specular reflection
        bRec.wo = Vector3f(-bRec.wi.x(), -bRec.wi.y(), bRec.wi.z());
        bRec.measure = EDiscrete;
        bRec.eta = 1.0f;

        // Compute Fresnel term for conductor using complex IOR
        float cosThetaI = Frame::cosTheta(bRec.wi);
        return fresnelConductor(cosThetaI, m_eta, m_k);
    }

    virtual std::string toString() const override {
        return tfm::format(
            "SmoothConductor[\n"
            "  eta = %s,\n"
            "  k = %s\n"
            "]",
            m_eta.toString(), m_k.toString()
        );
    }

private:
    Color3f m_eta; // Real part of the complex index of refraction
    Color3f m_k;   // Imaginary part of the complex index of refraction

    
Color3f fresnelConductor(float cosThetaI, const Color3f &eta, const Color3f &k) const {
    cosThetaI = std::clamp(cosThetaI, 0.0f, 1.0f);  // Clamp cosThetaI to [0, 1]
    float sinThetaI2 = 1.0f - cosThetaI * cosThetaI;

    // Complex refractive indices for each color channel
    using Complex = std::complex<float>;
    Complex etaR(eta.r(), k.r());
    Complex etaG(eta.g(), k.g());
    Complex etaB(eta.b(), k.b());

    // Helper lambda to calculate Fresnel reflectance for each channel
    auto fresnelChannel = [&](const Complex &etaComplex) -> float {
        Complex sinThetaT2 = sinThetaI2 / (etaComplex * etaComplex);
        Complex cosThetaT = std::sqrt(Complex(1.0f) - sinThetaT2);

        // Parallel and perpendicular reflection coefficients
        Complex rParallel = (etaComplex * cosThetaI - cosThetaT) /
                            (etaComplex * cosThetaI + cosThetaT);
        Complex rPerpendicular = (cosThetaI - etaComplex * cosThetaT) /
                                 (cosThetaI + etaComplex * cosThetaT);

        // Reflectance (magnitude squared of the coefficients)
        return 0.5f * (std::norm(rParallel) + std::norm(rPerpendicular));
    };

    // Calculate Fresnel reflectance for each RGB channel
    return Color3f(
        fresnelChannel(etaR),
        fresnelChannel(etaG),
        fresnelChannel(etaB)
    );
}


};

NORI_REGISTER_CLASS(SmoothConductor, "smooth_conductor");
NORI_NAMESPACE_END
