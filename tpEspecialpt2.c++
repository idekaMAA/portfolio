#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>

using namespace std;

// Estructuras
struct Equipo {
    char nombre[50];
};

struct Partido {
    char codigoDeTorneo[5];
    char fecha[8];
    int idFecha;
    int numeroDePartido;
    int idEquipoLocal;
    int golesLocal;
    int puntosLocal;
    int idEquipoVisitante;
    int golesVisitante;
    int puntosVisitante;
};

struct Posicion {
    int idEquipo;
    double promedioDescenso;
};

const int MAX_EQUIPOS = 20;
const int MAX_PARTIDOS = 1000;

// Funciones
void insertarOrdenado(Posicion tabla[], int &n, Posicion nuevo);
void mostrarTabla(Posicion tabla[], Equipo equipos[], int n);

//main
int main() {
    Equipo equipos[MAX_EQUIPOS];
    Partido partidos[MAX_PARTIDOS];
    Posicion tabla[MAX_EQUIPOS];

    int partidosJugados[MAX_EQUIPOS] = {0};
    int puntosAcumulados[MAX_EQUIPOS] = {0};
    int n = 0; // Número de equipos en la tabla

    // Leer nombres de los equipos desde el archivo binario "equipos.bin"
    FILE *arEquipos = fopen("equipos.bin", "rb");
    if (!arEquipos) {
        cerr << "Error al abrir equipos.bin" << endl;
        return 1;
    }
    fread(equipos, sizeof(Equipo), MAX_EQUIPOS, arEquipos);
    fclose(arEquipos);

    // Leer los partidos desde el archivo binario "goles2.bin"
    FILE *arPartidos = fopen("goles2.bin", "rb");
    if (!arPartidos) {
        cerr << "Error al abrir goles2.bin" << endl;
        return 1;
    }

    // Obtener el tamaño del archivo
    fseek(arPartidos, 0, SEEK_END);
    long fileSize = ftell(arPartidos);
    rewind(arPartidos);

    // Calcular la cantidad de partidos en el archivo
    int numPartidos = fileSize / sizeof(Partido);
    if (numPartidos > MAX_PARTIDOS) numPartidos = MAX_PARTIDOS; // Evitar desbordamientos

    fread(partidos, sizeof(Partido), numPartidos, arPartidos);
    fclose(arPartidos);

    // Procesar los partidos leídos
    for (int i = 0; i < numPartidos; i++) {
        int local = partidos[i].idEquipoLocal - 1;
        int visitante = partidos[i].idEquipoVisitante - 1;

        // Acumular puntos y partidos jugados para cada equipo
        puntosAcumulados[local] += partidos[i].puntosLocal;
        partidosJugados[local]++;

        puntosAcumulados[visitante] += partidos[i].puntosVisitante;
        partidosJugados[visitante]++;
    }

    // Mostrar los partidos leídos (para probar el arcrhivo)
/*    for (int i = 0; i < numPartidos; i++) {
        cout << "Partido " << i + 1 << ": "
             << "Torneo=" << partidos[i].codigoDeTorneo
             << ", Fecha=" << partidos[i].fecha
             << ", Local=" << partidos[i].idEquipoLocal
             << ", Visitante=" << partidos[i].idEquipoVisitante
             << ", Puntos Local=" << partidos[i].puntosLocal
             << ", Puntos Visitante=" << partidos[i].puntosVisitante << endl;
    }*/

    // Asegurar que todos los equipos aparezcan en la tabla, aunque no tengan partidos
    for (int i = 0; i < MAX_EQUIPOS; i++) {
        Posicion nuevo;
        nuevo.idEquipo = i + 1;
        nuevo.promedioDescenso = (partidosJugados[i] > 0) ? (double)puntosAcumulados[i] / partidosJugados[i] : 0.0;
        insertarOrdenado(tabla, n, nuevo);
    }

    // Mostrar la tabla de promedios de descenso ordenada
    mostrarTabla(tabla, equipos, n);

    return 0;
}


void insertarOrdenado(Posicion tabla[], int &n, Posicion nuevo) {
    int i = n - 1;
    while (i >= 0 && tabla[i].promedioDescenso < nuevo.promedioDescenso) {
        tabla[i + 1] = tabla[i];
        i--;
    }
    tabla[i + 1] = nuevo;
    n++;
}

// Función para mostrar la tabla de promedios
void mostrarTabla(Posicion tabla[], Equipo equipos[], int n) {
    cout << left << setw(30) << "Equipo" << right << setw(10) << "Promedio" << endl;
    cout << string(40, '-') << endl;

    for (int i = 0; i < n; ++i) {
        cout << left << setw(30) << equipos[tabla[i].idEquipo - 1].nombre
             << right << setw(10) << fixed << setprecision(2) << tabla[i].promedioDescenso << endl;
    }
}
