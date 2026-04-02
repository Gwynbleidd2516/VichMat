#ifndef MATH
#define MATH

#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <functional>
#include <algorithm>
#include <math.h>
using namespace std;

// ============================================================
//  Базовые структуры данных
// ============================================================

struct BodyState
{
    double x, y, z, Vx, Vy, Vz, m;
    BodyState() : x(0), y(0), z(0), Vx(0), Vy(0), Vz(0), m(1) {}
};

struct Derivatives
{
    double dxdt, dydt, dzdt, dVxdt, dVydt, dVzdt;
    Derivatives() : dxdt(0), dydt(0), dzdt(0), dVxdt(0), dVydt(0), dVzdt(0) {}

    Derivatives operator*(double s) const
    {
        Derivatives r;
        r.dxdt=dxdt*s; r.dydt=dydt*s; r.dzdt=dzdt*s;
        r.dVxdt=dVxdt*s; r.dVydt=dVydt*s; r.dVzdt=dVzdt*s;
        return r;
    }
    Derivatives &operator+=(const Derivatives &o)
    {
        dxdt+=o.dxdt; dydt+=o.dydt; dzdt+=o.dzdt;
        dVxdt+=o.dVxdt; dVydt+=o.dVydt; dVzdt+=o.dVzdt;
        return *this;
    }
    Derivatives operator+(const Derivatives &o) const
    {
        Derivatives r;
        r.dxdt=dxdt+o.dxdt; r.dydt=dydt+o.dydt; r.dzdt=dzdt+o.dzdt;
        r.dVxdt=dVxdt+o.dVxdt; r.dVydt=dVydt+o.dVydt; r.dVzdt=dVzdt+o.dVzdt;
        return r;
    }
};

class SystemState
{
public:
    std::vector<BodyState> bodies;
    std::vector<Derivatives> deriv;
    int N;
    SystemState(int n) : bodies(n), deriv(n), N(n) {}

    SystemState &operator=(const SystemState &other)
    {
        if (this != &other) { bodies=other.bodies; deriv=other.deriv; N=other.N; }
        return *this;
    }

    SystemState plus(double h, const std::vector<Derivatives> &d) const
    {
        SystemState copy = *this;
        for (int i = 0; i < N; i++)
        {
            copy.bodies[i].x  += h * d[i].dxdt;
            copy.bodies[i].y  += h * d[i].dydt;
            copy.bodies[i].z  += h * d[i].dzdt;
            copy.bodies[i].Vx += h * d[i].dVxdt;
            copy.bodies[i].Vy += h * d[i].dVydt;
            copy.bodies[i].Vz += h * d[i].dVzdt;
        }
        return copy;
    }
};

// ============================================================
//  Физика
// ============================================================

std::vector<Derivatives> compute_derivatives(const SystemState &state)
{
    std::vector<Derivatives> deriv(state.N);
    for (int i = 0; i < state.N; i++)
    {
        deriv[i].dxdt = state.bodies[i].Vx;
        deriv[i].dydt = state.bodies[i].Vy;
        deriv[i].dzdt = state.bodies[i].Vz;
    }
    for (int i = 0; i < state.N; i++)
        for (int j = i+1; j < state.N; j++)
        {
            double dx = state.bodies[j].x - state.bodies[i].x;
            double dy = state.bodies[j].y - state.bodies[i].y;
            double dz = state.bodies[j].z - state.bodies[i].z;
            double r  = sqrt(dx*dx + dy*dy + dz*dz);
            double r3 = r*r*r;
            double ci = state.bodies[j].m / r3;
            double cj = state.bodies[i].m / r3;
            deriv[i].dVxdt += dx*ci; deriv[i].dVydt += dy*ci; deriv[i].dVzdt += dz*ci;
            deriv[j].dVxdt -= dx*cj; deriv[j].dVydt -= dy*cj; deriv[j].dVzdt -= dz*cj;
        }
    return deriv;
}

// ============================================================
//  Численные методы
// ============================================================

void Euler_step(SystemState &state, double h)
{
    for (int i = 0; i < state.N; i++)
    {
        state.bodies[i].x  += h * state.bodies[i].Vx;
        state.bodies[i].y  += h * state.bodies[i].Vy;
        state.bodies[i].z  += h * state.bodies[i].Vz;
        state.bodies[i].Vx += h * state.deriv[i].dVxdt;
        state.bodies[i].Vy += h * state.deriv[i].dVydt;
        state.bodies[i].Vz += h * state.deriv[i].dVzdt;
    }
    state.deriv = compute_derivatives(state);
}

