# Put whatever files we want to install to the user's system,
# into same folder as this script. Then compile the script
# using the NSIS compiler, makensisw.

# name the installer
Outfile "phatbooks_installer.exe"

# set the install directory
InstallDir $PROGRAMFILES64\Phatbooks

#------------
# default section start; every NSIS script has at least one section.
section

# define output path
setOutPath $INSTDIR

# specify file to go in output path
File phatbooks.exe

# define uninstaller name
writeUninstaller $INSTDIR\uninstaller.exe

sectionEnd
# default section end
#-----------


#-----------
# create a section to define what the uninstaller does;
# the section will always be names "Uninstall"
section "Uninstall"

# always delete uninstaller first (not sure why)
delete $INSTDIR\uninstaller.exe

# now delete installed file
delete $INSTDIR\phatbooks.exe

# delete $INSTDIR only if empty (do NOT put /r option, or you risk
# deleting the user's own files in case they set $INSTDIR to some
# directory such as "Program Files", or put other of their own files
# in $INSTDIR).
rmdir $INSTDIR

sectionEnd
# uninstall section end
#---------


