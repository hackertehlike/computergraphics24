#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>

NORI_NAMESPACE_BEGIN

class PathMats : public Integrator {
public:
    PathMats(const PropertyList &props) {
        // Initialization (if needed)
    }

    void preprocess(const Scene *scene) override {
        // Optional: Perform any preprocessing steps
    }

    // As described in the exercise slides
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const override {
        Color3f L(0.0f);  // Accumulated radiance
        Color3f t(1.0f);  // throughput
        Ray3f pathRay = ray;

        while (true) {
            Intersection its;
            if (!scene->rayIntersect(pathRay, its)) {
                return L;
            }

            // Emitter time
            if (its.mesh->isEmitter()) {
                const Emitter *emitter = its.mesh->getEmitter();
                EmitterQueryRecord eRec(pathRay.o, its.p, its.shFrame.n);
                L += t * emitter->eval(eRec);
            }
            
            float successProbability = std::min(t.maxCoeff(), 0.99f);
            if (sampler->next1D() > successProbability)
                // RIP in pepperoni :(
                break;
            t /= successProbability;

            // Sample the BSDF to get the next path direction
            BSDFQueryRecord bRec(its.toLocal(-pathRay.d), its.uv);
            Color3f bsdfValue = its.mesh->getBSDF()->sample(bRec, sampler->next2D());

            // Update t with the BSDF
            // The BSDF sample function already divides by the probability of the sample and the cosine factor
            // I am a silly goose, I keep forgetting
            t *= bsdfValue;
            pathRay = Ray3f(its.p, its.toWorld(bRec.wo));
        }

        return L;
    }

    virtual ~PathMats() {}

    std::string toString() const override {
        return "PathMats[]";
    }

    EClassType getClassType() const override {
        return EIntegrator;
    }
};

NORI_REGISTER_CLASS(PathMats, "path_mats");
NORI_NAMESPACE_END
