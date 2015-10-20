#ifndef PARTICLETYPE_H
#define	PARTICLETYPE_H

#include "libs/rapidjson/prettywriter.h" // for stringify JSON
#include "libs/rapidjson/filereadstream.h"
#include "libs/rapidjson/document.h"

class ParticleType {
public:
    int id;
    double radius;
    double mass;
    double restitutionCoefficient;
    const rapidjson::Value& data;
    ParticleType(const rapidjson::Value& data) : data(data) {
        if (data.HasMember("radius")) {
            radius = data["radius"].GetDouble();
        }
        if (data.HasMember("mass")) {
            mass = data["mass"].GetDouble();
        }
        if (data.HasMember("restitutionCoefficient")) {
            restitutionCoefficient = data["restitutionCoefficient"].GetDouble();
        }
    };
private:
};

#endif	/* PARTICLETYPE_H */

