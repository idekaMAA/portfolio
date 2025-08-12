#include <iostream>
#include <fstream>

using namespace std;

//estructuras.
struct Formula{
    int numList;
    char nomPresi [30];
    char nomVPresi [30];
};

struct listas{
    int indice;
    int votosPositivos;
};

struct Padrón{
    int votosBlanco;
    int votosImpugnados;
    int listas[5];
};

struct Provincia {
    int Partido [10];
    int Localidad [20];
    Padrón padronTotal;
};

struct archVotos{
    int provincia;
    int partido;
    int localidad;
    int blancos;
    int impugnados;
    int listas[5];//aca están los votos positivos de cada lista o fomrula presidencial
};

struct Votos{
    int padron;
    int blancos;
    int impugnados;
    int listas[5];
};

Votos matrizVotos [24][10][20];

//delcaración de funciones:
void cargarVotos(ifstream &archivo);
void cargarFormulas(ifstream &archivo, Formula formulas[5]);
void presidente (Formula formulas[5]);
void gobernador();
void intendente();
void ausentismo();
void votosBlancos();
void votosImpugnados();

//main

int main(){
    Formula formulas[5];

    //abirir archivos
    ifstream archiVotos("archVotos_45.bin", ios::binary); //para probar los distintos archivos, debe reemplazar
    ifstream archiFormulas("Listas.bin", ios::binary);      // "none" por "45" o "4010"

    if(!archiVotos){
        cerr << "Error al abrir el archivo de votos."<< endl;
        return 1;
    }

    if(!archiFormulas){
        cerr << "Error al abrir el archivo de formulas."<< endl;
        return 1;
    }

    //cargo datos
    cargarVotos(archiVotos);
    cargarFormulas(archiFormulas,formulas);

    //cierro arch
    archiVotos.close();
    archiFormulas.close();

    //det formula ganadora
    cout <<endl;
    presidente(formulas);
    cout <<endl;
    cout <<endl;
    gobernador();
    cout <<endl;
    cout <<endl;
    intendente();
    cout <<endl;
    cout <<endl;
    ausentismo();
    cout <<endl;
    votosBlancos();
    cout <<endl;
    votosImpugnados();
    cout <<endl;

    return 0;
}

//funciones

void cargarVotos(ifstream &archivo){
    archVotos registro;
    while (archivo.read(reinterpret_cast<char*>(&registro),sizeof(archVotos))){
        int p=registro.provincia;
        int pa= registro.partido;
        int l= registro.localidad;

        if (p<24 && pa< 10 && l<20){
            matrizVotos[p][pa][l].padron=0;
            matrizVotos[p][pa][l].blancos = registro.blancos;
            matrizVotos[p][pa][l].impugnados = registro.impugnados;

            for (int i=0; i<5; i++){
                matrizVotos[p][pa][l].listas[i] = registro.listas[i];
                matrizVotos[p][pa][l].padron += registro.listas[i]; //suma vitos al padrón total
            }
            matrizVotos[p][pa][l].padron += registro.blancos + registro.impugnados;
        }
    }
}

void cargarFormulas(ifstream &archivo, Formula formulas[5]){
    for(int i = 0; i <5; i++){
        archivo.read(reinterpret_cast<char*>(&formulas[i]), sizeof(Formula));
        if(archivo.eof()){
            cerr<<"Error: el arvhico no contiene suficientes datos"<< endl;
            return;
        }
    }
}

void presidente (Formula formulas[5]){ 
     // 45%> de votos Positivos Emitidos, o +40% mas 10% dif.
    int votPosE[5]={0};
    int votosTotales=0;
    for (int i=0; i<24; i++){
        for (int j=0; j<10; j++){
            for (int k=0; k<20; k++){
                for (int t=0; t<5; t++){
                    votPosE[t] += matrizVotos[i][j][k].listas[t];
                    votosTotales += matrizVotos[i][j][k].listas[t];
                }
            }
        }
    }

    //porcentajes
    int ganador = -1, seg = -1;
    double maxPorcentaje = 0.0, segPor = 0.0;
    double porcentajes[5]= {0};

    for (int z=0; z<5; z++){
        porcentajes[z]= (votPosE[z]*100.0)/votosTotales;
        if(porcentajes[z]>maxPorcentaje){
            seg=ganador;
            segPor=maxPorcentaje;
            ganador=z;
            maxPorcentaje=porcentajes[z];
        }else if(porcentajes[z]>maxPorcentaje){
            seg = z;
            segPor= porcentajes[z];
        }
    }

    // Verificación de victoria en primera vuelta
    if (maxPorcentaje >= 45 || (maxPorcentaje >= 40 && (maxPorcentaje - segPor) >= 10)) {
        cout << "Presidente electo: " << formulas[ganador].nomPresi
             << " con " << maxPorcentaje << "% de los votos." << endl;
    } else {
        cout << "Es necesaria una segunda vuelta." << endl;
    }
}

