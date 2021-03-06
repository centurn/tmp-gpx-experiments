#include "pugixml/pugixml.hpp"

#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>

#include "gps_data.h"

namespace gps{

bool loadGPX(std::vector<Track>& dest, const char* filename){
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename);

    if(!result){
        std::cout << "XML [" << filename << "] parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
        std::cout << "Error description: " << result.description() << "\n";
        std::cout << "Error offset: " << result.offset << " (error at [..." << (filename + result.offset) << "]\n\n";
        return false;
    }

    for(auto trk: doc.child("gpx").children("trk")){
        std::string name = trk.child_value("name");
        std::cout << name << std::endl;
        dest.push_back(Track{filename, std::move(name), {}});
        Track& track = dest.back();

        std::istringstream istr;
        istr.imbue(std::locale("C"));// That's to be independent of user's locale

        for(auto trkseg: trk.children("trkseg")){
            for(auto trkpt: trkseg.children("trkpt")){
                track.points.push_back(Waypoint{});
                Waypoint& point = track.points.back();

                istr.str(trkpt.attribute("lat").value());
                istr.seekg(0);
                istr >> point.lat;

                istr.str(trkpt.attribute("lon").value());
                istr.seekg(0);
                istr >> point.lon;

                istr.str(trkpt.child_value("time"));
                istr.seekg(0);
                std::tm t = {};
                // Format sample:         2016-08-25T15:35:02Z
                istr >> std::get_time(&t, "%Y-%m-%dT%H:%M:%SZ");;
                if(istr.fail())
                    return false;
                point.time = mktime(&t);
            }
        }

        if(track.points.size() < 2)
            dest.pop_back();// Too small to be of any use
    }
    return true;
}

} // namespace gps
