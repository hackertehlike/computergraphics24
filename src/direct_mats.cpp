#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>

NORI_NAMESPACE_BEGIN

class DirectMats : public Integrator {
public:
    // Constructor: initialize any properties here
    DirectMats(const PropertyList &props) {
        // Initialization (if needed)
    }

    // Perform any scene-specific preprocessing (if needed)
    void preprocess(const Scene *scene) override {
        // Optional: Perform any preprocessing steps like caching important scene data
    }
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const override {
    // Get the intersection
    Intersection its;
    if (!scene->rayIntersect(ray, its)) {
        return Color3f(0.0f); // Return black if no intersection
    }

    Color3f L(0.0f);

    // Check if the intersected object is an emitter
    if (its.mesh->isEmitter()) {
        // Query the emitter for direct illumination
        const Emitter *emitter = its.mesh->getEmitter();
        EmitterQueryRecord eRec(ray.o, its.p, its.shFrame.n);
        L += emitter->eval(eRec); // Add emitter's radiance to the result
    }

    // Sample the BRDF instead of the emitter
    BSDFQueryRecord bRec(its.toLocal(-ray.d), its.uv);
    Color3f bsdfSample = its.mesh->getBSDF()->sample(bRec, sampler->next2D());

    // Create a shadow ray in the sampled direction
    Ray3f shadowRay(its.p, its.toWorld(bRec.wo));
    Intersection lightIts;
    if (scene->rayIntersect(shadowRay, lightIts) && lightIts.mesh->isEmitter()) {
        // If the ray intersects an emitter, retrieve its radiance
        const Emitter *emitter = lightIts.mesh->getEmitter();
        EmitterQueryRecord eRec(its.p, lightIts.p, lightIts.shFrame.n);
        Color3f Le = emitter->eval(eRec);

        // Add the radiance contribution WITHOUT dividing by pdf or adding cos term
        L += Le * bsdfSample;
    }

    return L; // Return the total computed radiance
}


    // Destructor
    virtual ~DirectMats() {}

    virtual std::string toString() const override {
        return "DirectMats[]";
    }

    // Return the class type
    virtual EClassType getClassType() const override {
        return EIntegrator;
    }
};

NORI_REGISTER_CLASS(DirectMats, "direct_mats");
NORI_NAMESPACE_END
