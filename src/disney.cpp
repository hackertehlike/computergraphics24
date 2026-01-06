/*
Reference: https://cseweb.ucsd.edu/~tzli/cse272/wi2023/homework1.pdf and the accompanying source code.
*/
#include <nori/bsdf.h>
#include <nori/frame.h>
#include <nori/texture.h>
#include <nori/warp.h>
#include <nori/common.h>

NORI_NAMESPACE_BEGIN

class Disney : public BSDF {
public:
    Disney(const PropertyList &propList) {
    PropertyList l;
    l.setColor("value", propList.getColor("baseColour", Color3f(0.5f)));
    m_baseColour = static_cast<Texture<Color3f> *>(
        NoriObjectFactory::createInstance("constant_color", l)
    );

    m_roughness = propList.getFloat("roughness", 0.5f);
    //m_specular = propList.getFloat("specular", 0.5f);
    //m_metallic = propList.getFloat("metallic", 0.0f);
    m_sheen = propList.getFloat("sheen", 0.0f);
    m_sheenTint = propList.getFloat("sheenTint", 0.5f);
    m_clearcoat = propList.getFloat("clearcoat", 0.0f);
    m_clearcoatGloss = propList.getFloat("clearcoatGloss", 1.0f);
}


    /* ======================= DIFFUSE =======================*/
    /* =======================================================*/

