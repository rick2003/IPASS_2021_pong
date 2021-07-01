#include "drawables.hpp"

drawable::drawable(hwlib::window &w, hwlib::xy location, hwlib::xy size, hwlib::xy bounce):
        w(w),
        location(location),
        size(size), bounce ( bounce) {}
bool drawable::within( int x, int a, int b ){
    return ( x >= a ) && ( x <= b );
}
hwlib::xy drawable::get_bounce(){
    return bounce;
}
bool drawable::overlaps( const drawable & other ){
    bool x_overlap = within(
            location.x,
            other.location.x,
            other.location.x + other.size.x
    ) || within(
            other.location.x,
            location.x ,
            location.x + size.x
    );

    bool y_overlap = within(
            location.y -1,
            other.location.y,
            other.location.y + other.size.y
    ) || within(
            other.location.y,
            location.y -1,
            location.y + size.y
    );

    return x_overlap && y_overlap;
}
hwlib::xy drawable::get_location(){return location;};

line::line( hwlib::window & w, hwlib::xy  location, hwlib::xy  end, hwlib::xy bounce):
        drawable(w, location, end-location, bounce),
        end( end )
{}
void line::draw(){
    hwlib::line x( location, end );
    x.draw( w );;
}

circle::circle( hwlib::window & w, hwlib::xy center, int radius, hwlib::xy bounce ):
        drawable( w, center - hwlib::xy(radius, radius) ,hwlib::xy( radius, radius ) * 2, bounce ),
        radius( radius )
{}
void circle::draw(){
    hwlib::circle c( location + hwlib::xy(radius, radius) , radius );
    c.draw( w );
}

rectangle::rectangle(hwlib::window &w, hwlib::xy location, hwlib::xy end, hwlib::xy bounce,  bool filled):
        drawable(w, location, (end - location), bounce),
        end(end),
        filled(filled) {}
void rectangle::draw(){
    if (!filled) {
        line top(w, location, hwlib::xy(end.x, location.y));
        line left(w, location, hwlib::xy(location.x, end.y));
        line bottom(w, hwlib::xy(location.x, end.y), hwlib::xy(end.x, end.y));
        line right(w, hwlib::xy(end.x, location.y), hwlib::xy(end.x, end.y + 1));
        top.draw();
        left.draw();
        bottom.draw();
        right.draw();
    } else {
        for (int i = location.y; i <= end.y; i++) {
            for (int j = location.x; j <= end.x; j++) {
                w.write(hwlib::xy(j, i));
            }
        }
    }
}

