#include <iostream>
#include <string>
#include <iomanip>
#include <new>

using namespace std;

struct Almacen {
    int    *id      = nullptr;
    double *peso    = nullptr;
    string *destino = nullptr;
    int    *prior   = nullptr;
    char   *estado  = nullptr;

    int capacidad = 0;
    int head = 0;
    int tail = 0;
};

bool inicializar(Almacen &a, int cap) {
    try {
        a.id      = new int[cap];
        a.peso    = new double[cap];
        a.destino = new string[cap];
        a.prior   = new int[cap];
        a.estado  = new char[cap];
        a.capacidad = cap;
        a.head = 0;
        a.tail = 0;
        return true;
    } catch (const bad_alloc&) {
        return false;
    }
}

bool duplicarCapacidad(Almacen &a) {
    int nuevaCap = (a.capacidad == 0 ? 1 : a.capacidad * 2);
    try {
        int*    newId      = new int[nuevaCap];
        double* newPeso    = new double[nuevaCap];
        string* newDestino = new string[nuevaCap];
        int*    newPrior   = new int[nuevaCap];
        char*   newEstado  = new char[nuevaCap];

        for (int i = 0; i < a.tail; ++i) {
            newId[i]      = a.id[i];
            newPeso[i]    = a.peso[i];
            newDestino[i] = a.destino[i];
            newPrior[i]   = a.prior[i];
            newEstado[i]  = a.estado[i];
        }

        delete[] a.id;      a.id      = newId;
        delete[] a.peso;    a.peso    = newPeso;
        delete[] a.destino; a.destino = newDestino;
        delete[] a.prior;   a.prior   = newPrior;
        delete[] a.estado;  a.estado  = newEstado;

        a.capacidad = nuevaCap;
        return true;
    } catch (const bad_alloc&) {
        return false;
    }
}

bool encolar(Almacen &a, int id, double peso, const string &destino, int prioridad) {
    if (a.tail >= a.capacidad) {
        if (!duplicarCapacidad(a)) return false;
    }
    a.id[a.tail]      = id;
    a.peso[a.tail]    = peso;
    a.destino[a.tail] = destino;
    a.prior[a.tail]   = prioridad;
    a.estado[a.tail]  = 'E';
    a.tail++;
    return true;
}

bool desencolar(Almacen &a) {
    while (a.head < a.tail && a.estado[a.head] != 'E') {
        a.head++;
    }
    if (a.head >= a.tail) {
        return false;
    }
    a.estado[a.head] = 'D';
    a.head++;
    return true;
}

bool inspeccionarFrente(const Almacen &a, int &idxOut) {
    int i = a.head;
    while (i < a.tail && a.estado[i] != 'E') i++;
    if (i >= a.tail) return false;
    idxOut = i;
    return true;
}

void reportePorDestino(const Almacen &a, const string &dest) {
    int conteo = 0;
    double sumaPeso = 0.0;
    for (int i = a.head; i < a.tail; ++i) {
        if (a.estado[i] == 'E' && a.destino[i] == dest) {
            conteo++;
            sumaPeso += a.peso[i];
        }
    }
    cout << "Reporte para destino '" << dest << "':\n";
    cout << "  Paquetes En Cola: " << conteo << "\n";
    if (conteo > 0) {
        cout << fixed << setprecision(2);
        cout << "  Peso Promedio: " << (sumaPeso / conteo) << " kg\n";
    } else {
        cout << "  Peso Promedio: N/A\n";
    }
}

void liberar(Almacen &a) {
    delete[] a.id;      a.id = nullptr;
    delete[] a.peso;    a.peso = nullptr;
    delete[] a.destino; a.destino = nullptr;
    delete[] a.prior;   a.prior = nullptr;
    delete[] a.estado;  a.estado = nullptr;
    a.capacidad = 0;
    a.head = a.tail = 0;
}