    /**
     * @brief Compute the Disney Diffuse BRDF component.
     *
     * This method calculates the diffuse reflection contribution for a given pair of directions (`wi` and `wo`).
     * Disney Diffuse is based on a modified Schlick-Fresnel approximation, so it is not a true Lambertian BRDF.
     *
     * @param bRec A BSDFQueryRecord containing the incoming (`wi`) and outgoing (`wo`) directions, 
     *             along with the UV coordinates for texture evaluation.
     * @return The computed Disney diffuse BRDF value as a `Color3f`.
     */
    Color3f f_baseDiffuse(const BSDFQueryRecord &bRec) const {

        // std::cout << "f_baseDiffuse: Evaluating Disney Diffuse BRDF" << std::endl;

        Vector3f wi = bRec.wi;
        Vector3f wo = bRec.wo;
        Vector3f h = wh(wi, wo);

        // Clamping for numerical stability
        float n_dot_in = std::clamp(std::abs(Frame::cosTheta(wi)), 0.f, 1.f);
        float n_dot_out = std::clamp(std::abs(Frame::cosTheta(wo)), 0.f, 1.f);

        float roughness = std::clamp(m_roughness, 0.01f, 1.f);

        float F_D90 = 0.5f + 2.f * roughness * h.dot(wo) * h.dot(wo);
        float F_D_in = 1.f + (F_D90 - 1.f) * std::pow(1.f - n_dot_in, 5.f);
        float F_D_out = 1.f + (F_D90 - 1.f) * std::pow(1.f - n_dot_out, 5.f);

        // // Debugging: Print intermediate values


        return m_baseColour->eval(bRec.uv) / M_PI * F_D_in * F_D_out * n_dot_out;
    }

    
    /**
     * @brief Evaluate the diffuse component of the Disney BRDF.
     *
     * This function evaluates the Disney Diffuse BRDF model for a given pair of directions (`wi` and `wo`).
     * It ensures that light is only reflected when both directions are above the surface (`cosTheta > 0`).
     * 
     * In the full Principled BRDF model, the diffuse component is a combination of the base diffuse and subsurface scattering.
     * However, since I'm not implementing subsurface scattering, this function only evaluates the base diffuse component.
     *
     * @param bRec A BSDFQueryRecord containing the incoming (`wi`) and outgoing (`wo`) directions, 
     *             along with the UV coordinates for texture evaluation.
     * @return The computed Disney Diffuse BRDF value as a `Color3f`.
     */
    Color3f evalDiffuse(const BSDFQueryRecord &bRec) const {

        // std::cout << "evalDiffuse: Evaluating Disney Diffuse BRDF" << std::endl;

        if(Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
            return Color3f(0.f);

        return f_baseDiffuse(bRec);
    }



    /**
     * @brief Compute the probability density function (PDF) for the diffuse component.
     *
     * This function computes the PDF of sampling a direction using cosine-weighted hemisphere sampling,
     * which is consistent with the Disney Diffuse BRDF.
     *
     * @param bRec A BSDFQueryRecord containing the incoming (`wi`) and outgoing (`wo`) directions.
     * @return The computed PDF value as a `float`.
     */
    float pdfDiffuse(const BSDFQueryRecord &bRec) const {

        // std::cout << "pdfDiffuse: Calculating Disney Diffuse BRDF PDF" << std::endl;

        if (Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
            return 0.0f;
        
        return INV_PI * Frame::cosTheta(bRec.wo);
    }



    /**
     * @brief Sample the Disney Diffuse BRDF component.
     *
     * This method samples an outgoing direction (`wo`) using cosine-weighted hemisphere sampling.
     * It ensures the sampled direction is above the surface and computes the corresponding
     * BRDF value weighted by the probability density function (PDF).
     *
     * @param bRec A BSDFQueryRecord to store the sampled outgoing direction (`wo`) and measure.
     * @param sample A 2D random sample on the unit square.
     * @return The weighted Disney Diffuse BRDF value as a `Color3f`.
     */
    Color3f sampleDiffuse(BSDFQueryRecord &bRec, const Point2f &sample) const {

        // std::cout << "sampleDiffuse: Sampling Disney Diffuse BRDF" << std::endl;

        bRec.measure = ESolidAngle;

        Vector3f wo = Warp::squareToCosineHemisphere(sample);

       if(Frame::cosTheta(bRec.wi) <= 0) {
            return Color3f(0.0f);
       }
       
        return f_baseDiffuse(bRec) * Frame::cosTheta(wo) / pdfDiffuse(bRec);
    }


    /* ======================== METAL ========================*/
    /* =======================================================*/

    // float G1(float cosTheta, float alpha) const {
    //     float tanTheta2 = (1.0f - cosTheta * cosTheta) / (cosTheta * cosTheta);
    //     float root = alpha * sqrt(1.0f + tanTheta2);
    //     return 2.0f / (1.0f + sqrt(1.0f + root * root));
    // }

    // float G_smith(const Vector3f &wi, const Vector3f &wo, const Vector3f &h, float alpha) const {
    //     float G1_wi = G1(Frame::cosTheta(wi), alpha);
    //     float G1_wo = G1(Frame::cosTheta(wo), alpha);
    //     return G1_wi * G1_wo;
    // }

    // Color3f evalMetal(const BSDFQueryRecord &bRec) const {
    //     // f_metal in the homework sheet

    //     if(Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
    //         return Color3f(0.0f);

    //     Vector3f h = wh(bRec.wi, bRec.wo);

    //     // Schlick's approximation for the Fresnel term
    //     Color3f F_m = m_baseColour->eval(bRec.uv) + (1 - m_baseColour->eval(bRec.uv)) * std::pow(1 - std::abs(h.dot(bRec.wo)), 5);

    //     Frame frame;
    //     Vector3f h_local = frame.toLocal(h);


    //     // GGX Normal Distribution Function (GTR2)
    //     // Because we don't have anisotropic materials, we can use GTR2 for D_m instead of GGX
    //     float roughness = std::clamp(m_roughness, 0.01f, 1.f);
    //     float alpha_min = 0.0001f;
    //     float alpha = std::max(alpha_min, roughness * roughness);

    //     float cosTheta_h = std::max(Frame::cosTheta(h), 0.0f); // Ensure cosTheta_h is non-negative
    //     float denom = (alpha * alpha - 1.0f) * cosTheta_h * cosTheta_h + 1.0f;
    //     denom = denom * denom; // Square the denominator
    //     float D_m = alpha * alpha / (M_PI * denom);

    //     // Smith's Geometry Term (G)
    //     auto G1 = [alpha](float cosTheta) {
    //         float tanTheta2 = (1.0f - cosTheta * cosTheta) / (cosTheta * cosTheta);
    //         float root = alpha * std::sqrt(1.0f + tanTheta2);
    //         return 2.0f / (1.0f + std::sqrt(1.0f + root * root));
    //     };

    //     float G_m = G1(Frame::cosTheta(bRec.wi)) * G1(Frame::cosTheta(bRec.wo));

    //     // Combine terms for the final BRDF value
    //     return F_m * D_m * G_m / (4.0f * Frame::cosTheta(bRec.wi) * Frame::cosTheta(bRec.wo));
    // }




    /* ====================== CLEARCOAT ======================*/
    /* =======================================================*/

    Color3f evalClearcoat(const BSDFQueryRecord &bRec) const {

        // std::cout << "evalClearcoat: Evaluating Disney Clearcoat BRDF" << std::endl;

        if (Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
            return Color3f(0.0f);
        Vector3f wi = bRec.wi;
        Vector3f wo = bRec.wo;
        Vector3f h = wh(wi, wo);

        // Disney BSDF uses a fixed eta of 1.5 for the clearcoat layer as stated in the exercise sheet
        float eta = 1.5f;
        float R_0 = (eta - 1.f) * (eta - 1.f) / (eta + 1.f) / (eta + 1.f);

        // Schlick's approximation for the Fresnel term
        float F_c = R_0 + (1 - R_0) * std::pow(1 - std::abs(h.dot(wo)), 5);

        float alpha_g = (1 - m_clearcoatGloss) * 0.1f + 0.001f;
        // Clamp alpha_g to avoid numerical instability
        alpha_g = std::clamp(alpha_g, 0.001f, 0.1f);
        float D_c = (alpha_g * alpha_g - 1) / (M_PI * std::log(alpha_g * alpha_g) * (1 + (alpha_g * alpha_g - 1) * h.z() * h.z()));

        // Lambda function for the shadowing-masking function G_c
        auto lambda_c = [](const Vector3f &w) {
            float wx = w.x();
            float wy = w.y();
            float wz = w.z();
            float term = (wx * 0.25f) * (wx * 0.25f) + (wy * 0.25f) * (wy * 0.25f);
            return (sqrt(1.0f + term / (wz * wz)) - 1.0f) / 2.0f;
        };

        auto g_c = [lambda_c](const Vector3f &w) {
            return 1.0f / (1.0f + lambda_c(w));
        };

        float G_c = g_c(bRec.wi) * g_c(bRec.wo);

        return F_c * D_c * G_c / (4 * std::abs(Frame::cosTheta(bRec.wi)));
    }

    float pdfClearcoat(const BSDFQueryRecord &bRec) const {

        // std::cout << "pdfClearcoat: Calculating Disney Clearcoat BRDF PDF" << std::endl;

        if (Frame::cosTheta(bRec.wi) <= 0
            || Frame::cosTheta(bRec.wo) <= 0)
            return 0.0f;

        Vector3f wo = bRec.wo;
        Vector3f h = wh(bRec.wi, bRec.wo);
        Frame frame;
        Vector3f h_local = frame.toLocal(h);

        float n_dot_h = std::abs(Frame::cosTheta(h));

        float alpha_g = (1 - m_clearcoatGloss) * 0.1f + 0.001f;
        alpha_g = std::clamp(alpha_g, 0.001f, 0.1f);
        float D_c = (alpha_g * alpha_g - 1) / (M_PI * std::log(alpha_g * alpha_g) * (1 + (alpha_g * alpha_g - 1) * h.z() * h.z()));

        return D_c * n_dot_h / 4.f * std::abs(h.dot(wo));
    }

    Color3f sampleClearcoat(BSDFQueryRecord &bRec, const Point2f &sample) const {

        // std::cout << "sampleClearcoat: Sampling Disney Clearcoat BRDF" << std::endl;

        if (Frame::cosTheta(bRec.wi) <= 0)
            return Color3f(0.0f);

        float alpha_g = (1 - m_clearcoatGloss) * 0.1f + 0.001f;
        alpha_g = std::clamp(alpha_g, 0.001f, 0.1f);

        // Clamping for numerical stability
        float cosTheta_h = sqrt(std::max(0.0f, (1.0f - pow(alpha_g * alpha_g, 1.0f - sample.x())) / (1.0f - alpha_g * alpha_g)));
        float sinTheta_h = sqrt(1.0f - cosTheta_h * cosTheta_h);
        float phi_h = 2.0f * M_PI * sample.y();

        Vector3f h = Vector3f(sinTheta_h * cos(phi_h), sinTheta_h * sin(phi_h), cosTheta_h);

        // Reflect
        Vector3f wo = 2.0f * (bRec.wi.dot(h)) * h - bRec.wi;

        if (Frame::cosTheta(wo) <= 0) return Color3f(0.0f); // Invalid reflection, ignore sample

        bRec.wo = wo;

        float pdf = pdfClearcoat(bRec);

        if (pdf == 0) return Color3f(0.0f);

        // evalClearcoat already includes the cosine term I think
        // return evalClearcoat(bRec) / pdf * std::max(Frame::cosTheta(bRec.wo), 0.f);
        return evalClearcoat(bRec) / pdf;
    }


    /* ======================== SHEEN ========================*/
    /* =======================================================*/

    Color3f C_tint(const BSDFQueryRecord &bRec) const {
        float luminance = m_baseColour->eval(bRec.uv).getLuminance();
        Color3f baseColour = m_baseColour->eval(bRec.uv);
        if (luminance == 0) return Color3f(1.f);

        return baseColour / luminance;
    }

    Color3f evalSheen(const BSDFQueryRecord &bRec) const {

        // std::cout << "evalSheen: Evaluating Disney Sheen BRDF" << std::endl;

        Vector3f wo = bRec.wo;
        Vector3f h = wh(bRec.wi, bRec.wo);

        Color3f C_sheen = (1 - m_sheenTint) + C_tint(bRec) * m_sheenTint;
        return C_sheen * std::pow(1 - std::abs(h.dot(wo)), 5) * std::clamp(std::abs(Frame::cosTheta(wo)), 0.f, 1.f);
    }

    float pdfSheen(const BSDFQueryRecord &bRec) const {

        // std::cout << "pdfSheen: Calculating Disney Sheen BRDF PDF" << std::endl;

        if (Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
            return 0.0f;

        Vector3f wo = bRec.wo;
        // Vector3f h = wh(bRec.wi, bRec.wo);

        float cosTheta = std::abs(Frame::cosTheta(wo));

        return std::max(0.f, INV_PI * cosTheta);

    }

    Color3f sampleSheen(BSDFQueryRecord &bRec, const Point2f &sample) const {

        // std::cout << "sampleSheen: Sampling Disney Sheen BRDF" << std::endl;

        if (Frame::cosTheta(bRec.wi) <= 0)
            return Color3f(0.0f);

        Vector3f h = Warp::squareToCosineHemisphere(sample);
        Vector3f wo = 2.f * (bRec.wi.dot(h)) * h - bRec.wi;

        if (Frame::cosTheta(wo) <= 0) return Color3f(0.0f);

        bRec.wo = wo;

        return evalSheen(bRec) / pdfSheen(bRec);
    }

    /* ============= PUTTING EVERYTHING TOGETHER =============*/
    /* =======================================================*/

    virtual Color3f eval(const BSDFQueryRecord &bRec) const override {
        return evalDiffuse(bRec);
    }

    virtual float pdf(const BSDFQueryRecord &bRec) const override {
        return pdfDiffuse(bRec);
    }

    virtual Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const override {

        float pdf_disney = pdfDiffuse(bRec);
        sampleDiffuse(bRec, sample);
        if (pdf_disney > 0)
            return eval(bRec) / pdf_disney * std::max(Frame::cosTheta(bRec.wo), 0.f);
        else
            return Color3f(0.f);
    }

    virtual void addChild(NoriObject *obj) override {
    if (obj->getClassType() == NoriObject::ETexture) {
        if (obj->getIdName() == "base_colour") {
            if (m_baseColour != nullptr)
                throw NoriException("DisneyBSDF: 'base_color' texture already exists.");
            m_baseColour = static_cast<Texture<Color3f> *>(obj);
        } else {
            throw NoriException("DisneyBSDF: Unsupported texture parameter '%s'!", obj->getIdName());
        }
    } else {
        throw NoriException("DisneyBSDF: Unsupported NoriObject type.");
    }
}

    virtual void activate() override {
        if (!m_baseColour) {
            PropertyList l;
            l.setColor("value", Color3f(0.5f)); // Default constant color
            m_baseColour = static_cast<Texture<Color3f> *>(
                NoriObjectFactory::createInstance("constant_color", l)
            );
        }
    }


    std::string toString() const override {
        return tfm::format(
            "DisneyBSDF[\n"
            "  baseColor = %s,\n"
            "  roughness = %f,\n"
            "  metallic = %f,\n"
            "  specular = %f,\n"
            "  sheen = %f,\n"
            "  clearcoat = %f,\n"
            "]",
            m_baseColour->toString(), 
            m_roughness,
            // m_metallic,
            // m_specular,
            m_sheen,
            m_sheenTint,
            m_clearcoat,
            m_clearcoatGloss);
    }

private:
    Texture<Color3f> *m_baseColour; // Base color as a texture (or a constant value)
    float m_roughness;
    // float m_metallic;
    // float m_specular;
    float m_sheen, m_sheenTint;
    float m_clearcoat, m_clearcoatGloss;

    Vector3f wh(const Vector3f &wi, const Vector3f &wo) const {
        return (wi + wo).normalized();
    }
};

NORI_REGISTER_CLASS(Disney, "disney");
NORI_NAMESPACE_END
