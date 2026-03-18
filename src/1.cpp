#include <iostream>
#include <vector>
#include <cmath>

struct BodyState {
    double x, y, z, Vx, Vy, Vz, m;
    BodyState() : x(0), y(0), z(0), Vx(0), Vy(0), Vz(0), m(0){}
};

struct Derivatives {
    double dxdt, dydt, dzdt, dVxdt, dVydt, dVzdt;
    Derivatives() : dxdt(0), dydt(0), dzdt(0), dVxdt(0), dVydt(0), dVzdt(0){}

    Derivatives operator*(double scalar) const{
        Derivatives result;
        result.dxdt = dxdt * scalar;
        result.dydt = dydt * scalar;
        result.dzdt = dzdt * scalar;
        result.dVxdt = dVxdt * scalar;
        result.dVydt = dVydt * scalar;
        result.dVzdt = dVzdt * scalar;
        return result;
    }

    Derivatives operator+=(const Derivatives& other){
        dxdt += other.dxdt;
        dydt += other.dydt;
        dzdt += other.dzdt;
        dVxdt += other.dVxdt;
        dVydt += other.dVydt;
        dVzdt += other.dVzdt;
        return *this;
    }

    Derivatives operator+(const Derivatives& other) const{
        Derivatives result;
        result.dxdt = dxdt + other.dxdt;
        result.dydt = dydt + other.dydt;
        result.dzdt = dzdt + other.dzdt;
        result.dVxdt = dVxdt + other.dVxdt;
        result.dVydt = dVydt + other.dVydt;
        result.dVzdt = dVzdt + other.dVzdt;
        return result;
    }
};

class SystemState{
public:
    std::vector<BodyState> bodies;
    std::vector<Derivatives> deriv;
    int N;
    SystemState(int n) : bodies(n), deriv(n), N(n){}

    SystemState& operator=(const SystemState& other){
        if (this == &other){
            return *this;
        }

        bodies = other.bodies;
        deriv = other.deriv;
        N = other.N;
        
        return *this;
    }

    SystemState plus(double h,const std::vector<Derivatives>& deriv) const{
        SystemState copy = *this;
        for(int i = 0; i < N; i++){
            copy.bodies[i].x += h * deriv[i].dxdt;
            copy.bodies[i].y += h * deriv[i].dydt;
            copy.bodies[i].z += h * deriv[i].dzdt;
            copy.bodies[i].Vx += h * deriv[i].dVxdt;
            copy.bodies[i].Vy += h * deriv[i].dVydt;
            copy.bodies[i].Vz += h * deriv[i].dVzdt;
        }

        return copy;
    }
};

std::vector<Derivatives> compute_derivatives(SystemState& state){
    std::vector<Derivatives> deriv(state.N);

    for (int i = 0; i < state.N; i++) {
        deriv[i].dxdt = state.bodies[i].Vx;
        deriv[i].dydt = state.bodies[i].Vy;
        deriv[i].dzdt = state.bodies[i].Vz;
    }

    for (int i = 0; i < state.N; i++) {
        for (int j = i + 1; j < state.N; j++) {
            double dx = state.bodies[j].x - state.bodies[i].x;
            double dy = state.bodies[j].y - state.bodies[i].y;
            double dz = state.bodies[j].z - state.bodies[i].z;
            
            double r = sqrt(dx*dx + dy*dy + dz*dz);
            double r3 = r * r * r;
            
            double coeff_i = state.bodies[j].m / r3;
            double coeff_j = state.bodies[i].m / r3;
            
            deriv[i].dVxdt += dx * coeff_i;
            deriv[i].dVydt += dy * coeff_i;
            deriv[i].dVzdt += dz * coeff_i;

            deriv[j].dVxdt -= dx * coeff_j;
            deriv[j].dVydt -= dy * coeff_j;
            deriv[j].dVzdt -= dz * coeff_j;
        }
    }
    return deriv;
}

void Euler_method(std::vector<BodyState>& bodies, std::vector<Derivatives>& deriv, int N, double h){
    for(int i = 0; i < N; i++){
        bodies[i].x += h * bodies[i].Vx;
        bodies[i].y += h * bodies[i].Vy;
        bodies[i].z += h * bodies[i].Vz;
        bodies[i].Vx += h * deriv[i].dVxdt;
        bodies[i].Vy += h * deriv[i].dVydt;
        bodies[i].Vz += h * deriv[i].dVzdt;
    }
}

void RK4_step(SystemState& state, double h){
    std::vector<Derivatives> k1 = compute_derivatives(state);
    SystemState SysSt = state.plus(h/2, k1);
    std::vector<Derivatives> k2 = compute_derivatives(SysSt);
    SysSt = state.plus(h/2, k2);
    std::vector<Derivatives> k3 = compute_derivatives(SysSt);
    SysSt = state.plus(h, k3);
    std::vector<Derivatives> k4 = compute_derivatives(SysSt);

    std::vector<Derivatives> combined(SysSt.N);

    for(int i = 0; i < SysSt.N; i++){
        combined[i] = k1[i] + k2[i] * 2 + k3[i] * 2 + k4[i];
    }
    state = state.plus(h/6, combined);
    state.deriv = compute_derivatives(state);
}

int main() {


  return 0;
}