const char* prioridadTexto(int p) {
    switch (p) {
        case 1: return "Alta";
        case 2: return "Media";
        case 3: return "Baja";
        default: return "Desconocida";
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "--- Sistema de Despacho Logistico MegaEnvio (Modo Punteros) ---\n";
    cout << "Inicializando sistema con capacidad para 50 paquetes...\n";

    Almacen a;
    if (!inicializar(a, 50)) {
        cerr << "Error: no se pudo asignar memoria inicial.\n";
        return 1;
    }

    cout << "Capacidad actual: " << a.capacidad << ". Paquetes en cola: 0.\n\n";

    int opcion = 0;
    cout << fixed << setprecision(2);

    do {
        cout << "Seleccione una operacion:\n";
        cout << "1. Agregar Paquete (Encolar)\n";
        cout << "2. Despachar Paquete (Desencolar)\n";
        cout << "3. Inspeccionar Frente de Cola\n";
        cout << "4. Reporte por Destino\n";
        cout << "5. Salir (Liberar Memoria)\n";
        cout << "Opcion seleccionada: ";
        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Entrada invalida.\n\n";
            continue;
        }
        cin.ignore(10000, '\n');

        if (opcion == 1) {
            int id; double peso; string destino; int prioridad;
            cout << "Ingrese ID: ";            cin >> id;       cin.ignore(10000, '\n');
            cout << "Ingrese Peso (kg): ";     cin >> peso;     cin.ignore(10000, '\n');
            cout << "Ingrese Destino: ";       getline(cin, destino);
            cout << "Prioridad (1=Alta,2=Media,3=Baja): "; cin >> prioridad; cin.ignore(10000, '\n');

            if (prioridad < 1 || prioridad > 3) {
                cout << "Prioridad invalida. Debe ser 1, 2 o 3.\n\n";
                continue;
            }

            if (!encolar(a, id, peso, destino, prioridad)) {
                cout << "Error: sin memoria al encolar (no se pudo duplicar capacidad).\n\n";
                continue;
            }

            int enCola = 0;
            for (int i = a.head; i < a.tail; ++i) if (a.estado[i] == 'E') enCola++;

            cout << "Paquete " << id << " encolado. "
                 << "Capacidad utilizada: " << a.tail << "/" << a.capacidad
                 << " (En cola: " << enCola << ").\n\n";

        } else if (opcion == 2) {
            int idx = -1;
            if (!inspeccionarFrente(a, idx)) {
                cout << "No hay paquetes en cola para despachar.\n\n";
            } else {
                cout << "Despachando paquete...\n";
                int idFront = a.id[idx];
                if (desencolar(a)) {
                    cout << "Paquete " << idFront << " despachado con exito. Estado: 'D'.\n\n";
                } else {
                    cout << "Error inesperado al despachar.\n\n";
                }
            }

        } else if (opcion == 3) {
            int idx;
            if (inspeccionarFrente(a, idx)) {
                cout << "Frente de la cola:\n";
                cout << "  ID: " << a.id[idx]
                     << " | Peso: " << a.peso[idx] << " kg"
                     << " | Destino: " << a.destino[idx]
                     << " | Prioridad: " << prioridadTexto(a.prior[idx]) << "\n\n";
            } else {
                cout << "La cola esta vacia (no hay 'E').\n\n";
            }

        } else if (opcion == 4) {
            string dest;
            cout << "Ingrese el destino para el reporte: ";
            getline(cin, dest);
            if (dest.empty()) {
                cout << "Destino vacio.\n\n";
            } else {
                reportePorDestino(a, dest);
                cout << "\n";
            }

        } else if (opcion == 5) {
            cout << "Liberando " << a.capacidad << " bloques de memoria asignada...\n";
            liberar(a);
            cout << "Sistema cerrado. ¡Memoria libre!\n";

        } else {
            cout << "Opcion invalida.\n\n";
        }

    } while (opcion != 5);

    return 0;
}
