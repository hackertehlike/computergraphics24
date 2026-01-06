#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>

NORI_NAMESPACE_BEGIN

class DirectEms : public Integrator {
public:
    // Constructor: initialize any properties here
    DirectEms(const PropertyList &props) {
        // Initialization (if needed)
    }

    // Perform any scene-specific preprocessing (if needed)
    void preprocess(const Scene *scene) override {
        // Optional: Perform any preprocessing steps like caching important scene data
    }

    // Sample the incident radiance along a ray
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const override {
        // Get the intersection
        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
            return Color3f(0.0f); // Return black if no intersection
        }

        Color3f L(0.0f);

        // Check if the intersected object is an emitter
        if (its.mesh->isEmitter()) {
            // std::cout << "Ray " << ray.o << " " << ray.d << " " << its.p << " " << its.mesh->getEmitter()->toString() << std::endl;
            // Query the emitter for direct illumination
            const Emitter *emitter = its.mesh->getEmitter();
            EmitterQueryRecord eRec(ray.o, its.p, its.shFrame.n);
            L += emitter->eval(eRec); // Add emitter's radiance to the result
        }

        // Sample a random emitter using the scene's method
        float rnd = sampler->next1D(); // Generate a random number in [0, 1]
        const Emitter *light = scene->getRandomEmitter(rnd); // Get a random emitter

        // Prepare the EmitterQueryRecord for the selected light
        EmitterQueryRecord lRec(its.p);
        Color3f Le = light->sample(lRec, sampler->next2D()) * scene->getLights().size(); // Sample the light

        // Prepare BSDFQueryRecord and evaluate the BSDF
        BSDFQueryRecord bRec(its.toLocal(-ray.d), its.toLocal(lRec.wi), ESolidAngle, its.uv);
        Color3f bsdf_component = its.mesh->getBSDF()->eval(bRec);

        // Check visibility
        if (!scene->rayIntersect(lRec.shadowRay)) {
            L += Le * bsdf_component * its.shFrame.n.dot(lRec.wi); // Include dot product with normal
        }

        return L; // Return the total computed radiance
    }


    // Destructor
    virtual ~DirectEms() {}

    // Return the type of the integrator (DirectEms in this case)
    virtual std::string toString() const override {
        return "DirectEms[]";
    }

    // Return the class type
    virtual EClassType getClassType() const override {
        return EIntegrator;
    }
};

NORI_REGISTER_CLASS(DirectEms, "direct_ems");
NORI_NAMESPACE_END
