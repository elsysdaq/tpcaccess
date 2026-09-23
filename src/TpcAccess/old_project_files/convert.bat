copy tpcsettings-1.x.xsd set_in
bin2inc.exe /C tpcsetting set_in set_out
move set_out tpcsettings.inc
del set_in