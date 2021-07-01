#ifndef DRAWABLES_H
#define DRAWABLES_H
#include <hwlib.hpp>


/// \brief
/// This class is the base for all the drawings and shapes.
/// \details
/// This drawable class includes all the functions used in the other classes. most of them are overwritten to
/// get their own shape etc.
class drawable {
protected:
    hwlib::window &w;
    hwlib::xy location;
    hwlib::xy size;
    hwlib::xy bounce;
public:
    /// \brief
    /// this constructor sets up a drawable class.
    /// \details
    /// This class includes a reference to the window therefore the shape can be drawn.
    /// @param w is the window reference.
    /// @param location represents the spot on the window
    /// @param size represents the size of the drawable. used for interaction with other object.
    /// @param bounce is used by the ball, when (1,1) the ball goes through this object. (-1,1) changes the x-speed, (1,-1) the y-speed.
    /// @attention this class can't be drawn, updated, or interact. most functions are virtual.
    drawable(hwlib::window &w, hwlib::xy location, hwlib::xy size, hwlib::xy bounce = {1,1});
    /// \brief
    /// this virtual function is later used to draw objects.
    virtual void draw() = 0;
    /// \brief
    /// this function returns true if x is between a and b.
    /// ~~~~~~~~~~~~~~~~~~~~~~.cpp
    /// return ( x >= a ) && ( x <= b );
    /// ~~~~~~~~~~~~~~~~~~~~~~
    /// @param x is the number in question.
    /// @param a is the left boundary.
    /// @param b is the right boundary.
    bool within( int x, int a, int b );
    /// \brief
    /// this virtual function is later used to update the location of objects.
    /// @see overlaps()
    virtual void update() {}
    /// \brief
    /// this virtual function is later used to interact with other objects.
    virtual void interact(drawable &other) {}
    /// \brief
    /// this function returns the bounce. Used in the interact function.
    hwlib::xy get_bounce();
    /// \brief
    /// this function returns if a object overlaps with another object.
    bool overlaps( const drawable & other );
    /// \brief
    /// this function returns the location.
    hwlib::xy get_location();
};

/// \brief
/// this class is used to draw lines.
/// \details
/// This class is a setup for the hwlib::line function. Because of this it is easier to use.
class line : public drawable{
protected:
    hwlib::xy end;
public:
    /// \brief
    /// this constructor sets up the line class.
    /// \details
    /// In this constructor is all the information given to write a line on the window.
    /// @param end is the location where the line ends.
    /// @param location is where the line starts.
    /// @see drawable
    line( hwlib::window & w, hwlib::xy  location, hwlib::xy  end, hwlib::xy bounce = {1,1});
    /// \brief
    /// this function draws a line on window w, starting from location to end.
    void draw() override;
};

/// \brief
/// this class is used to draw circles.
/// \details
/// This class is a setup for the hwlib::circle function. Because of this it is easier to use.
class circle : public drawable {
protected:
    int radius;
public:
    /// \brief
    /// this constructor sets up the circle class.
    /// \details
    /// In this constructor is all the information given to write a circle on the window.
    /// @param center is the center of the circle.
    /// @param radius is the radius of the circle.
    /// @see drawable
    circle( hwlib::window & w, hwlib::xy center, int radius, hwlib::xy bounce = {1,1} );
    /// \brief
    /// this function draws a circle on window w.
    void draw() override;
};

/// \brief
/// this class is used to draw rectangles.
/// \details
/// This class is a setup to draw rectangles. these rectangles can be filled or drawn by lines.
class rectangle : public drawable {
protected:
    hwlib::xy end;
    bool filled;
public:
    /// \brief
    /// this constructor sets up the rectangle class.
    /// \details
    /// In this constructor is all the information given to write a rectangle on the window.
    /// @param location is the left top of the rectangle.
    /// @param end right bottom of the rectangle.
    /// @param filled is true if the rectangle needs to be colored in, otherwise the rectangle is open.
    /// @see drawable
    rectangle(hwlib::window &w, hwlib::xy location, hwlib::xy end, hwlib::xy bounce = {1, 1},  bool gevuld = false);
    /// \brief
    /// this function draws a rectangle on window w. filled or not filled.
    void draw() override;
};

#endif //DRAWABLES_H