#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>

NORI_NAMESPACE_BEGIN

class Direct : public Integrator {
public:
    // Constructor: initialize any properties here
    Direct(const PropertyList &props) {
        // Initialization (if needed)
    }

    // Perform any scene-specific preprocessing (if needed)
    void preprocess(const Scene *scene) override {
        // Optional: Perform any preprocessing steps like caching important scene data
    }

    // Sample the incident radiance along a ray
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const override {
        // Get all emitters
        const auto &lights = scene->getLights();

        // Get the intersection
        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
            return Color3f(0.0f);
        }

        Color3f L(0.0f);

        // Loop over all emitters
        for (const auto &light : lights) {
            EmitterQueryRecord lRec(its.p);
            Color3f Le = light->sample(lRec, sampler->next2D());
            BSDFQueryRecord bRec(its.toLocal(-ray.d), its.toLocal(lRec.wi), ESolidAngle, its.uv);
            Color3f bsdf_component = its.mesh->getBSDF()->eval(bRec);
            
            if(!scene->rayIntersect(lRec.shadowRay)) {
                L += Le.cwiseProduct(bsdf_component) * its.shFrame.n.dot(lRec.wi);
            }
        }

        return L;
    }

    // Destructor
    virtual ~Direct() {}

    // Return the type of the integrator (Direct in this case)
    virtual std::string toString() const override {
        return "Direct[]";
    }

    // Return the class type
    virtual EClassType getClassType() const override {
        return EIntegrator;
    }
};

NORI_REGISTER_CLASS(Direct, "direct");
NORI_NAMESPACE_END
