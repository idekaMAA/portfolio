# BitBusters - TP Sistemas Operativos 1C2025

**Trabajo Práctico Cuatrimestral - Episodio III: Revenge of the Cth**  
Cátedra de Sistemas Operativos - UTN FRBA

## Grupo BitBusters

**Comisión:** K2013
**Integrantes:**

- Gaston Orqueda | gaston.orqueda@frba.utn.edu.ar
- Mariana Aylén Albornoz | malbornoz@frba.utn.edu.ar
- Francisco Javier Franchi | ffranchi@frba.utn.edu.ar
- Alexis Nahuel Petta | apeta@frba.utn.edu.ar
- Máximo Juan Manuel Hidalgo | mxhidalgo@frba.utn.edu.ar

## Descripción

Simulación de un sistema operativo distribuido compuesto por los siguientes módulos:

- **Kernel** (Planificación de procesos, gestión de IO)
- **CPU** (Ejecución de instrucciones, TLB, Caché)
- **Memoria + SWAP** (Paginación multinivel, espacio de usuario)
- **IO** (Dispositivos de entrada/salida simulados)

## Estructura del proyecto

- **bin/** → Binarios compilados  
- **config/** → Archivos de configuración  
- **src/** → Código fuente de cada módulo  
- **logs/** → Logs de ejecución  
- **docs/** → Documentación del proyecto  
- **scripts/** → Scripts de despliegue y testing  
- **tests/** → Casos de prueba  
- **swap/** → Archivos swap  
- **dumps/** → Memory dumps

## Requisitos

- Linux
- gcc
- so-commons-library (proporcionada por la cátedra)

## Checkpoint

Para cada checkpoint de control obligatorio, se debe crear un tag en el
repositorio con el siguiente formato:

```
checkpoint-{número}
```

Donde `{número}` es el número del checkpoint, ejemplo: `checkpoint-1`.

Para crear un tag y subirlo al repositorio, podemos utilizar los siguientes
comandos:

```bash
git tag -a checkpoint-{número} -m "Checkpoint {número}"
git push origin checkpoint-{número}
```

> [!WARNING]
> Asegúrense de que el código compila y cumple con los requisitos del checkpoint
> antes de subir el tag.

## Entrega

Para desplegar el proyecto en una máquina Ubuntu Server, podemos utilizar el
script [so-deploy] de la cátedra:

```bash
git clone https://github.com/sisoputnfrba/so-deploy.git
cd so-deploy
./deploy.sh -r=release -p=shared -p=kernel -p=cpu -p=memoria -p=io "tp-{año}-{cuatri}-{grupo}"
```

El mismo se encargará de instalar las Commons, clonar el repositorio del grupo
y compilar el proyecto en la máquina remota.

> [!NOTE]
> Ante cualquier duda, pueden consultar la documentación en el repositorio de
> [so-deploy], o utilizar el comando `./deploy.sh --help`.

## Guías útiles

- [Cómo interpretar errores de compilación](https://docs.utnso.com.ar/primeros-pasos/primer-proyecto-c#errores-de-compilacion)
- [Cómo utilizar el debugger](https://docs.utnso.com.ar/guias/herramientas/debugger)
- [Cómo configuramos Visual Studio Code](https://docs.utnso.com.ar/guias/herramientas/code)
- **[Guía de despliegue de TP](https://docs.utnso.com.ar/guías/herramientas/deploy)**

[so-commons-library]: https://github.com/sisoputnfrba/so-commons-library
[so-deploy]: https://github.com/sisoputnfrba/so-deploy