void Leapfrog_step(SystemState &state, double h)
{
    for (int i = 0; i < state.N; i++)
    {
        state.bodies[i].Vx += 0.5*h * state.deriv[i].dVxdt;
        state.bodies[i].Vy += 0.5*h * state.deriv[i].dVydt;
        state.bodies[i].Vz += 0.5*h * state.deriv[i].dVzdt;
        state.bodies[i].x  += h * state.bodies[i].Vx;
        state.bodies[i].y  += h * state.bodies[i].Vy;
        state.bodies[i].z  += h * state.bodies[i].Vz;
    }
    state.deriv = compute_derivatives(state);
    for (int i = 0; i < state.N; i++)
    {
        state.bodies[i].Vx += 0.5*h * state.deriv[i].dVxdt;
        state.bodies[i].Vy += 0.5*h * state.deriv[i].dVydt;
        state.bodies[i].Vz += 0.5*h * state.deriv[i].dVzdt;
    }
}

void RK4_step(SystemState &state, double h)
{
    std::vector<Derivatives> k1 = compute_derivatives(state);
    std::vector<Derivatives> k2 = compute_derivatives(state.plus(h/2, k1));
    std::vector<Derivatives> k3 = compute_derivatives(state.plus(h/2, k2));
    std::vector<Derivatives> k4 = compute_derivatives(state.plus(h,   k3));
    std::vector<Derivatives> combined(state.N);
    for (int i = 0; i < state.N; i++)
        combined[i] = k1[i] + k2[i]*2 + k3[i]*2 + k4[i];
    state = state.plus(h/6, combined);
    state.deriv = compute_derivatives(state);
}

// DP8 коэффициенты
static const double c2  = 0.526001519587677318e-01;
static const double c3  = 0.789002279381515978e-01;
static const double c4  = 0.118350341907227397e+00;
static const double c5  = 0.281649658092772603e+00;
static const double c6  = 0.333333333333333333e+00;
static const double c7  = 0.25e+00;
static const double c8  = 0.307692307692307692e+00;
static const double c9  = 0.651282051282051282e+00;
static const double c10 = 0.6e+00;
static const double c11 = 0.857142857142857142e+00;

static const double a21 =  5.26001519587677318e-02;
static const double a31 =  1.97250569845378994e-02, a32 = 5.91751709536136983e-02;
static const double a41 =  2.95875854768068491e-02, a43 = 8.87627564304205475e-02;
static const double a51 =  2.41365641954049e-01, a53 =-8.84549479328286086e-01, a54 = 9.24834003261792003e-01;
static const double a61 =  3.7037037037037037e-02, a64 = 1.70828608729473871e-01, a65 = 1.25467687566822429e-01;
static const double a71 =  3.7109375e-02, a74 = 1.70252211019544039e-01, a75 = 6.02165389804559092e-02, a76 =-1.7578125e-02;
static const double a81 =  3.70920001185047927e-02, a84 = 1.70383925712239993e-01, a85 = 1.07262030446373284e-01, a86 =-1.53194377486244882e-02, a87 = 8.27378916792996988e-03;
static const double a91 =  6.24110958716075712e-01, a94 =-3.36089262944694129e+00, a95 =-8.68219346841726006e-01, a96 = 2.72170569800576667e+01, a97 =-2.43093619143939525e+01, a98 = 7.67917658501012917e+00;
static const double a101=  4.77662536438264366e-01, a104=-2.48811461997166764e+00, a105=-5.90290826836842996e-01, a106= 2.12300514481811942e+01, a107=-1.87717063725980367e+01, a108= 5.99004794812679254e+00, a109=-6.26835576062994894e-01;
static const double a111= -9.31463175788185445e-01, a114= 5.64011468450664607e+00, a115= 2.19149943912950647e+00, a116=-2.77767416803128227e+01, a117= 2.64920887616478832e+01, a118=-9.26750500674845044e+00, a119= 4.29400992168673737e-01, a1110= 1.26148050801826451e+00;
static const double a121=  2.27331014751653820e-01, a124=-1.05344954667372501e+01, a125=-2.00087205822486002e+00, a126=-1.79589318631187990e+01, a127= 2.79488845294199600e+01, a128=-2.85899827713502369e+00, a129=-8.87285693353062954e+00, a1210= 1.23605671757943034e+01, a1211= 6.43392880736874482e-01;