void gobernador(){
    for (int p=0; p<24;p++){
        int votList[5]={0};

        for (int j=0; j<10; j++){
            for (int k=0; k<20; k++){
                for (int t=0; t<5; t++){
                    votList[t]+= matrizVotos[p][j][k].listas[t];
                }
            }
        }
        int ganador=0;
        for(int z=1; z<5; z++){
            if(votList[z] > votList[ganador]){
                ganador = z;
            }
        }
        cout << "Gobernador electo en provincia "<< p+1<< " es de la lista: "<<ganador+1<<" con "<< votList[ganador]<<" votos"<<endl;
    }
}

void intendente(){
    for (int p=0; p<24; p++){
        for (int l=0; l<10; l++){
            int votListal[5]={0};

            for (int j=0; j<20; j++){
                for (int k=0; k<5; k++){
                    votListal[k] += matrizVotos[p][l][j].listas[k];
                }
            }

            int ganador = 0;
            for(int t = 1; t<5; t++){
                if (votListal[t] > votListal[ganador]){
                    ganador = t;
                }
            }
            cout << "Intendente electo en la localidad "<<l+1<<" de la provincia "<<p+1<<" es de la lista "<< ganador+1<< " con "<<votListal[ganador] << " votos."<< endl;
        }
        cout <<endl;
    }
}

void ausentismo() {
    int totPadron = 0, totVotantes = 0;
    for (int p = 0; p < 24; p++) {
        int totalAusentesProv = 0, totalPadronProv = 0;
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 20; k++) {
                totPadron += matrizVotos[p][j][k].padron;
                totVotantes += matrizVotos[p][j][k].padron - (matrizVotos[p][j][k].blancos + matrizVotos[p][j][k].impugnados);
                totalPadronProv += matrizVotos[p][j][k].padron;
                totalAusentesProv += matrizVotos[p][j][k].blancos + matrizVotos[p][j][k].impugnados;
            }
        }
        double porcentajeProv = (totalAusentesProv * 100.0) / totalPadronProv;
        cout << "Provincia " << p + 1 << " - Porcentaje de ausentismo: " << porcentajeProv << "%" << endl;
    }
    double porcentajeAusentismo = 100.0 - ((totVotantes * 100.0) / totPadron);
    cout << "Porcentaje de ausentismo a nivel nacional: " << porcentajeAusentismo << "%" << endl;
}


void votosBlancos() {
    int totalPadronNac = 0, totalBlancosNac = 0;
    for (int p = 0; p < 24; p++) {
        int totalBlancosProv = 0;
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 20; k++) {
                totalPadronNac += matrizVotos[p][j][k].padron;
                totalBlancosNac += matrizVotos[p][j][k].blancos;
                totalBlancosProv += matrizVotos[p][j][k].blancos;
            }
        }
        cout << "Provincia " << p + 1 << " - Votos en blanco: " << totalBlancosProv << endl;
    }
    double porcentajeNacional = (totalBlancosNac * 100.0) / totalPadronNac;
    cout << "A nivel nacional - Votos en blanco: " << porcentajeNacional << "%" << endl;
}

void votosImpugnados() {
    int totalPadronNac = 0, totalImpugnadosNac = 0;
    for (int p = 0; p < 24; p++) {
        int totalImpugnadosProv = 0;
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 20; k++) {
                totalPadronNac += matrizVotos[p][j][k].padron;
                totalImpugnadosNac += matrizVotos[p][j][k].impugnados;
                totalImpugnadosProv += matrizVotos[p][j][k].impugnados;
            }
        }
        cout << "Provincia " << p + 1 << " - Votos impugnados: " << totalImpugnadosProv << endl;
    }
    double porcentajeNacional = (totalImpugnadosNac * 100.0) / totalPadronNac;
    cout << "A nivel nacional - Votos impugnados: " << porcentajeNacional << "%" << endl;
}


