/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <nori/integrator.h>
#include <nori/sampler.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>
#include <nori/scene.h>
#include <nori/photon.h>

NORI_NAMESPACE_BEGIN

class PhotonMapper : public Integrator {
public:
    /// Photon map data structure
    typedef PointKDTree<Photon> PhotonMap;

    PhotonMapper(const PropertyList &props) {
        /* Lookup parameters */
        m_photonCount  = props.getInteger("photonCount", 1000000);
        m_photonRadius = props.getFloat("photonRadius", 0.0f /* Default: automatic */);
    }

    virtual void preprocess(const Scene *scene) override {
        cout << "Gathering " << m_photonCount << " photons .. ";
        cout.flush();

        /* Create a sample generator for the preprocess step */
        Sampler *sampler = static_cast<Sampler *>(
            NoriObjectFactory::createInstance("independent", PropertyList()));

        /* Allocate memory for the photon map */
        m_photonMap = std::unique_ptr<PhotonMap>(new PhotonMap());
        m_photonMap->reserve(m_photonCount);

		/* Estimate a default photon radius */
		if (m_photonRadius == 0)
			m_photonRadius = scene->getBoundingBox().getExtents().norm() / 500.0f;

		/* How to add a photon?
		 * m_photonMap->push_back(Photon(
		 *	Point3f(0, 0, 0),  // Position
		 *	Vector3f(0, 0, 1), // Direction
		 *	Color3f(1, 2, 3)   // Power
		 * ));
		 */

		// put your code to trace photons here
        // Define termination probability
        
        int depth = 0;

        while (m_photonMap->size() < m_photonCount) {
            // Initialize ray with a random origin and direction based on scene lighting
            Ray3f ray;
            Color3f W = scene->getRandomEmitter(sampler->next1D())->samplePhoton(ray, sampler->next2D(), sampler->next2D());

            while (true) {
                // If the ray doesn't intersect anything, break
                Intersection its;
                if (!scene->rayIntersect(ray, its)) break;

                if (its.mesh->getBSDF()->isDiffuse() && W.isValid()) {
                    // Add the photon to the photon map
                    m_photonMap->push_back(Photon(its.p, -ray.d, W * scene->getLights().size()));
                }


                 float terminationProbability = 1 - std::min(0.99f, W.maxCoeff());   
                // Russian Roulette termination check
                if (depth > threshold && sampler->next1D() < terminationProbability) {
                    break;
                } else {
                    // Adjust weight based on survival
                    W /= (1.0f - terminationProbability);
                }

                BSDFQueryRecord bRec = BSDFQueryRecord(its.toLocal(-ray.d).normalized(), its.uv);
                Color3f bsdf = its.mesh->getBSDF()->sample(bRec, sampler->next2D());

                if (bsdf.isZero()) break;

                W *= bsdf;
                ray = Ray3f(its.p, its.toWorld(bRec.wo));
                depth++;
            }

            emittedPhotonCount++;
        }

		/* Build the photon map */
        m_photonMap->build();
    }

    
    virtual Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &_ray) const override {

        // Define termination probability

        Color3f L = Color3f(0.f);
        Color3f t = Color3f(1.f);
        Ray3f ray = _ray;
        Intersection its;
        int depth = 0;

        while (scene->rayIntersect(ray, its)) {
            if (its.mesh->isEmitter()) {
                EmitterQueryRecord eRec(ray.o, its.p, its.shFrame.n);
                L += t * its.mesh->getEmitter()->eval(eRec) * scene->getLights().size();
            }

            if (its.mesh->getBSDF()->isDiffuse()) {
                std::vector<uint32_t> results;
                m_photonMap->search(its.p, m_photonRadius, results);
                Color3f Lp = Color3f(0.f);
                for (uint32_t i : results) {
                    const Photon &photon = (*m_photonMap)[i];
                    BSDFQueryRecord bRec(its.toLocal(-ray.d), its.toLocal(photon.getDirection()), ESolidAngle, its.uv);
                    Color3f bsdf = its.mesh->getBSDF()->eval(bRec);

                    Lp += bsdf * photon.getPower();
                }
                return L += t * Lp / (M_PI * m_photonRadius * m_photonRadius * emittedPhotonCount);
            }

            float terminationProbability = 1 - std::min(0.99f, t.maxCoeff());

            // Russian Roulette termination check
            if (depth > threshold && sampler->next1D() < terminationProbability) {
                break;
            } else {
                // Adjust weight based on survival
                t /= (1.0f - terminationProbability);
            }

            BSDFQueryRecord bRec(its.toLocal(-ray.d), its.uv);
            Color3f bsdf = its.mesh->getBSDF()->sample(bRec, sampler->next2D());
            if (bsdf.isZero()) break;

            t *= bsdf;
            ray = Ray3f(its.p, its.toWorld(bRec.wo), Epsilon, INFINITY);
            depth++;
        }

        return L;

    }

    virtual std::string toString() const override {
        return tfm::format(
            "PhotonMapper[\n"
            "  photonCount = %i,\n"
            "  photonRadius = %f\n"
            "]",
            m_photonCount,
            m_photonRadius
        );
    }
    
private:
    /* 
     * Important: m_photonCount is the total number of photons deposited in the photon map,
     * NOT the number of emitted photons. You will need to keep track of those yourself.
     */ 
    int m_photonCount;
    float m_photonRadius;
    int emittedPhotonCount;
    std::unique_ptr<PhotonMap> m_photonMap;
    int threshold = 3;
};

NORI_REGISTER_CLASS(PhotonMapper, "photonmapper");
NORI_NAMESPACE_END