static const double b1=5.42937341165687296e-02, b6=4.45031289275240888e+00, b7=1.89151789931450038e+00, b8=-5.8012039600105847e+00, b9=3.1116436695781989e-01, b10=-1.52160949662516078e-01, b11=2.01365400804030348e-01, b12=4.47106157277725905e-02;
static const double e1=0.1312004499419488073e-01, e6=-0.1225156446376204440e-05, e7=-0.4957589496572501915e-03, e8=0.1664377182454986536e-02, e9=-0.3550085900823018509e-03, e10=0.2341883419600798046e-03, e11=0.3435651745687565260e-04;

static std::vector<Derivatives> make_acc(int N,
    const std::vector<Derivatives>& k1, double a1,
    const std::vector<Derivatives>& k2, double a2,
    const std::vector<Derivatives>& k3 = {}, double a3 = 0,
    const std::vector<Derivatives>& k4 = {}, double a4 = 0,
    const std::vector<Derivatives>& k5 = {}, double a5 = 0,
    const std::vector<Derivatives>& k6 = {}, double a6 = 0,
    const std::vector<Derivatives>& k7 = {}, double a7 = 0,
    const std::vector<Derivatives>& k8 = {}, double a8 = 0,
    const std::vector<Derivatives>& k9 = {}, double a9 = 0,
    const std::vector<Derivatives>& k10= {}, double a10= 0,
    const std::vector<Derivatives>& k11= {}, double a11= 0)
{
    std::vector<Derivatives> d(N);
    for (int i = 0; i < N; i++)
    {
        d[i] = k1[i]*a1 + k2[i]*a2;
        if (!k3.empty())  d[i] += k3[i]*a3;
        if (!k4.empty())  d[i] += k4[i]*a4;
        if (!k5.empty())  d[i] += k5[i]*a5;
        if (!k6.empty())  d[i] += k6[i]*a6;
        if (!k7.empty())  d[i] += k7[i]*a7;
        if (!k8.empty())  d[i] += k8[i]*a8;
        if (!k9.empty())  d[i] += k9[i]*a9;
        if (!k10.empty()) d[i] += k10[i]*a10;
        if (!k11.empty()) d[i] += k11[i]*a11;
    }
    return d;
}

static void dp8_stages(const SystemState &state, double h,
    std::vector<Derivatives> &k1,  std::vector<Derivatives> &k2,
    std::vector<Derivatives> &k3,  std::vector<Derivatives> &k4,
    std::vector<Derivatives> &k5,  std::vector<Derivatives> &k6,
    std::vector<Derivatives> &k7,  std::vector<Derivatives> &k8,
    std::vector<Derivatives> &k9,  std::vector<Derivatives> &k10,
    std::vector<Derivatives> &k11, std::vector<Derivatives> &k12)
{
    int N = state.N;
    k1  = compute_derivatives(state);
    k2  = compute_derivatives(state.plus(h*c2,  make_acc(N, k1,a21, k1,0)));
    k3  = compute_derivatives(state.plus(h*c3,  make_acc(N, k1,a31, k2,a32)));
    k4  = compute_derivatives(state.plus(h*c4,  make_acc(N, k1,a41, k3,a43)));
    k5  = compute_derivatives(state.plus(h*c5,  make_acc(N, k1,a51, k3,a53, k4,a54)));
    k6  = compute_derivatives(state.plus(h*c6,  make_acc(N, k1,a61, k4,a64, k5,a65)));
    k7  = compute_derivatives(state.plus(h*c7,  make_acc(N, k1,a71, k4,a74, k5,a75, k6,a76)));
    k8  = compute_derivatives(state.plus(h*c8,  make_acc(N, k1,a81, k4,a84, k5,a85, k6,a86, k7,a87)));
    k9  = compute_derivatives(state.plus(h*c9,  make_acc(N, k1,a91, k4,a94, k5,a95, k6,a96, k7,a97, k8,a98)));
    k10 = compute_derivatives(state.plus(h*c10, make_acc(N, k1,a101,k4,a104,k5,a105,k6,a106,k7,a107,k8,a108,k9,a109)));
    k11 = compute_derivatives(state.plus(h*c11, make_acc(N, k1,a111,k4,a114,k5,a115,k6,a116,k7,a117,k8,a118,k9,a119,k10,a1110)));
    k12 = compute_derivatives(state.plus(h,     make_acc(N, k1,a121,k4,a124,k5,a125,k6,a126,k7,a127,k8,a128,k9,a129,k10,a1210,k11,a1211)));
}

