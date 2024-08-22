# TP Sistemas Operativos

La carrera de Ingeniería en Sistemas de Información en la UTN tiene una materia llamada Sistemas Operativos, donde se explican los conceptos generales de los sistemas operativos, como por ejemplo, cómo los sistemas operativos administran las aplicaciones, la memoria, los archivos y las carpetas.

> Cursé Sistemas Operativos el 1er cuatrimestre de 2024, así que hay cosas que pudieron haber cambiado a lo largo del tiempo.

En esta materia, existe un TP (Trabajo Práctico) cuyo objetivo es afianzar todos los conocimientos relacionados. Este TP consiste, resumidamente, en simular el comportamiento de un Sistema Operativo real, implementando en el lenguaje C ciertas funcionalidades dadas por un enunciado. El enunciado de este TP es el siguiente:
> [Enunciado del TP UTNSO 1C2024](https://docs.google.com/document/d/1-AqFTroovEMcA1BfC2rriB5jsLE6SUa4mbcAox1rPec "Enunciado del UTNTP 1C2024")

Este repositorio contiene mi implementación/resolución del TP.

Cualquier TP de esta materia, para aprobar, debe ser capaz de pasar unas pruebas que determinan si el sistema satisface o no los requerimientos. El sistema de este repositorio es capaz de pasar todas las pruebas satisfactoriamente. Las pruebas de este TP son:
> [Pruebas del TP UTNSO 1C2024](https://docs.google.com/document/d/1XsBsJynoN5A9PTsTEaZsj0q3zsEtcnLgdAHOQ4f_4-g "Pruebas del TP UTNSO 1C2024")

Mi grupo y yo, quienes diseñamos e implementamos el sistema, somos:
- [Julian Fajardo (yo)](https://github.com/julienf04 "Julian Fajardo (yo)")
- [Sebastian Alejandro Romero Rocha](https://github.com/SebaR1 "Sebastian Alejandro Romero Rocha")
- [Francisco Lezcano](https://github.com/FranX90 "Francisco Lezcano")
- [Vanesa Noelia Romero](https://github.com/vanenromero "Vanesa Noelia Romero")


------------


------------


------------

> El texto anterior es un resumen que quise presentar. El siguiente texto es el README.md original que nos dejó la cátedra.
















# tp-scaffold

Esta es una plantilla de proyecto diseñada para generar un TP de Sistemas
Operativos de la UTN FRBA.

## Dependencias

Para poder compilar y ejecutar el proyecto, es necesario tener instalada la
biblioteca [so-commons-library] de la cátedra:

```bash
git clone https://github.com/sisoputnfrba/so-commons-library
cd so-commons-library
make debug
make install
```

## Compilación

Cada módulo del proyecto se compila de forma independiente a través de un
archivo `makefile`. Para compilar un módulo, es necesario ejecutar el comando
`make` desde la carpeta correspondiente.

El ejecutable resultante se guardará en la carpeta `bin` del módulo.

## Importar desde Visual Studio Code

Para importar el workspace, debemos abrir el archivo `tp.code-workspace` desde
la interfaz o ejecutando el siguiente comando desde la carpeta raíz del
repositorio:

```bash
code tp.code-workspace
```

## Checkpoint

Para cada checkpoint de control obligatorio, se debe crear un tag en el
repositorio con el siguiente formato:

```
checkpoint-{número}
```

Donde `{número}` es el número del checkpoint.

Para crear un tag y subirlo al repositorio, podemos utilizar los siguientes
comandos:

```bash
git tag -a checkpoint-{número} -m "Checkpoint {número}"
git push origin checkpoint-{número}
```

Asegúrense de que el código compila y cumple con los requisitos del checkpoint
antes de subir el tag.

## Entrega

Para desplegar el proyecto en una máquina Ubuntu Server, podemos utilizar el
script [so-deploy] de la cátedra:

```bash
git clone https://github.com/sisoputnfrba/so-deploy.git
cd so-deploy
./deploy.sh -r=release -p=utils -p=kernel -p=cpu -p=memoria -p=entradasalida "tp-{año}-{cuatri}-{grupo}"
```

El mismo se encargará de instalar las Commons, clonar el repositorio del grupo
y compilar el proyecto en la máquina remota.

Ante cualquier duda, podés consultar la documentación en el repositorio de
[so-deploy], o utilizar el comando `./deploy.sh -h`.

[so-commons-library]: https://github.com/sisoputnfrba/so-commons-library
[so-deploy]: https://github.com/sisoputnfrba/so-deploy
