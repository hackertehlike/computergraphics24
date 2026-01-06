#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/bsdf.h>
NORI_NAMESPACE_BEGIN

class MaterialIntegrator : public Integrator {
public:
    MaterialIntegrator(const PropertyList &props) {

    }

    virtual Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        
        Intersection its;
        if (!scene->rayIntersect(ray, its)) 
            return Color3f(0.0f);  // No intersection, return black
        
        const BSDF *bsdf = its.mesh->getBSDF();

        Vector3f n(0.f, 0.f, 1.f);
        BSDFQueryRecord bRec(its.toLocal(-ray.d).normalized(), n, EMeasure::ESolidAngle, its.uv);

        Color3f bsdfValue = bsdf->eval(bRec);

        return bsdfValue;
    }


    std::string toString() const {
        return "MaterialIntegrator[]";
    }
};


NORI_REGISTER_CLASS(MaterialIntegrator, "material");
NORI_NAMESPACE_END