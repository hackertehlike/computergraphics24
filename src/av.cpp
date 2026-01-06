#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class AverageVisibility : public Integrator {
private:
    float m_length;
    const float epsilon = 1e-4f;

public:
    AverageVisibility(const PropertyList &props) {
        m_length = props.getFloat("length", 1.0f);
    }

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(1.0f);

        Vector3f hemisphere_sample_point = Warp::sampleUniformHemisphere(sampler, its.shFrame.n);
        Ray3f wo(its.p, hemisphere_sample_point, epsilon, m_length);
        
        if(scene->rayIntersect(wo, its)) {
            return Color3f(0.0f);
        }
        else {
            return Color3f(1.0f);
        }
    }

    std::string toString() const {
        return "AverageVisibility[]";
    }
};


NORI_REGISTER_CLASS(AverageVisibility, "av");
NORI_NAMESPACE_END

