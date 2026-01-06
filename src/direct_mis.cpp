#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>

NORI_NAMESPACE_BEGIN

class DirectMis : public Integrator {
public:
    DirectMis(const PropertyList &props) {
        // Initialization (if needed)
    }

    void preprocess(const Scene *scene) override {
        // Optional preprocessing (if needed)
    }

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const override {
        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
            return Color3f(0.0f); // Return black if no intersection
        }

        Color3f L(0.0f);

        // Check if the intersected object is an emitter
        if (its.mesh->isEmitter()) {
            const Emitter *emitter = its.mesh->getEmitter();
            EmitterQueryRecord eRec(ray.o, its.p, its.shFrame.n);
            L += emitter->eval(eRec); // Add emitter's radiance to the result
        }

        Color3f F_em(0.0f), F_mat(0.0f);
        float pdf_em = 0.0f, pdf_bsdf = 0.0f;

        // Perform BSDF sampling
        BSDFQueryRecord bRec(its.toLocal(-ray.d), its.uv); // Initialize BSDF query
        Color3f bsdfSample = its.mesh->getBSDF()->sample(bRec, sampler->next2D());

        // Handle EDiscrete case by skipping emitter sampling and using unweighted material sampling
        if (bRec.measure == EDiscrete) {
            Ray3f shadowRay(its.p, its.toWorld(bRec.wo));
            Intersection lightIts;
            if (scene->rayIntersect(shadowRay, lightIts) && lightIts.mesh->isEmitter()) {
                const Emitter *emitter = lightIts.mesh->getEmitter();
                EmitterQueryRecord eRec(its.p, lightIts.p, lightIts.shFrame.n);
                F_mat = emitter->eval(eRec) * bsdfSample; // Direct material contribution without MIS weighting
            }
        } else {
            // --- Emitter sampling part ---
            float rnd = sampler->next1D();
            const Emitter *light = scene->getRandomEmitter(rnd);

            EmitterQueryRecord lRec(its.p);
            Color3f Le = light->sample(lRec, sampler->next2D()) * scene->getLights().size();

            // Evaluate the BSDF in the direction of the light sample
            BSDFQueryRecord emBRec(its.toLocal(-ray.d), its.toLocal(lRec.wi), ESolidAngle, its.uv);
            Color3f bsdfVal = its.mesh->getBSDF()->eval(emBRec);

            if (!scene->rayIntersect(lRec.shadowRay)) {
                // MIS weight calculation for emitter sampling
                pdf_em = light->pdf(lRec);
                pdf_bsdf = its.mesh->getBSDF()->pdf(emBRec);
                if (pdf_em + pdf_bsdf > 1e-8) {
                    float weight_em = pdf_em / (pdf_em + pdf_bsdf);
                    F_em = weight_em * Le * bsdfVal * std::max(lRec.wi.dot(its.shFrame.n), 0.f);
                }
            }

            // Now handle the BSDF sampling direction for MIS
            Ray3f shadowRay(its.p, its.toWorld(bRec.wo));
            Intersection lightIts;
            if (scene->rayIntersect(shadowRay, lightIts) && lightIts.mesh->isEmitter()) {
                const Emitter *emitter = lightIts.mesh->getEmitter();
                EmitterQueryRecord eRec(its.p, lightIts.p, lightIts.shFrame.n);
                Color3f Le_bsdf = emitter->eval(eRec);

                pdf_bsdf = its.mesh->getBSDF()->pdf(bRec);
                pdf_em = emitter->pdf(eRec);
                if (pdf_bsdf + pdf_em > 1e-8) {
                    float weight_bsdf = pdf_bsdf / (pdf_bsdf + pdf_em);
                    F_mat = weight_bsdf * Le_bsdf * bsdfSample;
                }
            }
        }

        // Combine both sampling contributions
        L += F_em + F_mat;

        return L; // Return the total computed radiance
    }   

    virtual ~DirectMis() {}

    virtual std::string toString() const override {
        return "DirectMis[]";
    }

    virtual EClassType getClassType() const override {
        return EIntegrator;
    }
};

NORI_REGISTER_CLASS(DirectMis, "direct_mis");
NORI_NAMESPACE_END
