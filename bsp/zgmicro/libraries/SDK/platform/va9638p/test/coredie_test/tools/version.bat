@echo off
set verbuild="..\coreshare\tools\verbuild.exe"
set pluginPath="..\coreshare\tools\plugin.exe"
set revisionPath=".\common\revision.h"

%verbuild% %revisionPath% *.*.*.+ -xF -b1.0.0.0 -e100.100.100.100 -s -t0
%pluginPath% git ".\common\revision.h"
