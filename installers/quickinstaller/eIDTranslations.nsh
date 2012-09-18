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

;below are the screen header texts, they should be 38 characters maximum
LangString ls_welcome ${LANG_ENGLISH} "Welcome to the eID QuickInstaller"
LangString ls_welcome ${LANG_FRENCH} "Bienvenue à l'eID QuickInstaller"
LangString ls_welcome ${LANG_DUTCH} "Welkom bij de eID QuickInstaller"
LangString ls_welcome ${LANG_GERMAN} "Willkommen auf der eID Installer"

LangString ls_complete ${LANG_ENGLISH} "Installation complete"
LangString ls_complete ${LANG_FRENCH} "Installation complète"
LangString ls_complete ${LANG_DUTCH} "Installatie voltooid"
LangString ls_complete ${LANG_GERMAN} "Installation abgeschlossen"

LangString ls_pleaseconnect ${LANG_ENGLISH} "Please connect your cardreader"
LangString ls_pleaseconnect ${LANG_FRENCH} "Connectez votre lecteur de carte"
LangString ls_pleaseconnect ${LANG_DUTCH} "Gelieve uw kaartlezer aan te sluiten"
LangString ls_pleaseconnect ${LANG_GERMAN} "Bitte verbinden Sie Ihren Kartenleser"

LangString ls_noreaderfound ${LANG_ENGLISH} "No cardreader was found"
LangString ls_noreaderfound ${LANG_FRENCH} "Aucun lecteur de cartes détecté"
LangString ls_noreaderfound ${LANG_DUTCH} "Geen kaartlezer gevonden"
LangString ls_noreaderfound ${LANG_GERMAN} "Kein Kartenleser gefunden"

LangString ls_pleaseinsertcard ${LANG_ENGLISH} "Please insert your beID card"
LangString ls_pleaseinsertcard ${LANG_FRENCH} ""
LangString ls_pleaseinsertcard ${LANG_DUTCH} "Steek de beID kaart in de kaartlezer"
LangString ls_pleaseinsertcard ${LANG_GERMAN} ""

LangString ls_nocardfound ${LANG_ENGLISH} "No beID card was found"
LangString ls_nocardfound ${LANG_FRENCH} ""
LangString ls_nocardfound ${LANG_DUTCH} "Geen beID kaart gevonden"
LangString ls_nocardfound ${LANG_GERMAN} ""

LangString ls_cardread ${LANG_ENGLISH} "Card Read"
LangString ls_cardread ${LANG_FRENCH} "Carte lu"
LangString ls_cardread ${LANG_DUTCH} "Kaart gelezen"
LangString ls_cardread ${LANG_GERMAN} "Karte gelesen"

;below are the content prefixes, preferably below 12 characters
LangString ls_name ${LANG_ENGLISH} "Name:"
LangString ls_name ${LANG_FRENCH} "Nom:"
LangString ls_name ${LANG_DUTCH} "Naam:"
LangString ls_name ${LANG_GERMAN} "Name:"

LangString ls_address ${LANG_ENGLISH} "Address:"
LangString ls_address ${LANG_FRENCH} "Adresse:"
LangString ls_address ${LANG_DUTCH} "Adres:"
LangString ls_address ${LANG_GERMAN} "Adresse:"

LangString ls_error ${LANG_ENGLISH} "Error:"
LangString ls_error ${LANG_FRENCH} "Erreur:"
LangString ls_error ${LANG_DUTCH} "Fout:"
LangString ls_error ${LANG_GERMAN} "Fehler:"

;below are the button's texts, they should be 12 characters maximum
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

;below texts are error reports, they are show in a message box, no length limits
LangString ls_testfailed ${LANG_ENGLISH} "STR:Test failed,$\n$\ngo back and try again,$\nor reboot your pc and launch the eID Viewer to continue testing"
LangString ls_testfailed ${LANG_FRENCH} ""
LangString ls_testfailed ${LANG_DUTCH} "STR:Test gefaald,$\n$\nga terug en probeer opnieuw,$\nof herstart uw pc en start de eID Viewer om verder te testen"
LangString ls_testfailed ${LANG_GERMAN} ""

LangString ls_errorreadersearch ${LANG_ENGLISH} "Error while searching for cardreaders"
LangString ls_errorreadersearch ${LANG_FRENCH} "Problème rencontré pendant la détection du lecteur de cartes"
LangString ls_errorreadersearch ${LANG_DUTCH} "Probleem opgetreden bij het zoeken naar kaartlezers"
LangString ls_errorreadersearch ${LANG_GERMAN} ""

LangString ls_errorreadingcard ${LANG_ENGLISH} "Error while trying to read from card"
LangString ls_errorreadingcard ${LANG_FRENCH} ""
LangString ls_errorreadingcard ${LANG_DUTCH} "Probleem opgetreden bij het lezen van de kaart"
LangString ls_errorreadingcard ${LANG_GERMAN} ""

;below are texts linked to the bitmaps, no length limits
LangString ls_bitmapwelcome ${LANG_ENGLISH} "Background welcome image"
LangString ls_bitmapwelcome ${LANG_FRENCH} ""
LangString ls_bitmapwelcome ${LANG_DUTCH} "Achtergrond welkom afbeelding"
LangString ls_bitmapwelcome ${LANG_GERMAN} ""

LangString ls_bitmapconnectreader ${LANG_ENGLISH} "Background image asking to connect reader"
LangString ls_bitmapconnectreader ${LANG_FRENCH} ""
LangString ls_bitmapconnectreader ${LANG_DUTCH} "Achtergrond afbeelding die vraagt om de lezer aan te sluiten"
LangString ls_bitmapconnectreader ${LANG_GERMAN} ""

LangString ls_bitmapinsertcard ${LANG_ENGLISH} "Background image asking to insert eID card into the cardreader"
LangString ls_bitmapinsertcard ${LANG_FRENCH} ""
LangString ls_bitmapinsertcard ${LANG_DUTCH} "Achtergrond afbeelding die vraagt om de eID kaart in de kaartlezer te steken"
LangString ls_bitmapinsertcard ${LANG_GERMAN} ""
