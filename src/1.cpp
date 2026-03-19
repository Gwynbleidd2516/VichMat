#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <functional>

struct BodyState {
    double x, y, z, Vx, Vy, Vz, m;
    BodyState() : x(0), y(0), z(0), Vx(0), Vy(0), Vz(0), m(1){}
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

    Derivatives& operator+=(const Derivatives& other){
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

    SystemState plus(double h, const std::vector<Derivatives>& d) const {
        SystemState copy = *this;
        for (int i = 0; i < N; i++) {
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

std::vector<Derivatives> compute_derivatives(const SystemState& state){
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

void Euler_step(SystemState& state, double h){
    for(int i = 0; i < state.N; i++){
        state.bodies[i].x += h * state.bodies[i].Vx;
        state.bodies[i].y += h * state.bodies[i].Vy;
        state.bodies[i].z += h * state.bodies[i].Vz;
        state.bodies[i].Vx += h * state.deriv[i].dVxdt;
        state.bodies[i].Vy += h * state.deriv[i].dVydt;
        state.bodies[i].Vz += h * state.deriv[i].dVzdt;
    }

    state.deriv = compute_derivatives(state);
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

void Leapfrog_step(SystemState& state, double h){
    for(int i = 0; i < state.N; i++){
        state.bodies[i].Vx += 0.5 * h * state.deriv[i].dVxdt;
        state.bodies[i].Vy += 0.5 * h * state.deriv[i].dVydt;
        state.bodies[i].Vz += 0.5 * h * state.deriv[i].dVzdt;

        state.bodies[i].x += h * state.bodies[i].Vx;
        state.bodies[i].y += h * state.bodies[i].Vy;
        state.bodies[i].z += h * state.bodies[i].Vz;
    }

    state.deriv = compute_derivatives(state);

    for(int i = 0; i < state.N; i++){
        state.bodies[i].Vx += 0.5 * h * state.deriv[i].dVxdt;
        state.bodies[i].Vy += 0.5 * h * state.deriv[i].dVydt;
        state.bodies[i].Vz += 0.5 * h * state.deriv[i].dVzdt;
    }   

}

void init_figure_eight(SystemState& state) {

    state.bodies[0].x =  0.97000436; state.bodies[0].y = -0.24308753; state.bodies[0].z = 0;
    state.bodies[1].x = -0.97000436; state.bodies[1].y =  0.24308753; state.bodies[1].z = 0;
    state.bodies[2].x =  0.0;        state.bodies[2].y =  0.0;        state.bodies[2].z = 0;

    double vx = 0.93240737 / 2.0;
    double vy = 0.86473146 / 2.0;
    state.bodies[0].Vx =  vx; state.bodies[0].Vy =  vy;
    state.bodies[1].Vx =  vx; state.bodies[1].Vy =  vy;
    state.bodies[2].Vx = -2*vx; state.bodies[2].Vy = -2*vy;

    for (int i = 0; i < 3; i++) state.bodies[i].m = 1.0;
    state.deriv = compute_derivatives(state);
}

void init_polygon(SystemState& state, double R = 1.0) {
    int N = state.N;

    // Вычислим нужную угловую скорость:
    // omega^2 * R = sum_{j != 0} m / r_{0j}^2 * cos(angle)  — проекция на радиус
    // Для равных масс и правильного N-угольника:
    //   omega^2 = (1/R^3) * sum_{k=1}^{N-1}  sin(pi*k/N) / (4*sin^2(pi*k/N)) * ...
    // Проще использовать готовую формулу:
    //   omega^2 = (1/(4*R^3)) * sum_{k=1}^{N-1}  1 / sin(pi*k/N)
    // (при G=1, m=1)
    double sum = 0.0;
    for (int k = 1; k < N; k++)
        sum += 1.0 / std::sin(M_PI * k / N);
    double omega = std::sqrt(sum / (4.0 * R * R * R));

    for (int i = 0; i < N; i++) {
        double angle = 2.0 * M_PI * i / N;
        state.bodies[i].x  =  R * std::cos(angle);
        state.bodies[i].y  =  R * std::sin(angle);
        state.bodies[i].z  =  0.0;
        state.bodies[i].Vx = -omega * R * std::sin(angle);
        state.bodies[i].Vy =  omega * R * std::cos(angle);
        state.bodies[i].Vz =  0.0;
        state.bodies[i].m  =  1.0;
    }
    state.deriv = compute_derivatives(state);
}

double compute_energy(const SystemState& state) {
    double K = 0, U = 0;
    for (int i = 0; i < state.N; i++) {
        double v2 = state.bodies[i].Vx * state.bodies[i].Vx
                  + state.bodies[i].Vy * state.bodies[i].Vy
                  + state.bodies[i].Vz * state.bodies[i].Vz;
        K += 0.5 * state.bodies[i].m * v2;
    }
    for (int i = 0; i < state.N; i++)
        for (int j = i+1; j < state.N; j++) {
            double dx = state.bodies[j].x - state.bodies[i].x;
            double dy = state.bodies[j].y - state.bodies[i].y;
            double dz = state.bodies[j].z - state.bodies[i].z;
            double r = sqrt(dx*dx + dy*dy + dz*dz);
            U -= state.bodies[i].m * state.bodies[j].m / r;
        }
    return K + U;
}

double compute_angular_momentum_z(const SystemState& state) {
    double Lz = 0.0;
    for (int i = 0; i < state.N; i++)
        Lz += state.bodies[i].m * (state.bodies[i].x * state.bodies[i].Vy
                                 - state.bodies[i].y * state.bodies[i].Vx);
    return Lz;
}


void write_csv_header(std::ofstream& out, int N) {
    out << "t,E,Lz";
    for (int i = 0; i < N; i++)
        out << ",x" << i << ",y" << i << ",z" << i
            << ",Vx" << i << ",Vy" << i << ",Vz" << i;
    out << "\n";
}

void write_csv_row(std::ofstream& out, double t, const SystemState& state) {
    out << std::setprecision(15)
        << t << ","
        << compute_energy(state) << ","
        << compute_angular_momentum_z(state);
    for (int i = 0; i < state.N; i++)
        out << "," << state.bodies[i].x
            << "," << state.bodies[i].y
            << "," << state.bodies[i].z
            << "," << state.bodies[i].Vx
            << "," << state.bodies[i].Vy
            << "," << state.bodies[i].Vz;
    out << "\n";
}

void save_initial_state(const std::string& filename, const SystemState& state) {
    std::ofstream f(filename, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot open: " + filename);
    f.write(reinterpret_cast<const char*>(&state.N), sizeof(int));
    for (int i = 0; i < state.N; i++)
        f.write(reinterpret_cast<const char*>(&state.bodies[i]), sizeof(BodyState));
}

// Загрузить начальное состояние
SystemState load_initial_state(const std::string& filename) {
    std::ifstream f(filename, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot open: " + filename);
    int N;
    f.read(reinterpret_cast<char*>(&N), sizeof(int));
    SystemState state(N);
    for (int i = 0; i < N; i++)
        f.read(reinterpret_cast<char*>(&state.bodies[i]), sizeof(BodyState));
    state.deriv = compute_derivatives(state);
    return state;
}

enum class Method { EULER, LEAPFROG, RK4 };

struct ExperimentParams {
    Method      method      = Method::RK4;
    double      h           = 0.001;      // шаг интегрирования
    double      T_end       = 6.3259 * 5; // время симуляции
    int         output_every = 10;        // записывать каждые N шагов
    std::string output_file = "trajectory.csv";
    bool        realtime    = false;      // режим: файл или реальное время
};

using RealtimeCallback = std::function<bool(double t, const SystemState& state)>;

SystemState run_experiment(
    const SystemState&    initial,
    const ExperimentParams& params,
    RealtimeCallback      callback = nullptr)
{
    SystemState state = initial;

    std::ofstream out;
    if (!params.realtime) {
        out.open(params.output_file);
        if (!out) throw std::runtime_error("Cannot open: " + params.output_file);
        write_csv_header(out, state.N);
    }

    int steps = static_cast<int>(params.T_end / params.h);

    // Лямбда: шаг нужного метода
    auto do_step = [&](SystemState& s) {
        switch (params.method) {
            case Method::EULER:    Euler_step(s, params.h);    break;
            case Method::LEAPFROG: Leapfrog_step(s, params.h); break;
            case Method::RK4:      RK4_step(s, params.h);      break;
        }
    };

    for (int step = 0; step <= steps; step++) {
        double t = step * params.h;

        if (step % params.output_every == 0) {
            if (params.realtime) {
                if (callback && !callback(t, state))
                    break; // визуализатор сигнализирует об остановке
            } else {
                write_csv_row(out, t, state);
            }
        }

        if (step < steps)
            do_step(state);
    }

    return state;
}

void sensitivity_experiment(
    const SystemState&  initial,
    double              delta,        // величина возмущения
    double              h,
    double              T_end,
    Method              method,
    const std::string&  output_file)
{
    SystemState ref  = initial;
    SystemState pert = initial;
    pert.bodies[0].x += delta;
    pert.deriv = compute_derivatives(pert); // пересчитать после изменения

    std::ofstream out(output_file);
    if (!out) throw std::runtime_error("Cannot open: " + output_file);
    out << "t,deviation,E_ref\n";

    int steps = static_cast<int>(T_end / h);

    auto do_step = [&](SystemState& s) {
        switch (method) {
            case Method::EULER:    Euler_step(s, h);    break;
            case Method::LEAPFROG: Leapfrog_step(s, h); break;
            case Method::RK4:      RK4_step(s, h);      break;
        }
    };

    for (int step = 0; step <= steps; step++) {
        double t = step * h;

        if (step % 100 == 0) {
            double dev = 0.0;
            for (int i = 0; i < ref.N; i++) {
                double dx = ref.bodies[i].x - pert.bodies[i].x;
                double dy = ref.bodies[i].y - pert.bodies[i].y;
                double dz = ref.bodies[i].z - pert.bodies[i].z;
                dev += std::sqrt(dx*dx + dy*dy + dz*dz);
            }
            out << std::setprecision(15)
                << t << "," << dev << "," << compute_energy(ref) << "\n";
        }

        if (step < steps) {
            do_step(ref);
            do_step(pert);
        }
    }
}


int main() {
    const double PERIOD = 6.3259;

    // --- 1. Сравнение трёх методов на восьмёрке ---
    for (auto [method_name, method] : std::initializer_list<std::pair<std::string, Method>>{
        {"euler",    Method::EULER},
        {"leapfrog", Method::LEAPFROG},
        {"rk4",      Method::RK4}
    }) {
        SystemState state(3);
        init_figure_eight(state);

        ExperimentParams p;
        p.method       = method;
        p.h            = 0.001;
        p.T_end        = PERIOD * 10;
        p.output_every = 10;
        p.output_file  = "eight_" + method_name + ".csv";
        p.realtime     = false;

        // Сохраним начальное состояние для воспроизводимости
        save_initial_state("eight_initial.bin", state);

        run_experiment(state, p);
        std::cout << "Done: " << p.output_file << "\n";
    }

    // --- 2. Восьмёрка из сохранённого состояния (проверка save/load) ---
    {
        SystemState loaded = load_initial_state("eight_initial.bin");
        ExperimentParams p;
        p.method       = Method::RK4;
        p.h            = 0.001;
        p.T_end        = PERIOD * 2;
        p.output_every = 10;
        p.output_file  = "eight_from_file.csv";
        run_experiment(loaded, p);
        std::cout << "Done: " << p.output_file << "\n";
    }

    // --- 3. Правильный N-угольник (5 тел) ---
    {
        SystemState state(5);
        init_polygon(state, 1.0);

        ExperimentParams p;
        p.method       = Method::RK4;
        p.h            = 0.001;
        p.T_end        = 30.0;
        p.output_every = 10;
        p.output_file  = "polygon5.csv";
        run_experiment(state, p);
        std::cout << "Done: " << p.output_file << "\n";
    }

    // --- 4. Чувствительность к начальным условиям ---
    for (double delta : {1e-3, 1e-6, 1e-9, 1e-12}) {
        SystemState state(3);
        init_figure_eight(state);

        std::string fname = "sensitivity_1e" +
            std::to_string(static_cast<int>(std::log10(delta))) + ".csv";

        sensitivity_experiment(state, delta, 0.001, PERIOD * 20,
                               Method::RK4, fname);
        std::cout << "Done sensitivity delta=" << delta << " -> " << fname << "\n";
    }

    // --- 5. Режим реального времени (пример интеграции с визуализатором) ---
    // Визуализатор вызывает run_experiment с params.realtime = true
    // и передаёт свой callback. Пример заглушки:
    {
        SystemState state(3);
        init_figure_eight(state);

        ExperimentParams p;
        p.method       = Method::RK4;
        p.h            = 0.001;
        p.T_end        = PERIOD * 5;
        p.output_every = 10;
        p.realtime     = true;

        int frame = 0;
        run_experiment(state, p, [&](double t, const SystemState& s) -> bool {
            // Здесь визуализатор получит t и s и нарисует кадр.
            // Для примера просто считаем кадры.
            (void)t; (void)s;
            frame++;
            return true; // вернуть false — остановить симуляцию
        });
        std::cout << "Realtime simulation: " << frame << " frames rendered\n";
    }

    return 0;
}