void DP8_step(SystemState &state, double h)
{
    std::vector<Derivatives> k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12;
    dp8_stages(state, h, k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12);
    std::vector<Derivatives> sol(state.N);
    for (int i = 0; i < state.N; i++)
        sol[i] = k1[i]*b1 + k6[i]*b6 + k7[i]*b7 + k8[i]*b8
               + k9[i]*b9 + k10[i]*b10 + k11[i]*b11 + k12[i]*b12;
    state = state.plus(h, sol);
    state.deriv = compute_derivatives(state);
}

double DP8_adaptive_step(SystemState &state, double h,
                         double tol_rel = 1e-10, double tol_abs = 1e-12)
{
    const double h_min=1e-8, h_max=0.1, safety=0.9, min_factor=0.2, max_factor=5.0;
    while (true)
    {
        std::vector<Derivatives> k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12;
        dp8_stages(state, h, k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12);

        std::vector<Derivatives> sol8(state.N);
        for (int i = 0; i < state.N; i++)
            sol8[i] = k1[i]*b1 + k6[i]*b6 + k7[i]*b7 + k8[i]*b8
                    + k9[i]*b9 + k10[i]*b10 + k11[i]*b11 + k12[i]*b12;
        SystemState next = state.plus(h, sol8);

        std::vector<Derivatives> err_d(state.N);
        for (int i = 0; i < state.N; i++)
            err_d[i] = k1[i]*e1 + k6[i]*e6 + k7[i]*e7 + k8[i]*e8
                     + k9[i]*e9 + k10[i]*e10 + k11[i]*e11;

        double err = 0.0; int cnt = 0;
        for (int i = 0; i < state.N; i++)
        {
            auto sq = [&](double y0, double yn, double dy) {
                double sc = tol_abs + tol_rel * std::max(std::abs(y0), std::abs(yn));
                double e  = h * dy / sc; return e*e;
            };
            err += sq(state.bodies[i].x,  next.bodies[i].x,  err_d[i].dxdt);
            err += sq(state.bodies[i].y,  next.bodies[i].y,  err_d[i].dydt);
            err += sq(state.bodies[i].Vx, next.bodies[i].Vx, err_d[i].dVxdt);
            err += sq(state.bodies[i].Vy, next.bodies[i].Vy, err_d[i].dVydt);
            cnt += 4;
        }
        err = std::sqrt(err / cnt);

        double factor = safety * std::pow(1.0/(err+1e-300), 1.0/8.0);
        factor = std::min(max_factor, std::max(min_factor, factor));
        double h_new = std::min(h_max, std::max(h_min, h*factor));

        if (err <= 1.0)
        {
            next.deriv = compute_derivatives(next);
            state = next;
            return h_new;
        }
        else
        {
            h = h_new;
            if (h <= h_min)
            {
                next.deriv = compute_derivatives(next);
                state = next;
                return h_min;
            }
        }
    }
}

// ============================================================
//  Начальные условия
// ============================================================

void init_figure_eight(SystemState &state)
{
    state.bodies[0].x = 0.97000436;  state.bodies[0].y = -0.24308753; state.bodies[0].z = 0;
    state.bodies[1].x =-0.97000436;  state.bodies[1].y =  0.24308753; state.bodies[1].z = 0;
    state.bodies[2].x = 0.0;         state.bodies[2].y =  0.0;        state.bodies[2].z = 0;
    double vx = 0.93240737/2.0, vy = 0.86473146/2.0;
    state.bodies[0].Vx = vx;   state.bodies[0].Vy = vy;   state.bodies[0].Vz = 0;
    state.bodies[1].Vx = vx;   state.bodies[1].Vy = vy;   state.bodies[1].Vz = 0;
    state.bodies[2].Vx =-2*vx; state.bodies[2].Vy =-2*vy; state.bodies[2].Vz = 0;
    for (int i = 0; i < 3; i++) state.bodies[i].m = 1.0;
    state.deriv = compute_derivatives(state);
}

