#include <nori/emitter.h>
#include <nori/scene.h>
#include <nori/shape.h>

NORI_NAMESPACE_BEGIN

class PointLight : public Emitter {
public:
    // Constructor that takes properties from the XML file
    PointLight(const PropertyList &props) {
        m_position = props.getPoint3("position");
        m_power = props.getColor("power");
    }

    // Sample the point light
    Color3f sample(EmitterQueryRecord &lRec, const Point2f &sample) const override {
        lRec.wi = (m_position - lRec.ref).normalized();
        lRec.p = m_position;
        lRec.pdf = 1.0f;  // Delta distribution, so pdf is 0
        lRec.shadowRay=Ray3f(lRec.ref, lRec.wi, Epsilon, (lRec.p - lRec.ref).norm());
        return m_power / (4 * M_PI * (m_position - lRec.ref).squaredNorm());
    }

    // Evaluate the point light (for delta distributions, return 0)
    Color3f eval(const EmitterQueryRecord &lRec) const override {
        return m_power/(4 * M_PI);
    }

    // PDF for point light is always 0
    float pdf(const EmitterQueryRecord &lRec) const override {
        return 1.0f;  // Delta distribution, pdf is zero
    }

    // Destructor
    virtual ~PointLight() {}

    // Return class type (Emitter in this case)
    virtual EClassType getClassType() const override { return EEmitter; }

    std::string toString() const override {
        return "PointLight[]";
    }

private:
    Point3f m_position;  // Position of the point light
    Color3f m_power;     // Power (Watts) of the point light
};

NORI_REGISTER_CLASS(PointLight, "point")
NORI_NAMESPACE_END