#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>

NORI_NAMESPACE_BEGIN

class PathMIS : public Integrator {
public:
    PathMIS(const PropertyList &props) {
        // Initialization (if needed)
    }

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const override {
        Color3f L(0.0f);  // Accumulated radiance
        Color3f t(1.0f);  // Throughput factor
        Ray3f pathRay = ray;

        EMeasure measure = EDiscrete;
        float pdfBsdf = 1.0f;

        while (true) {
            Intersection its;
            if (!scene->rayIntersect(pathRay, its)) {
                return L;
            }

            // Check if the intersected object is an emitter
            if (its.mesh->isEmitter()) {
                const Emitter *emitter = its.mesh->getEmitter();
                EmitterQueryRecord eRec(pathRay.o, its.p, its.shFrame.n);

                if(measure == EDiscrete) {
                    L += t * emitter->eval(eRec);

                } else {
                    float pdfEmitter = emitter->pdf(eRec) / scene->getLights().size();
                    float weightBsdf = pdfBsdf / (pdfBsdf + pdfEmitter);
                    if (pdfEmitter + pdfBsdf == 0) weightBsdf = 0;
                    else weightBsdf = pdfBsdf / (pdfBsdf + pdfEmitter);

                    L += t * emitter->eval(eRec) * weightBsdf;
                }

                //break;
            }

            // Russian Roulette termination
            float successProbability = std::min(t.maxCoeff(), 0.99f);
            if (sampler->next1D() > successProbability) break;
            t /= successProbability;

            const BSDF *bsdf = its.mesh->getBSDF();

            // ---- Emitter Sampling ----
            const Emitter *randomEmitter = scene->getRandomEmitter(sampler->next1D());
            EmitterQueryRecord lRec(its.p);
            Color3f Le = randomEmitter->sample(lRec, sampler->next2D()) * scene->getLights().size();

            if (lRec.pdf > 0 && Le.maxCoeff() > 0) {
                // Check visibility with a shadow ray
                if (!scene->rayIntersect(lRec.shadowRay)) {
                    BSDFQueryRecord bRec(its.toLocal(-pathRay.d), its.toLocal(lRec.wi), ESolidAngle, its.uv);
                    Color3f f = bsdf->eval(bRec);
                    float weightEmitter = (lRec.pdf + bsdf->pdf(bRec) > 0) ? (lRec.pdf / scene->getLights().size() ) / (lRec.pdf  / scene->getLights().size() + bsdf->pdf(bRec)) : 0;
                
                    // Add the emitter contribution with MIS weighting
                    L += t * Le * f * Frame::cosTheta(its.toLocal(lRec.wi)) * weightEmitter;
                }
            }


            // ---- BSDF Sampling ----
            BSDFQueryRecord bRec(its.toLocal(-pathRay.d), its.uv);

            // Update throughput and trace the BSDF ray
            Color3f f = bsdf->sample(bRec, sampler->next2D());
            t *= f;

            // Break if throughput becomes negligible
            if (t.maxCoeff() < 1e-6f) break;

            // Compute MIS weight for BSDF sample
            float pdfBsdf = bsdf->pdf(bRec);

            pathRay = Ray3f(its.p, its.toWorld(bRec.wo));

        }

        return L;
    }

    virtual ~PathMIS() {}

    std::string toString() const override {
        return "PathMIS[]";
    }

    EClassType getClassType() const override {
        return EIntegrator;
    }
};

NORI_REGISTER_CLASS(PathMIS, "path_mis");
NORI_NAMESPACE_END
