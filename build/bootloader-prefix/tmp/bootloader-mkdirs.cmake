# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/borja.dominguez.martin/esp/esp-idf/components/bootloader/subproject"
  "/Users/borja.dominguez.martin/Library/CloudStorage/OneDrive-UniversidaddeAlcala/AA_UAH_Borja_Domínguez/4º/2do_Cuatrimestre/TFG/Nuevo/TFG_Borja_Dominguez/build/bootloader"
  "/Users/borja.dominguez.martin/Library/CloudStorage/OneDrive-UniversidaddeAlcala/AA_UAH_Borja_Domínguez/4º/2do_Cuatrimestre/TFG/Nuevo/TFG_Borja_Dominguez/build/bootloader-prefix"
  "/Users/borja.dominguez.martin/Library/CloudStorage/OneDrive-UniversidaddeAlcala/AA_UAH_Borja_Domínguez/4º/2do_Cuatrimestre/TFG/Nuevo/TFG_Borja_Dominguez/build/bootloader-prefix/tmp"
  "/Users/borja.dominguez.martin/Library/CloudStorage/OneDrive-UniversidaddeAlcala/AA_UAH_Borja_Domínguez/4º/2do_Cuatrimestre/TFG/Nuevo/TFG_Borja_Dominguez/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/borja.dominguez.martin/Library/CloudStorage/OneDrive-UniversidaddeAlcala/AA_UAH_Borja_Domínguez/4º/2do_Cuatrimestre/TFG/Nuevo/TFG_Borja_Dominguez/build/bootloader-prefix/src"
  "/Users/borja.dominguez.martin/Library/CloudStorage/OneDrive-UniversidaddeAlcala/AA_UAH_Borja_Domínguez/4º/2do_Cuatrimestre/TFG/Nuevo/TFG_Borja_Dominguez/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/borja.dominguez.martin/Library/CloudStorage/OneDrive-UniversidaddeAlcala/AA_UAH_Borja_Domínguez/4º/2do_Cuatrimestre/TFG/Nuevo/TFG_Borja_Dominguez/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/borja.dominguez.martin/Library/CloudStorage/OneDrive-UniversidaddeAlcala/AA_UAH_Borja_Domínguez/4º/2do_Cuatrimestre/TFG/Nuevo/TFG_Borja_Dominguez/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
