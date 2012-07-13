LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Dutch.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\French.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\German.nlf"



VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Belgium eID Middleware"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Software for reading the Belgium Identity Card"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Fedict"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright © 2012"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "eID QuickInstaller"

LangString ls_welcome ${LANG_ENGLISH} "Welcome to the Eid QuickInstaller"
LangString ls_welcome ${LANG_FRENCH} "Bienvenue à l'eID QuickInstaller"
LangString ls_welcome ${LANG_DUTCH} "Welkom bij de Eid QuickInstaller"
LangString ls_welcome ${LANG_GERMAN} "Herzlich Willkommen auf der eID QuickInstaller"

LangString ls_complete ${LANG_ENGLISH} "Installation complete"
LangString ls_complete ${LANG_FRENCH} "Installation complète"
LangString ls_complete ${LANG_DUTCH} "Installatie voltooid"
LangString ls_complete ${LANG_GERMAN} "Installation abgeschlossen"

LangString ls_install ${LANG_ENGLISH} "Install"
LangString ls_install ${LANG_FRENCH} "Installer"
LangString ls_install ${LANG_DUTCH} "Installeren"
LangString ls_install ${LANG_GERMAN} "Installieren"

LangString ls_cancel ${LANG_ENGLISH} "Cancel"
LangString ls_cancel ${LANG_FRENCH} "Annuler"
LangString ls_cancel ${LANG_DUTCH} "Annuleren"
LangString ls_cancel ${LANG_GERMAN} "Abbrechen"

LangString ls_next ${LANG_ENGLISH} "Next"
LangString ls_next ${LANG_FRENCH} "Prochaine"
LangString ls_next ${LANG_DUTCH} "Volgende"
LangString ls_next ${LANG_GERMAN} "Nächste"

LangString ls_back ${LANG_ENGLISH} "Back"
LangString ls_back ${LANG_FRENCH} "Précédente"
LangString ls_back ${LANG_DUTCH} "Vorige"
LangString ls_back ${LANG_GERMAN} "Zurück"

LangString ls_close ${LANG_ENGLISH} "Close"
LangString ls_close ${LANG_FRENCH} "Fermer"
LangString ls_close ${LANG_DUTCH} "Sluiten"
LangString ls_close ${LANG_GERMAN} "Beenden"

LangString ls_finish ${LANG_ENGLISH} "Finish"
LangString ls_finish ${LANG_FRENCH} "OK"
LangString ls_finish ${LANG_DUTCH} "OK"
LangString ls_finish ${LANG_GERMAN} "OK"

LangString ls_test ${LANG_ENGLISH} "Test"
LangString ls_test ${LANG_FRENCH} "Tester"
LangString ls_test ${LANG_DUTCH} "Testen"
LangString ls_test ${LANG_GERMAN} "Prüfen"