void init_polygon(SystemState &state, double R = 1.0)
{
    int N = state.N;
    double sum = 0.0;
    for (int k = 1; k < N; k++) sum += 1.0/std::sin(M_PI*k/N);
    double omega = std::sqrt(sum/(4.0*R*R*R));
    for (int i = 0; i < N; i++)
    {
        double angle = 2.0*M_PI*i/N;
        state.bodies[i].x  =  R*std::cos(angle);
        state.bodies[i].y  =  R*std::sin(angle);
        state.bodies[i].z  =  0.0;
        state.bodies[i].Vx = -omega*R*std::sin(angle);
        state.bodies[i].Vy =  omega*R*std::cos(angle);
        state.bodies[i].Vz =  0.0;
        state.bodies[i].m  =  1.0;
    }
    state.deriv = compute_derivatives(state);
}

void init_super_eight(SystemState &state)
{
    if (state.N != 4) { std::cerr << "init_super_eight: need 4 bodies\n"; return; }
    state.bodies[0].x =  1.382857; state.bodies[0].y = 0.0; state.bodies[0].z = 0.0;
    state.bodies[1].x =  0.157866; state.bodies[1].y = 0.0; state.bodies[1].z = 0.0;
    state.bodies[2].x = -0.157866; state.bodies[2].y = 0.0; state.bodies[2].z = 0.0;
    state.bodies[3].x = -1.382857; state.bodies[3].y = 0.0; state.bodies[3].z = 0.0;
    state.bodies[0].Vx = 0.0; state.bodies[0].Vy =  0.584873; state.bodies[0].Vz = 0.0;
    state.bodies[1].Vx = 0.0; state.bodies[1].Vy = -1.871935; state.bodies[1].Vz = 0.0;
    state.bodies[2].Vx = 0.0; state.bodies[2].Vy =  1.871935; state.bodies[2].Vz = 0.0;
    state.bodies[3].Vx = 0.0; state.bodies[3].Vy = -0.584873; state.bodies[3].Vz = 0.0;
    for (int i = 0; i < 4; i++) state.bodies[i].m = 1.0;
    state.deriv = compute_derivatives(state);
}

void init_butterfly(SystemState &state)
{
    if (state.N != 3) { std::cerr << "init_butterfly: need 3 bodies\n"; return; }
    state.bodies[0].x = -1.0; state.bodies[0].y = 0.0; state.bodies[0].z = 0.0;
    state.bodies[1].x =  1.0; state.bodies[1].y = 0.0; state.bodies[1].z = 0.0;
    state.bodies[2].x =  0.0; state.bodies[2].y = 0.0; state.bodies[2].z = 0.0;
    double vx = 0.30689, vy = 0.12551;
    state.bodies[0].Vx = vx;      state.bodies[0].Vy = vy;      state.bodies[0].Vz = 0;
    state.bodies[1].Vx = vx;      state.bodies[1].Vy = vy;      state.bodies[1].Vz = 0;
    state.bodies[2].Vx = -2.0*vx; state.bodies[2].Vy = -2.0*vy; state.bodies[2].Vz = 0;
    for (int i = 0; i < 3; i++) state.bodies[i].m = 1.0;
    state.deriv = compute_derivatives(state);
}

void init_moth(SystemState &state)
{
    if (state.N != 3) { std::cerr << "init_moth: need 3 bodies\n"; return; }
    state.bodies[0].x = -1.0; state.bodies[0].y = 0.0; state.bodies[0].z = 0.0;
    state.bodies[1].x =  1.0; state.bodies[1].y = 0.0; state.bodies[1].z = 0.0;
    state.bodies[2].x =  0.0; state.bodies[2].y = 0.0; state.bodies[2].z = 0.0;
    double vx = 0.39295, vy = 0.09758;
    state.bodies[0].Vx = vx;      state.bodies[0].Vy = vy;      state.bodies[0].Vz = 0;
    state.bodies[1].Vx = vx;      state.bodies[1].Vy = vy;      state.bodies[1].Vz = 0;
    state.bodies[2].Vx = -2.0*vx; state.bodies[2].Vy = -2.0*vy; state.bodies[2].Vz = 0;
    for (int i = 0; i < 3; i++) state.bodies[i].m = 1.0;
    state.deriv = compute_derivatives(state);
}

