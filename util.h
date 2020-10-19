#ifndef UTIL_H
#define UTIL_H

#include <Halide.h>

using namespace Halide;

inline Func linspace_func(Expr start, Expr stop, RDom r) {
    Var x{"x"};
    Expr step("step");
    step = (stop - start) / (r.x.extent() - Expr(1));
    Func linspace("linspace");
    linspace(x) = start;
    linspace(r) = start + r * step;
    // force "stop" value to avoid floating point deviations
    linspace(r.x.extent() - 1) = stop;
    return linspace;
}

inline Func arange_func(Expr start, Expr stop, Expr step) {
    Var x{"x"};
    Expr extent("extent");
    extent = ConciseCasts::i32(ceil((stop - start) / step));
    RDom r(0, extent, "r");
    Func arange("arange");
    arange(x) = start;
    arange(r) = start + r * step;
    return arange;
}

// a and b are assumed to be vectors of length 3
inline Func cross3_func(Func a, Func b) {
    Var x{"x"};
    Func cross("cross");
    // // c_x = a_y * b_z − a_z * b_y
    // cross(0) = a(1) * b(2) - a(2) * b(1);
    // // c_y = a_z * b_x − a_x * b_z
    // cross(1) = a(2) * b(0) - a(0) * b(2);
    // // c_z = a_x * b_y − a_y * b_x
    // cross(2) = a(0) * b(1) - a(1) * b(0);
    cross(x) = a((x + 1) % 3) * b((x + 2) % 3) - a((x + 2) % 3) * b((x + 1) % 3);
    return cross;
}

inline Expr hstack1_expr(Func a, Func b, Expr extent, Var x) {
    return select(x < extent,
                  a(clamp(x, 0, extent - 1)),
                  b(clamp(x - extent, 0, extent - 1)));
}

inline Func hstack1_func(Func a, Func b, Expr extent, const std::string &name = "hstack1") {
    Var x{"x"};
    Func f(name);
    f(x) = hstack1_expr(a, b, extent, x);
    return f;
}

inline Expr hstack2_expr(Func a, Func b, Expr x_extent, Var x, Var y) {
    return select(x < x_extent,
                  a(clamp(x, 0, x_extent - 1), y),
                  b(clamp(x - x_extent, 0, x_extent - 1), y));
}

inline Func hstack2_func(Func a, Func b, Expr x_extent, const std::string &name = "hstack2") {
    Var x{"x"}, y{"y"};
    Func f(name);
    f(x, y) = hstack2_expr(a, b, x_extent, x, y);
    return f;
}

inline Expr vstack1_expr(Func a, Func b, Expr extent, Var x, Var y) {
    return select(y == 0,
                  a(clamp(x, 0, extent - 1)),
                  b(clamp(x, 0, extent - 1)));
}

inline Func vstack1_func(Func a, Func b, Expr extent, const std::string &name = "vstack1") {
    Var x{"x"}, y{"y"};
    Func f(name);
    f(x, y) = vstack1_expr(a, b, extent, x, y);
    return f;
}

inline Expr vstack2_expr(Func a, Func b, Expr y_extent, Var x, Var y) {
    return select(y < y_extent,
                  a(x, clamp(y, 0, y_extent - 1)),
                  b(x, clamp(y - y_extent, 0, y_extent - 1)));
}

inline Func vstack2_func(Func a, Func b, Expr y_extent, const std::string &name = "vstack2") {
    Var x{"x"}, y{"y"};
    Func f(name);
    f(x, y) = vstack2_expr(a, b, y_extent, x, y);
    return f;
}

inline Expr log2f_expr(Expr x) {
    Expr log2 = log(x) / log(Expr(2.0f));
    return log2;
}

inline Expr norm_expr(Expr in) {
    return sqrt(sum(in * in));
}

inline Expr repeat1_expr(Func a, Expr extent_a, Var x) {
    return a(x % extent_a);
}

inline Func repeat1_func(Func a, Expr extent_a, const std::string &name = "repeat1") {
    Var x{"x"};
    Func f(name);
    f(x) = repeat1_expr(a, extent_a, x);
    return f;
}

#endif
