/* Copyright 2015-2017 - Alessandro Fabbri, Stefano Sinigardi */

/***************************************************************************
This file is part of utils.

utils is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

utils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with utils. If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/


#ifndef UTILS_JSONTOHTML_HPP
#define UTILS_JSONTOHTML_HPP

#include <iomanip>
#include <sstream>
#include <physycom/split.hpp>

using namespace std;

const std::string html_header =
R"(
<!DOCTYPE html>
<html> 
<head> 
<meta http-equiv="content-type" content="text/html; charset=UTF-8" /> 
<title>
Display TRIP 
</title> 
<style>
html, body, #map-canvas {
  height: 100%;
  margin: 0px;
  padding: 0px
}
      #panel {
position: absolute;
top: 5px;
right: 0%;
margin-left: -180px;
z-index: 5;
background-color: #fff;
padding: 5px;
border: 1px solid #999;
}
</style>
<script type="text/javascript"  src="https://maps.googleapis.com/maps/api/js?v=3.exp"></script>
<!-- Remote Physycom libraries, bound to 9f9ad71c commit -->
<script type="text/javascript" src="https://cdn.rawgit.com/physycom/ruler/9f9ad71c/markerwithlabel.js"></script>
<script type="text/javascript" src="https://cdn.rawgit.com/physycom/ruler/9f9ad71c/ContextMenu.js"></script>
<link rel="stylesheet" type="text/css" href="https://cdn.rawgit.com/physycom/ruler/9f9ad71c/ruler_map.css">
<script type="text/javascript" src="https://cdn.rawgit.com/physycom/ruler/9f9ad71c/ruler_map.js"></script>   
<!-- Remote png libraries -->
<script type="text/javascript" src="https://github.com/niklasvh/html2canvas/releases/download/0.4.1/html2canvas.js"></script>
<script type="text/javascript" src="http://code.jquery.com/jquery-latest.min.js" ></script>   
</head> 

<body>
)";

constexpr char STYLE_PNG[] = "png";
constexpr char STYLE_REN[] = "ren";
constexpr char STYLE_POLY[] = "poly";
constexpr char STYLE_POLYREN[] = "polyren";
constexpr char STYLE_POLYPNG[] = "polypng";
vector<string> allowed_styles({STYLE_PNG, STYLE_REN, STYLE_POLY, STYLE_POLYREN, STYLE_POLYPNG});

constexpr char STYLE_DEFAULT[] = "polypng";

std::string HSLtoRGB(double hue, double sat, double light) 
{
  double red, green, blue;
  red = green = blue = light;                     // grey is default
  double v;
  v = light <= 0.5 ? light*(1. + sat) : light + sat - light*sat;
  if (v > 0) {
    double m, sv;
    int sextant;
    double fract, vsf, mid1, mid2;
    m = light + light - v; sv = (v - m) / v;
    hue *= 6.0; sextant = int(hue);
    fract = hue - sextant; vsf = v*sv*fract; mid1 = m + vsf; mid2 = v - vsf;
    switch (sextant) {
    case 0: red = v; green = mid1; blue = m; break;
    case 1: red = mid2; green = v; blue = m; break;
    case 2: red = m; green = v; blue = mid1; break;
    case 3: red = m; green = mid2; blue = v; break;
    case 4: red = mid1; green = m; blue = v; break;
    case 5: red = v; green = m; blue = mid2; break;
    }
  }
  std::stringstream stream;

  red *= 255;
  green *= 255;
  blue *= 255;
  stream
    << std::setfill('0') << std::setw(2) << std::hex << int(red)
    << std::setfill('0') << std::setw(2) << std::hex << int(green)
    << std::setfill('0') << std::setw(2) << std::hex << int(blue);
  return stream.str();
};

class json_to_html
{
public:
  bool export_map, verbose;
  int undersampling;
  vector<vector<jsoncons::json>> trips;
  vector<jsoncons::json> records;

  json_to_html(bool export_map_ = false, bool verbose = true, int undersampling_ = 1);
  void digest(std::string raw_json);
  template<typename... Func> void push_file(std::string filename, Func... f);
  template<typename... Func> void push(std::string raw_json, Func... f);
  template<typename... json_t> void init_specs(std::string, json_t... jconf);
  std::string get_html();
  void dump_html(std::string filename);

private:
  int idx_new_trip;
  vector<string> colors_button, colors_text;
  vector<string> trip_tag, trip_style;
};

json_to_html::json_to_html(bool export_map_, bool verbose_, int undersampling_)
{
  export_map = export_map_;
  verbose = verbose_;
  undersampling = undersampling_;
}

// store the original json data
// according to obj or arr
void json_to_html::digest(std::string raw_json)
{
  jsoncons::json _records = jsoncons::json::parse(raw_json);
  vector<jsoncons::json> temp_trip;
  if (_records.size() == 0) throw std::runtime_error("Empty input JSON");
  if (_records.is_array()) 
  {
    for (size_t k = 0; k < _records.size(); ++k) 
      records.push_back(_records[k]);    
  }
  else if (_records.is_object()) 
  {
    for (auto it = _records.begin_members(); it != _records.end_members(); ++it) 
      records.push_back(it->value());
  }
  else throw std::runtime_error("JSON is not array nor object");
}

// variadic push raw json string to trips
// accepts a lambda for splitting function
template<typename... Func> void json_to_html::push(std::string raw_json, Func... f)
{
  digest(raw_json);
  // by making a tuple out of f... 
  // the function passed becomes callable
  auto tuplef = make_tuple(f...);
  idx_new_trip = trips.size();
  get<0>(tuplef)();
  records.clear();
}

template<> void json_to_html::push(std::string raw_json)
{
  digest(raw_json);
  idx_new_trip = trips.size();
  trips.push_back(records);
  records.clear();
}

// utility wrapper for files 
template<typename... Func> void json_to_html::push_file(std::string filename, Func... f)
{
  jsoncons::json _records = jsoncons::json::parse_file(filename);
  push(_records.to_string(), f...);
}

template<> void json_to_html::push_file(std::string filename)
{
  jsoncons::json _records = jsoncons::json::parse_file(filename);
  push(_records.to_string());
}

// utility wrapper for files 
template<typename... json_t> void json_to_html::init_specs(std::string tag, json_t... jspec)
{
  auto jtuple = std::make_tuple(jspec...);
  auto js = get<0>(jtuple);
  for (int i = idx_new_trip; i < (int)trips.size(); ++i) 
  {
    if(js.has_member("color")) colors_button.push_back(js["color"].as_string());
    else colors_button.push_back("FF0000");
    
    colors_text.push_back("000000");

    if(js.has_member("style")) 
    {
      auto style = js["style"].as_string();
      if( physycom::belongs_to(style,allowed_styles) ) trip_style.push_back(style);
      else 
      {
        cerr << "WARNING: Style \"" << style << "\" unknown. Setting to default(" << STYLE_DEFAULT << ")" << endl;
        trip_style.push_back(STYLE_DEFAULT);
      }
    }
    else trip_style.push_back(STYLE_DEFAULT);
    
    trip_tag.push_back(tag + ( (trips.size() - idx_new_trip == 1) ? string("") : ("_" + std::to_string(i))));
  }
}

template<>
void json_to_html::init_specs(std::string tag)
{
  for (int i = idx_new_trip; i < (int)trips.size(); ++i) 
  {
    colors_button.push_back("0000FF");
    colors_text.push_back("000000");
    trip_style.push_back(STYLE_DEFAULT);
    trip_tag.push_back(tag + ( (trips.size() - idx_new_trip == 1) ? string("") : ("_" + std::to_string(i))));
  }
}

std::string json_to_html::get_html()
{
  if( trips.size() == 0 ) throw std::runtime_error("Empty trip vector");

  std::stringstream output;
  output << html_header;
  output << R"(    
  <div id="map" style="width: )" << (export_map?"600px":"100%") << R"(; height: )" << (export_map ? "600px" : "100%") << R"(;"></div>

  <script type="text/javascript">
  )";
  for (size_t i = 0; i < trips.size(); ++i) 
  {
    output << "\t\t\tvar Trajectory_trip_" << i << ";" << endl;
    output << "\t\t\tvar Markers_trip_" << i << " = [];" << endl;
    output << "\t\t\tvar PolyPath_trip_" << i << " = [];" << endl;
  }
  output << endl << "\t\t\tvar map;" << endl << endl;
  output << "\t\t\tfunction initialize(){" << endl;
  for (size_t i = 0; i < trips.size(); i++) 
  {
    output << endl << "\t\t\t\tvar Locations_trip_" << i << " = [" << endl;
    // start of gps points 
    unsigned int last_timestamp = 0;
    for (size_t j = 0; j < trips[i].size(); j++) 
    {
      if (j % undersampling) continue;
      string tooltip("#" + to_string(j));
      
      if (verbose) 
      {
        if (trips[i][j].has_member("date"))
          tooltip = "date: " + trips[i][j].at("date").as<string>();
        if (trips[i][j].has_member("alt"))
          tooltip += "<br />altitude: " + trips[i][j].at("alt").as<string>();
        if (trips[i][j].has_member("delta_dist"))
          tooltip += "<br />ds (m): " + trips[i][j].at("delta_dist").as<string>();
        if (trips[i][j].has_member("timestamp")) 
        {
          try 
          {
            if (j != 0) last_timestamp = trips[i][j - 1].at("timestamp").as<unsigned int>();
            else last_timestamp = 0;
            tooltip += "<br />timestamp: " + to_string(trips[i][j].at("timestamp").as<unsigned int>());
            tooltip += "<br />dt (s): " + to_string(trips[i][j].at("timestamp").as<unsigned int>() - last_timestamp);
          }
          catch (...) 
          {
            // old format compatibility (crash avoiding)
            try 
            {
              tooltip += "<br />date:" + trips[i][j].at("timestamp").as<string>();
            }
            catch (...) {
              tooltip += "<br />timestamp: NA";
            }
          }
        }
        if (trips[i][j].has_member("heading"))
          tooltip += "<br />heading: " + trips[i][j].at("heading").as<string>();
        if (trips[i][j].has_member("speed"))
          tooltip += "<br />speed: " + trips[i][j].at("speed").as<string>();
        if (trips[i][j].has_member("enabling"))
          tooltip += "<br />cause: " + trips[i][j].at("enabling").as<string>();
        if (trips[i][j].has_member("tracking_glonass")) 
        {
          tooltip += "<br />glonass sats (used/seen): " + trips[i][j].at("using_glonass").as<string>() + " / " + trips[i][j].at("tracking_glonass").as<string>();
          tooltip += "<br />gps sats (used/seen): " + trips[i][j].at("using_gps").as<string>() + " / " + trips[i][j].at("tracking_gps").as<string>();
          tooltip += "<br />total sats (used/seen): " + trips[i][j].at("using_total").as<string>() + " / " + trips[i][j].at("tracking_total").as<string>();
        }
        if (trips[i][j].has_member("fix"))
          tooltip += "<br />fix: " + trips[i][j].at("fix").as<string>();
        if (trips[i][j].has_member("global_index"))
          tooltip += "<br />global index: " + trips[i][j].at("global_index").as<string>();
      }
      output
      << "["
      << fixed << setprecision(6)
      << (trips[i][j].has_member("lat") ? trips[i][j].at("lat").as<double>() : 90.0)
      << ","
      << (trips[i][j].has_member("lon") ? trips[i][j].at("lon").as<double>() : 0.0)
      << ",'<p>"
      << tooltip
      << "</p>']"
      << (j != trips[i].size() - 1 ? ',' : ' ')
      << endl;
    }
    // end of gps points
    output << "\t\t\t\t]" << endl;
  }
  output << R"(
  map = new google.maps.Map(document.getElementById('map'), {
    mapTypeId : google.maps.MapTypeId.ROADMAP,
    disableDefaultUI: )" << std::boolalpha << export_map << std::dec << R"(
  });

  var infowindow = new google.maps.InfoWindow();
  var Marker, i;
  var bounds = new google.maps.LatLngBounds();
)";
  
  for (size_t i = 0; i < trips.size(); i++) 
  {
    output << "//////////////////////////////////////////////////////// TRIP " << i << endl;
    output << R"(
  for (i = 0; i<Locations_trip_)" << i << R"(.length; i++) 
  {
    var point = new google.maps.LatLng( Locations_trip_)" << i << "[i][0], Locations_trip_" << i << R"([i][1] )

    bounds.extend(point);
    )";
    
    if (trip_style[i] == STYLE_PNG || trip_style[i] == STYLE_POLYPNG) 
    {
      output << R"(
    var marker_url;
    if ( Locations_trip_)" << i << R"([i][2].search("first_last") != -1 )
      marker_url = 'http://maps.gpsvisualizer.com/google_maps/icons/circle/blue.png';
    else if ( Locations_trip_)" << i << R"([i][2].search("rdp_engine") != -1 )
      marker_url = 'http://maps.gpsvisualizer.com/google_maps/icons/circle/green.png';
    else if ( Locations_trip_)" << i << R"([i][2].search("smart_restore") != -1 )
      marker_url = 'http://maps.gpsvisualizer.com/google_maps/icons/circle/yellow.png';
    else if ( Locations_trip_)" << i << R"([i][2].search("ignition_on") != -1 )
      marker_url = 'http://maps.gpsvisualizer.com/google_maps/icons/circle/white.png';
    else if ( Locations_trip_)" << i << R"([i][2].search("ignition_off") != -1 )
      marker_url = 'http://maps.gpsvisualizer.com/google_maps/icons/circle/black.png';
    else
      marker_url = 'http://maps.gpsvisualizer.com/google_maps/icons/circle/green.png';

    Marker = new google.maps.Marker({
        position: point,
        map: map,
        zIndex: point[1],
        icon: marker_url
    });
    Markers_trip_)" << i << R"(.push(Marker);
)";
    }

    if (trip_style[i] == STYLE_REN || trip_style[i] == STYLE_POLYREN) 
    {
      output << R"(
    Marker = new google.maps.Marker({
      position: point,
      map : map,
      icon : {
        path: google.maps.SymbolPath.CIRCLE,
        strokeColor : '#)" << colors_button[i] << R"(',
        scale : 3
      }
    });
    Markers_trip_)" << i << R"(.push(Marker);
)";
    }

    if (trip_style[i] != STYLE_POLY)
    {
      output << R"(
    google.maps.event.addListener(Marker, 'click', (function(marker, i) {
      return function() {
        infowindow.setContent(Locations_trip_)" << i << R"([i][2]);
        infowindow.open(map, marker);
      }
    })(Marker, i));
)";
    }

    output << R"(
  }
)";

    if (trip_style[i] != STYLE_REN && trip_style[i] != STYLE_PNG) 
    {
      output << R"(
  for (i = 0; i<Locations_trip_)" << i << R"(.length; i++) {
    PolyPath_trip_)" << i << R"(.push(new google.maps.LatLng(Locations_trip_)" << i << R"([i][0], Locations_trip_)" << i << R"([i][1]))
  }
  Trajectory_trip_)" << i << R"( = new google.maps.Polyline({
    path: PolyPath_trip_)" << i << R"(,
    geodesic: true,
    strokeColor: '#)" << colors_button[i] << R"(',
    strokeOpacity: 1.,
    strokeWeight: 2
  });)";
      output << R"(
  Trajectory_trip_)" << i << R"(.setMap(map);
      
)";
    }
  }
  output << endl << "\t\t\t\tmap.fitBounds(bounds);" << endl;
  output << "\t\t\t\truler_map = new RulerMap(map)" << endl;

  if (export_map) 
  {
    output << R"(
    google.maps.event.addListenerOnce(map, 'tilesloaded', function(){
      setTimeout(saveImage, 1000);
    }); 

    )";
  }

  output << "\t\t}" << endl;
  for (size_t i = 0; i < trips.size(); i++) 
  {
    output << R"(
    function toggle_trip_)" << i << "(){" << endl;
    if (trip_style[i] == STYLE_POLYREN || trip_style[i] == STYLE_POLYPNG || trip_style[i] == STYLE_POLY) output << R"(
      Trajectory_trip_)" << i << ".setMap(Trajectory_trip_" << i << ".getMap() ? null : map);" << endl;
    if (trip_style[i] != STYLE_POLY ) output << R"(
      for (i = 0; i < Markers_trip_)" << i << R"(.length; i++) {
        var mark = Markers_trip_)" << i << R"([i];
        mark.setMap(mark.getMap() ? null : map);
      })";
      output << R"(
    })";
  }

  if (export_map) 
  {
    output << R"(
    function saveImage(){
      html2canvas($("#map"), {
        useCORS: true,
        onrendered: function(canvas) {
          var dataURL = canvas.toDataURL("image/png");
          var pom = document.createElement('a');
          pom.setAttribute('href', dataURL ) ;
          pom.setAttribute('download', "map.png" );
          pom.click();
          setTimeout(function(){ window.close(); }, 3000);
        }
      }); 
    }
    )";
  }

  output << R"(
  google.maps.event.addDomListener(window, 'load', initialize);

  </script>
  <div id = "panel">
  )";
  for (size_t i = 0; i < trips.size(); ++i) 
  {
    output << "\t\t\t<button onclick = \"toggle_trip_" << i 
           << "()\" style = \"background-color:#" << colors_button[i] << "; color:#" << colors_text[i] << "\">" << trip_tag[i] << "</button>" << endl;
  }
  output << R"(    </div>
  </body>
  </html>
  )";

  return output.str();
}

void json_to_html::dump_html(std::string filename)
{
  ofstream html(filename);
  if(!html) throw std::runtime_error("Unable to create output file : " + filename);
  html << get_html() << endl;
  html.close();
}

#endif // #define UTILS_JSONTOHTML_HPP
