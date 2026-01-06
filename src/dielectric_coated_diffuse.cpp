#include <nori/bsdf.h>
#include <nori/frame.h>
#include <nori/common.h>
#include <nori/warp.h>
#include <cmath>

NORI_NAMESPACE_BEGIN

class DielectricCoatedDiffuse : public BSDF {
public:
    DielectricCoatedDiffuse(const PropertyList &props) {
        m_extIOR = props.getFloat("extIOR", 1.0f); // Default to air
        m_intIOR = props.getFloat("intIOR", 1.5f); // Default to glass
        m_albedo = props.getColor("albedo", Color3f(0.5f)); // Default albedo
        // m_thickness = props.getFloat("thickness", 1.0f); // Default thickness
        // m_absorption = props.getColor("absorption", Color3f(0.0f)); // No absorption by default
    }

    virtual Color3f eval(const BSDFQueryRecord &bRec) const override {
        // // Only handle directions above the surface
        // if (Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
        //     return Color3f(0.0f);

        // // Outer layer reflection (Fresnel term)
        // float F_in = fresnel(Frame::cosTheta(bRec.wi), m_extIOR, m_intIOR);
        // Color3f outerReflection = F_in; // fs1(ωi → ωo)

        // // Compute refraction through the dielectric layer
        // float eta = m_extIOR / m_intIOR;
        // float sinThetaTSqr = eta * eta * (1 - Frame::cosTheta(bRec.wi) * Frame::cosTheta(bRec.wi));

        // if (sinThetaTSqr > 1.0f) { // Total internal reflection (TIR)
        //     return outerReflection;
        // }

        // // Fresnel transmission (T(θ))
        // float T_in = 1.0f - F_in;

        // float F_out = fresnel(Frame::cosTheta(bRec.wo), m_intIOR, m_extIOR);
        // float T_out = 1.0f - F_out;

        // // Medium attenuation using Beer-Lambert law
        // float d = m_thickness / std::max(Frame::cosTheta(bRec.wi), 0.01f);
        // Color3f attenuation = exp(-m_absorption * d);

        // // Inner diffuse layer contribution (Lambertian BRDF)
        // Color3f innerDiffuse = m_albedo * INV_PI; // fs2(ω′i → ω′o)
        // innerDiffuse *= T_in * T_out; // T(θ)² * a(θi, θo)

        // // Final BSDF value
        // return outerReflection + innerDiffuse;

        return Color3f(0.0f);
    }


    virtual float pdf(const BSDFQueryRecord &bRec) const override {
            
        // Cosine-weighted PDF for Lambertian sampling
        // float basePdf = Frame::cosTheta(bRec.wo) * INV_PI;

        // // Compute refractive indices and angles
        // float eta = m_extIOR / m_intIOR;  // Ratio of indices of refraction
        // float cosThetaI = Frame::cosTheta(bRec.wi);
        // float sinThetaTSqr = eta * eta * (1.0f - cosThetaI * cosThetaI);

        // // Check for total internal reflection (no transmission possible)
        // if (sinThetaTSqr > 1.0f) {
        //     return 0.0f;
        // }

        // float cosThetaT = std::sqrt(1.0f - sinThetaTSqr);

        // // Calculate Fresnel reflection and transmission probabilities
        // float F = fresnel(cosThetaI, m_extIOR, m_intIOR);
        // float T = 1.0f - F;

        // // Adjust PDF by the change in solid angle due to refraction
        // float refractiveScaling = (eta * eta) * (cosThetaT / cosThetaI);

        // // Return the corrected PDF for transmitted light
        // // including the Fresnel transmission probability and angular density correction
        // return basePdf * T * refractiveScaling;

        return 0.0;
    }

    virtual Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const override {
        // Sample reflection or transmission based on Fresnel term
        float F_in = fresnel(Frame::cosTheta(bRec.wi), m_extIOR, m_intIOR);

        if (sample.x() < F_in) {
            // Reflect
            bRec.wo = Vector3f(
                -bRec.wi.x(),
                -bRec.wi.y(),
                bRec.wi.z()
            );
            bRec.measure = EDiscrete;
            bRec.eta = 1.0f;
            return Color3f(1.0); // Return Fresnel reflection
        } else {
            // Sample the diffuse layer
            bRec.wo = Warp::squareToCosineHemisphere(sample);
            bRec.eta = 1.0f;
            bRec.measure = ESolidAngle;

            if (Frame::cosTheta(bRec.wo) <= 0)
                return Color3f(0.0f);

            // // Transmission + Diffuse contribution
            // float T_in = 1.0f - F_in;

            // float F_out = fresnel(Frame::cosTheta(bRec.wo), m_intIOR, m_extIOR);
            // float T_out = 1.0f - F_out;
            // float d = m_thickness / std::max(Frame::cosTheta(bRec.wi), 0.01f);
            // Color3f attenuation = exp(-m_absorption * d);

            // return m_albedo / M_PI * T_in * T_out * attenuation;
            return m_albedo / M_PI;
        }
    }

    virtual std::string toString() const override {
        return tfm::format(
            "DielectricCoatedDiffuse[\n"
            "  extIOR = %f,\n"
            "  intIOR = %f,\n"
            "  albedo = %s,\n"
            "  thickness = %f,\n"
            "  absorption = %s\n"
            "]",
            m_extIOR, m_intIOR, m_albedo.toString()
            // m_thickness, m_absorption.toString()
        );
    }

private:
    float m_extIOR, m_intIOR;       // Indices of refraction
    Color3f m_albedo;              // Diffuse albedo
    // float m_thickness;             // Layer thickness
    // Color3f m_absorption;          // Absorption coefficient
};

NORI_REGISTER_CLASS(DielectricCoatedDiffuse, "dielectric_coated_diffuse");
NORI_NAMESPACE_END
