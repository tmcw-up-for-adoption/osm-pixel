#include <iostream>
#include <getopt.h>
#include <osmium/io/any_input.hpp>
#include <osmium/io/any_output.hpp>
#include <sstream>
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>

void print_help() {
    std::cout << "osm_pixel INFILE\n\n" \
              << "Draws every node in an OpenStreetMap Planet";
}

class DrawHandler : public osmium::handler::Handler<DrawHandler> {

    public:
    boost::gil::rgb8_image_t img{2880 * 4, 2880 * 4};
    boost::gil::rgb8_pixel_t black;
    boost::gil::rgb8_pixel_t white;
    boost::gil::rgb8_image_t::view_t v;
    int x, y;

    DrawHandler(const std::string& fieldname) {
        boost::gil::rgb8_pixel_t white(255, 255, 255);
        boost::gil::rgb8_pixel_t black(0, 0, 0);
        fill_pixels(view(img), white);
    }

    void node(const osmium::Node& node) {
        x = floor((node.lon() + 180.0) / 360.0 * 2880.0 * 4.0);
        y = floor(
            (1.0 - log(
                tan(node.lat() * M_PI / 180.0) +
                1.0 / cos(node.lat() * M_PI / 180.0)
            ) / M_PI) / 2.0 * 2880.0 * 4.0);
        if (x > (2880 * 4)) x = 2880 * 4;
        else if (x < 0) x = 0;
        if (y > (2880 * 4)) y = 2880 * 4;
        else if (y < 0) y = 0;
        view(img)(x, y) = black;
    }

    void saveImage() {
        boost::gil::png_write_view("map.png", const_view(img));
    }
};

int main(int argc, char* argv[]) {
    std::string input_filename;
    int remaining_args = argc - optind;

    input_filename =  argv[optind];

    osmium::io::Reader reader(input_filename);
    osmium::io::Header header = reader.open();

    DrawHandler draw_handler("name");
    reader.apply(draw_handler);
    google::protobuf::ShutdownProtobufLibrary();
    std::ostringstream buf;

    draw_handler.saveImage();
}
