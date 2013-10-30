#include <iostream>
#include <getopt.h>

#include <osmium/io/any_input.hpp>

#include <osmium/io/any_output.hpp>

#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>

void print_help() {
    std::cout << "name_count INFILE\n\n" \
              << "Outputs a name frequency table.";
}

typedef std::pair<std::string, int> namepair;
struct IntCmp {
    bool operator()(const namepair &lhs, const namepair &rhs) {
        return lhs.second > rhs.second;
    }
};

class MyCountHandler : public osmium::handler::Handler<MyCountHandler> {

    public:

    // http://stackoverflow.com/questions/5889235/boost-gil-create-image
    boost::gil::rgb8_image_t img{360, 180};
    boost::gil::rgb8_pixel_t black;
    boost::gil::rgb8_pixel_t white;

    MyCountHandler(const std::string& fieldname) {
        // boost::gil::rgb8_image_t img(360 * 2, 180 * 2);
        boost::gil::rgb8_pixel_t white(255, 255, 255);
        boost::gil::rgb8_pixel_t black(0, 0, 0);
        fill_pixels(view(img), white);
    }

    void node(const osmium::Node& node) {
        const char* name = node.tags().get_value_by_key("name");
        boost::gil::rgb8_image_t::view_t v = view(img);
        if (name) {
            v(round(node.lon() + 180), round(node.lat() + 90)) = black;
        }
    }

    void dump() {
        boost::gil::png_write_view("map.png", const_view(img));
    }
};

int main(int argc, char* argv[]) {
    std::string input_filename;
    std::string output_filename("ogr_out");
    int remaining_args = argc - optind;

    if (remaining_args == 1) {
        input_filename =  argv[optind];
    } else {
        input_filename = "-";
    }
    osmium::io::Reader reader(input_filename);
    osmium::io::Header header = reader.open();
    MyCountHandler count_handler("name");
    reader.apply(count_handler);
    google::protobuf::ShutdownProtobufLibrary();
    std::ostringstream buf;
    count_handler.dump();
}
