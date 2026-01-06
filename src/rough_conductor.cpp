#include <nori/bsdf.h>
#include <nori/frame.h>
#include <nori/warp.h>
#include <nori/common.h>
#include <complex>

NORI_NAMESPACE_BEGIN

/// Rough conductor material using microfacet model with Beckmann distribution
class RoughConductor : public BSDF {
public:
    RoughConductor(const PropertyList &props) {
        /* Complex IOR components for conductor (real part eta, imaginary part k) */
        m_eta = props.getColor("eta", Color3f(0.0f)); // Real part of IOR
        m_k = props.getColor("k", Color3f(0.0f));     // Imaginary part of IOR
        m_alpha = props.getFloat("alpha", 0.1f);      // Roughness parameter (Beckmann alpha)
    }

    virtual Color3f eval(const BSDFQueryRecord &bRec) const override {
        if (Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
            return Color3f(0.0f);

        // Half-vector
        Vector3f wh = (bRec.wi + bRec.wo).normalized();
        float cosThetaI = Frame::cosTheta(bRec.wi);
        float cosThetaO = Frame::cosTheta(bRec.wo);
        float cosThetaH = Frame::cosTheta(wh);

        // D: Beckmann distribution
        float D = beckmannDistribution(cosThetaH);

        // Fresnel term
        Color3f F = fresnelConductor(bRec.wi.dot(wh), m_eta, m_k);

        // G: Smith masking-shadowing
        float G = smithG1(bRec.wi, wh) * smithG1(bRec.wo, wh);

        // Calculate the BRDF value
        return F * D * G / (4.0f * cosThetaI * cosThetaO);
    }

    virtual float pdf(const BSDFQueryRecord &bRec) const override {
        if (Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
            return 0.0f;

        // Calculate half-vector and PDF for Beckmann distribution
        Vector3f wh = (bRec.wi + bRec.wo).normalized();
        float D = beckmannDistribution(Frame::cosTheta(wh));
        float pdfWH = D * Frame::cosTheta(wh);

        // Apply change of variables
        return pdfWH / (4.0f * std::abs(bRec.wo.dot(wh)));
    }

    virtual Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const override {
        if (Frame::cosTheta(bRec.wi) <= 0)
            return Color3f(0.0f);

        // Sample the Beckmann distribution for the half-vector
        Vector3f wh = Warp::squareToBeckmann(sample, m_alpha);
        bRec.wo = 2.0f * bRec.wi.dot(wh) * wh - bRec.wi;

        // Ensure the outgoing direction is in the correct hemisphere
        if (Frame::cosTheta(bRec.wo) <= 0)
            return Color3f(0.0f);

        // Return the evaluated BRDF scaled by the PDF
        return eval(bRec) * Frame::cosTheta(bRec.wo) / pdf(bRec);
    }

    virtual std::string toString() const override {
        return tfm::format(
            "RoughConductor[\n"
            "  eta = %s,\n"
            "  k = %s,\n"
            "  alpha = %f\n"
            "]",
            m_eta.toString(), m_k.toString(), m_alpha
        );
    }

private:
    Color3f m_eta; // Real part of the complex index of refraction
    Color3f m_k;   // Imaginary part of the complex index of refraction
    float m_alpha; // Roughness parameter

    // Beckmann Microfacet Distribution
    float beckmannDistribution(float cosThetaH) const {
        float tanThetaH2 = (1.0f - cosThetaH * cosThetaH) / (cosThetaH * cosThetaH);
        float alpha2 = m_alpha * m_alpha;
        return std::exp(-tanThetaH2 / alpha2) / (M_PI * alpha2 * cosThetaH * cosThetaH * cosThetaH * cosThetaH);
    }

    // Smith's masking-shadowing function for Beckmann
    float smithG1(const Vector3f &v, const Vector3f &wh) const {
        float tanTheta = Frame::tanTheta(v);
        if (tanTheta == 0.0f)
            return 1.0f;
        float a = 1.0f / (m_alpha * tanTheta);
        return (a < 1.6f) ? (3.535f * a + 2.181f * a * a) / (1.0f + 2.276f * a + 2.577f * a * a) : 1.0f;
    }

    // Fresnel function for conductors with complex IOR
    Color3f fresnelConductor(float cosThetaI, const Color3f &eta, const Color3f &k) const {
        cosThetaI = std::clamp(cosThetaI, 0.0f, 1.0f);
        float sinThetaI2 = 1.0f - cosThetaI * cosThetaI;

        using Complex = std::complex<float>;
        Complex etaR(eta.r(), k.r());
        Complex etaG(eta.g(), k.g());
        Complex etaB(eta.b(), k.b());

        auto fresnelChannel = [&](const Complex &etaComplex) -> float {
            Complex sinThetaT2 = sinThetaI2 / (etaComplex * etaComplex);
            Complex cosThetaT = std::sqrt(Complex(1.0f) - sinThetaT2);
            Complex rParallel = (etaComplex * cosThetaI - cosThetaT) /
                                (etaComplex * cosThetaI + cosThetaT);
            Complex rPerpendicular = (cosThetaI - etaComplex * cosThetaT) /
                                     (cosThetaI + etaComplex * cosThetaT);
            return 0.5f * (std::norm(rParallel) + std::norm(rPerpendicular));
        };

        return Color3f(
            fresnelChannel(etaR),
            fresnelChannel(etaG),
            fresnelChannel(etaB)
        );
    }
};

NORI_REGISTER_CLASS(RoughConductor, "rough_conductor");
NORI_NAMESPACE_END
