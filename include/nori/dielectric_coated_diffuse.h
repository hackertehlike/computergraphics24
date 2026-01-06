#include <nori/bsdf.h>
#include <nori/frame.h>
#include <nori/warp.h>
#include <nori/common.h>
#include <nori/texture.h>

NORI_NAMESPACE_BEGIN

/// Dielectric-coated diffuse BSDF
class DielectricCoatedDiffuse : public BSDF {
public:
    DielectricCoatedDiffuse(const PropertyList &propList);

    virtual Color3f eval(const BSDFQueryRecord &bRec) const override;
    virtual float pdf(const BSDFQueryRecord &bRec) const override;
    virtual Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const override;
    virtual std::string toString() const override;

private:
    float m_intIOR;  // Interior IOR of dielectric
    float m_extIOR;  // Exterior IOR of dielectric
    Texture<Color3f> *m_albedo;  // Diffuse layer albedo
};

NORI_REGISTER_CLASS(DielectricCoatedDiffuse, "dielectric_coated_diffuse");

NORI_NAMESPACE_END
