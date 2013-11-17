#!/bin/bash
clear
echo "Compiling QDM Files"
echo "QEO_HOME is $QEO_HOME"

$QEO_HOME/tools/qeo-codegen/bin/qeo-codegen -l c -o ../caminfopi QCamData.xml
$QEO_HOME/tools/qeo-codegen/bin/qeo-codegen -l c -o ../irblasterpi QIRBlaster.xml
$QEO_HOME/tools/qeo-codegen/bin/qeo-codegen -l c -o ../masterapp QMasterCam.xml
