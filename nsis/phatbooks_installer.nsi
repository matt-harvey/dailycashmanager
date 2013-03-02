# Compile using the NSIS compiler, makensisw

# name the installer
Outfile "phatbooks_installer.exe"

# default section start; every NSIS script has at least one section.
section

# Create a popup box with an OK button and the text "Hello world"
messageBox MB_OK "Hello world!"

# default section end
sectionEnd