// ============================================================
//  Диагностика
// ============================================================

double compute_energy(const SystemState &state)
{
    double K = 0, U = 0;
    for (int i = 0; i < state.N; i++)
    {
        double v2 = state.bodies[i].Vx*state.bodies[i].Vx
                  + state.bodies[i].Vy*state.bodies[i].Vy
                  + state.bodies[i].Vz*state.bodies[i].Vz;
        K += 0.5*state.bodies[i].m*v2;
    }
    for (int i = 0; i < state.N; i++)
        for (int j = i+1; j < state.N; j++)
        {
            double dx = state.bodies[j].x-state.bodies[i].x;
            double dy = state.bodies[j].y-state.bodies[i].y;
            double dz = state.bodies[j].z-state.bodies[i].z;
            U -= state.bodies[i].m*state.bodies[j].m/sqrt(dx*dx+dy*dy+dz*dz);
        }
    return K+U;
}

double compute_angular_momentum_z(const SystemState &state)
{
    double Lz = 0.0;
    for (int i = 0; i < state.N; i++)
        Lz += state.bodies[i].m*(state.bodies[i].x*state.bodies[i].Vy
                                -state.bodies[i].y*state.bodies[i].Vx);
    return Lz;
}

// ============================================================
//  Запись в CSV
// ============================================================

void write_csv_header(std::ofstream &out, int N)
{
    out << "t,E,Lz";
    for (int i = 0; i < N; i++)
        out << ",x" << i << ",y" << i << ",z" << i
            << ",Vx" << i << ",Vy" << i << ",Vz" << i;
    out << "\n";
}

void write_csv_row(std::ofstream &out, double t, const SystemState &state)
{
    out << std::setprecision(15) << t << "," << compute_energy(state)
        << "," << compute_angular_momentum_z(state);
    for (int i = 0; i < state.N; i++)
        out << "," << state.bodies[i].x << "," << state.bodies[i].y << "," << state.bodies[i].z
            << "," << state.bodies[i].Vx << "," << state.bodies[i].Vy << "," << state.bodies[i].Vz;
    out << "\n";
}

