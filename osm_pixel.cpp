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

class MyCountHandler : public osmium::handler::Handler<MyCountHandler> {

    public:

    // http://stackoverflow.com/questions/5889235/boost-gil-create-image
    boost::gil::rgb8_image_t img{2880 * 8, 1440 * 8};
    boost::gil::rgb8_pixel_t black;
    boost::gil::rgb8_pixel_t white;
    boost::gil::rgb8_image_t::view_t v;
    unsigned int count;

    MyCountHandler(const std::string& fieldname) {
        boost::gil::rgb8_pixel_t white(255, 255, 255);
        boost::gil::rgb8_pixel_t black(0, 0, 0);
        fill_pixels(view(img), white);
        count = 0;
    }

    void node(const osmium::Node& node) {
        // const char* name = node.tags().get_value_by_key("name");
        // if (name) {
            view(img)(round((node.lon() + 180) * 8 * 8), round((90 - node.lat()) * 8 * 8)) = black;
            count++;
            if (count % 100000000 == 0) {
                std::cout << "count: " << count << std::endl;
                boost::gil::png_write_view("map.png", const_view(img));
            }
        // }
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
