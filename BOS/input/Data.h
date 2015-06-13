/* 
 * File:   Data.h
 * Author: nicorivas
 *
 * Created on June 13, 2015, 11:23 AM
 */

#ifndef DATA_H
#define	DATA_H

#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>

template<unsigned int DIM>
class Data {
public:
    Data();
    Data(const Data& orig);
    virtual ~Data();
    
    void read(const std::string& file, Simulation<DIM>& sim)
    {
        using namespace rapidjson;
        Document docJson;
        std::stringstream sstr;

        // We read the file to a string.
        std::ifstream in(file.c_str());
        sstr << in.rdbuf();
        docJson.Parse(sstr.str().c_str());

        assert(docJson.HasMember("particleNumber"));
        assert(docJson["particleNumber"].IsInt());

        assert(docJson.HasMember("length"));
        const Value& lengthJson = docJson["length"];
        for (size_t d = 0; d < DIM; d++) {
            Vector<DIM> domain;
            domain[d] = lengthJson[d].GetDouble();
        }

        assert(docJson.HasMember("boundaries"));
        const Value& boundariesJson = docJson["boundaries"];
        assert(boundariesJson.IsArray());
        for (SizeType i = 0; i < boundariesJson.Size(); i++) {// Uses SizeType instead of size_t
            Vector<DIM> normal;
            Vector<DIM> position;
            for (size_t d = 0; d < DIM; d++) {
                normal[d] = boundariesJson[i]["normal"][d].GetDouble();
                position[d] = boundariesJson[i]["position"][d].GetDouble();
            }
            sim.addWall({position,normal});
        }   
    }
private:

};

#endif	/* DATA_H */