void save_initial_state(const std::string &filename, const SystemState &state)
{
    std::ofstream f(filename, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot open: " + filename);
    f.write(reinterpret_cast<const char*>(&state.N), sizeof(int));
    for (int i = 0; i < state.N; i++)
        f.write(reinterpret_cast<const char*>(&state.bodies[i]), sizeof(BodyState));
}

SystemState load_initial_state(const std::string &filename)
{
    std::ifstream f(filename, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot open: " + filename);
    int N; f.read(reinterpret_cast<char*>(&N), sizeof(int));
    SystemState state(N);
    for (int i = 0; i < N; i++)
        f.read(reinterpret_cast<char*>(&state.bodies[i]), sizeof(BodyState));
    state.deriv = compute_derivatives(state);
    return state;
}

// ============================================================
//  Параметры методов
// ============================================================

enum class Method { EULER, LEAPFROG, RK4, DP8, DP8_ADAPTIVE };

struct ExperimentParams
{
    Method method       = Method::RK4;
    double h            = 0.001;
    double T_end        = 6.3259 * 5;
    int    output_every = 10;
    std::string output_file = "trajectory.csv";
    bool   realtime     = false;
    double tol_rel      = 1e-10;
    double tol_abs      = 1e-12;
};

using RealtimeCallback = std::function<bool(double t, const SystemState &state)>;

SystemState run_experiment(
    const SystemState &initial,
    const ExperimentParams &params,
    RealtimeCallback callback = nullptr)
{
    SystemState state = initial;
    std::ofstream out;
    if (!params.realtime)
    {
        out.open(params.output_file);
        if (!out) throw std::runtime_error("Cannot open: " + params.output_file);
        write_csv_header(out, state.N);
    }

    double h = params.h;
    double t = 0.0;

    auto do_step = [&]()
    {
        switch (params.method)
        {
        case Method::EULER:        Euler_step(state, h);    break;
        case Method::LEAPFROG:     Leapfrog_step(state, h); break;
        case Method::RK4:          RK4_step(state, h);      break;
        case Method::DP8:          DP8_step(state, h);      break;
        case Method::DP8_ADAPTIVE:
            h = DP8_adaptive_step(state, h, params.tol_rel, params.tol_abs);
            break;
        }
    };

    if (params.method == Method::DP8_ADAPTIVE)
    {
        int step = 0;
        while (t < params.T_end)
        {
            if (t + h > params.T_end) h = params.T_end - t;
            if (step % params.output_every == 0)
            {
                if (params.realtime) { if (callback && !callback(t, state)) return state; }
                else                 { write_csv_row(out, t, state); }
            }
            do_step(); t += h; step++;
        }
    }
    else
    {
        int steps = static_cast<int>(params.T_end / h);
        for (int step = 0; step <= steps; step++)
        {
            t = step * h;
            if (step % params.output_every == 0)
            {
                if (params.realtime) { if (callback && !callback(t, state)) break; }
                else                 { write_csv_row(out, t, state); }
            }
            if (step < steps) do_step();
        }
    }
    return state;
}

void sensitivity_experiment(
    const SystemState &initial,
    double delta, double h, double T_end,
    Method method, const std::string &output_file)
{
    SystemState ref  = initial;
    SystemState pert = initial;
    pert.bodies[0].x += delta;
    pert.deriv = compute_derivatives(pert);
    std::ofstream out(output_file);
    if (!out) throw std::runtime_error("Cannot open: " + output_file);
    out << "t,deviation,E_ref\n";
    int steps = static_cast<int>(T_end / h);
    auto do_step = [&](SystemState &s) {
        switch (method) {
        case Method::EULER:        Euler_step(s, h);        break;
        case Method::LEAPFROG:     Leapfrog_step(s, h);     break;
        case Method::RK4:          RK4_step(s, h);          break;
        case Method::DP8:          DP8_step(s, h);          break;
        case Method::DP8_ADAPTIVE: DP8_adaptive_step(s, h); break;
        }
    };
    for (int step = 0; step <= steps; step++)
    {
        double t = step * h;
        if (step % 100 == 0)
        {
            double dev = 0.0;
            for (int i = 0; i < ref.N; i++)
            {
                double dx = ref.bodies[i].x-pert.bodies[i].x;
                double dy = ref.bodies[i].y-pert.bodies[i].y;
                double dz = ref.bodies[i].z-pert.bodies[i].z;
                dev += std::sqrt(dx*dx+dy*dy+dz*dz);
            }
            out << std::setprecision(15) << t << "," << dev << "," << compute_energy(ref) << "\n";
        }
        if (step < steps) { do_step(ref); do_step(pert); }
    }
}

// ============================================================
//  Сравнение методов
// ============================================================
//
//  Запускаем несколько методов с одинаковых начальных условий параллельно
//  и на каждом шаге сохраняем расстояние между их траекториями.
//
//  Расстояние между двумя состояниями A и B в момент t:
//    dist(A,B,t) = sum_{i=0}^{N-1} sqrt( (xA_i - xB_i)^2 + (yA_i - yB_i)^2 + (zA_i - zB_i)^2 )
//  Это суммарное евклидово расстояние по всем телам — показывает,
//  насколько сильно разошлись траектории в данный момент.
//
//  В конце также пишем RMSD (root mean square deviation) по времени —
//  одно число, характеризующее «среднее расхождение» за всё время симуляции:
//    RMSD(A,B) = sqrt( mean_t( dist(A,B,t)^2 ) )
//  По нему удобно сравнивать: чем меньше RMSD между методом X и DP8 (эталоном),
//  тем точнее метод X.
//
//  Параметры:
//    initial      — начальное состояние (одинаковое для всех методов)
//    methods      — список методов для сравнения
//    method_names — названия методов (для заголовка CSV)
//    h            — шаг для фиксированных методов
//    T_end        — время симуляции
//    output_every — записывать каждые N шагов (чтобы не раздувать файл)
//    output_file  — имя CSV-файла с результатами

struct CompareResult
{
    // RMSD между каждой парой методов за всё время симуляции
    // rmsd[i][j] = RMSD между methods[i] и methods[j]
    std::vector<std::vector<double>> rmsd;
    std::vector<std::string> names;
};

CompareResult compare_methods(
    const SystemState &initial,
    const std::vector<Method> &methods,
    const std::vector<std::string> &method_names,
    double h,
    double T_end,
    int output_every,
    const std::string &output_file)
{
    int M = (int)methods.size(); // количество сравниваемых методов
    int N = initial.N;           // количество тел

    // Создаём отдельную копию состояния для каждого метода —
    // каждый интегрируется независимо от одной стартовой точки
    std::vector<SystemState> states(M, initial);

    // Открываем CSV
    std::ofstream out(output_file);
    if (!out) throw std::runtime_error("Cannot open: " + output_file);

    // Заголовок: t, E_Euler, E_Leapfrog, ..., dist(A,B), dist(A,C), ...
    out << "t";
    for (int m = 0; m < M; m++)
        out << ",E_" << method_names[m];
    for (int a = 0; a < M; a++)
        for (int b = a+1; b < M; b++)
            out << ",dist_" << method_names[a] << "_vs_" << method_names[b];
    out << "\n";

    // Накапливаем суммы квадратов расстояний для подсчёта RMSD в конце
    // sum_sq[a][b] = сумма dist(a,b,t)^2 по всем записанным шагам
    std::vector<std::vector<double>> sum_sq(M, std::vector<double>(M, 0.0));
    int record_count = 0;

    int steps = static_cast<int>(T_end / h);

    for (int step = 0; step <= steps; step++)
    {
        double t = step * h;

        if (step % output_every == 0)
        {
            out << std::setprecision(15) << t;

            // Энергия каждого метода (должна сохраняться; отклонение = ошибка метода)
            for (int m = 0; m < M; m++)
                out << "," << compute_energy(states[m]);

            // Расстояние между каждой парой методов в текущий момент t
            for (int a = 0; a < M; a++)
            {
                for (int b = a+1; b < M; b++)
                {
                    // Суммируем евклидовы расстояния по всем телам
                    double dist = 0.0;
                    for (int i = 0; i < N; i++)
                    {
                        double dx = states[a].bodies[i].x - states[b].bodies[i].x;
                        double dy = states[a].bodies[i].y - states[b].bodies[i].y;
                        double dz = states[a].bodies[i].z - states[b].bodies[i].z;
                        dist += std::sqrt(dx*dx + dy*dy + dz*dz);
                    }
                    out << "," << dist;
                    sum_sq[a][b] += dist * dist;
                }
            }
            out << "\n";
            record_count++;
        }

        // Делаем шаг каждым методом независимо
        if (step < steps)
        {
            for (int m = 0; m < M; m++)
            {
                switch (methods[m])
                {
                case Method::EULER:        Euler_step(states[m], h);        break;
                case Method::LEAPFROG:     Leapfrog_step(states[m], h);     break;
                case Method::RK4:          RK4_step(states[m], h);          break;
                case Method::DP8:          DP8_step(states[m], h);          break;
                case Method::DP8_ADAPTIVE: DP8_adaptive_step(states[m], h); break;
                }
            }
        }
    }

    // Считаем RMSD для каждой пары
    // RMSD(A,B) = sqrt( mean( dist(A,B,t)^2 ) ) = sqrt( sum_sq[a][b] / record_count )
    CompareResult result;
    result.names = method_names;
    result.rmsd.assign(M, std::vector<double>(M, 0.0));
    for (int a = 0; a < M; a++)
        for (int b = a+1; b < M; b++)
            result.rmsd[a][b] = result.rmsd[b][a] =
                std::sqrt(sum_sq[a][b] / std::max(record_count, 1));

    // Пишем итоговую таблицу RMSD в конец того же файла
    out << "\n# RMSD summary (root mean square distance between trajectories)\n";
    out << "#";
    for (int m = 0; m < M; m++) out << "," << method_names[m];
    out << "\n";
    for (int a = 0; a < M; a++)
    {
        out << "# " << method_names[a];
        for (int b = 0; b < M; b++)
            out << "," << std::setprecision(6) << result.rmsd[a][b];
        out << "\n";
    }

    std::cout << "[compare_methods] Done. Output: " << output_file << "\n";
    std::cout << "[compare_methods] RMSD matrix:\n";
    for (int a = 0; a < M; a++)
    {
        std::cout << "  " << method_names[a] << ": ";
        for (int b = 0; b < M; b++)
            std::cout << method_names[b] << "=" << std::scientific
                      << std::setprecision(3) << result.rmsd[a][b] << "  ";
        std::cout << "\n";
    }

    return result;
}

#endif